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
class StitchInfoFilter;

namespace bundle
{
  class Helper {
  
    public:

      static void getData(
        const std::vector<TKeyPoints> &keyPoints,
        const std::vector<TMat> &descriptors,
        const std::vector<cv::Size> &imageSizes,
        const std::vector<double> &fieldOfViews,
        const std::vector<const StitchInfo *> &rStitchOrder,
        std::vector<CameraParams> &cameraParamsV,
        std::vector<ImageFeatures> &imageFeaturesV);

      static void getMatchesInfo(
        const std::vector<const StitchInfo *> &rStitchOrder,
        const std::vector<const StitchInfo *> &rStitchInfos,
        const StitchInfoFilter &stitchInfoFilter,
        std::vector<MatchesInfo> &matchesInfoV);

      static bool getMatchInfo(
        size_t srcImageIndex, size_t dstImageIndex,
        const StitchInfoFilter &stitchInfoFilter,
        double sumDeltaHV,
        const StitchInfo &stitchInfo, MatchesInfo &outMatchesInfo);
  };

}
}

#endif