#ifndef IMGALIGN_BUNDLE_HELPERH
#define IMGALIGN_BUNDLE_HELPERH

#include "../CommonTypes.h"
#include <vector>
#include "../../precomp.hpp"

#include "MatchesInfo.h"
#include "CameraParams.h"
#include "ImageFeatures.h"

namespace imgalign
{
struct MatchInfo;
struct StitchInfo;

namespace bundle
{
  class Helper {
  
    public:

  // MatchesInfo getMatchesInfo(const MatchInfo &matchInfo, int srcI, int dstI);
  // CameraParams getCameraParams(double focal = 1.0);
  // ImageFeatures getImageFeatures();

    static void getData(
      const std::vector<TKeyPoints> &keyPoints,
      const std::vector<TMat> &descriptors,
      const std::vector<cv::Size> &imageSizes,
      const std::vector<double> &fieldOfViews,
      const std::vector<const StitchInfo *> &rStitchOrder,
      const std::vector<const StitchInfo *> &rStitchInfos,
      double confidenceThreshCam,
      std::vector<MatchesInfo> &matchesInfoV,
      std::vector<CameraParams> &cameraParamsV,
      std::vector<ImageFeatures> &imageFeaturesV);

    static bool getMatchInfo(
      size_t srcImageIndex, size_t dstImageIndex,
      double confidenceThresh,
      double sumDeltaHV,
      const MatchInfo &matchInfo, MatchesInfo &outMatchesInfo);
  };

}
}

#endif