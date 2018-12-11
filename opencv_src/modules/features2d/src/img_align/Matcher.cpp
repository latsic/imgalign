
#include "Matcher.h"

#include <exception>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include "opencv2/imgproc/types_c.h"

#include "TimeUtils.h"
#include "LogUtils.h"
#include "Display.h"
#include "Settings.h"

using namespace cv;



Ptr<Feature2D> featureDetFixed;
Ptr<Feature2D> featureDesFixed;
Ptr<Feature2D> featureDetMoving;
Ptr<Feature2D> featureDesMoving;

namespace imgalign
{

void CreateFeatureDetector(Ptr<Feature2D>& featureDet, DetType inDetType);
void CreateDescriptorComputer(Ptr<Feature2D>& featureDes, DesType inDesType);

Ptr<Feature2D> createSift(const Settings &settings);
Ptr<Feature2D> createSurf(const Settings &settings);
Ptr<Feature2D> createOrb(const Settings &settings);
Ptr<Feature2D> createBrisk(const Settings &settings);
Ptr<Feature2D> createKaze(const Settings &settings);
Ptr<Feature2D> createAkaze(const Settings &settings);

void CreateFeatureDetector(Ptr<Feature2D>& featureDet, const Settings &settings);
void CreateDescriptorComputer(Ptr<Feature2D>& featureDes, const Settings &settings);

void sortMatchInfos(TMatchInfos& matchInfos);

Ptr<DescriptorMatcher> matcher;

Matcher::Type::Type(DetType inDetType, DesType inDesType, TransformFinderType inTransformFinderType, MatcherType inMatcherType)
	: detType(inDetType)
	, desType(inDesType)
	, matcherType(inMatcherType)
	, transformFinderType(inTransformFinderType)
{
}



void
CreateFeatureDetector(Ptr<Feature2D>& featureDet, DetType inDetType)
{
	switch(inDetType){
		case DetType::DET_BRISK:	featureDet = BRISK::create(30, 3, 1.0); break;
		case DetType::DET_SURF:		featureDet = SURF::create(100, 4, 3, true, false); break;
		case DetType::DET_SIFT:		featureDet = SIFT::create(0, 3, 0.04, 10.0, 1.6); break;
		case DetType::DET_ORB:		featureDet = ORB::create(3000, 1.2f, 8, 21, 0, 2, ORB::HARRIS_SCORE, 61, 20); break;
		case DetType::DET_ORB2:		featureDet = ORB::create(3000, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 41, 20); break;
		case DetType::DET_KAZE:   featureDet = KAZE::create(false, false, 0.001f, 4, 4, KAZE::DIFF_PM_G2);
		case DetType::DET_AKAZE:  featureDet = AKAZE::create(AKAZE::DESCRIPTOR_MLDB, 0, 3, 0.001f, 4, 4, KAZE::DIFF_PM_G2);
		case DetType::DET_GFTT:   break;
		case DetType::DET_HARRIS:	
		default:{
			throw std::logic_error("not implemented");
		}
	}
}

void
CreateDescriptorComputer(Ptr<Feature2D>& featureDes, DesType inDesType)
{
	switch(inDesType){
		case DesType::DES_BRISK: 	featureDes = BRISK::create(30, 3, 1.0); break;
		case DesType::DES_SURF:		featureDes = SURF::create(100, 4, 3, true, false); break;
		case DesType::DES_SIFT:		featureDes = SIFT::create(0, 3, 0.04, 10.0, 1.6); break;
		case DesType::DES_FREAK:	featureDes = FREAK::create(); break;
		case DesType::DES_ORB:		featureDes = ORB::create(3000, 1.2f, 8, 21, 0, 2, ORB::HARRIS_SCORE, 61, 20); break;
		case DesType::DES_ORB2:		featureDes = ORB::create(3000, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 41, 20); break;
		case DesType::DES_KAZE:   featureDes = KAZE::create(false, false, 0.001f, 4, 4, KAZE::DIFF_PM_G2);
		case DesType::DES_AKAZE:  featureDes = AKAZE::create(AKAZE::DESCRIPTOR_MLDB, 0, 3, 0.001f, 4, 4, KAZE::DIFF_PM_G2);
		default:{
			throw std::logic_error("not implemented");
		}
	}
}

Ptr<Feature2D> createSift(const Settings &settings)
{
	return	SIFT::create(
						(int)settings.getValue(eSift_featuresN),
						(int)settings.getValue(eSift_octaveLayersN),
						settings.getValue(eSift_contrastThresh),
						settings.getValue(eSift_edgeThresh),
						settings.getValue(eSift_sigma));
}
Ptr<Feature2D> createSurf(const Settings &settings)
{
	return	SURF::create(
						settings.getValue(eSurf_hessianThresh),
						(int)settings.getValue(eSurf_octavesN),
						(int)settings.getValue(eSurf_octaveLayersN),
						settings.getValue(eSurf_extended) > 0.0f,
						false);
}
Ptr<Feature2D> createOrb(const Settings &settings)
{
	return	ORB::create(
						(int)settings.getValue(eOrb_featuresN),
						settings.getValue(eOrb_scale),
						(int)settings.getValue(eOrb_levelsN),
						settings.getValue(eOrb_edgeThresh),
						0, 2, ORB::HARRIS_SCORE,
						settings.getValue(eOrb_patchSize),
						20);
}
Ptr<Feature2D> createBrisk(const Settings &settings)
{
	return	BRISK::create(
						settings.getValue(eBrisk_thresh),
						(int)settings.getValue(eBrisk_octavesN),
						settings.getValue(eBrisk_patternScale));
}

Ptr<Feature2D> createKaze(const Settings &settings)
{
	return KAZE::create(false, false,
		settings.getValue(eKaze_thresh),
		settings.getValue(eKaze_octavesN),
    settings.getValue(eKaze_octaveLayersN),
	  KAZE::DIFF_PM_G2);
}

Ptr<Feature2D> createAkaze(const Settings &settings)
{
	return AKAZE::create(AKAZE::DESCRIPTOR_MLDB, 0, 3,
                       settings.getValue(eAkaze_thresh),
											 settings.getValue(eAkaze_octavesN),
                       settings.getValue(eAkaze_octaveLayersN),
											 KAZE::DIFF_PM_G2);
}

void
CreateFeatureDetector(Ptr<Feature2D>& featureDet, const Settings &settings)
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

void
CreateDescriptorComputer(Ptr<Feature2D>& featureDes, const Settings &settings)
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

Matcher::Matcher(
	const cv::Mat& inFixedImage,
	const cv::Mat& inMovingImage,
	double scaleFactorFixedImage,
	double scaleFactorMovingImage,
	DetType inDetType,
	DesType inDesType,
	TransformFinderType inTransformFinderType,
	MatcherType inMatcherType)

	: type(inDetType, inDesType, inTransformFinderType, inMatcherType)
	, ransacThresh(13.0)
	, ransacConfidence(0.995)
	, _scaleFactorFixedImage(scaleFactorFixedImage)
	, _scaleFactorMovingImage(scaleFactorMovingImage)
{
FUNCLOG("Matcher");
	
	cvtColor(inFixedImage, fixedImage, CV_RGB2GRAY);
	cvtColor(inMovingImage, movingImage, CV_RGB2GRAY);

	CreateFeatureDetector(featureDetFixed, inDetType);
	CreateDescriptorComputer(featureDesFixed, inDesType);
	CreateFeatureDetector(featureDetMoving, inDetType);
	CreateDescriptorComputer(featureDesMoving, inDesType);

	switch(getMatcherType()){

		case MatcherType::BF:    matcher = DescriptorMatcher::create("BruteForce-Hamming");  break;
		case MatcherType::BF2:   matcher = DescriptorMatcher::create("BruteForce-Hamming(2)");  break;
		case MatcherType::FLANN: matcher = DescriptorMatcher::create("FlannBased");  break;
		default:{
			throw std::logic_error("not implemented");
		}
	}
}
Matcher::Matcher(
	const cv::Mat& inFixedImage,
	const cv::Mat& inMovingImage,
	double scaleFactorFixedImage,
	double scaleFactorMovingImage,
	const Settings &settings,
	TConstPoints &fixedImageMaskPolygon,
	TConstPoints &movingImageMaskPolygon)
	: type(
			settings.getDetType(),
			settings.getDesType(),
			settings.getTransformFinderType(),
			settings.getMatcherType())
	, ransacThresh(13.0)
	, ransacConfidence(0.995)
	, _scaleFactorFixedImage(scaleFactorFixedImage)
	, _scaleFactorMovingImage(scaleFactorMovingImage)
{
FUNCLOG("Matcher");	

	settings.logParams();

	_fixedImageMaskPolygon.assign(fixedImageMaskPolygon.begin(), fixedImageMaskPolygon.end());
	_movingImageMaskPolygon.assign(movingImageMaskPolygon.begin(), movingImageMaskPolygon.end());

	cvtColor(inFixedImage, fixedImage, CV_RGB2GRAY);
	cvtColor(inMovingImage, movingImage, CV_RGB2GRAY);

	CreateFeatureDetector(featureDetFixed, settings);
	CreateDescriptorComputer(featureDesFixed, settings);
	CreateFeatureDetector(featureDetMoving, settings);
	CreateDescriptorComputer(featureDesMoving, settings);

	switch(getMatcherType()){

		case MatcherType::BF:    matcher = DescriptorMatcher::create("BruteForce-Hamming");  break;
		case MatcherType::BF2:   matcher = DescriptorMatcher::create("BruteForce-Hamming(2)");  break;
		case MatcherType::FLANN: matcher = DescriptorMatcher::create("FlannBased");  break;
		default:{
			throw std::logic_error("not implemented");
		}
	}

	matchFilterSpreadAuto = (bool)settings.getValue(eMatchFilterSpreadAuto);
	matchFilterSpreadFactor = settings.getValue(eMatchFilterSpreadFactor);
	matchFilterMinMatchesToRetain = (int)settings.getValue(eMatchFilterMinMatchesToRetain);
	matchFilterMaxMatchesToRetain = (int)settings.getValue(eMatchFilterMaxMatchesToRetain);
}

MatcherType
Matcher::getMatcherType() const
{
	switch(type.matcherType){
		case MatcherType::AUTO:{
			switch(type.desType){
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
			return type.matcherType;
		}
	}
}

void
Matcher::detectGFTT(TConstMat& inImage, TKeyPoints& outKeyPoints) const
{
FUNCLOGTIMEL("detectGFTT");

	int maxCorners = 80000;

	//Parameters for Shi-Tomasi algorithm
	TPoints2f corners;
	double qualityLevel = 0.101;
	double minDistance = 4;
	int blockSize = 5;
	bool useHarrisDetector = false;
	double k = 0.04;

	/// Apply corner detection
	goodFeaturesToTrack(inImage, corners, maxCorners, qualityLevel,
               minDistance, noArray(), blockSize, useHarrisDetector, k);

	for(TPoints2f::const_iterator it = corners.begin(); it != corners.end(); ++it){
		outKeyPoints.push_back(TKeyPoint(*it, 1));
	}
}

void
Matcher::detect(cv::Feature2D* feature2d, const cv::Mat& inImage, TKeyPoints& outKeyPoints, TConstPoints &maskPolygon, long& outTimeMs) const
{
FUNCLOGTIME("detect", outTimeMs);

	CV_Assert(!inImage.empty());
	
	if(maskPolygon.size() > 0 && type.detType != DetType::DET_GFTT) {
		
		Mat mask = Mat::zeros(inImage.size(), CV_8UC1);
		auto *pts = &maskPolygon[0];
		int ptsN = (int)maskPolygon.size();
		fillPoly(mask, &pts, &ptsN, 1, cv::Scalar::all(255));
		feature2d->detect(inImage, outKeyPoints, mask);
	}
	else {
		feature2d->detect(inImage, outKeyPoints);
	}
}

void
Matcher::compute(cv::Feature2D* feature2d, const cv::Mat& inImage, TKeyPoints& keyPoints, TMat& outMat, long& outTimeMs) const
{
FUNCLOGTIME("compute", outTimeMs);
	feature2d->compute(inImage, keyPoints, outMat);
}

void
Matcher::match(TConstMat& inDescriptors1, TConstMat& inDecsriptors2, TMatches& outMatches, long& outTimeMs) const
{
FUNCLOGTIME("match", outTimeMs);
	
	matcher->match(inDescriptors1, inDecsriptors2, outMatches);
}

void
Matcher::filterLowestOctave(TConstKeyPoints& inKeyPoints, TKeyPoints& outKeyPoints) const
{
FUNCLOGTIMEL("filterLowestOctave");

	int maxOctave = 0;
	int minOcatve = 100000;

	for(TConstKeyPoints::const_iterator it = inKeyPoints.begin(); it != inKeyPoints.end(); ++it){

		if(it->octave < minOcatve) minOcatve = it->octave;
		if(it->octave > maxOctave) maxOctave = it->octave;
	}


	LogUtils::getLog() << "minOcatve/maxOctave/: " << minOcatve << "/" << maxOctave << std::endl;;
	
	for(TConstKeyPoints::const_iterator it = inKeyPoints.begin(); it != inKeyPoints.end(); ++it){

		if(it->octave == maxOctave){
			outKeyPoints.push_back(*it);
		}
	}

}

void
Matcher::filterSort(TConstMatches& inMatches, TMatches& outMatches, size_t matchesN)
{
FUNCLOGTIMEL("filterSort");
	
	struct Sort{

		bool operator()(const DMatch& m1, const DMatch& m2){

			return m1.distance < m2.distance;
		}
	} mySort;

	outMatches.assign(inMatches.begin(), inMatches.end());


	std::sort(outMatches.begin(), outMatches.end(), mySort);
	if(outMatches.size() > matchesN) outMatches.resize(matchesN);
}

void
Matcher::filter(TConstMatches& inMatches, TMatches& outMatches)
{
FUNCLOGTIMEL("filter");

	maxDist = 0;
	minDist = 100000;

	for(TConstMatches::const_iterator it = inMatches.begin(); it != inMatches.end(); ++it){

		if(it->distance < minDist) minDist = it->distance;
		if(it->distance > maxDist) maxDist = it->distance;
	}

	double factor = 1.0;

	if(minDist == 0.0){
		//filterSort(inMatches, outMatches, 200);
		minDist = maxDist / 10.0;
		LogUtils::getLog() << "minDist = maxDist / 10.0" << std::endl;
	}
	else{
		switch(getMatcherType()){
			case MatcherType::FLANN: factor = 2.2; break;
			case MatcherType::BF:    factor = 6.0; break;
			case MatcherType::BF2:   factor = 6.0; break;
			default: {}
		}
	}

	if(!matchFilterSpreadAuto) {
		factor = matchFilterSpreadFactor;
	}

	for(TConstMatches::const_iterator it = inMatches.begin(); it != inMatches.end(); ++it){

		if(it->distance < max(factor * minDist, factor * minDist)){
			outMatches.push_back(*it);
		}
	}

	if((int)outMatches.size() < matchFilterMinMatchesToRetain){
		filterSort(inMatches, outMatches, matchFilterMinMatchesToRetain);
		LogUtils::getLog() << "Match count smaller " << matchFilterMinMatchesToRetain << ", not applying another filter " << std::endl;
	}
	else if((int)outMatches.size() > matchFilterMaxMatchesToRetain){
		filterSort(inMatches, outMatches, matchFilterMaxMatchesToRetain);
		LogUtils::getLog() << "Match count larger " << matchFilterMaxMatchesToRetain << ", reducing matches to " << matchFilterMaxMatchesToRetain  << std::endl;
	}
	else{
		TMatches outMatches2;
		filterSort(outMatches, outMatches2, outMatches.size());
		outMatches.clear();
		outMatches.assign(outMatches2.begin(), outMatches2.end());
	}

	maxDistUsed = outMatches.back().distance;
	maxDistFactorUsed = minDist > 0 ? maxDistUsed / minDist : 0;

}

bool
Matcher::getHomography(TConstPoints2f& inSrcPoints, TConstPoints2f& inDstPoints, TBools& outBools, TMat& outTransform, long& outTimeMs) const
{
FUNCLOGTIME("getHomography", outTimeMs);

	Mat mask(1, inSrcPoints.size(), CV_8UC1);
	mask = Mat::zeros(mask.size(), CV_8UC1);
	
	switch(type.transformFinderType){

		case TransformFinderType::TFT_RANSAC: outTransform = cv::findHomography(inSrcPoints, inDstPoints, RANSAC, ransacThresh, mask, 2000, ransacConfidence); break;
		case TransformFinderType::TFT_RHO:    outTransform = cv::findHomography(inSrcPoints, inDstPoints, RHO, ransacThresh, mask, 2000, ransacConfidence); break;
		case TransformFinderType::TFT_LMEDS:  outTransform = cv::findHomography(inSrcPoints, inDstPoints, LMEDS, ransacThresh, mask); break;
	}

	for(size_t matchIndex = 0; matchIndex < inSrcPoints.size(); ++ matchIndex){

		if(mask.at<uchar>(matchIndex) == 1) outBools.push_back(true);
		else                                outBools.push_back(false);
	}

	return !outTransform.empty() && outTransform.size().width != 0;
}

void
Matcher::getMatchingPoints(TConstMatches& inMatches, TConstKeyPoints inKeyPtsQ, TConstKeyPoints inKeyPtsT, TPoints2f& outPtsQ, TPoints2f& outPtsT) const
{
FUNCLOG("getMatchingPoints");

	for(size_t i = 0; i < inMatches.size(); i++){

		outPtsQ.push_back(inKeyPtsQ[inMatches[i].queryIdx].pt);
		outPtsT.push_back(inKeyPtsT[inMatches[i].trainIdx].pt);
	}
}

void
Matcher::getMatchingPoints(TMatchInfos& inMatchesInfos, TPoints2f& outPtsF, TPoints2f& outPtsM) const
{
FUNCLOG("getMatchingPoints");

	for(const auto &rMatchInfo : inMatchesInfos) {

		TConstKeyPoint& kPtF = std::get<0>(rMatchInfo.first);
		TConstKeyPoint& kPtM = std::get<1>(rMatchInfo.first);

		outPtsF.push_back(kPtF.pt);
		outPtsM.push_back(kPtM.pt);
	}
}

void sortMatchInfos(TMatchInfos& matchInfos)
{
	struct Sort{
		bool operator()(const TMatchInfo& matchInfo1, const TMatchInfo& matchInfo2){
			return matchInfo1.second.distance < matchInfo2.second.distance;
		}
	} mySort;

	std::sort(matchInfos.begin(), matchInfos.end(), mySort);
}

void
Matcher::detectAndCompute(cv::Feature2D* feature2dDet, cv::Feature2D* feature2dDes, TConstMat& inImage,
	TKeyPoints& outKeyPoints, TConstPoints &maskPolygon, TMat& outMat, long& outTimeDMs, long& outTimeCMs, bool /*isFixedImage*/) const
{
	detect(feature2dDet, inImage, outKeyPoints, maskPolygon, outTimeDMs);
	compute(feature2dDes, inImage, outKeyPoints, outMat, outTimeCMs);
}


int
Matcher::match()
{
FUNCLOGTIME("match", msTotal);

	TMat descriptors1, descriptors2;
	TMatches matchesAll, matchesGood;
	TBools bools;

	detectAndCompute(
		featureDetFixed, featureDesFixed, fixedImage, fixedKeyPoints, _fixedImageMaskPolygon,
		descriptors1, msDetectFixed, msComputeFixed, true);
	detectAndCompute(
		featureDetMoving, featureDesMoving, movingImage, movingKeyPoints, _movingImageMaskPolygon,
		descriptors2, msDetectMoving, msComputeMoving, false);

	if(fixedKeyPoints.size() == 0) {
		LogUtils::getLog() << "FixedImage: No KeyPoints found" << std::endl;
		return 0;
	}
	if(movingKeyPoints.size() == 0) {
		LogUtils::getLog() << "MovingImage: No KeyPoints found" << std::endl;
		return 0;
	}

	match(descriptors1, descriptors2, matchesAll, msMatching);
	filter(matchesAll, matchesGood);

	scaleKeyPoints();	

	getMatchingPoints(
		matchesGood, fixedKeyPoints, movingKeyPoints,
		goodMatchesFixedImagePtsOrdered, goodMatchesMovingImagePtsOrdered);

	const bool homographyFound = getHomography(
		goodMatchesMovingImagePtsOrdered, goodMatchesFixedImagePtsOrdered, bools, transform, msFindTransform);

	for(TMatches::const_iterator it = matchesAll.begin(); it != matchesAll.end(); ++it){
		allMatches.push_back(TMatchInfo(
			TKeyPointPair(fixedKeyPoints[it->queryIdx], movingKeyPoints[it->trainIdx]), *it));
	}
	sortMatchInfos(allMatches);


	for(TMatches::const_iterator it = matchesGood.begin(); it != matchesGood.end(); ++it){
		filteredMatches.push_back(TMatchInfo(
			TKeyPointPair(fixedKeyPoints[it->queryIdx], movingKeyPoints[it->trainIdx]), *it));
	}

	if(homographyFound){

		TBools::const_iterator itBools = bools.begin();
		for(TMatches::const_iterator it = matchesGood.begin(); it != matchesGood.end(); ++it, ++itBools){
			
			if(*itBools) {
				matchesInliers.push_back(TMatchInfo(
					TKeyPointPair(fixedKeyPoints[it->queryIdx], movingKeyPoints[it->trainIdx]), *it));
			}
			else {
				matchesOutliers.push_back(TMatchInfo(
					TKeyPointPair(fixedKeyPoints[it->queryIdx], movingKeyPoints[it->trainIdx]), *it));
			}
		}

		sortMatchInfos(matchesInliers);
		sortMatchInfos(matchesOutliers);

		getMatchingPoints(matchesInliers, goodInlierMatchesFixedImagePtsOrdered, goodInlierMatchesMovingImagePtsOrdered);
	}

	if(!homographyFound) return 0;
	else if(getMatchesInliers().size() * 5 < getMatchesOutliers().size()) return 2;
	return 1;
}

void
Matcher::scaleKeyPoints()
{
	for(TKeyPoints::iterator it = fixedKeyPoints.begin(); it != fixedKeyPoints.end(); ++it) {
		it->pt.x = it->pt.x * _scaleFactorFixedImage;
		it->pt.y = it->pt.y * _scaleFactorFixedImage;
	}
	for(TKeyPoints::iterator it = movingKeyPoints.begin(); it != movingKeyPoints.end(); ++it) {
		it->pt.x = it->pt.x * _scaleFactorMovingImage;
		it->pt.y = it->pt.y * _scaleFactorMovingImage;
	}
}

TPoints2f
Matcher::getGoodMatchesFixedImagePts2f() const
{
	TPoints2f points2f;
	for(TConstMatchInfos::const_iterator it = getMatchesFiltered().begin(); it != getMatchesFiltered().end(); ++it){

		TConstKeyPoint& kP1 = std::get<0>(std::get<0>(*it));
		points2f.push_back(Point2f(kP1.pt.x, kP1.pt.y));
	}
	return points2f;	
}

TPoints2f
Matcher::getGoodMatchesMovingImageKeyPts2f() const
{
	TPoints2f points2f;
	for(TConstMatchInfos::const_iterator it = getMatchesFiltered().begin(); it != getMatchesFiltered().end(); ++it){

		TConstKeyPoint& kP2 = std::get<1>(std::get<0>(*it));
		points2f.push_back(Point2f(kP2.pt.x, kP2.pt.y));
	}
	return points2f;
}

TConstKeyPoints&
Matcher::getFixedImageKeyPts() const
{
	return fixedKeyPoints;
}

TConstKeyPoints&
Matcher::getMovingImageKeyPts() const
{
	return movingKeyPoints;
}

TConstKeyPoints
Matcher::getFixedImageInlierKeyPoints() const
{
	TKeyPoints keyPoints;
	for(TMatchInfos::const_iterator it = matchesInliers.begin(); it != matchesInliers.end(); ++it){
		
		keyPoints.push_back(std::get<0>(std::get<0>(*it)));
	}
	return keyPoints;
}

TConstKeyPoints
Matcher::getMovingImageInlierKeyPoints() const
{
	TKeyPoints keyPoints;
	for(TMatchInfos::const_iterator it = matchesInliers.begin(); it != matchesInliers.end(); ++it){
		
		keyPoints.push_back(std::get<1>(std::get<0>(*it)));
	}
	return keyPoints;
}

TConstMat&
Matcher::getTransform() const
{
	return transform;
}

TMat
Matcher::getTransformInverse() const
{
	return transform.inv();
}

const Matcher::Type&
Matcher::getType() const
{
	return type;
}

TConstMatchInfos&
Matcher::getMatchesAll() const
{
	return allMatches;
}

TConstMatchInfos&
Matcher::getMatchesFiltered() const
{
	return filteredMatches;
}

TConstMatchInfos&
Matcher::getMatchesInliers() const
{
	return matchesInliers;
}

TConstMatchInfos&
Matcher::getMatchesOutliers() const
{
	return matchesOutliers;
}


double
Matcher::getMinDist() const
{
	return minDist;
}

double
Matcher::getMaxDist() const
{
	return maxDist;
}

double
Matcher::getMaxDistFactorUsed() const
{
	return maxDistFactorUsed;
}

double
Matcher::getMaxDistUsed() const
{
	return maxDistUsed;
}

long
Matcher::getMsDetectFixed() const
{
	return msDetectFixed;
}
long
Matcher::getMsDetectMoving() const
{
	return msDetectMoving;
}
long
Matcher::getMsComputeFixed() const
{
	return msComputeFixed;
}
long
Matcher::getMsComputeMoving() const
{
	return msComputeMoving;
}
long
Matcher::getMsMatching() const
{
	return msMatching;
}
long
Matcher::getMsFindTransform() const
{
	return msFindTransform;
}
long
Matcher::getMsTotal() const
{
	return msTotal;
}

long
Matcher::getFixedImageWidth() const
{
	return fixedImage.cols;
}
long
Matcher::getFixedImageHeight() const
{
	return fixedImage.rows;
}
long
Matcher::getMovingImageWidth() const
{
	return movingImage.cols;
}
long
Matcher::getMovingImageHeight() const
{
	return movingImage.rows;
}

std::string         
Matcher::getDetectorName() const
{
	switch(type.detType){

		case DetType::DET_BRISK:		return "BRISK";
		case DetType::DET_SURF:		return "SURF";
		case DetType::DET_SIFT:		return "SIFT";
		case DetType::DET_HARRIS:	return "HARRIS";
		case DetType::DET_GFTT:		return "GFTT";
		case DetType::DET_ORB:		return "ORB";
		case DetType::DET_ORB2:		return "ORB2";
		case DetType::DET_KAZE:		return "KAZE";
		case DetType::DET_AKAZE:		return "AKAZE";
		default: {}
	}
	return "";
}

std::string
Matcher::getDescriptorName() const
{
	switch(type.desType){
		case DesType::DES_BRISK: return "BRISK";
		case DesType::DES_SURF:  return "SURF";
		case DesType::DES_SIFT:	return "SIFT";
		case DesType::DES_FREAK:	return "FREAK";
		case DesType::DES_ORB:		return "ORB";
		case DesType::DES_ORB2:		return "ORB2";
		case DesType::DES_KAZE:		return "KAZE";
		case DesType::DES_AKAZE:		return "AKAZE";
		default: {}
	}
	return "";
}

std::string
Matcher::getMatcherName() const
{
	switch(getMatcherType()){
		case MatcherType::FLANN: return "FLANN";
		case MatcherType::BF:    return "BFHAMMING";
		case MatcherType::BF2:   return "BFHAMMING2";
		case MatcherType::AUTO:   return "AUTO";
		default: {}
	}
	return "";
}

std::string
Matcher::getTransformFinderName() const
{
	switch(type.transformFinderType){
		case TransformFinderType::TFT_RANSAC: return "RANSAC";
		case TransformFinderType::TFT_RHO:	 	return "RHO";
		case TransformFinderType::TFT_LMEDS:  return "LMEDS";
		default: {}
	}

	return "";
}

double
Matcher::getDistMin(TMatchInfos matchInfos) const
{
	if(matchInfos.empty()) return -1.0;
	return matchInfos.front().second.distance;
}
double
Matcher::getDistMax(TMatchInfos matchInfos) const
{
	if(matchInfos.empty()) return -1.0;
	return matchInfos.back().second.distance;
}
double
Matcher::getDistAverage(TMatchInfos matchInfos) const
{
	if(matchInfos.empty()) return -1.0;
	double sum = 0.0;
	for(TMatchInfos::const_iterator it = matchInfos.begin(); it != matchInfos.end(); ++it){
		sum += it->second.distance;
	}
	return sum / matchInfos.size();
}
double
Matcher::getDistMedian(TMatchInfos matchInfos) const
{
	if(matchInfos.empty()) return -1.0;
	return matchInfos.at(matchInfos.size() / 2).second.distance;
}
std::vector<double>
Matcher::getDists(TMatchInfos matchInfos) const
{
	std::vector<double> dists;
	for(TMatchInfos::const_iterator it = matchInfos.begin(); it != matchInfos.end(); ++it){
		dists.push_back(it->second.distance);
	}
	return dists;
}

double
Matcher::getInliersDistMin() const
{
	return getDistMin(matchesInliers);
}
double
Matcher::getInliersDistMax() const
{
	return getDistMax(matchesInliers);
}
double
Matcher::getInliersDistAverage() const
{
	return getDistAverage(matchesInliers);
}
double
Matcher::getInliersDistMedian() const
{
	return getDistMedian(matchesInliers);
}
std::vector<double>
Matcher::getInliersDists() const
{
	return getDists(matchesInliers);
}

double
Matcher::getOutliersDistMin() const
{
	return getDistMin(matchesOutliers);
}
double
Matcher::getOutliersDistMax() const
{
	return getDistMax(matchesOutliers);
}
double
Matcher::getOutliersDistAverage() const
{
	return getDistAverage(matchesOutliers);
}
double
Matcher::getOutliersDistMedian() const
{
	return getDistMedian(matchesOutliers);
}
std::vector<double>
Matcher::getOutliersDists() const
{
	return getDists(matchesOutliers);
}

double
Matcher::getAllMatchesDistMin() const
{
	return getDistMin(allMatches);
}
double
Matcher::getAllMatchesDistMax() const
{
	return getDistMax(allMatches);
}
double
Matcher::getAllMatchesDistAverage() const
{
	return getDistAverage(allMatches);
}
double
Matcher::getAllMatchesDistMedian() const
{
	return getDistMedian(allMatches);
}
std::vector<double>
Matcher::getAllMatchesDists() const
{
	return getDists(allMatches);
}

double
Matcher::getGoodMatchesDistMin() const
{
	return getDistMin(filteredMatches);
}
double
Matcher::getGoodMatchesDistMax() const
{
	return getDistMax(filteredMatches);
}
double
Matcher::getGoodMatchesDistAverage() const
{
	return getDistAverage(filteredMatches);
}
double
Matcher::getGoodMatchesDistMedian() const
{
	return getDistMedian(filteredMatches);
}
std::vector<double>
Matcher::getGoodMatchesDists() const
{
	return getDists(filteredMatches);
}

double
Matcher::getRansacThresh() const
{
	return ransacThresh;
}
double
Matcher::getRansacConfidence() const
{
	return ransacConfidence;
}

TConstPoints2f
Matcher::getGoodMatchesFixedImagePtsOrdered() const
{
	return goodMatchesFixedImagePtsOrdered;
}

TConstPoints2f
Matcher::getGoodMatchesMovingImagePtsOrdered() const
{
	return goodMatchesMovingImagePtsOrdered;
}

TConstPoints2f
Matcher::getGoodInlierMatchesFixedImagePtsOrdered() const
{
	return goodInlierMatchesFixedImagePtsOrdered;
}

TConstPoints2f
Matcher::getGoodInlierMatchesMovingImagePtsOrdered() const
{
	return goodInlierMatchesMovingImagePtsOrdered;
}

void
Matcher::compare(
	InputArray image, DetType detType, const Settings &settings,
	TKeyPoints &outKeyPoints, TConstPoints &maskPolygon)
{
	Ptr<Feature2D> featureDet2d;

	switch(detType){
		case DetType::DET_BRISK: featureDet2d = createBrisk(settings); break;
		case DetType::DET_SURF: featureDet2d = createSurf(settings); break;
		case DetType::DET_SIFT: featureDet2d = createSift(settings); break;
		case DetType::DET_ORB: featureDet2d = createOrb(settings); break;
		case DetType::DET_KAZE: featureDet2d = createKaze(settings); break;
		case DetType::DET_AKAZE: featureDet2d = createAkaze(settings); break;
		case DetType::DET_ORB2: featureDet2d = ORB::create(3000, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 41, 20); break;
		case DetType::DET_GFTT: break;
		case DetType::DET_HARRIS:	
		default:{

			throw std::logic_error("not implemented");
		}
	}


	if(maskPolygon.size() > 0) {
		Mat mask = Mat::zeros(image.size(), CV_8UC1);
		auto *pts = &maskPolygon[0];
		int ptsN = (int)maskPolygon.size();
		fillPoly(mask, &pts, &ptsN, 1, cv::Scalar::all(255));
		featureDet2d->detect(image, outKeyPoints, mask);
	}
	else {
		featureDet2d->detect(image, outKeyPoints);
	}
}


} // imgalign