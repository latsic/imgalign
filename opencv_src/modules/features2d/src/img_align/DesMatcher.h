#ifndef IMGALIGN_DESMATCHERH
#define IMGALIGN_DESMATCHERH

#include "CommonTypes.h"
#include "EnumTypes.h"
#include "../precomp.hpp"

namespace cv
{
  class Feature2D;
  class DescriptorMatcher;
}

namespace imgalign
{ 
  enum class DataExtractionMode {
    eMin = 0,
    eBasic = 1,
    eExtended = 2
  };
  
  struct FilterInfo {
    double minDist, maxDist;
    double maxDistUsed, maxDistFactorUsed;
  };

  struct MatchInfo {
    TMat homography;

    TMatches allMatches;
    TMatches filteredMatches;
    TMatchInfos allMatchInfos;
    TMatchInfos filteredMatchInfos;
    TMatchInfos inlierMatchInfos;
    TMatchInfos outlierMatchInfos;
    
    TPoints2f filteredPts1;
    TPoints2f filteredPts2;

    TPoints2f inlierPts1;
    TPoints2f inlierPts2;

    bool success = false;
    double confidence = 0.0;
    bool preservesOrientation = false;
    double determinant = 0.0;
    double svdConditionNumber = 0.0;
    bool svdConditionNumberIsSane = false;

    FilterInfo filterInfo;

    int allMatchesCount = 0;
    int filteredMatchesCount = 0;
    int inlierMatchesCount = 0;
    int outlierMatchesCount = 0;

    bool isHomographyGood() const;
    MatchInfo getInverse() const;
    void logInfo(bool logH = false) const;

    void dismissDetailData();

    void getInlierPts(TPoints2f &pts1, TPoints2f &pts2) const;
  };

  class DesMatcher {
    public:

      MatchInfo match(
        TransformFinderType tfType,
        TConstMat& inDescriptors1, TConstMat& inDescriptors2,
        TConstKeyPoints &keyPoints1, TConstKeyPoints &keyPoints2,
        DataExtractionMode dataExtractionMode) const;

      void match(TConstMat& inDescriptors1, TConstMat& inDescriptors2, TMatches &outMatches) const;
      void matchFilter(TConstMat& inDescriptors1, TConstMat& inDescriptors2, TMatches &outMatches) const;

      FilterInfo filter(TConstMatches& inMatches, TMatches& outMatches) const;

      double factor = 1.0;
      int matchFilterMinMatchesToRetain = 1;
      int matchFilterMaxMatchesToRetain = 100;
      cv::Ptr<cv::DescriptorMatcher> matcher = nullptr;
  };
}

#endif