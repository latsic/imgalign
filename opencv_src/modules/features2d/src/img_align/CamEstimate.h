#ifndef IMGALIGN_CAMESTIMATEH
#define IMGALIGN_CAMESTIMATEH

#include "CommonTypes.h"
#include "EnumTypes.h"
#include "MultiStitcher.h"
#include "../precomp.hpp"

namespace imgalign {

class CamEstimate {

  public:

    CamEstimate(
      const std::vector<TKeyPoints> &keyPoints,
      const std::vector<TMat> &descriptors,
      const std::vector<cv::Size> &imageSizes,
      const std::vector<double> &fieldOfViews,
      const std::vector<const StitchInfo *> &rStitchInfos,
      double confidenceThresh,
      double confidenceThreshMax);
    
    bool run(
      int iterationNr,
      const std::vector<const StitchInfo *> &rStitchOrder);

  private:

    const std::vector<TKeyPoints> &_keyPoints;
    const std::vector<TMat> &_descriptors;
    const std::vector<cv::Size> &_imageSizes;
    const std::vector<double> &_fieldOfViews;
    const std::vector<const StitchInfo *> &_rStitchInfos;
    double _confidenceThresh;
    double _confidenceThreshMax;

    // std::vector<int> *pImageIndexBlacklist = nullptr;
};

}

#endif