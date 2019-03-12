
#include "Helper.h"
#include "../MultiStitcher.h"
#include "../LogUtils.h"
#include "../StitchInfoFilter.h"
#include <map>

namespace imgalign
{
namespace bundle
{

bool Helper::getMatchInfo(
  size_t srcImageIndex, size_t dstImageIndex,
  const StitchInfoFilter &stitchInfoFilter,
  double sumDeltaHV,
  const StitchInfo &stitchInfo, MatchesInfo &outMatchesInfo)
{
  FUNCLOGTIMEL("Helper::getMatchInfo");

  auto &m = outMatchesInfo;

  m.confidence = m.sumDeltaHV = 0.0;
  m.num_inliers = m.num_filtered = m.num_all = m.num_outlier = 0;

  m.src_img_idx = srcImageIndex;
  m.dst_img_idx = dstImageIndex;

  if(stitchInfoFilter.pass(stitchInfo)) {
    
    const MatchInfo &matchInfo = stitchInfo.matchInfo;
    const auto &inliers = matchInfo.inlierMatchInfos;

    m.confidence = matchInfo.confidence;
    m.confidence = m.confidence > 10 ? 0.0 : m.confidence;

    m.sumDeltaHV = sumDeltaHV;

    for(auto itM = inliers.begin(); itM != inliers.end(); ++itM) {
      m.matches.push_back(std::get<1>(*itM));
      m.inliers_mask.push_back(255);
    }

    m.num_inliers = (int)inliers.size();
    m.num_filtered = (int)matchInfo.filteredMatchesCount;
    m.num_all = (int)matchInfo.allMatchesCount;
    m.num_outlier = (int)matchInfo.outlierMatchesCount;

    matchInfo.homography.convertTo(m.H, CV_64F);
    return true;

  }
  return false;
}

void Helper::getData(
    const std::vector<TKeyPoints> &keyPoints,
    const std::vector<TMat> &descriptors,
    const std::vector<cv::Size> &imageSizes,
    const std::vector<double> &fieldOfViews,
    const std::vector<const StitchInfo *> &rStitchOrder,
    std::vector<CameraParams> &cameraParamsV,
    std::vector<ImageFeatures> &imageFeaturesV)
{
  FUNCLOGTIMEL("Helper::getData");

  cameraParamsV.clear();
  imageFeaturesV.clear();

  for(const auto *stitchInfo : rStitchOrder) {
    
    size_t srcImageIndex = stitchInfo->srcImageIndex;

    float focalLengthPx = (imageSizes[srcImageIndex].width * 0.5) /
      tan(fieldOfViews[srcImageIndex] * 0.5 * CV_PI / 180);

    CameraParams camParams(focalLengthPx);
    camParams.ppx = imageSizes[srcImageIndex].width * 0.5;
    camParams.ppy = imageSizes[srcImageIndex].height * 0.5;
    TMat eye = TMat::eye(3, 3, CV_64F);
    eye.copyTo(camParams.R);

    cameraParamsV.push_back(std::move(camParams));
  }
  
  for(size_t i = 0; i < rStitchOrder.size(); ++i) {
    
    size_t srcImageIndex = rStitchOrder[i]->srcImageIndex;
    
    ImageFeatures imgF;

    imgF.img_idx = i;
    imgF.img_size = imageSizes[srcImageIndex];
    imgF.keypoints.assign(keyPoints[srcImageIndex].begin(), keyPoints[srcImageIndex].end());
    //descriptors[srcImageIndex].copyTo(imgF.descriptors);

    imageFeaturesV.push_back(std::move(imgF));
  }
  if(!descriptors.empty()) {
    for(size_t i = 0; i < rStitchOrder.size(); ++i) {
      size_t srcImageIndex = rStitchOrder[i]->srcImageIndex;
      descriptors[srcImageIndex].copyTo(imageFeaturesV[i].descriptors);
    }
  }


}

void Helper::getMatchesInfo(
  const std::vector<const StitchInfo *> &rStitchOrder,
  const std::vector<const StitchInfo *> &rStitchInfos,
  const StitchInfoFilter &stitchInfoFilter,
  std::vector<MatchesInfo> &matchesInfoV)
{
  FUNCLOGTIMEL("Helper::getMatchesInfo");

  matchesInfoV.clear();

  std::map<size_t, size_t> arrayIndexToImageIndex;
  std::map<size_t, size_t> imageIndexToArrayIndex;
  int imagesToStitchN = rStitchOrder.size();
  for(size_t i = 0; i < rStitchOrder.size(); ++i) {
    size_t srcImageIndex = rStitchOrder[i]->srcImageIndex;
    arrayIndexToImageIndex.insert(std::make_pair(i, srcImageIndex));
    imageIndexToArrayIndex.insert(std::make_pair(srcImageIndex, i));
  }

  std::string baStitchInfos;
  std::stringstream sStream;
  sStream << "BA tiles: ";

  matchesInfoV.resize(imagesToStitchN * imagesToStitchN);
  for(const auto *stitchInfo : rStitchInfos) {

    size_t srcImageIndex = stitchInfo->srcImageIndex;
    size_t dstImageIndex = stitchInfo->dstImageIndex;

    if(imageIndexToArrayIndex.find(srcImageIndex) == imageIndexToArrayIndex.end()) continue;
    if(imageIndexToArrayIndex.find(dstImageIndex) == imageIndexToArrayIndex.end()) continue;

    size_t arrayIndexSrc = imageIndexToArrayIndex[dstImageIndex];
    size_t arrayIndexDst = imageIndexToArrayIndex[srcImageIndex];

    MatchesInfo m;

    double sumDeltaHV = std::abs(stitchInfo->deltaH) + std::abs(stitchInfo->deltaV);

    if(getMatchInfo(srcImageIndex, dstImageIndex, stitchInfoFilter,
        sumDeltaHV, *stitchInfo, m)) {

      sStream << srcImageIndex << "->" << dstImageIndex << "("
        << arrayIndexSrc << "->" <<  arrayIndexDst << ") "
        << stitchInfo->matchInfo.confidence << " " << sumDeltaHV << ", ";
    }

    matchesInfoV[arrayIndexSrc * imagesToStitchN + arrayIndexDst] = std::move(m);
  }

  LogUtils::getLogUserInfo() << sStream.str() << std::endl;

}


}
}