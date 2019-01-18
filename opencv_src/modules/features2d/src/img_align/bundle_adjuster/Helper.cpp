
#include "Helper.h"
#include "../MultiStitcher.h"
#include "../LogUtils.h"
#include <map>

namespace imgalign
{
namespace bundle
{

void Helper::getData(
    const std::vector<TKeyPoints> &keyPoints,
    const std::vector<TMat> &descriptors,
    const std::vector<cv::Size> &imageSizes,
    const std::vector<double> &fieldOfViews,
    const std::vector<const StitchInfo *> &stitchInfos,
    const StitchInfo *stitchInfoFirstLast,
    std::vector<MatchesInfo> &matchesInfoV,
    std::vector<CameraParams> &cameraParamsV,
    std::vector<ImageFeatures> &imageFeaturesV)
{
  FUNCLOGTIMEL("Helper::getData");

  matchesInfoV.clear();
  cameraParamsV.clear();
  imageFeaturesV.clear();

  std::map<size_t, size_t> srcIndexMap;
  for(size_t i = 0; i < stitchInfos.size(); ++i) {
    size_t srcImageIndex = stitchInfos[i]->srcImageIndex;
    srcIndexMap.insert(std::make_pair(i, srcImageIndex));
  }

  for(const auto *stitchInfo : stitchInfos) {
    
    size_t srcIndex = stitchInfo->srcImageIndex;

    float focalLengthPx = (imageSizes[srcIndex].width * 0.5) /
      tan(fieldOfViews[srcIndex] * 0.5 * CV_PI / 180);

    CameraParams camParams(focalLengthPx);
    camParams.ppx = imageSizes[srcIndex].width * 0.5;
    camParams.ppy = imageSizes[srcIndex].height * 0.5;
    TMat eye = TMat::eye(3, 3, CV_64F);
    eye.copyTo(camParams.R);

    cameraParamsV.push_back(std::move(camParams));
  }
 
  for(size_t i = 0; i < stitchInfos.size(); ++i) {
    
    size_t srcIndex = stitchInfos[i]->srcImageIndex;
    
    ImageFeatures imgF;

    imgF.img_idx = i;
    imgF.img_size = imageSizes[srcIndex];
    imgF.keypoints.assign(keyPoints[srcIndex].begin(), keyPoints[srcIndex].end());
    descriptors[srcIndex].copyTo(imgF.descriptors);
    imageFeaturesV.push_back(std::move(imgF));
  }

  for(size_t i = 0; i < stitchInfos.size(); ++i) {
    for(size_t j = 0; j < stitchInfos.size(); ++j) {  
      MatchesInfo m;
      
      size_t srcIndex = srcIndexMap[j];
      size_t dstIndex = srcIndexMap[i];
      m.confidence = 0.0;

      TMat(0, 0, CV_64F).copyTo(m.H);

      if(srcIndex == dstIndex) {
        matchesInfoV.push_back(std::move(m));
        continue;
      }

      auto applyMatchInfo = [&](const MatchInfo &matchInfo) {

        m.confidence = 1.1;
        const auto &inliers = matchInfo.inlierMatchInfos;

        for(auto itM = inliers.begin(); itM != inliers.end(); ++itM) {
          m.matches.push_back(std::get<1>(*itM));
          m.inliers_mask.push_back(255);
        }
        m.num_inliers = (int)inliers.size();

        TMat h;
        matchInfo.homography.convertTo(h, CV_64F);

        h.copyTo(m.H);
      };

      auto applyInfoIf = [&](const StitchInfo *stitchInfo) {

        if(stitchInfo->srcImageIndex == srcIndex && stitchInfo->dstImageIndex == dstIndex) {
          applyMatchInfo(stitchInfo->matchInfo);
        }
        else if(stitchInfo->srcImageIndex == dstIndex && stitchInfo->dstImageIndex == srcIndex) {
          applyMatchInfo(stitchInfo->matchInfoInverse);
        }
      };

      // if(stitchInfoFirstLast != nullptr) {
      //   applyInfoIf(stitchInfoFirstLast);
      // }

      for(auto it = stitchInfos.begin(); it != stitchInfos.end(); ++it) {

        applyInfoIf(*it);
        
        // //if((*it)->srcImageIndex == j && (*it)->dstImageIndex == i) {
        // if((*it)->srcImageIndex == srcIndex && (*it)->dstImageIndex == dstIndex) {
          
        //   //LogUtils::getLog() << "Helper::getData " << "src " << j << " dst " << i << std::endl;

        //   m.confidence = 1.1;
        //   const auto &inliers = (*it)->matchInfo.inlierMatchInfos;

        //   for(auto itM = inliers.begin(); itM != inliers.end(); ++itM) {
        //     m.matches.push_back(std::get<1>(*itM));
        //     m.inliers_mask.push_back(255);
        //   }
        //   m.num_inliers = (int)inliers.size();

        //   TMat h;
        //   (*it)->matchInfo.homography.convertTo(h, CV_64F);

        //   h.copyTo(m.H);
        // }
        // else if((*it)->srcImageIndex == dstIndex && (*it)->dstImageIndex == srcIndex) {
          
        //   //LogUtils::getLog() << "Helper::getData " << "src " << i << " dst " << j << std::endl;

        //   m.confidence = 1.1;
        //   const auto &inliers = (*it)->matchInfoInverse.inlierMatchInfos;

        //   for(auto itM = inliers.begin(); itM != inliers.end(); ++itM) {
        //     m.matches.push_back(std::get<1>(*itM));
        //     m.inliers_mask.push_back(255);
        //   }
        //   m.num_inliers = (int)inliers.size();

        //   TMat h;
        //   (*it)->matchInfoInverse.homography.convertTo(h, CV_64F);

        //   h.copyTo(m.H);
        // }
      }

      matchesInfoV.push_back(std::move(m));      
    }
  }

}

}
}