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

    eStitch_projection = 303,
    eStitch_projection2 = 304,
    eStitch_seamBlend = 501,
    eStitch_colorTransfer = 502,
    eStitch_viewAngle1 = 511,
    eStitch_yaw1 = 513,
    eStitch_pitch1 = 514,
    eStitch_viewAngle2 = 521,
    eStitch_yaw2 = 523,
    eStitch_pitch2 = 524,

    eStitch_yaw2Auto = 525,
    eStitch_pitch2Auto = 526,

    eStitch_projectionTypePlane = 550,
    eStitch_projectionTypeAffine = 551,
    eStitch_projectionTypeCylindrical = 552,
    eStitch_projectionTypeSpherical = 553,
    eStitch_projectionTypeFisheye = 554,
    eStitch_projectionTypeStereographic = 555,
    eStitch_projectionTypeRectilinear = 556,
    eStitch_projectionTypeRectilinearPortrait = 557,
    eStitch_projectionTypePanini = 558,
    eStitch_projectionTypeMercator = 559,
    eStitch_projectionTypeTransverseMercator = 560,
    eStitch_projectionTypeNone = 561,
    eStitch_projectionTypeRectilinearA2B1 = 562,
    eStitch_projectionTypeRectilinearPortraitA2B1 = 563,
    eStitch_projectionTypePaniniA2B1 = 564,
    eStitch_projectionTypePaniniPortraitA2B1 = 565,

    eBundleAdjustType_none = 570,
    eBundleAdjustType_ray = 571,
    eBundleAdjustType_reproj = 572,
    eBundleAdjustType_auto = 573,
    eBundleAdjustType_rayretry = 569,
    eBundleAdjustType_ray2 = 568,
    eBundleAdjustType_reproj2 = 567,
    eBundleAdjustType_reprojcap = 566,
    eBundleAdjustType_ray3 = 602,

    eBlendType_none = 574,
    eBlendType_multiBand = 575,
    eBlendType_feather = 576,

    eSeamFinderType_Vornoi = 578,
    eSeamFinderType_Graphcut = 579,

    eMultiStitch_seamFinderType = 577,
    eMultiStitch_projection = 305,
    eMultiStitch_rectifyPerspective = 581,
    eMultiStitch_camEstimate = 582,
    eMultiStitch_bundleAdjustType = 583,
    eMultiStitch_waveCorrection = 584,
    eMultiStitch_seamBlend = 585,
    eMultiStitch_colorTransfer = 586,
    eMultiStitch_calcImageOrder = 587,
    eMultiStitch_calcCenterImage = 588,
    eMultiStitch_confidenceThresh = 589,
    eMultiStitch_exposureCompensator = 590,
    //eMultiStitch_blendType = 591,
    eMultiStitch_blendStrength = 592,
    eMultiStitch_rectifyStretch = 593,
    eMultiStitch_warpFirst = 580,

    eMultiStitch_confidenceThreshCam = 594,
    eMultiStitch_confidenceThreshCamManual = 595,

    eMultiStitch_limitResultPreview = 596,
    eMultiStitch_limitInputView = 597,
    eMultiStitch_disposeInputImages = 598,
    eMultiStitch_limitLiveStitchingPreview = 599,
    eMultiStitch_preserveAlphaChannelValue = 600,
    eMultiStitch_liveUpdateCycle = 601
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
      BundleAdjustType getBundleAdjustType() const;
      // BlendType getBlendType() const;
      SeamFinderType getSeamFinderType() const;

      
      void init();
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