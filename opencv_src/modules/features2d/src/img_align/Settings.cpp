
#include "Settings.h"

#include <numeric>
#include "EnumTypes.h"
#include "LogUtils.h"

namespace imgalign
{
  Settings::Settings()
  {
    init();
  }

  void Settings::init() {
    paramValuesExt = {

      { eImageCap, { 250000.0f, "imageCap" } },
      { eImageCapInput, { 1000000.0f, "imageCapInput" } },

      { eSift_featuresN, { 0.0f, "Sift_featuresN" } },
      { eSift_octaveLayersN, { 3.0f, "Sift_octaveLayersN" } },
      { eSift_contrastThresh, { 0.04f, "Sift_contrastThresh" } },
      { eSift_edgeThresh, { 10.0f, "Sift_edgeThresh" } },
      { eSift_sigma, { 1.6f, "Sift_sigma" } },

      { eSurf_hessianThresh, { 100.0f, "Surf_hessianThresh" } },
      { eSurf_octavesN, { 4.0f, "Surf_octavesN" } },
      { eSurf_octaveLayersN, { 3.0f, "Surf_octaveLayersN" } },
      { eSurf_extended, { 0.0f, "Surf_extended" } },

      { eOrb_featuresN, { 500.0f, "Orb_featuresN" } },
      { eOrb_scale, { 1.2f, "Orb_scale" } },
      { eOrb_levelsN, { 8.0f, "Orb_levelsN" } },
      { eOrb_edgeThresh, { 31.0f, "Orb_edgeThresh" } },
      { eOrb_patchSize, { 31.0f, "Orb_patchSize" } },

      { eBrisk_thresh, { 30.0f, "Brisk_thresh" } },
      { eBrisk_octavesN, { 3.0f, "Brisk_octavesN" } },
      { eBrisk_patternScale, { 1.0f, "Brisk_patternScale" } },

      { eKaze_thresh, { 0.001f, "Kaze_thresh" } },
      { eKaze_octavesN, { 4.0f , "Kaze_octavesN" } },
      { eKaze_octaveLayersN, { 4.0f, "Kaze_octaveLayersN" } },
      
      { eAkaze_thresh, { 0.001f, "Akaze_thresh" } },
      { eAkaze_octavesN, { 4.0f, "Akaze_octavesN" } },
      { eAkaze_octaveLayersN, { 4.0f, "Akaze_octaveLayersN" } },

      { eMatchFilterSpreadAuto, { 1.0f, "MatchFilterSpreadAuto" } },
      { eMatchFilterSpreadFactor, { 2.2f, "MatchFilterSpreadFactor" } },
      { eMatchFilterMinMatchesToRetain, { 30.0f, "MatchFilterMinMatchesToRetain" } },
      { eMatchFilterMaxMatchesToRetain, { 300.0f, "MatchFilterMaxMatchesToRetain" } },

      { eFloodFillTolerance, { 10.0f, "FloodFillTolerance" } },
      { eAlignSelectionOverlay, { (float)0.0f, "AlignSelectionOverlay" } },
      { eLogInfoEnabled, { (float)0.0f, "LogInfoEnabled" } },
      { eLogErrorEnabled, { (float)1.0f, "LogErrorEnabled" } },
      { eLogAssertEnabled, { (float)1.0f, "LogAssertEnabled" } },
      { eLogExternEnabled, { (float)0.0f, "LogExternEnabled" } },

      { eDetType, { (float)eDetType_sift, "DetType" } },
      { eDesType, { (float)eDesType_sift, "DesType" } },
      { eMatcherType, { (float)eMatcherType_auto, "MatcherType" } },
      { eTransformFinderType, { (float)eTransformFinderType_ransac, "TransformFinderType" } },

      { eStitch_projection, { (float)eStitch_projectionTypeSpherical, "Stitch_projection" } },
      { eStitch_projection2, { (float)eStitch_projectionTypeSpherical, "Stitch_projection2" } },
      { eStitch_seamBlend, { 1.0f, "Stitch_seamBlend" } },
      { eStitch_colorTransfer, { 1.0f, "Stitch_colorTransfer" } },
      
      { eStitch_viewAngle1, { 45.0f, "Stitch_viewAngle1" } },
      { eStitch_yaw1, { 0.0f, "Stitch_yaw1" } },
      { eStitch_pitch1, { 0.0f, "Stitch_pitch1" } },
      
      { eStitch_viewAngle2, { 45.0f, "viewAngle2" } },
      { eStitch_yaw2, { 0.0f, "Stitch_yaw2" } },
      { eStitch_pitch2, { 0.0f, "Stitch_pitch2" } },

      { eStitch_yaw2Auto, { 1.0f, "Yaw 2 auto" } },
      { eStitch_pitch2Auto, { 1.0f, "Pitch 2 auto" } },

      { eMultiStitch_projection, { (float)eStitch_projectionTypeSpherical, "MultiStitch_projection" } },
      { eMultiStitch_rectify, { 1.0f, "MultiStitch_rectify" } },
      { eMultiStitch_camEstimate, { 1.0, "eMultiStitch_camEstimate" } },
      { eMultiStitch_bundleAdjustType, { (float)eBundleAdjustType_reproj, "MultiStitch_bundleAdjustType" } },
      { eMultiStitch_waveCorrection, { 0.0f, "MultiStitch_waveCorrection" } },
      { eMultiStitch_seamBlend, { 1.0f, "MultiStitch_seamBlend" } },
      { eMultiStitch_colorTransfer, { 0.0f, "MultiStitch_colorTransfer" } },
      { eMultiStitch_calcImageOrder, { 1.0f, "MultiStitch_calcImageOrder" } },
      { eMultiStitch_calcCenterImage, { 1.0f, "MultiStitch_calcCenterImage" } },
      { eMultiStitch_confidenceThresh, { 0.2f, "MultiStitch_confidenceThresh" } },
      { eMultiStitch_exposureCompensator, { 0.0f, "MultiStitch_exposureCompensator"} }
    };
  }

  DetType Settings::getDetType(ParamType eDetType)
  {
    switch(eDetType) {
      case eDetType_sift: return DetType::DET_SIFT;
      case eDetType_surf: return DetType::DET_SURF;
      case eDetType_orb: return DetType::DET_ORB;
      case eDetType_brisk: return DetType::DET_BRISK;
      case eDetType_kaze: return DetType::DET_KAZE;
      case eDetType_akaze: return DetType::DET_AKAZE;
      default: return DetType::DET_SIFT;
    }
  }

  DetType Settings::getDetType() const
  {
    return Settings::getDetType((ParamType)((int)getValue(eDetType)));
  }
  DesType Settings::getDesType() const
  {
    switch((int)getValue(eDesType)) {
      case eDesType_sift: return DesType::DES_SIFT;
      case eDesType_surf: return DesType::DES_SURF;
      case eDesType_orb: return DesType::DES_ORB;
      case eDesType_brisk: return DesType::DES_BRISK;
      case eDesType_freak: return DesType::DES_FREAK;
      case eDesType_kaze: return DesType::DES_KAZE;
      case eDesType_akaze: return DesType::DES_AKAZE;
      default: return DesType::DES_SIFT;
    }
  }
  MatcherType Settings::getMatcherType() const
  {
    switch((int)getValue(eMatcherType)) {
      case eMatcherType_flann: return MatcherType::FLANN;
      case eMatcherType_bfhamming: return MatcherType::BF;
      case eMatcherType_bfhamming2: return MatcherType::BF2;
      case eMatcherType_auto: return MatcherType::AUTO;
      default: return MatcherType::AUTO;
    }
  }
  TransformFinderType Settings::getTransformFinderType() const
  {
    switch((int)getValue(eTransformFinderType)) {
      case eTransformFinderType_ransac: return TransformFinderType::TFT_RANSAC;
      case eTransformFinderType_rho: return TransformFinderType::TFT_RHO;
      case eTransformFinderType_lmeds: return TransformFinderType::TFT_LMEDS;
      default: return TransformFinderType::TFT_RANSAC;
    }
  }

  BundleAdjustType Settings::getBundleAdjustType() const
  {
    switch((int)getValue(eMultiStitch_bundleAdjustType)) {
      case eBundleAdjustType_none: return BundleAdjustType::BAT_NONE;
      case eBundleAdjustType_ray: return BundleAdjustType::BAT_RAY;
      case eBundleAdjustType_reproj: return BundleAdjustType::BAT_REPROJ;
      default: return BundleAdjustType::BAT_REPROJ;
    }
  }

  float Settings::getValue(ParamType type) const
  {
    auto it = paramValuesExt.find(type);
    if(it == paramValuesExt.end())
    {
      throw std::logic_error("Params::getValue: unknown parameter");
    }
    return it->second.value;
  }

  void Settings::setValues(
    const std::vector<int> &paramTypes,
    const std::vector<float> &_paramValues)
  {
    if(paramTypes.size() != _paramValues.size())
    {
      throw std::logic_error("Settings::setValues: count of types and values dont match");
    }

    init();

    for(size_t i = 0; i < paramTypes.size(); ++i) {

      if(  (ParamType)paramTypes[i] == eCompareDetType
        || (ParamType)paramTypes[i] == eCompareImageType
        || (ParamType)paramTypes[i] == eFloodFillTolerance
        || (ParamType)paramTypes[i] == eAlignSelectionOverlay
        || (ParamType)paramTypes[i] == eImageCapInput)
      {
        continue;
      }

      auto it = this->paramValuesExt.find((ParamType)paramTypes[i]);
      if(it == this->paramValuesExt.end()) {
        throw std::logic_error(
            "Settings::setvalues: param with id "
          + std::to_string(paramTypes[i])
          + " not found");
      } 

      this->paramValuesExt[(ParamType)paramTypes[i]].value = _paramValues[i];
    }
  }

  void Settings::logParams() const
  {
    for(const auto &p : paramValuesExt) {
      LogUtils::getLog() << p.second.name << ": " << p.second.value << std::endl; 
    }
  }
}

