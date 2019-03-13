
#include "FeatureFactory.h"
#include "Settings.h"
#include "DesMatcher.h"
#include "../precomp.hpp"
#include "LogUtils.h"
using namespace cv;

namespace imgalign
{

Ptr<Feature2D> FeatureFactory::createSift(const Settings &settings)
{
  return SIFT::create(
    (int)settings.getValue(eSift_featuresN),
    (int)settings.getValue(eSift_octaveLayersN),
    settings.getValue(eSift_contrastThresh),
    settings.getValue(eSift_edgeThresh),
    settings.getValue(eSift_sigma));
}
Ptr<Feature2D> FeatureFactory::createSurf(const Settings &settings)
{
  return SURF::create(
    settings.getValue(eSurf_hessianThresh),
    (int)settings.getValue(eSurf_octavesN),
    (int)settings.getValue(eSurf_octaveLayersN),
    settings.getValue(eSurf_extended) > 0.0f,
    false);
}
Ptr<Feature2D> FeatureFactory::createOrb(const Settings &settings)
{
  return ORB::create(
    (int)settings.getValue(eOrb_featuresN),
    settings.getValue(eOrb_scale),
    (int)settings.getValue(eOrb_levelsN),
    settings.getValue(eOrb_edgeThresh),
    0, 2, ORB::HARRIS_SCORE,
    settings.getValue(eOrb_patchSize),
    20);
}
Ptr<Feature2D> FeatureFactory::createBrisk(const Settings &settings)
{
  return BRISK::create(
    settings.getValue(eBrisk_thresh),
    (int)settings.getValue(eBrisk_octavesN),
    settings.getValue(eBrisk_patternScale));
}

Ptr<Feature2D> FeatureFactory::createKaze(const Settings &settings)
{
  return KAZE::create(false, false,
    settings.getValue(eKaze_thresh),
    settings.getValue(eKaze_octavesN),
    settings.getValue(eKaze_octaveLayersN),
    KAZE::DIFF_PM_G2);
}

Ptr<Feature2D> FeatureFactory::createAkaze(const Settings &settings)
{
  return AKAZE::create(AKAZE::DESCRIPTOR_MLDB, 0, 3,
    settings.getValue(eAkaze_thresh),
    settings.getValue(eAkaze_octavesN),
    settings.getValue(eAkaze_octaveLayersN),
    KAZE::DIFF_PM_G2);
}

void FeatureFactory::CreateFeatureDetector(Ptr<Feature2D> &featureDet, const Settings &settings)
{
  switch(settings.getDetType()){
		case DetType::DET_BRISK: featureDet = createBrisk(settings); break;
		case DetType::DET_SURF: featureDet = createSurf(settings); break;
		case DetType::DET_SIFT: featureDet = createSift(settings); break;
		case DetType::DET_ORB: featureDet = createOrb(settings); break;
		case DetType::DET_KAZE: featureDet = createKaze(settings); break;
		case DetType::DET_AKAZE: featureDet = createAkaze(settings); break;
		case DetType::DET_ORB2: featureDet = ORB::create(3000, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 41, 20); break;
		case DetType::DET_GFTT: break;
		case DetType::DET_HARRIS:	
		default:{
			throw std::logic_error("not implemented");
		}
	}
}

void FeatureFactory::CreateDescriptorComputer(Ptr<Feature2D> &featureDes, const Settings &settings)
{
  switch(settings.getDesType()){
		case DesType::DES_BRISK: featureDes = createBrisk(settings); break;
		case DesType::DES_SURF:	featureDes = createSurf(settings); break;
		case DesType::DES_SIFT:	featureDes = createSift(settings); break;
		case DesType::DES_FREAK:	featureDes = FREAK::create(); break;
		case DesType::DES_ORB:		featureDes = createOrb(settings); break;
		case DesType::DES_KAZE: featureDes = createKaze(settings); break;
		case DesType::DES_AKAZE: featureDes = createAkaze(settings); break;
		case DesType::DES_ORB2:	featureDes = ORB::create(3000, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 41, 20); break;
		default:{
			throw std::logic_error("not implemented");
		}
	}
}

DesMatcher FeatureFactory::CreateMatcher(
  const Settings &settings,
  TConstMat *inDescriptors, 
  TConstKeyPoints *inKeyPoints)
{
  FUNCLOGTIMEL("FeatureFactory::CreateMatcher");

  if(inDescriptors != nullptr && inKeyPoints == nullptr) {
    throw std::logic_error("Can not create feature matcher, no key points given.");

  }
  if(inDescriptors == nullptr && inKeyPoints != nullptr) {
    throw std::logic_error("Can not create feature matcher, no descriptors given.");
  }

  DesMatcher desMatcher(inDescriptors, inKeyPoints);
  
  switch(getMatcherType(settings)){

		case MatcherType::BF: {
      desMatcher.matcher = DescriptorMatcher::create("BruteForce-Hamming");
      desMatcher.factor = 6.0;
      break;
    }
		case MatcherType::BF2: {
      desMatcher.matcher = DescriptorMatcher::create("BruteForce-Hamming(2)");
      desMatcher.factor = 6.0;
      break;
    }
		case MatcherType::FLANN: {
      desMatcher.matcher = DescriptorMatcher::create("FlannBased");
      desMatcher.factor = 2.2;
      break;
    }
		default:{
			throw std::logic_error("not implemented");
		}
	}

  if(inDescriptors != nullptr) desMatcher.matcher->add(*inDescriptors);

  auto matchFilterSpreadAuto = (bool)settings.getValue(eMatchFilterSpreadAuto);
  auto matchFilterSpreadFactor = settings.getValue(eMatchFilterSpreadFactor);

  desMatcher.matchFilterMinMatchesToRetain = (int)settings.getValue(eMatchFilterMinMatchesToRetain);
  desMatcher.matchFilterMaxMatchesToRetain = (int)settings.getValue(eMatchFilterMaxMatchesToRetain);

  if(!matchFilterSpreadAuto) {
    desMatcher.factor = matchFilterSpreadFactor;
  }
  
  return desMatcher;
}

MatcherType FeatureFactory::getMatcherType(const Settings &settings)
{
  FUNCLOGTIMEL("FeatureFactory::getMatcherType");

  switch(settings.getMatcherType()){
		case MatcherType::AUTO:{
			switch(settings.getDesType()){
				case DesType::DES_KAZE:
				case DesType::DES_SIFT:
				case DesType::DES_SURF: return MatcherType::FLANN; break;
				case DesType::DES_ORB:
				case DesType::DES_BRISK:
				case DesType::DES_AKAZE:
				case DesType::DES_FREAK: return MatcherType::BF; break;
				case DesType::DES_ORB2:  return MatcherType::BF2; break;
			}
		}
		default:{
		}
	}
  return settings.getMatcherType();
  
}


} //imgalign