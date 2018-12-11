#include "../precomp.hpp"
#include "Matcher.h"
#include "ImageUtils.h"
#include "Display.h"
#include "LogUtils.h"
#include "Settings.h"
#include "EnumTypes.h"
#include "TimeUtils.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include "opencv2/imgproc/types_c.h"

#include <stdio.h>
#include <ostream>
#include <iostream>
#include <memory>


namespace cv
{

ImgAlign::~ImgAlign() {}

void ImgAlign::set(
	CV_IN_OUT std::vector<int> &/*valueTypes*/,
	CV_IN_OUT std::vector<float> &/*values*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

int ImgAlign::match()
{
  CV_Error(Error::StsNotImplemented, "");
}

int ImgAlign::match_getAlignedImage(
  OutputArray /*alignedImage*/)
{
  CV_Error(Error::StsNotImplemented, "");
}
    
int ImgAlign::match_getTransMatrix(
  OutputArray /*transMat*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

int ImgAlign::match_getExtData(
	CV_IN_OUT std::vector<Point> &/*fixedImageMaskPolygon*/,
	CV_IN_OUT std::vector<Point> &/*movingImageMaskPolygon*/,
  OutputArray /*transMat*/,
  CV_OUT std::vector<Point> &/*goodMatchesFixedImage*/,
  CV_OUT std::vector<Point> &/*goodMatchesMovingImage*/,
	CV_OUT std::vector<Point> &/*goodInlierMatchesFixedImage*/,
  CV_OUT std::vector<Point> &/*goodInlierMatchesMovingImage*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getImageAligned(
  InputArray /*transMatrix*/,
  OutputArray /*alignedImage*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getImageBlended(
  InputArray /*transMatrix*/,
  float /*weight*/,
  OutputArray /*blendImage*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getImageBlendedPolygonFixedImage(
	InputArray /*transMatrix*/,
	float /*weight*/,
	int /*doOverlay*/,
	CV_IN_OUT std::vector<Point> &/*polygon*/,
	OutputArray /*blendImage*/)
{
	CV_Error(Error::StsNotImplemented, "");
}
        
void ImgAlign::getImageBlendedPolygonMovingImage(
	InputArray /*transMatrix*/,
	float /*weight*/,
	int /*doOverlay*/,
	CV_IN_OUT std::vector<Point> &/*polygon*/,
	OutputArray /*blendImage*/)
{
	CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getImageFloodFillFixedImage(
	cv::InputArray /*transMatrix*/,
	CV_IN_OUT std::vector<cv::Point> &/*seedPts*/,
	CV_IN_OUT std::vector<int> &/*floodFillTolerances*/,
	float /*weight*/,
	int /*doOverlay*/,
	cv::OutputArray /*blendImage*/)
{
	CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getImageFloodFillMovingImage(
	cv::InputArray /*transMatrix*/,
	CV_IN_OUT std::vector<cv::Point> &/*seedPts*/,
	CV_IN_OUT std::vector<int> &/*floodFillTolerances*/,
	float /*weight*/,
	int /*doOverlay*/,
	cv::OutputArray /*blendImage*/)
{
	CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getFixedImageWithMatchedPoints(
  CV_IN_OUT std::vector<Point> &/*points*/,
  OutputArray /*fixedImageWithMatchedPoints*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getMovingImageWithMatchedPoints(
  CV_IN_OUT std::vector<Point> &/*points*/,
  OutputArray /*movingImageWithMatchedPoints*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getSideBySideImage(
  CV_IN_OUT std::vector<Point> &/*matchedPointsFixedImage*/,
  CV_IN_OUT std::vector<Point> &/*matchedPointsMovingImage*/,
  OutputArray /*sideBySideImage*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::compare(
  int /*imageType*/,
  CV_IN_OUT std::vector<int> &/*detectorTypes*/,
  OutputArray /*sideBySideImage*/,
	CV_IN_OUT std::vector<int> &/*timeUsedMs*/,
	CV_IN_OUT std::vector<int> &/*keyPointsN*/,
	CV_IN_OUT std::vector<Point> &/*fixedImageMaskPolygon*/,
	CV_IN_OUT std::vector<Point> &/*movingImageMaskPolygon*/)
{
	CV_Error(Error::StsNotImplemented, "");
}

} // namespace cv

namespace imgalign
{

class ImgAlign_Impl : public cv::ImgAlign
{
	public:

    ImgAlign_Impl(
      cv::InputArray fixedImage,
      cv::InputArray movingImage);

    void set(
			CV_IN_OUT std::vector<int> &valueTypes,
			CV_IN_OUT std::vector<float> &values) CV_OVERRIDE;

    int match() CV_OVERRIDE;

    int match_getAlignedImage(
      cv::OutputArray alignedImage) CV_OVERRIDE;

    int match_getTransMatrix(
      cv::OutputArray transMat) CV_OVERRIDE;

    int match_getExtData(
			CV_IN_OUT std::vector<cv::Point> &fixedImageMaskPolygon,
			CV_IN_OUT std::vector<cv::Point> &movingImageMaskPolygon,
      cv::OutputArray transMat,
      CV_OUT std::vector<cv::Point> &goodMatchesFixedImage,
      CV_OUT std::vector<cv::Point> &goodMatchesMovingImage,
	    CV_OUT std::vector<cv::Point> &goodInlierMatchesFixedImage,
      CV_OUT std::vector<cv::Point> &goodInlierMatchesMovingImage) CV_OVERRIDE;

    void getImageAligned(
      cv::InputArray transMatrix,
      cv::OutputArray alignedImage) CV_OVERRIDE;

    void getImageBlended(
      cv::InputArray transMatrix,
      float weight,
      cv::OutputArray blendImage) CV_OVERRIDE;

		void getImageBlendedPolygonFixedImage(
			cv::InputArray transMatrix,
			float weight,
			int doOverlay,
			CV_IN_OUT std::vector<cv::Point> &polygon,
			cv::OutputArray blendImage) CV_OVERRIDE;
        
    void getImageBlendedPolygonMovingImage(
			cv::InputArray transMatrix,
			float weight,
			int doOverlay,
			CV_IN_OUT std::vector<cv::Point> &polygon,
			cv::OutputArray blendImage) CV_OVERRIDE;

		void getImageFloodFillFixedImage(
			cv::InputArray transMatrix,
			CV_IN_OUT std::vector<cv::Point> &seedPts,
			CV_IN_OUT std::vector<int> &floodFillTolerances,
			float weight,
			int doOverlay,
			cv::OutputArray blendImage) CV_OVERRIDE;

		void getImageFloodFillMovingImage(
			cv::InputArray transMatrix,
			CV_IN_OUT std::vector<cv::Point> &seedPts,
			CV_IN_OUT std::vector<int> &floodFillTolerances,
			float weight,
			int doOverlay,
			cv::OutputArray blendImage) CV_OVERRIDE;

    void getFixedImageWithMatchedPoints(
      CV_IN_OUT std::vector<cv::Point> &points,
      cv::OutputArray fixedImageWithMatchedPoints) CV_OVERRIDE;

    void getMovingImageWithMatchedPoints(
      CV_IN_OUT std::vector<cv::Point> &points,
      cv::OutputArray movingImageWithMatchedPoints) CV_OVERRIDE;

    void getSideBySideImage(
      CV_IN_OUT std::vector<cv::Point> &matchedPointsFixedImage,
      CV_IN_OUT std::vector<cv::Point> &matchedPointsMovingImage,
      cv::OutputArray sideBySideImage) CV_OVERRIDE;

		void compare(
			int imageType,
			CV_IN_OUT std::vector<int> &detectorTypes,
			cv::OutputArray sideBySideImage,
			CV_IN_OUT std::vector<int> &timeUsedMs,
			CV_IN_OUT std::vector<int> &keyPointsN,
			CV_IN_OUT std::vector<cv::Point> &fixedImageMaskPolygon,
			CV_IN_OUT std::vector<cv::Point> &movingImageMaskPolygon) CV_OVERRIDE;

	private:
		void createMatcher();
		std::vector<cv::Mat> m_transformedImages;
    std::unique_ptr<Matcher> spMatcher;
		std::vector<cv::Point> _fixedImageMaskPolygon;
		std::vector<cv::Point> _movingImageMaskPolygon;
    cv::Mat fixedImageIn;
    cv::Mat movingImageIn;
		cv::Mat fixedImage;
		cv::Mat movingImage;
		int matchSuccess = 0;
		Settings settings;
		double scaleFixedImage = 1.0;
		double scaleMovingImage = 1.0;
};

} // imgalign

namespace cv
{

Ptr<ImgAlign> ImgAlign::create(
  InputArray fixedImage,
  InputArray movingImage)
{	
	return makePtr<imgalign::ImgAlign_Impl>(fixedImage, movingImage);
}

} // cv

using namespace cv;

namespace imgalign
{

ImgAlign_Impl::ImgAlign_Impl(
  InputArray fixedImage,
  InputArray movingImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::ImgAlign_Impl");
	try {
		fixedImage.copyTo(fixedImageIn);
		movingImage.copyTo(movingImageIn);
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::set(
	CV_IN_OUT std::vector<int> &valueTypes,
	CV_IN_OUT std::vector<float> &values)
{
	FUNCLOGTIMEL("ImgAlign_Impl::set");

	try {
		settings.setValues(valueTypes, values);
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::createMatcher()
{
	FUNCLOGTIMEL("ImgAlign_Impl::createMatcher");

	try {

		CV_Assert(!fixedImageIn.empty());
		CV_Assert(!movingImageIn.empty());

		int maxPixelsN = (int)settings.getValue(eImageCap);
		scaleFixedImage = ImageUtils::resize(fixedImageIn, fixedImage, maxPixelsN);
		scaleMovingImage = ImageUtils::resize(movingImageIn, movingImage, maxPixelsN);

		TPoints maskFixed;
		for(auto pt : _fixedImageMaskPolygon) {
			maskFixed.push_back(Point((int)(pt.x * scaleFixedImage), (int)(pt.y * scaleFixedImage)));
		}

		TPoints maskMoving;
		for(auto pt : _movingImageMaskPolygon) {
			maskMoving.push_back(Point((int)(pt.x * scaleMovingImage), (int)(pt.y * scaleMovingImage)));
		}

		spMatcher = std::unique_ptr<Matcher>(new Matcher(
			fixedImage,
			movingImage,
			1.0 / scaleFixedImage,
			1.0 / scaleMovingImage,
			settings,
			maskFixed,
			maskMoving
		));
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

int ImgAlign_Impl::match()
{
	FUNCLOGTIMEL("ImgAlign_Impl::match");

	try {

		createMatcher();
		matchSuccess = spMatcher->match();
		return matchSuccess;
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

int ImgAlign_Impl::match_getAlignedImage(
	OutputArray alignedImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::match_getAlignedImage");

	try {

		createMatcher();
		LogUtils::getLog() << "createMatcher done" << "std::endl";
		LogUtils::getLog() << (spMatcher == nullptr ? "spMatcher is null" : "spMatcher ok")  << "std::endl";
		matchSuccess = spMatcher->match();
		LogUtils::doLog(*spMatcher);

		if(matchSuccess == 0) return 0;

		alignedImage.create(fixedImageIn.size(), movingImageIn.type());
		warpPerspective(movingImageIn, alignedImage, spMatcher->getTransform(), fixedImageIn.size());
		
		return matchSuccess;
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}
    

int ImgAlign_Impl::match_getTransMatrix(
  OutputArray transMat)
{
	FUNCLOGTIMEL("ImgAlign_Impl::match_getTransMatrix");

	try {

		createMatcher();
		matchSuccess = spMatcher->match();
		LogUtils::doLog(*spMatcher);

		if(matchSuccess == 0) return 0;

		spMatcher->getTransform().copyTo(transMat);
		return matchSuccess;
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

int ImgAlign_Impl::match_getExtData(
	CV_IN_OUT std::vector<Point> &fixedImageMaskPolygon,
	CV_IN_OUT std::vector<Point> &movingImageMaskPolygon,
  OutputArray transMat,
  CV_OUT std::vector<Point> &goodMatchesFixedImage,
  CV_OUT std::vector<Point> &goodMatchesMovingImage,
	CV_OUT std::vector<Point> &goodInlierMatchesFixedImage,
  CV_OUT std::vector<Point> &goodInlierMatchesMovingImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::match_getExtData");

	try {

		_fixedImageMaskPolygon.assign(fixedImageMaskPolygon.begin(), fixedImageMaskPolygon.end());
		_movingImageMaskPolygon.assign(movingImageMaskPolygon.begin(), movingImageMaskPolygon.end());

		createMatcher();
		matchSuccess = spMatcher->match();
		LogUtils::doLog(*spMatcher);

		if(matchSuccess == 0) return 0;

		spMatcher->getTransform().copyTo(transMat);

		auto toPts = [&](TPoints &ptsI, TConstPoints2f &&pts){
			for(auto &pt : pts) {
				ptsI.push_back(Point((int)pt.x, (int)pt.y));
			}
		};
		goodMatchesFixedImage.clear();
		goodMatchesMovingImage.clear();
		toPts(goodMatchesFixedImage, spMatcher->getGoodMatchesFixedImagePtsOrdered());
		toPts(goodMatchesMovingImage, spMatcher->getGoodMatchesMovingImagePtsOrdered());

		goodInlierMatchesFixedImage.clear();
		goodInlierMatchesMovingImage.clear();
		toPts(goodInlierMatchesFixedImage, spMatcher->getGoodInlierMatchesFixedImagePtsOrdered());
		toPts(goodInlierMatchesMovingImage, spMatcher->getGoodInlierMatchesMovingImagePtsOrdered());

		return matchSuccess;
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getImageAligned(
  InputArray transMatrix,
  OutputArray alignedImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageAligned");

	try {

		alignedImage.create(fixedImageIn.size(), movingImageIn.type());
		warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getImageBlended(
  InputArray transMatrix,
  float weight,
  OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageBlended");

	try {

		Mat alignedImage(fixedImageIn.size(), movingImageIn.type());
		warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());

		blendImage.create(fixedImageIn.size(), fixedImageIn.type());
		addWeighted(fixedImageIn, 1.0 - weight, alignedImage, weight, 0.0, blendImage);
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getImageBlendedPolygonFixedImage(
	InputArray transMatrix,
	float weight,
	int doOverlay,
	CV_IN_OUT std::vector<Point> &polygon,
	OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageBlendedPolygonFixedImage");
	
	try {

		if(polygon.empty()) {
			return getImageBlended(transMatrix, weight, blendImage);
		}

		std::vector<Point> polygonScaled;
		for(auto pt : polygon) {
			polygonScaled.push_back(Point((int)(pt.x * 1.0), (int)(pt.y * 1.0)));
		}

		Mat alignedImage(fixedImageIn.size(), movingImageIn.type());
		warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());

		blendImage.create(fixedImageIn.size(), fixedImageIn.type());

		ImageUtils::blendImages(alignedImage, fixedImageIn, weight, doOverlay, polygonScaled, blendImage.getMatRef());
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}
        
void ImgAlign_Impl::getImageBlendedPolygonMovingImage(
	InputArray transMatrix,
	float weight,
	int doOverlay,
	CV_IN_OUT std::vector<Point> &polygon,
	OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageBlendedPolygonMovingImage");

	try {

		if(polygon.empty()) {
			return getImageBlended(transMatrix, weight, blendImage);
		}

		std::vector<Point> polygonScaled;
		for(auto pt : polygon) {
			polygonScaled.push_back(Point((int)(pt.x * 1.0), (int)(pt.y * 1.0)));
		}

		cv::Mat polygonImage, polygonMask;
		ImageUtils::clearPolygonInvFromImage(movingImageIn, polygonScaled, polygonImage, polygonMask);

		Mat polygonImageAligned(fixedImageIn.size(), movingImageIn.type());
		warpPerspective(movingImageIn, polygonImageAligned, transMatrix, fixedImageIn.size());

		Mat polygonMaskAligned(fixedImageIn.size(), polygonMask.type());
		warpPerspective(polygonMask, polygonMaskAligned, transMatrix, fixedImageIn.size());

		blendImage.create(fixedImageIn.size(), fixedImageIn.type());
		ImageUtils::blendImages(polygonImageAligned, fixedImageIn, weight, doOverlay, polygonMaskAligned, blendImage.getMatRef());
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getImageFloodFillFixedImage(
	InputArray transMatrix,
	CV_IN_OUT std::vector<Point> &seedPts,
	CV_IN_OUT std::vector<int> &floodFillTolerances,
	float weight,
	int doOverlay,
	cv::OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageFloodFillFixedImage");

	try {

		CV_Assert(seedPts.size() == floodFillTolerances.size());

		if(seedPts.empty()) {
			return getImageBlended(transMatrix, weight, blendImage);
		}

		std::vector<Point> ptsScaled;
		for(const auto &pt : seedPts) {
			ptsScaled.push_back(Point((int)(pt.x * 1.0), (int)(pt.y * 1.0)));
		}

		TMat mask;
		ImageUtils::floodFillMask(fixedImageIn, ptsScaled, floodFillTolerances, mask);

		Mat alignedImage(fixedImageIn.size(), movingImageIn.type());
		warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());

		blendImage.create(fixedImageIn.size(), fixedImageIn.type());
		ImageUtils::blendImages(alignedImage, fixedImageIn, weight, doOverlay, mask, blendImage.getMatRef());
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getImageFloodFillMovingImage(
	InputArray transMatrix,
	CV_IN_OUT std::vector<Point> &seedPts,
	CV_IN_OUT std::vector<int> &floodFillTolerances,
	float weight,
	int doOverlay,
	OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageFloodFillMovingImage");

	try {

		CV_Assert(seedPts.size() == floodFillTolerances.size());

		if(seedPts.empty()) {
			return getImageBlended(transMatrix, weight, blendImage);
		}

		std::vector<Point> ptsScaled;
		for(const auto &pt : seedPts) {
			ptsScaled.push_back(Point((int)(pt.x * 1.0), (int)(pt.y * 1.0)));
		}

		std::vector<Point> ptsWarped = ImageUtils::transformPts(ptsScaled, transMatrix.getMat());

		Mat alignedImage(fixedImageIn.size(), movingImageIn.type());
		warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());

		TMat mask;
		ImageUtils::floodFillMask(alignedImage, ptsWarped, floodFillTolerances, mask);

		blendImage.create(fixedImageIn.size(), fixedImageIn.type());
		ImageUtils::blendImages(alignedImage, fixedImageIn, weight, doOverlay, mask, blendImage.getMatRef());
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getFixedImageWithMatchedPoints(
  CV_IN_OUT std::vector<Point> &points,
  OutputArray fixedImageWithMatchedPoints)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getFixedImageWithMatchedPoints");

	try {

		std::vector<TConstMat> images;
		images.push_back(fixedImageIn);

		auto display = std::unique_ptr<Display>(new Display(images, fixedImageIn.size(), 0, 0, fixedImageIn.type()));

		display->generateSideBySideImage();
		display->drawPoints(points, 0);

		fixedImageWithMatchedPoints.create(display->getDestImage().size(), display->getDestImage().type());
		display->getDestImage().copyTo(fixedImageWithMatchedPoints);
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getMovingImageWithMatchedPoints(
  CV_IN_OUT std::vector<Point> &points,
  OutputArray movingImageWithMatchedPoints)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getMovingImageWithMatchedPoints");

	try {

		std::vector<TConstMat> images;
		images.push_back(movingImageIn);

		auto display = std::unique_ptr<Display>(new Display(images, movingImageIn.size(), 0, 0, movingImageIn.type()));
		
		display->generateSideBySideImage();
		display->drawPoints(points, 0);

		movingImageWithMatchedPoints.create(display->getDestImage().size(), display->getDestImage().type());
		display->getDestImage().copyTo(movingImageWithMatchedPoints);
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getSideBySideImage(
  CV_IN_OUT std::vector<Point> &matchedPointsFixedImage,
	CV_IN_OUT std::vector<Point> &matchedPointsMovingImage,
	OutputArray sideBySideImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getSideBySideImage");

	try {

		std::vector<TConstMat> images;
		images.push_back(fixedImageIn);
		images.push_back(movingImageIn);

		auto display = std::unique_ptr<Display>(new Display(images, fixedImageIn.size(), 0, 0, fixedImageIn.type()));
		display->clear();

		display->generateSideBySideImage();
		display->drawLines(matchedPointsFixedImage, matchedPointsMovingImage, 0, 1, TColor(255, 0, 255, 255));

		sideBySideImage.create(display->getDestImage().size(), display->getDestImage().type());
		display->getDestImage().copyTo(sideBySideImage);
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::compare(
	int imageType,
	CV_IN_OUT std::vector<int> &detectorTypes,
	OutputArray sideBySideImage,
	CV_IN_OUT std::vector<int> &timeUsedMs,
	CV_IN_OUT std::vector<int> &keyPointsN,
	CV_IN_OUT std::vector<cv::Point> &fixedImageMaskPolygon,
	CV_IN_OUT std::vector<cv::Point> &movingImageMaskPolygon)
{
	try {
		FUNCLOGTIMEL("ImgAlign_Impl::compare");

		if(	 (ParamType)imageType != eCompareImageTypeFixed 
			&& (ParamType)imageType != eCompareImageTypeMoving) {

			throw std::logic_error("image type not supported");
		}

		Mat srcImage = imageType == eCompareImageTypeFixed
			? fixedImageIn
			: movingImageIn;

		const std::vector<cv::Point> &maskPolygon = imageType == eCompareImageTypeFixed
			? fixedImageMaskPolygon
			: movingImageMaskPolygon;

		int maxPixelsN = (int)settings.getValue(eImageCap);
		
		Mat srcImageStretched;
		double scaleF = ImageUtils::resize(srcImage, srcImageStretched, maxPixelsN);
		
		TPoints mask;
		for(auto pt : maskPolygon) {
			mask.push_back(Point((int)(pt.x * scaleF), (int)(pt.y * scaleF)));
		}

		Mat srcImageGray;
		cvtColor(srcImageStretched, srcImageGray, CV_RGB2GRAY);
		
		std::vector<TKeyPoints> keyPointsArray;
		std::vector<TConstMat> images;

		timeUsedMs.clear();
		keyPointsN.clear();

		for(auto type : detectorTypes) {
			auto timeStart = milliseconds();
			TKeyPoints keyPoints;
			Matcher::compare(srcImageGray, Settings::getDetType((ParamType)type), settings, keyPoints, mask);
			keyPointsArray.push_back(keyPoints);
			timeUsedMs.push_back(milliseconds() - timeStart);
			keyPointsN.push_back(keyPoints.size());
			images.push_back(srcImage);
		}

		const auto scaleFInv = 1.0 / scaleF;
		for(size_t index = 0; index < images.size(); ++index) {
			for(TKeyPoints::iterator it = keyPointsArray[index].begin(); it != keyPointsArray[index].end(); ++it) {
				it->pt.x = it->pt.x * scaleFInv;
				it->pt.y = it->pt.y * scaleFInv;
			}
		}

		auto display = std::unique_ptr<Display>(new Display(images, srcImage.size(), 0, 0, srcImage.type()));
		display->clear();
		display->generateSideBySideImage();

		for(size_t index = 0; index < images.size(); ++index) {
			display->drawKeyPoints(keyPointsArray[index], index);
		}
		sideBySideImage.create(display->getDestImage().size(), display->getDestImage().type());
		display->getDestImage().copyTo(sideBySideImage);
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}

}


} // imgalign