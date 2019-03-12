
#include "Matcher.h"

#include <exception>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include "opencv2/imgproc/types_c.h"

#include "warpers/warpers.hpp"

#include "TimeUtils.h"
#include "LogUtils.h"
#include "Display.h"
#include "Settings.h"
#include "WarperHelper.h"
#include "ImageUtils.h"
#include "FeatureFactory.h"
#include "DesMatcher.h"
#include "Homography.h"

using namespace cv;



Ptr<Feature2D> featureDetFixed;
Ptr<Feature2D> featureDesFixed;
Ptr<Feature2D> featureDetMoving;
Ptr<Feature2D> featureDesMoving;

namespace imgalign
{
DesMatcher desMatcher;

Matcher::Type::Type(DetType inDetType, DesType inDesType, TransformFinderType inTransformFinderType, MatcherType inMatcherType)
	: detType(inDetType)
	, desType(inDesType)
	, matcherType(inMatcherType)
	, transformFinderType(inTransformFinderType)
{
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

	inFixedImage.copyTo(fixedImage);
	inMovingImage.copyTo(movingImage);

	FeatureFactory::CreateFeatureDetector(featureDetFixed, settings);
	FeatureFactory::CreateDescriptorComputer(featureDesFixed, settings);
	FeatureFactory::CreateFeatureDetector(featureDetMoving, settings);
	FeatureFactory::CreateDescriptorComputer(featureDesMoving, settings);

	desMatcher = FeatureFactory::CreateMatcher(settings);
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
	
	desMatcher.matcher->match(inDescriptors1, inDecsriptors2, outMatches);
}

bool
Matcher::getHomography(TConstPoints2f& inSrcPoints, TConstPoints2f& inDstPoints, TBools& outBools, TMat& outTransform, long& outTimeMs) const
{
FUNCLOGTIME("getHomography", outTimeMs);

	return Homography::getHomography(type.transformFinderType, inDstPoints, inSrcPoints, outBools, outTransform, ransacThresh, ransacConfidence);
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

	scaleKeyPoints();

	MatchInfo matchInfo = desMatcher.match(
		type.transformFinderType,
		descriptors1,
		descriptors2,
		fixedKeyPoints,
		movingKeyPoints,
		DataExtractionMode::eExtended);

	goodMatchesFixedImagePtsOrdered = matchInfo.filteredPts1;
	goodMatchesMovingImagePtsOrdered = matchInfo.filteredPts2;
	allMatches = matchInfo.allMatchInfos;
	filteredMatches = matchInfo.filteredMatchInfos;

	maxDist = matchInfo.filterInfo.maxDist;
	minDist = matchInfo.filterInfo.minDist;
	maxDistUsed = matchInfo.filterInfo.maxDistUsed;
	maxDistFactorUsed = matchInfo.filterInfo.maxDistFactorUsed;

	if(!matchInfo.success) {
		return 0;
	}

	transform = matchInfo.homography;
	matchesInliers = matchInfo.inlierMatchInfos;
	matchesOutliers = matchInfo.outlierMatchInfos;
	goodInlierMatchesFixedImagePtsOrdered = matchInfo.inlierPts1;
	goodInlierMatchesMovingImagePtsOrdered = matchInfo.inlierPts2;
	
	return matchInfo.confidence > 0.2
		? 1
		: 2;
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
		case DetType::DET_BRISK: featureDet2d = FeatureFactory::createBrisk(settings); break;
		case DetType::DET_SURF: featureDet2d = FeatureFactory::createSurf(settings); break;
		case DetType::DET_SIFT: featureDet2d = FeatureFactory::createSift(settings); break;
		case DetType::DET_ORB: featureDet2d = FeatureFactory::createOrb(settings); break;
		case DetType::DET_KAZE: featureDet2d = FeatureFactory::createKaze(settings); break;
		case DetType::DET_AKAZE: featureDet2d = FeatureFactory::createAkaze(settings); break;
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


void
Matcher::getHomography(
	double fieldOfViewFixedImage,
	double fieldOfViewMovingImage,
	bool calcYaw2,
  bool calcPitch2,
	double yaw1, double pitch1,
	double &yaw2, double &pitch2,
	int projectionType1,
	int projectionType2,
	TMat &outHomography)
{
	FUNCLOGTIMEL("Matcher::getHomography");

	if(goodMatchesFixedImagePtsOrdered.size() == 0) {
		throw std::logic_error("Template image has no keypoints"); 
	}
	if(goodMatchesMovingImagePtsOrdered.size() == 0) {
		throw std::logic_error("Moving image has no keypoints");
	}
	if(transform.empty() && (calcYaw2 || calcPitch2)) {
		throw std::logic_error("Can not calculate relative rotation without an existing homography matrix");
	}

	double fw = fixedImage.size().width * _scaleFactorFixedImage;
	double fh = fixedImage.size().height * _scaleFactorFixedImage;
	double mw = movingImage.size().width * _scaleFactorMovingImage;
	double mh = movingImage.size().height * _scaleFactorMovingImage;

	std::vector<Point2f> ptsFixedImage(
		goodMatchesFixedImagePtsOrdered.begin(),
		goodMatchesFixedImagePtsOrdered.end());

	std::vector<Point2f> ptsMovingImage(
		goodMatchesMovingImagePtsOrdered.begin(),
		goodMatchesMovingImagePtsOrdered.end());
	
	
	if(calcYaw2 || calcPitch2) {
		double yaw_ = yaw2;
		double pitch_ = pitch2;
		WarperHelper::getRelativeRotation(fw, fh, mw, mh, fieldOfViewFixedImage, transform, yaw2, pitch2);
		if(!calcYaw2) yaw2 = yaw_;
		if(!calcPitch2) pitch2 = pitch_;
	}

	TMat rotMat1, rotMat2;
	WarperHelper::getMatR(yaw1, pitch1, 0, rotMat1);
	WarperHelper::getMatR(yaw1 + yaw2, pitch1 + pitch2, 0, rotMat2);

	Homography::getHomography(
		ptsFixedImage, ptsMovingImage,
		cv::Size(fw, fh), cv::Size(mw, mh),
		nullptr, nullptr,
		fieldOfViewFixedImage, fieldOfViewMovingImage,
		rotMat1, rotMat2,
		projectionType1, projectionType2,
		type.transformFinderType, outHomography);
}



} // imgalign