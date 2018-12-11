#ifndef IMGALIGN_SETTINGSH
#define IMGALIGN_SETTINGSH


#include <map>
#include <vector>
#include <string>
#include "EnumTypes.h"

namespace imgalign
{
  enum ParamType {
    
    eImageCap = 10,
    eImageCapInput = 11,
    
    eSift_featuresN = 20,
    eSift_octaveLayersN = 21,
    eSift_contrastThresh = 22,
    eSift_edgeThresh = 23,
    eSift_sigma = 24,
    
    eSurf_hessianThresh = 30,
    eSurf_octavesN = 31,
    eSurf_octaveLayersN = 32,
    eSurf_extended = 33,

    eOrb_featuresN = 40,
    eOrb_scale = 41,
    eOrb_levelsN = 42,
    eOrb_edgeThresh = 43,
    eOrb_patchSize = 44,

    eBrisk_thresh = 50,
    eBrisk_octavesN = 51,
    eBrisk_patternScale = 52,

    eKaze_thresh = 53,
    eKaze_octavesN = 54,
    eKaze_octaveLayersN = 55,
    
    eAkaze_thresh = 56,
    eAkaze_octavesN = 57,
    eAkaze_octaveLayersN = 58,

    eDetType_sift = 60,
    eDetType_surf = 61,
    eDetType_orb = 62,
    eDetType_brisk = 63,
    eDetType_kaze = 64,
    eDetType_akaze = 65,

    eDesType_sift = 80,
    eDesType_surf = 81,
    eDesType_orb = 82,
    eDesType_brisk = 83,
    eDesType_freak = 84,
    eDesType_kaze = 85,
    eDesType_akaze = 86,

    eMatcherType_flann = 100,
    eMatcherType_bfhamming = 101,
    eMatcherType_bfhamming2 = 102,
    eMatcherType_auto = 103,

    eTransformFinderType_ransac = 120,
    eTransformFinderType_rho = 121,
    eTransformFinderType_lmeds = 122,

    eCompareImageTypeFixed = 130,
    eCompareImageTypeMoving = 131,

    eMatchFilterSpreadAuto = 140,
    eMatchFilterSpreadFactor = 141,
    eMatchFilterMinMatchesToRetain = 142,
    eMatchFilterMaxMatchesToRetain = 143,

    eFloodFillTolerance = 150,
    eAlignSelectionOverlay = 151,
    eLogInfoEnabled = 152,
    eLogErrorEnabled = 153,
    eLogAssertEnabled = 154,
    eLogExternEnabled = 155,

    eDetType = 200,
    eDesType = 201,
    eMatcherType = 202,
    eTransformFinderType = 203,

    eCompareDetType = 301,
    eCompareImageType = 302,

  };

  class Settings {
    public:
      struct Param {
        float value;
        std::string name;
      };

    public:
      Settings();

      DetType getDetType() const;
      DesType getDesType() const;
      MatcherType getMatcherType() const;
      TransformFinderType getTransformFinderType() const;
      
      float getValue(ParamType paramType) const;
      void setValues(
        const std::vector<int> &paramTypes,
        const std::vector<float> &paramValues);

      void logParams() const;

      static DetType getDetType(ParamType eDetType);

    private:
      std::map<ParamType, Param> paramValuesExt;

  };

} // imgalign

#endif