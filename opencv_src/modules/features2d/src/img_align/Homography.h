#ifndef IMGALIGN_HOMOGRAPHYH
#define IMGALIGN_HOMOGRAPHYH

#include "CommonTypes.h"
#include "EnumTypes.h"


namespace imgalign
{
  
  class Homography
  {
    public:

      static bool getHomography(
        TConstPoints2f& inDstPoints,
        TConstPoints2f& inSrcPoints,
        TMat& outTransform);

      static bool getHomography(
        TransformFinderType tfType,
        TConstPoints2f& inDstPoints,
        TConstPoints2f& inSrcPoints,
        TBools& outMask,
        TMat& outTransform,
        double ransacThresh = 3.0,
        double ransacConfidence = 0.995);

      static bool getHomography(
        TConstPoints2f &pts1, TConstPoints2f &pts2,
        const cv::Size &s1, const cv::Size &s2,
        TConstMat *pMatK1, TConstMat *pMatK2,
        double fieldOfView1, double fieldOfView2,
        TConstMat &rotMat1,
        TConstMat &rotMat2,
        int projType1, int projType2,
        TransformFinderType tfType,
        TMat &outHomography,
        double *globalScale = nullptr,
        TPoints2f *warpedPts1 = nullptr,
        TPoints2f *warpedPts2 = nullptr);

      static double determinant(TConstMat &homography);

      static bool isSvdConditionNumberSane(
        TConstMat &homography,
        double &conditionNumber,
        double threshHold = 10e7);
  };


} // imgalign

#endif
