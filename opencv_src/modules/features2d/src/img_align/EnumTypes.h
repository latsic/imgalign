
#ifndef IMGALIGN_ENUMTYPESH
#define IMGALIGN_ENUMTYPESH

namespace imgalign
{
  enum class DetType{
    DET_BRISK = 0,
    DET_SURF = 1,
    DET_SIFT = 2,
    DET_HARRIS = 3,
    DET_GFTT = 4,
    DET_ORB = 5,
    DET_ORB2 = 6,
    DET_KAZE = 7,
    DET_AKAZE = 8
  };
  enum class DesType{
    DES_BRISK = 0,
    DES_SURF = 1,
    DES_SIFT = 2,
    DES_FREAK = 3,
    DES_ORB = 4,
    DES_ORB2 = 6,
    DES_KAZE = 7,
    DES_AKAZE = 8
  };
  enum class MatcherType{
    FLANN = 0, BF = 1, BF2 = 2, AUTO = 3
  };
  enum class TransformFinderType{
    TFT_RANSAC = 0, TFT_RHO = 1, TFT_LMEDS = 2
  };
  enum class WaveCorrectType{
    WCT_NONE = 0, WCT_H = 1, WCT_V = 2, WCT_AUTO = 3
  };
  enum class BundleAdjustType{
    BAT_NONE = 0, BAT_RAY = 1, BAT_REPROJ = 2
  };
}

#endif