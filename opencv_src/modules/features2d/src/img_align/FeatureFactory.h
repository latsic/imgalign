#ifndef IMGALIGN_FEATUREFACTORYH
#define IMGALIGN_FEATUREFACTORYH

#include "CommonTypes.h"
#include "EnumTypes.h"

namespace cv
{
  class Feature2D;
  class DescriptorMatcher;
}

namespace imgalign
{
  class Settings;
  class DesMatcher;

  class FeatureFactory
  {
    public:
      static void CreateFeatureDetector(cv::Ptr<cv::Feature2D> &featureDet, const Settings &settings);
      static void CreateDescriptorComputer(cv::Ptr<cv::Feature2D> &featureDes, const Settings &settings);
      static DesMatcher CreateMatcher(const Settings &settings);
      
      static cv::Ptr<cv::Feature2D> createSift(const Settings &settings);
      static cv::Ptr<cv::Feature2D> createSurf(const Settings &settings);
      static cv::Ptr<cv::Feature2D> createOrb(const Settings &settings);
      static cv::Ptr<cv::Feature2D> createBrisk(const Settings &settings);
      static cv::Ptr<cv::Feature2D> createKaze(const Settings &settings);
      static cv::Ptr<cv::Feature2D> createAkaze(const Settings &settings);

      static MatcherType getMatcherType(const Settings &settings);
  };

}

#endif