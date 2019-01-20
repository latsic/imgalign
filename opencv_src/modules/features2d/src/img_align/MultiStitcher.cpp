
#include "MultiStitcher.h"
#include "Settings.h"
#include "LogUtils.h"
#include "WarperHelper.h"
#include "Homography.h"
#include "FeatureFactory.h"
#include "ImageUtils.h"

#include "bundle_adjuster/Helper.h"
#include "bundle_adjuster/motion_estimators.hpp"

#include "opencv2/imgproc/types_c.h"

#include <numeric>


using namespace cv;
using namespace imgalign::bundle;

namespace imgalign
{

namespace
{
  void logStitchOrder(
    double globalScale,
    const std::vector<TMat> &srcImages,
    const MultiStitcher::TStitchOrder &stitchOrder,
    const StitchInfo *stitchInfoFirstLast) {
    
    if(stitchOrder.empty()) return;

    LogUtils::getLog() << "==========================" << std::endl;
    LogUtils::getLog() << "stitchOrder: " << std::endl;
    
    auto logStitchInfo = [&](const StitchInfo *stitchInfo, bool logMats) {

      double w = srcImages[stitchInfo->srcImageIndex].size().width;
      double h = srcImages[stitchInfo->srcImageIndex].size().height;

      auto vfw = WarperHelper::getFieldOfView(w, h, stitchInfo->matK.at<double>(0, 0));
      auto vfh = WarperHelper::getFieldOfView(w, h, stitchInfo->matK.at<double>(1, 1)) * (h / w);

      LogUtils::getLog()
        << stitchInfo->srcImageIndex << "->" << stitchInfo->dstImageIndex
        << ", confidence: " << stitchInfo->matchInfo.confidence
        << ", matches count all/filtered/outlier/inlier: "
          << stitchInfo->matchInfo.allMatchInfos.size() << "/"
          << stitchInfo->matchInfo.filteredMatchInfos.size() << "/"
          << stitchInfo->matchInfo.outlierMatchInfos.size() << "/"
          << stitchInfo->matchInfo.inlierMatchInfos.size()
        << ", determinant: " << stitchInfo->matchInfo.determinant
        << ", svdConditionNumber: " << stitchInfo->matchInfo.svdConditionNumber
        << ", deltaH/deltaV [°]: " << stitchInfo->deltaH << "/" << stitchInfo->deltaV
        << ", absH/absV [°]: " << stitchInfo->absH << "/" << stitchInfo->absV
        << ", w/h [px]: " << w << "/" << h
        << ", vfw/vfh [°]: " << vfw << "/" << vfh 
        << std::endl;
        if(logMats) {
          LogUtils::logMat("R", stitchInfo->matR);
          LogUtils::logMat("K", stitchInfo->matK);
        }
    };
    
    for(auto it = ++stitchOrder.begin(); it != stitchOrder.end(); ++it) {
      logStitchInfo(*it, false);
      LogUtils::getLog() << "--------------------------" << std::endl;
    }
    if(stitchInfoFirstLast != nullptr) {
      logStitchInfo(stitchInfoFirstLast, true);
    }
    LogUtils::getLog() << "global scale: " << globalScale << std::endl;
    LogUtils::getLog() << "==========================" << std::endl;
  }

  bool isReasonableSize(int w, int h, int warpedW, int WarpedH, size_t imageIndex) {
    if(warpedW * WarpedH < 16 * w * h) {
      return true;
    }
    
    LogUtils::getLogUserError()
      << "warped image " << imageIndex << ",  unreasonable size: "
      <<  w << "x" << h << " => " << warpedW << "x" << WarpedH
      << ", aborting. "
      << "Advice: Try different bundle adjustment type." << std::endl;

    return false;
  }

  void logCameras(const MultiStitcher::TStitchOrder &stitchOrder)
  { 
    size_t index = 0;
    for(auto it = stitchOrder.begin(); it != stitchOrder.end(); ++it) {
      LogUtils::getLog() << "Index " << index << " ";
      LogUtils::logMat("R", (*it)->matR);
      ++index;
    }

    index = 0;
    for(auto it = stitchOrder.begin(); it != stitchOrder.end(); ++it) {
      LogUtils::getLog() << "Index " << index << " ";
      LogUtils::logMat("K", (*it)->matK);
      ++index;
    }
  }
}

MultiStitcher::MultiStitcher(
  const std::vector<TMat> &inSrcImages,
  const Settings &inSettings)

  : srcImages(inSrcImages)
  , points(inSrcImages.size())
  , descriptors(inSrcImages.size())
  , settings(inSettings)
{

  FUNCLOGTIMEL("MultiStitcher::MultiStitcher");

  if(inSrcImages.empty()) {
    throw std::logic_error("No src image input");
  }
  
  int maxPixelsN = (int)settings.getValue(eImageCap);
  srcImagesScaled.resize(inSrcImages.size());
  size_t imageIndex = 0;
  for(const auto &image : inSrcImages) {

    Mat srcImageGray;
		cvtColor(image, srcImageGray, CV_RGBA2GRAY);

    scaleFactors.push_back(
      1.0 / ImageUtils::resize(srcImageGray, srcImagesScaled[imageIndex++], maxPixelsN));
  }
}

TConstMat &
MultiStitcher::getStitchedImage()
{
  FUNCLOGTIMEL("MultiStitcher::getStitchedImage");
  stitchedImage.stitch(false, seamBlend, compensateExposure, rectify);
  return stitchedImage.image;
}

void
MultiStitcher::releaseStitchedImage()
{
  FUNCLOGTIMEL("MultiStitcher::releaseStitchedImage");
  stitchedImage.image.release();
}

void
MultiStitcher::releaseStitchedData()
{
  FUNCLOGTIMEL("MultiStitcher::releaseStitchedData");

  stitchedImage.stitchedInfos.clear();
}

TConstMat &
MultiStitcher::getStitchedImageCurrent()
{
  FUNCLOGTIMEL("MultiStitcher::getStitchedImageCurrent");
  stitchedImage.stitch(true, false, false, false);
  return stitchedImage.image;
}

bool
MultiStitcher::initStiching(
  const std::vector<double> &inFieldsOfView,
  std::vector<int> &outStitchIndices)
{
  FUNCLOGTIMEL("MultiStitcher::initStiching");

  if(inFieldsOfView.size() != srcImages.size())
  {
    throw std::logic_error("Invalid field of view data");
  }
  fieldsOfView.assign(inFieldsOfView.begin(), inFieldsOfView.end());

  projectionType = settings.getValue(eMultiStitch_projection);
  rectify = settings.getValue(eMultiStitch_rectify);
  camEstimate = settings.getValue(eMultiStitch_camEstimate);
  bundleAdjustType = settings.getBundleAdjustType();
  colorTransfer = settings.getValue(eMultiStitch_colorTransfer);
  seamBlend = settings.getValue(eMultiStitch_seamBlend);
  compensateExposure = settings.getValue(eMultiStitch_exposureCompensator);
  calcImageOrder = settings.getValue(eMultiStitch_calcImageOrder);
  confidenceThresh = settings.getValue(eMultiStitch_confidenceThresh);
  globalScale = 0.0;
  wcType = settings.getValue(eMultiStitch_waveCorrection) > 0.0
    ? WaveCorrectType::WCT_AUTO
    : WaveCorrectType::WCT_NONE;
  calcCenterImage = settings.getValue(eMultiStitch_calcCenterImage);
  stitchInfoFirstLast = nullptr;

  tfType = settings.getTransformFinderType();
  matcher = FeatureFactory::CreateMatcher(settings);

  LogUtils::getLog() << "projectionType " << projectionType << std::endl;
  LogUtils::getLog() << "rectify " << rectify << std::endl;
  LogUtils::getLog() << "camEstimate " << camEstimate << std::endl;
  LogUtils::getLog() << "bundleAdjust " << (int)(settings.getValue(eMultiStitch_bundleAdjustType)) << std::endl;
  LogUtils::getLog() << "waveCorrection " << (wcType != WaveCorrectType::WCT_NONE ? "on" : "off") << std::endl;
  LogUtils::getLog() << "colorTransfer " << colorTransfer << std::endl;
  LogUtils::getLog() << "seamBlend " << seamBlend << std::endl;
  LogUtils::getLog() << "compensateExposure " << compensateExposure << std::endl;
  LogUtils::getLog() << "calcImageOrder " << calcImageOrder << std::endl;
  LogUtils::getLog() << "calcCenterImage " << calcCenterImage << std::endl;
  LogUtils::getLog() << "confidenceThresh " << confidenceThresh << std::endl;

  computeKeyPoints();

  for(auto &stitchInfo : stitchInfos) {
    stitchInfo->resetCamData();
  }

  centerImageIndex = 0;
  if(calcCenterImage) {
    centerImageIndex = getCenterImage();
  }

  stitchOrder = computeStitchOrder();
  //stitchInfoFirstLast = computeStitchInfoFirstLast(stitchOrder);
  
  for(auto *stitchInfo : stitchOrder) {
    outStitchIndices.push_back(stitchInfo->srcImageIndex);
  }

  if(stitchOrder.size() < 2) {
    return false;
  }
  centerImageIndex = stitchOrder[0]->srcImageIndex;

  computeRelativeRotation(stitchOrder);
  computeAbsRotation(stitchOrder);
  setCamMatrices(stitchOrder);
  //logCameras(stitchOrder);

  auto resetRotMatrices = [&](){
    TMat eye = TMat::eye(3, 3, CV_64F);
    for(auto *stitchInfo : stitchOrder) {
      eye.copyTo(stitchInfo->matR);
    }
  };

  if( (bundleAdjustType == BundleAdjustType::BAT_NONE && !camEstimate)
    || !camEstimateAndBundleAdjust(stitchOrder, stitchInfoFirstLast, globalScale)) {
    
    globalScale = 0.0;
    resetRotMatrices();
  }
  else {
    waveCorrection(stitchOrder);
  }

  logStitchOrder(globalScale, srcImages, stitchOrder, stitchInfoFirstLast);
  
  stitchedImage.init(
    srcImages[centerImageIndex],
    projectionType,
    stitchOrder,
    globalScale == 0.0 ? nullptr : &globalScale);
  
  return true;
}

int
MultiStitcher::stitchAll()
{
  FUNCLOGTIMEL("MultiStitcher::stitchAll");

  for(auto it = stitchOrder.begin(); it != stitchOrder.end(); ++it) {
    if(!stitch(**it)) break;
  }
  return (int)stitchedImage.imageIndices.size();
}

bool MultiStitcher::stitchNext(const StitchInfo &stitchInfo)
{
  FUNCLOGTIMEL("MultiStitcher::stitchNext");
  return stitch(stitchInfo);
  // LogUtils::getLogUserInfo() << "Stitching "
  //   << stitchInfo.srcImageIndex << "->" << stitchInfo.dstImageIndex << ", w/h =>"
  //   << stitchedImage.image.size().width << "/" << stitchedImage.image.size().width << std::endl;
  // return ok;
}

const StitchInfo *
MultiStitcher::findNextMatch(const TStitchOrder &rStitchOrder)
{
  FUNCLOGTIMEL("MultiStitcher::findNextMatch");

  if(rStitchOrder.empty()) return nullptr;

  TIndices dstIndices;
  TIndices srcIndices;
  for(size_t i = 0; i < srcImages.size(); ++i) {

    auto it = std::find_if(rStitchOrder.begin(), rStitchOrder.end(),
    [&](const StitchInfo *stitchInfo) {
      return stitchInfo->srcImageIndex == i;
    });

    if(it == rStitchOrder.end()) srcIndices.push_back(i);
    else dstIndices.push_back(i);
  }

  return findNextMatch(dstIndices, srcIndices, &rStitchOrder);
}

const StitchInfo *
MultiStitcher::findNextMatch(
  TConstIndices &dstI,
  TConstIndices &srcI,
  const TStitchOrder *pStitchOrder)
{ 
  FUNCLOGTIMEL("MultiStitcher::findNextMatch");

  if(srcI.empty() || dstI.empty()) return nullptr;
  
  const StitchInfo *stitchInfoP = nullptr;
  for(auto dstIndex : dstI) { 
    for(auto srcIndex : srcI) {
      const auto *stitchInfo = getStitchInfo(dstIndex, srcIndex);

      if(  stitchInfo->matchInfo.success
        && stitchInfo->matchInfo.confidence > confidenceThresh
        && stitchInfo->matchInfo.isHomographyGood()
        && stitchInfo->matchInfoInverse.isHomographyGood()) {

        if(stitchInfo->deltaH == 0.0 && stitchInfo->deltaV == 0.0) {
          computeRelativeRotation(*stitchInfo);
        }

        if(stitchInfoP == nullptr) {
          stitchInfoP = stitchInfo;
          continue;
        }
        auto deltaAngleSumNew = std::abs(stitchInfo->deltaH) + std::abs(stitchInfo->deltaV);
        auto deltaAngleSumOld = std::abs(stitchInfoP->deltaH) + std::abs(stitchInfoP->deltaV);

        auto confidenceNew = stitchInfo->matchInfo.confidence;
        auto confidenceOld = stitchInfoP->matchInfo.confidence;

        if(confidenceNew + 0.3 < confidenceOld) {
          // keep old
        }
        else if(confidenceNew > confidenceOld + 0.3) {
          stitchInfoP = stitchInfo;
        }
        else if((confidenceNew < 0.3 || confidenceOld < 0.3) && confidenceNew - confidenceOld > 0.1) {
          stitchInfoP = stitchInfo;
        }
        else if((confidenceNew < 0.3 || confidenceOld < 0.3) && confidenceOld - confidenceNew > 0.1) {
          // keep old
        }

        else if(  pStitchOrder != nullptr
          && std::abs(deltaAngleSumNew - deltaAngleSumOld) < 0.2 * deltaAngleSumOld) {
          
          auto deltaAbsAngleSumNew = std::abs(stitchInfo->absH - (*pStitchOrder)[0]->absH)
                                   + std::abs(stitchInfo->absV - (*pStitchOrder)[0]->absV);
          auto deltaAbsAngleSumOld = std::abs(stitchInfoP->absH - (*pStitchOrder)[0]->absH)
                                   + std::abs(stitchInfoP->absV - (*pStitchOrder)[0]->absV);

          if(deltaAbsAngleSumNew < deltaAbsAngleSumOld) {
            stitchInfoP = stitchInfo;
          }
        }
        else if(deltaAngleSumNew < deltaAngleSumOld) {
          stitchInfoP = stitchInfo;
        }
      }
    }
  }


  return stitchInfoP;
}

bool
MultiStitcher::stitch(const StitchInfo &stitchInfo)
{
  FUNCLOGTIMEL("MultiStitcher::stitch");

  auto srcIndex = stitchInfo.srcImageIndex;
  auto dstIndex = stitchInfo.dstImageIndex;

  auto fieldOfViewSrc = fieldsOfView[srcIndex];
  auto srcImage = srcImages[srcIndex];
  if(colorTransfer) {
    srcImage = ImageUtils::colorTransfer(srcImages[centerImageIndex], srcImage);
  }
  
  TMat rotMat1, rotMat2;
  WarperHelper::getMatR(0, 0, 0, rotMat1);
  WarperHelper::getMatR(0, 0, 0, rotMat2);

  TMat *kMat1 = nullptr;
  TMat *kMat2 = nullptr;

  rotMat2 = stitchInfo.matR;
  kMat2 = &stitchInfo.matK;

  TPoints2f ptsWarped1 = stitchedImage.getTransformedPts(
    stitchInfo.matchInfo.inlierPts1,
    srcImages[dstIndex].size(),
    dstIndex,
    globalScale == 0.0 ? nullptr : &globalScale);

  TMat homography;
  bool homographyFound = Homography::getHomography(
    ptsWarped1,
    stitchInfo.matchInfo.inlierPts2, 
    cv::Size(0, 0), //stitchedImage.image.size(),
    srcImage.size(),
    kMat1, kMat2,
    0.0,//stitchedImage.fieldOfView,
    fieldOfViewSrc,
    rotMat1,
    rotMat2,
    eStitch_projectionTypeNone,
    projectionType,
    settings.getTransformFinderType(),
    homography,
    globalScale == 0.0 ? nullptr : &globalScale,
    nullptr,
    nullptr);
  
  if(!homographyFound) {
    throw std::logic_error("No homography found on warped images");
  }

  TMat srcImageProjected;
  if((ParamType)projectionType != eStitch_projectionTypeNone) {
    if(kMat2 != nullptr) {
      WarperHelper::warpImage(
        projectionType, srcImage, srcImageProjected, *kMat2, rotMat2, globalScale == 0.0 ? nullptr : &globalScale);
    }
    else {
      WarperHelper::warpImage(
        projectionType, srcImage, srcImageProjected, fieldOfViewSrc, rotMat2);
    }
  }
  else {
    srcImage.copyTo(srcImageProjected);
  }

  int w1 = stitchedImage.imageSize.width;
  int h1 = stitchedImage.imageSize.height;
  int w2 = srcImageProjected.size().width;
  int h2 = srcImageProjected.size().height;

  {
    
    TMat homography2;
    homography.copyTo(homography2);
    double tx, ty, t, r, b, l;
    WarperHelper::getBox(w2, h2, homography2, tx, ty, t, r, b, l);

    // Sanity check!
    if(!isReasonableSize(w2, h2, (int)(r - l), (int)(b - t), srcIndex)) {
      return false;
    }

    if(tx < 0) {
      //stitchedImage.tlCorners[srcIndex].x = -tx;
      stitchedImage.setCornerXFor(srcIndex, -tx);
    }
    if(ty < 0) {
      //stitchedImage.tlCorners[srcIndex].y = -ty;
      stitchedImage.setCornerYFor(srcIndex, -ty);
    }
    
    Mat alignedImage;
    WarperHelper::warpPerspective(
      srcImageProjected, homography2,
      cv::Size((int)(r - l), (int)(b - t)),
      stitchedImage.warpedImage(srcIndex));

    stitchedImage.createMaskFor(srcIndex);

    // LogUtils::getLog() << "MultiStitcher::stitch w/h"
    // <<  stitchedImage.warpedImage(srcIndex).size().width << "/"
    // <<  stitchedImage.warpedImage(srcIndex).size().height << std::endl;
  }
  
  double tx, ty, t, r, b, l;
  WarperHelper::getBox(
    w1, h1, w2, h2,
    homography,
    tx, ty, t, r, b, l);

  // LogUtils::getLog() << "src->dst 2, " << stitchInfo.srcImageIndex << "->" << stitchInfo.dstImageIndex << std::endl;
  // LogUtils::getLog() << "tx: " << tx << std::endl;
  // LogUtils::getLog() << "ty: " << ty << std::endl;
  // LogUtils::getLog() << "l: " << l << std::endl;
  // LogUtils::getLog() << "r: " << r << std::endl;
  // LogUtils::getLog() << "t: " << t << std::endl;
  // LogUtils::getLog() << "b: " << b << std::endl;
  

  stitchedImage.setHomographyFor(srcIndex, homography);
  if(kMat2 != nullptr) stitchedImage.setKMatFor(srcIndex, *kMat2);
  stitchedImage.setRMatFor(srcIndex, rotMat2);

  //homography.copyTo(stitchedImage.homographies[srcIndex]);
  //rotMat2.copyTo(stitchedImage.rMatVector[srcIndex]);
  //if(kMat2 != nullptr) kMat2->copyTo(stitchedImage.kMatVector[srcIndex]);
  stitchedImage.addTranslation(tx, ty);
  stitchedImage.imageIndices.push_back(srcIndex);
  stitchedImage.imageSize = cv::Size((int)(b - t), (int)(r - l));
  
  // stitchedImage.fieldOfView = WarperHelper::fieldOfView(
  //   stitchedImage.fieldOfView, fieldOfViewSrc, stitchInfo.deltaH);
  return true;
}

void
MultiStitcher::computeKeyPoints()
{
  FUNCLOGTIMEL("MultiStitcher::computeKeyPoints");

  if(keyPointsComputed)
  {
    return;
  }

  cv::Ptr<cv::Feature2D> featureDet;
  cv::Ptr<cv::Feature2D> featureDes;

  FeatureFactory::CreateFeatureDetector(featureDet, settings);
  FeatureFactory::CreateDescriptorComputer(featureDes, settings);

  size_t imageIndex = 0;
  keyPoints.resize(srcImagesScaled.size());
  descriptors.resize(srcImagesScaled.size());
  for(auto &image : srcImagesScaled) {

    featureDet->detect(image, keyPoints[imageIndex]);
    featureDes->compute(image, keyPoints[imageIndex], descriptors[imageIndex]);

    auto ptIndex = 0;
    points[imageIndex].resize(keyPoints[imageIndex].size());
    for(auto &keyPoint : keyPoints[imageIndex]) {
      keyPoint.pt.x *= scaleFactors[imageIndex];
      keyPoint.pt.y *= scaleFactors[imageIndex];
      points[imageIndex][ptIndex].x = keyPoint.pt.x;
      points[imageIndex][ptIndex].y = keyPoint.pt.y;

      ++ptIndex;
    }
    ++imageIndex;

    LogUtils::getLogUserInfo() << "keypoints image "
      << imageIndex << "/" << srcImagesScaled.size() << std::endl;
  }
  srcImagesScaled.clear();
  keyPointsComputed = true;
}

const StitchInfo *
MultiStitcher::getStitchInfo(size_t dstI, size_t srcI)
{
  //FUNCLOGTIMEL("MultiStitcher::getStitchInfo");

  if(dstI >= srcImages.size() || srcI >= srcImages.size()) {
    throw std::logic_error("invalid image index");
  }
  auto it = std::find_if(stitchInfos.begin(), stitchInfos.end(),
    [&](const std::shared_ptr<StitchInfo> stitchInfo) {
    return stitchInfo->srcImageIndex == srcI
        && stitchInfo->dstImageIndex == dstI;
  });
  if(it != stitchInfos.end())
  {
    return (*it).get();
  }

  //auto startTime = imgalign::milliseconds();

  auto spStitchInfo = std::make_shared<StitchInfo>(srcI, dstI);
  stitchInfos.push_back(spStitchInfo);
  if(dstI != srcI) {

    spStitchInfo->matchInfo = matcher.match(
      tfType, descriptors[dstI], descriptors[srcI],
      keyPoints[dstI], keyPoints[srcI]);

      if(LogUtils::isDebug) {
        LogUtils::getLog() << "Matching from " << srcI << " to " << dstI << ", confidence: "
          << spStitchInfo->matchInfo.confidence << ", determinant: "
          << spStitchInfo->matchInfo.determinant <<  std::endl;
      }

      if(spStitchInfo->matchInfo.success) {

        spStitchInfo->matchInfoInverse = spStitchInfo->matchInfo.getInverse();

        // spStitchInfo->matchInfoInverse = matcher.match(tfType,
        //   descriptors[srcI], descriptors[dstI],
        //   keyPoints[srcI], keyPoints[dstI]);
      }
  }
  //LogUtils::getLog() << "getStitchInfo time used: " << (imgalign::milliseconds() - startTime) << " ms" << std::endl;
  return spStitchInfo.get();
}

size_t MultiStitcher::getCenterImage()
{
  FUNCLOGTIMEL("MultiStitcher::getCenterImage");

  if(srcImages.empty()) {
    throw std::logic_error("No image data available");
  }

  if(srcImages.size() <= 2) {
    return 0;
  }

  auto _stitchOrder = computeStitchOrder(srcImages.size() / 2);
  computeRelativeRotation(_stitchOrder);
  
  struct Info {
    double absH = 0.0;
    double absV = 0.0;
  };
  std::vector<Info> infos(srcImages.size());

  for(auto it = _stitchOrder.begin(); it != _stitchOrder.end(); ++it) {

    auto dstI = (*it)->dstImageIndex;
    auto srcI = (*it)->srcImageIndex;

    infos[srcI].absH = infos[dstI].absH + (*it)->deltaH;
    infos[srcI].absV = infos[dstI].absV + (*it)->deltaV;
  }
  double minH, maxH, minV, maxV;
  minH = minV = std::numeric_limits<double>::max();
  maxH = maxV = -std::numeric_limits<double>::max();
  for(auto it = infos.begin(); it != infos.end(); ++it) {
    
    if(it->absH < minH) minH = it->absH;
    if(it->absH > maxH) maxH = it->absH;
    if(it->absV < minV) minV = it->absV;
    if(it->absV > maxV) maxV = it->absV;
  }

  double centerH = 0.5 * (maxH + minH);
  double centerV = 0.5 * (maxV + minV);

  size_t centerIndex = 0;
  size_t index = 0;
  double deviation = std::numeric_limits<double>::max();
  for(auto it = infos.begin(); it != infos.end(); ++it) {
    auto diffH = std::abs(it->absH - centerH);
    auto diffV = std::abs(it->absV - centerV);

    if(std::abs(diffH) + std::abs(diffV) < deviation) {
      deviation = std::abs(diffH) + std::abs(diffV);
      centerIndex = index;
    }
    ++index;
  }

  LogUtils::getLogUserInfo() << "Starting with image: " << centerIndex << std::endl;

  return centerIndex;
}

const StitchInfo *
MultiStitcher::computeStitchInfoFirstLast(TStitchOrder &rStitchOrder)
{
  FUNCLOGTIMEL("MultiStitcher::computeStitchInfoFirstLast");

  if(rStitchOrder.size() > 2) {
    auto *stitchInfo = getStitchInfo(rStitchOrder.back()->srcImageIndex, rStitchOrder[0]->dstImageIndex);
    if(  stitchInfo != nullptr
      && stitchInfo->matchInfo.isHomographyGood()
      && stitchInfo->matchInfo.confidence > confidenceThresh) {
        
      return stitchInfo;
    }
  }
  return nullptr;
}

MultiStitcher::TStitchOrder MultiStitcher::computeStitchOrder(size_t startIndex)
{
  FUNCLOGTIMEL("MultiStitcher::computeStitchOrder");

  if(srcImages.empty()) {
    throw std::logic_error("No image data available");
  }

  TStitchOrder _stitchOrder;
  _stitchOrder.push_back(getStitchInfo(startIndex, startIndex));

  if(srcImages.size() < 2) {
    return _stitchOrder;
  }
  
  if(!calcImageOrder) {
    LogUtils::getLog() << "Matching images, using input image order as stitching order" << std::endl;
    
    size_t lastDstSuccess = startIndex;

    int matchesFoundN = 0;

    auto calcNext = [&](int srcIndex) {

      size_t dstIndex = lastDstSuccess;
    
      auto *stitchInfo = getStitchInfo(dstIndex, srcIndex);
      if(  stitchInfo != nullptr
        && stitchInfo->matchInfo.isHomographyGood()
        && stitchInfo->matchInfo.confidence > confidenceThresh) {
        
        lastDstSuccess = srcIndex;
        _stitchOrder.push_back(stitchInfo);

        // LogUtils::getLog() << "Match found: "
        //   << stitchInfo->srcImageIndex << "->" << stitchInfo->dstImageIndex << ", "
        //   << "confidence: " << stitchInfo->matchInfo.confidence << std::endl;

        LogUtils::getLogUserInfo() << "Matching, found "
          << stitchInfo->srcImageIndex << "->" << stitchInfo->dstImageIndex << ", "
          << ++matchesFoundN << "/" << srcImages.size() - 1
          << std::endl;
      }
    };
    
    for(size_t i = startIndex; i < srcImages.size() - 1; ++i){
      calcNext(i + 1);
    }
    lastDstSuccess = startIndex;
    for(int i = startIndex; i > 0; --i){
      calcNext(i - 1);
    }

    // for(size_t i = startIndex; i < srcImages.size() - 1; ++i) {
      
    //   size_t srcIndex = i + 1;
    //   size_t dstIndex = lastDstSuccess;

    //   auto *stitchInfo = getStitchInfo(dstIndex, srcIndex);
    //   if(  stitchInfo != nullptr
    //     && stitchInfo->matchInfo.isHomographyGood()
    //     && stitchInfo->matchInfo.confidence > confidenceThresh) {
        
    //     lastDstSuccess = srcIndex;
    //     _stitchOrder.push_back(stitchInfo);

    //     LogUtils::getLog() << "Match found: "
    //       << stitchInfo->srcImageIndex << "->" << stitchInfo->dstImageIndex << ", "
    //       << "confidence: " << stitchInfo->matchInfo.confidence << std::endl;
    //   }
    // }
  }
  else {
    LogUtils::getLog() << "Matching images and computing stitching order" << std::endl;
    
    // std::function<int(int)> nthTriangularNumber = [&nthTriangularNumber](int n) -> int {
    //   if(n == 1) return 1;
    //   if(n == 0) return 0;
    //   return n + nthTriangularNumber(n - 1);
    // };
    int matchesFound = 0;
    int totalMatchesToFind = srcImages.size() - 1;//nthTriangularNumber(srcImages.size() - 1);

    while(true) {
      const auto *stitchInfo = findNextMatch(_stitchOrder);
      if(stitchInfo == nullptr) break;
      _stitchOrder.push_back(stitchInfo);

      LogUtils::getLogUserInfo() << "Matching, found "
          << stitchInfo->srcImageIndex << "->" << stitchInfo->dstImageIndex << ", "
          << ++matchesFound << "/" << totalMatchesToFind
          << std::endl;

      // LogUtils::getLog() << "Match found: "
      //     << stitchInfo->srcImageIndex << "->" << stitchInfo->dstImageIndex << ", "
      //     << "confidence: " << stitchInfo->matchInfo.confidence << std::endl;
    }
    
  }
  LogUtils::getLog() << "done" << std::endl;
  return _stitchOrder;
}

MultiStitcher::TStitchOrder
MultiStitcher::computeStitchOrder()
{
  FUNCLOGTIMEL("MultiStitcher::computeStitchOrder");

  auto _stitchOrder = computeStitchOrder(centerImageIndex);
  
  auto failIndex = centerImageIndex;
  for(size_t i = 0; i < srcImages.size() && _stitchOrder.size() < 2; ++i) {

    if(i == centerImageIndex) continue;

    LogUtils::getLog()
      << "Failed to compute stitchOrder with start index " << failIndex
      << " retrying with index " << i << std::endl; 

    _stitchOrder = computeStitchOrder(i);
    failIndex = i;
  }
  return _stitchOrder;
}

void
MultiStitcher::computeRelativeRotation(TStitchOrder &rStitchOrder)
{
  FUNCLOGTIMEL("MultiStitcher::computeRelativeRotation");

  for(auto it = rStitchOrder.begin(); it != rStitchOrder.end(); ++it) {
    computeRelativeRotation(**it);
  }
}

void
MultiStitcher::computeRelativeRotation(const StitchInfo &stitchInfo)
{
  FUNCLOGTIMEL("MultiStitcher::computeRelativeRotation");

  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    stitchInfo.deltaH = 0.0;
    stitchInfo.deltaV = 0.0;
    return;
  }

  auto w1 = srcImages[stitchInfo.dstImageIndex].size().width;
  auto h1 = srcImages[stitchInfo.dstImageIndex].size().height;
  auto w2 = srcImages[stitchInfo.srcImageIndex].size().width;
  auto h2 = srcImages[stitchInfo.srcImageIndex].size().height;

  WarperHelper::getRelativeRotation(
    w1, h1, w2, h2,
    fieldsOfView[stitchInfo.dstImageIndex],
    stitchInfo.matchInfo.homography,
    stitchInfo.deltaH, stitchInfo.deltaV);
}

void
MultiStitcher::computeAbsRotation(TStitchOrder &rStitchOrder)
{
  FUNCLOGTIMEL("MultiStitcher::computeAbsRotation");

  if(rStitchOrder.size() < 2) return;

  double minH, maxH, minV, maxV;
  minH = minV = -std::numeric_limits<double>::max();
  maxH = maxV = std::numeric_limits<double>::max();

  auto itLast = rStitchOrder.begin();
  for(auto it = ++rStitchOrder.begin(); it != rStitchOrder.end(); ++it, ++itLast) {

    double absH = (*itLast)->absH + (*it)->deltaH;
    double absV = (*itLast)->absV + (*it)->deltaV;

    (*it)->absH = absH;
    (*it)->absV = absV;

    WarperHelper::getMatR(absH, absV, 0, (*it)->matR);

    if(absH < minH) minH = absH;
    if(absH > maxH) maxH = absH;
    if(absV < minV) minV = absV;
    if(absV > maxV) maxV = absV;
  }

  estimatedFieldOfViewH = maxH - minH;
  estimatesFieldOfViewV = maxV - minV;
}

void
MultiStitcher::setCamMatrices(TStitchOrder &rStitchOrder)
{
  FUNCLOGTIMEL("MultiStitcher::setCamMatrices");

  for(auto it = rStitchOrder.begin(); it != rStitchOrder.end(); ++it) {

    size_t srcIndex = (*it)->srcImageIndex;
    auto w = srcImages[srcIndex].size().width;
    auto h = srcImages[srcIndex].size().height;

    auto fLenPx = WarperHelper::getFocalLengthPx(w, h, fieldsOfView[srcIndex]);
    WarperHelper::getMatK(w, h, fLenPx, (*it)->matK);
  }
}

bool
MultiStitcher::camEstimateAndBundleAdjust(
  TStitchOrder &rStitchOrder, const StitchInfo *pStitchInfoFirstLast, double &rGlobalScale)
{
  FUNCLOGTIMEL("MultiStitcher::MultiStitcher::camEstimateAndBundleAdjust");

  if(bundleAdjustType == BundleAdjustType::BAT_NONE && !camEstimate) return false;

  std::string ba_cost_func, ba_refine_mask;
  if(bundleAdjustType == BundleAdjustType::BAT_RAY) {
    ba_cost_func = "ray";
    ba_refine_mask = "xxx_x";
  }
  else if(bundleAdjustType == BundleAdjustType::BAT_REPROJ){
    ba_cost_func = "reproj";
    ba_refine_mask = "xxx_x";
  }
  else if(bundleAdjustType != BundleAdjustType::BAT_NONE) {
    LogUtils::getLog() << "Unknown bundle adjust type" << std::endl;
    throw std::logic_error("Unknown bundle adjust type");
  }

  double confThresh = 0.8;
  std::string estimator_type = "not_affine";
  rGlobalScale = 0.0;

  std::vector<MatchesInfo> matchesInfoV;
  std::vector<CameraParams> cameraParamsV;
  std::vector<ImageFeatures> imageFeaturesV;

  std::vector<cv::Size> imageSizes;
  for(size_t i = 0; i < srcImages.size(); ++i) {
    imageSizes.push_back(srcImages[i].size());
  }

  Helper::getData(
    keyPoints, descriptors, imageSizes, fieldsOfView, rStitchOrder, pStitchInfoFirstLast,
    matchesInfoV, cameraParamsV, imageFeaturesV);

  if(camEstimate) {

    LogUtils::getLogUserInfo() << "Estimating camera params ..." << std::endl;

    Ptr<detail::Estimator> estimator;
    if(estimator_type == "affine") {
      estimator = makePtr<detail::AffineBasedEstimator>();
    }
    else {
      estimator = makePtr<detail::HomographyBasedEstimator>();
    }
    cameraParamsV.clear();
    if (!(*estimator)(imageFeaturesV, matchesInfoV, cameraParamsV)) {
        LogUtils::getLogUserInfo() << "Camera params estimation failed.";
        return false;
    }
  }

  for(size_t i = 0; i < cameraParamsV.size(); ++i) {
    Mat rMat;
    cameraParamsV[i].R.convertTo(rMat, CV_32F);
    rMat.copyTo(cameraParamsV[i].R);
  }

  bool bundleAdjustFailed = false;

  if(bundleAdjustType != BundleAdjustType::BAT_NONE) {
    
    LogUtils::getLogUserInfo() << "Bundle adjustement, type " << ba_cost_func << std::endl;

    Ptr<detail::BundleAdjusterBase> adjuster = nullptr;
    if(ba_cost_func == "reproj") adjuster = makePtr<detail::BundleAdjusterReproj>();
    else if(ba_cost_func == "ray") adjuster = makePtr<detail::BundleAdjusterRay>();
    else if(ba_cost_func == "affine") adjuster = makePtr<detail::BundleAdjusterAffinePartial>();
    else if(ba_cost_func == "no") adjuster = makePtr<detail::NoBundleAdjuster>();
    else {
      LogUtils::getLog() << "Unknown bundle adjustment cost function: '" << ba_cost_func << " " << std::endl;
    }
    if(adjuster != nullptr) {
      std::vector<CameraParams> cameraParamsBundleV(cameraParamsV.begin(), cameraParamsV.end());
      adjuster->setConfThresh(confThresh);
      Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
      if (ba_refine_mask[0] == 'x') refine_mask(0,0) = 1;
      if (ba_refine_mask[1] == 'x') refine_mask(0,1) = 1;
      if (ba_refine_mask[2] == 'x') refine_mask(0,2) = 1;
      if (ba_refine_mask[3] == 'x') refine_mask(1,1) = 1;
      if (ba_refine_mask[4] == 'x') refine_mask(1,2) = 1;
      adjuster->setRefinementMask(refine_mask);
      if (!(*adjuster)(imageFeaturesV, matchesInfoV, cameraParamsBundleV))
      {
        LogUtils::getLogUserInfo() << "Bundle adjustement failed." << std::endl;
        if(!camEstimate) return false;
        bundleAdjustFailed = true;
      }
      else {
        cameraParamsV.clear();
        cameraParamsV.assign(cameraParamsBundleV.begin(), cameraParamsBundleV.end());
      }
    }
  }
  if(bundleAdjustType == BundleAdjustType::BAT_NONE || bundleAdjustFailed) {
    TMat eye = TMat::eye(3, 3, CV_32F);
    for(auto &camParams : cameraParamsV) {
      eye.copyTo(camParams.R);
    }
  }

  std::vector<double> focals;
  size_t i = 0;
  for(auto stitchInfo : rStitchOrder) {  

    cameraParamsV[i].R.convertTo(stitchInfo->matR, CV_64F);
    cameraParamsV[i].K().convertTo(stitchInfo->matK, CV_64F);
    focals.push_back(cameraParamsV[i].focal);
    ++i;
  }
  
  std::sort(focals.begin(), focals.end());
  rGlobalScale = focals.size() % 2 == 1
    ? focals[focals.size() / 2]
    : (focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;
  
  return true;
}

void MultiStitcher::waveCorrection(TStitchOrder &rStitchOrder)
{
  FUNCLOGTIMEL("MultiStitcher::waveCorrection");
  if(!LogUtils::isDebug) {
    LogUtils::getLogUserInfo() << "Wave correction" << std::endl;
  }
  
  if(wcType == WaveCorrectType::WCT_NONE) return;
  
  bool horizonal = wcType == WaveCorrectType::WCT_H;
  if(wcType == WaveCorrectType::WCT_V) {
    horizonal = false;
  }
  else if(wcType == WaveCorrectType::WCT_AUTO) {
    horizonal = estimatedFieldOfViewH >= estimatesFieldOfViewV;
  }

  std::vector<TMat> rMats(rStitchOrder.size());
  size_t i = 0;
  for(auto *stitchInfo : rStitchOrder) {
    stitchInfo->matR.convertTo(rMats[i], CV_32F);
    ++i;
  }
  
  // for(auto *stitchInfo : rStitchOrder) {
  //   LogUtils::logMat("rb", stitchInfo->matR);
  // }

  WarperHelper::waveCorrect(rMats, horizonal);

  i = 0;
  for(auto *stitchInfo : rStitchOrder) {
    rMats[i].convertTo(stitchInfo->matR, CV_64F);
    ++i;
  }

  // for(auto *stitchInfo : rStitchOrder) {
  //   LogUtils::logMat("ra", stitchInfo->matR);
  // }

}

const MultiStitcher::TStitchOrder &
MultiStitcher::getStitchOrder() const
{
  FUNCLOGTIMEL("MultiStitcher::getStitchOrder");
  return stitchOrder;
}

StitchedImage::StitchedInfo::StitchedInfo()
{
  FUNCLOGTIMEL("MultiStitcher::StitchedInfo");

  TMat eye = TMat::eye(3, 3, CV_64F);
  eye.copyTo(homography);
  eye.copyTo(rMat);
  eye.copyTo(kMat);
  ty = ty = tlCorner.y = tlCorner.y = 0.0;
}

void
StitchedImage::init(
  const TMat &inImage,
  int inProjType,
  //double inFieldOfView,
  std::vector<const StitchInfo *> &stitchOrder,
  //size_t startImageIndex,
  //size_t imagesN,
  //TConstMat &rotMat,
  //TConstMat &kMat,
  double *globalScale)
{
  FUNCLOGTIMEL("StitchedImage::init");
  auto startIndex = stitchOrder[0]->srcImageIndex;
  projType = inProjType;
  imageIndices.resize(1);
  imageIndices[0] = startIndex;

  stitchedInfos.clear();
  for(const auto *stitchInfo : stitchOrder) {
    stitchedInfos.insert(std::make_pair(stitchInfo->srcImageIndex, StitchedInfo()));
  }

  auto &stitchedInfo = stitchedInfos[startIndex];
  stitchOrder[0]->matR.copyTo(stitchedInfo.rMat);
  stitchOrder[0]->matK.copyTo(stitchedInfo.kMat);

  if(projType != eStitch_projectionTypeNone) {
    WarperHelper::warpImage(projType, inImage, stitchedInfo.warpedImage, stitchedInfo.kMat, stitchedInfo.rMat, globalScale);
  }
  else {
    inImage.copyTo(stitchedInfo.warpedImage);
  }

  createMaskFor(startIndex);
  imageSize = stitchedInfo.warpedImage.size();
}

void
StitchedImage::addTranslation(double tx, double ty)
{
  FUNCLOGTIMEL("StitchedImage::addTranslation");

  for(auto i : imageIndices) {

    auto &stitchedInfo = stitchedInfos[i];
    stitchedInfo.tx += tx;
    stitchedInfo.ty += ty;
  }
}

TPoints2f
StitchedImage::getTransformedPts(
  TConstPoints2f &pts,
  const cv::Size &size,
  size_t inIndex,
  double *globalScale) const
{
  FUNCLOGTIMEL("StitchedImage::getTransformedPts");

  if(std::find_if(imageIndices.begin(), imageIndices.end(), [&](const size_t &index) {
    return index == inIndex;
  }) == imageIndices.end()) {
    LogUtils::getLog() << "error index: " << inIndex << std::endl;
    throw std::logic_error("Stitched image does not contain an image with the given index");
  }

  TPoints2f ptsWarped(pts.begin(), pts.end());
  auto it = stitchedInfos.find(inIndex);
  const StitchedInfo &stitchedInfo = it->second;

  WarperHelper::warpPoints(
    size.width,
    size.height,
    stitchedInfo.kMat,
    stitchedInfo.rMat,
    projType,
    ptsWarped,
    globalScale);

  ptsWarped = WarperHelper::transformPtsf(ptsWarped, stitchedInfo.homography);

  for(auto &pt : ptsWarped) {
    pt.x += stitchedInfo.tx;
    pt.y += stitchedInfo.ty;
  }

  return ptsWarped;
}

void StitchedImage::createMaskFor(size_t imageIndex)
{
  FUNCLOGTIMEL("StitchedImage::createMaskFor");

  auto &stitchedInfo = stitchedInfos[imageIndex];

  stitchedInfo.warpedMask = TMat::zeros(stitchedInfo.warpedImage.size(), CV_8UC1);

  auto itMask = stitchedInfo.warpedMask.begin<uint8_t>();
  auto imageBegin = stitchedInfo.warpedImage.begin<cv::Vec4b>();
  auto imageEnd = stitchedInfo.warpedImage.end<cv::Vec4b>();

  for(auto it = imageBegin; it != imageEnd; ++it, ++itMask) {
    if((*it)[3] == 255) {
      *itMask = (*it)[3];
    }
  }
}

void
StitchedImage::setHomographyFor(size_t imageIndex, TConstMat &h)
{
  auto &stitchedInfo = stitchedInfos[imageIndex];
  h.copyTo(stitchedInfo.homography);
}
void
StitchedImage::setKMatFor(size_t imageIndex, TConstMat &k)
{
  auto &stitchedInfo = stitchedInfos[imageIndex];
  k.copyTo(stitchedInfo.kMat);
}
void
StitchedImage::setRMatFor(size_t imageIndex, TConstMat &r)
{
  auto &stitchedInfo = stitchedInfos[imageIndex];
  r.copyTo(stitchedInfo.rMat);
}
void
StitchedImage::setXTranslationFor(size_t imageIndex, double tx)
{
  auto &stitchedInfo = stitchedInfos[imageIndex];
  stitchedInfo.tx = tx;
}
void
StitchedImage::setYTranslationFor(size_t imageIndex, double ty)
{
  auto &stitchedInfo = stitchedInfos[imageIndex];
  stitchedInfo.ty = ty;
}
void
StitchedImage::setCornerXFor(size_t imageIndex, double x)
{
  auto &stitchedInfo = stitchedInfos[imageIndex];
  stitchedInfo.tlCorner.x = x;
}
void
StitchedImage::setCornerYFor(size_t imageIndex, double y)
{
  auto &stitchedInfo = stitchedInfos[imageIndex];
  stitchedInfo.tlCorner.y = y;
}

TMat&
StitchedImage::warpedImage(size_t imageIndex)
{
  auto &stitchedInfo = stitchedInfos[imageIndex];
  return stitchedInfo.warpedImage;
}

void
StitchedImage::stitch(bool fastAndUgly, bool seamBlend, bool compensateExposure, bool rectify)
{
  FUNCLOGTIMEL("StitchedImage::stitch");

  std::vector<cv::Point> tlCornersI;
  std::vector<TMat> images;
  std::vector<TMat> masks;

  for(auto i : imageIndices) {

    auto &stitchedInfo = stitchedInfos[i];

    tlCornersI.push_back(Point(
      stitchedInfo.tx + stitchedInfo.tlCorner.x,
      stitchedInfo.ty + stitchedInfo.tlCorner.y));
    
    images.push_back(stitchedInfo.warpedImage);
    masks.push_back(stitchedInfo.warpedMask);
  }
  
  TMat tempImage;
  if(fastAndUgly) {
    
    ImageUtils::stitchFast(
      images, masks, tlCornersI, tempImage);
  }
  else {
    ImageUtils::stitch(
      images, masks, tlCornersI, seamBlend,
      compensateExposure, tempImage);
  }

  if(!fastAndUgly) {
    LogUtils::getLogUserInfo() << "Cropping" << std::endl;
  }

  if(!rectify) {
    ImageUtils::crop(tempImage, image);
    return;
  }

  TMat croppedImage;
  ImageUtils::crop(tempImage, croppedImage);

  LogUtils::getLogUserInfo() << "Rectifying" << std::endl;

  if(!ImageUtils::rectify(croppedImage, image)) {
    croppedImage.copyTo(image);
  }
}

StitchInfo::StitchInfo(size_t srcI, size_t dstI)
  : srcImageIndex(srcI)
  , dstImageIndex(dstI)
{
  FUNCLOGTIMEL("StitchInfo::StitchInfo");
  resetCamData();
}

void StitchInfo::resetCamData()
{
  FUNCLOGTIMEL("StitchInfo::resetCamData");

  absH = 0.0;
  absV = 0.0;
  deltaH = 0;
  deltaV = 0;
  TMat eye = TMat::eye(3, 3, CV_64F);
  eye.copyTo(matR);
  eye.copyTo(matK);
}

} // imgalign