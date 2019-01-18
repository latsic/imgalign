#include "../precomp.hpp"
#include "Matcher.h"
#include "ImageUtils.h"
#include "WarperHelper.h"
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
#include <limits>
#include <iomanip>


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
  double /*weight*/,
	int /*doOverlay*/,
  OutputArray /*blendImage*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getImageBlendedPolygonFixedImage(
	InputArray /*transMatrix*/,
	double /*weight*/,
	int /*doOverlay*/,
	CV_IN_OUT std::vector<Point> &/*polygon*/,
	OutputArray /*blendImage*/)
{
	CV_Error(Error::StsNotImplemented, "");
}
        
void ImgAlign::getImageBlendedPolygonMovingImage(
	InputArray /*transMatrix*/,
	double /*weight*/,
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
	double /*weight*/,
	int /*doOverlay*/,
	cv::OutputArray /*blendImage*/)
{
	CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getImageFloodFillMovingImage(
	cv::InputArray /*transMatrix*/,
	CV_IN_OUT std::vector<cv::Point> &/*seedPts*/,
	CV_IN_OUT std::vector<int> &/*floodFillTolerances*/,
	double /*weight*/,
	int /*doOverlay*/,
	cv::OutputArray /*blendImage*/)
{
	CV_Error(Error::StsNotImplemented, "");
}

void ImgAlign::getStitchedImage(
	cv::InputArray /*transMatrix*/,
	CV_IN_OUT std::vector<Point> &/*goodMatchesFixedImage*/,
	double /*weight*/,
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

double ImgAlign::stitch(
	InputArray /*transMatrix*/,
	int /*projectionType1*/,
	int /*projectionType2*/,
	int /*blend*/,
	int /*transferColors*/,
	double /*fieldOfViewFixedImage*/,
	double /*fieldOfViewMovingImage*/,
	int /*calcYaw2*/,
  int /*calcPitch2*/,
	double /*yaw1*/, double /*pitch1*/,
	double /*yaw2*/, double /*pitch2*/,
	OutputArray /*stitchedImage*/)
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
      double weight,
			int doOverlay,
      cv::OutputArray blendImage) CV_OVERRIDE;

		void getImageBlendedPolygonFixedImage(
			cv::InputArray transMatrix,
			double weight,
			int doOverlay,
			CV_IN_OUT std::vector<cv::Point> &polygon,
			cv::OutputArray blendImage) CV_OVERRIDE;
        
    void getImageBlendedPolygonMovingImage(
			cv::InputArray transMatrix,
			double weight,
			int doOverlay,
			CV_IN_OUT std::vector<cv::Point> &polygon,
			cv::OutputArray blendImage) CV_OVERRIDE;

		void getImageFloodFillFixedImage(
			cv::InputArray transMatrix,
			CV_IN_OUT std::vector<cv::Point> &seedPts,
			CV_IN_OUT std::vector<int> &floodFillTolerances,
			double weight,
			int doOverlay,
			cv::OutputArray blendImage) CV_OVERRIDE;

		void getImageFloodFillMovingImage(
			cv::InputArray transMatrix,
			CV_IN_OUT std::vector<cv::Point> &seedPts,
			CV_IN_OUT std::vector<int> &floodFillTolerances,
			double weight,
			int doOverlay,
			cv::OutputArray blendImage) CV_OVERRIDE;

		void getStitchedImage(
			cv::InputArray transMatrix,
			CV_IN_OUT std::vector<cv::Point> &goodMatchesFixedImage,
			double weight,
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

		double stitch(
			cv::InputArray transMatrix,
			int projectionType1,
			int projectionType2,
			int blend,
			int transferColors,
			double fieldOfViewFixedImage,
			double fieldOfViewMovingImage,
			int calcYaw2,
      int calcPitch2,
			double yaw1, double pitch1,
			double yaw2, double pitch2,
			cv::OutputArray stitchedImage) CV_OVERRIDE;

	private:
		void createMatcher();
		std::vector<cv::Mat> m_transformedImages;
    std::unique_ptr<Matcher> spMatcher;
		std::vector<cv::Point> _fixedImageMaskPolygon;
		std::vector<cv::Point> _movingImageMaskPolygon;
    cv::Mat fixedImageIn;
    cv::Mat movingImageIn;
    //cv::Mat fixedImageInOrig;
    //cv::Mat movingImageInOrig;
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
		
		LogUtils::getLog() << std::setprecision(8);

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

		LogUtils::isDebug = false;
    if(settings.getValue(eLogInfoEnabled) > 0.0) {
      LogUtils::isDebug = true;
    }
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

		TMat fixedImageTemp, movingImageTemp;
		cvtColor(fixedImageIn, fixedImageTemp, CV_RGBA2GRAY);
		cvtColor(movingImageIn, movingImageTemp, CV_RGBA2GRAY);

		scaleFixedImage = ImageUtils::resize(fixedImageTemp, fixedImage, maxPixelsN);
		scaleMovingImage = ImageUtils::resize(movingImageTemp, movingImage, maxPixelsN);

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
		matchSuccess = spMatcher->match();

		if(matchSuccess == 0) return 0;

		WarperHelper::warpPerspective(movingImageIn, spMatcher->getTransform(), fixedImageIn.size(), alignedImage.getMatRef());

		//alignedImage.create(fixedImageIn.size(), movingImageIn.type());
		//warpPerspective(movingImageIn, alignedImage, spMatcher->getTransform(), fixedImageIn.size());
		
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
		//LogUtils::doLog(*spMatcher);

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

		//alignedImage.create(fixedImageIn.size(), movingImageIn.type());
		WarperHelper::warpPerspective(movingImageIn, transMatrix.getMat(), fixedImageIn.size(), alignedImage.getMatRef());

		//alignedImage.create(fixedImageIn.size(), movingImageIn.type());
		//warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getImageBlended(
  InputArray transMatrix,
  double weight,
	int doOverlay,
  OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageBlended");

	try {

		Mat alignedImage;
		WarperHelper::warpPerspective(movingImageIn, transMatrix.getMat(), fixedImageIn.size(), alignedImage);

		//Mat alignedImage(fixedImageIn.size(), movingImageIn.type());
		//warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());

		blendImage.create(fixedImageIn.size(), fixedImageIn.type());
		ImageUtils::blendImages(alignedImage, fixedImageIn, weight, doOverlay, blendImage.getMatRef());
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

void ImgAlign_Impl::getImageBlendedPolygonFixedImage(
	InputArray transMatrix,
	double weight,
	int doOverlay,
	CV_IN_OUT std::vector<Point> &polygon,
	OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageBlendedPolygonFixedImage");
	
	try {

		if(polygon.empty()) {
			if(doOverlay) {
				fixedImageIn.copyTo(blendImage);
			}
			else {
				blendImage.create(fixedImageIn.size(), fixedImageIn.type());
				ImageUtils::dimImage(fixedImageIn, weight, blendImage.getMatRef());
			}
			return;
		}

		std::vector<Point> polygonScaled;
		for(auto pt : polygon) {
			polygonScaled.push_back(Point((int)(pt.x * 1.0), (int)(pt.y * 1.0)));
		}

		Mat alignedImage;
		WarperHelper::warpPerspective(movingImageIn, transMatrix.getMat(), fixedImageIn.size(), alignedImage);

		//Mat alignedImage(fixedImageIn.size(), movingImageIn.type());
		//warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());

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
	double weight,
	int doOverlay,
	CV_IN_OUT std::vector<Point> &polygon,
	OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageBlendedPolygonMovingImage");

	try {

		if(polygon.empty()) {
			if(doOverlay) {
				fixedImageIn.copyTo(blendImage);
			}
			else {
				blendImage.create(fixedImageIn.size(), fixedImageIn.type());
				ImageUtils::dimImage(fixedImageIn, weight, blendImage.getMatRef());
			}
			return;
		}

		std::vector<Point> polygonScaled;
		for(auto pt : polygon) {
			polygonScaled.push_back(Point((int)(pt.x * 1.0), (int)(pt.y * 1.0)));
		}

		cv::Mat polygonImage, polygonMask;
		ImageUtils::clearPolygonInvFromImage(movingImageIn, polygonScaled, polygonImage, polygonMask);

		Mat polygonImageAligned;
		WarperHelper::warpPerspective(movingImageIn, transMatrix.getMat(), fixedImageIn.size(), polygonImageAligned);

		//Mat polygonImageAligned(fixedImageIn.size(), movingImageIn.type());
		//warpPerspective(movingImageIn, polygonImageAligned, transMatrix, fixedImageIn.size());

		Mat polygonMaskAligned;
		WarperHelper::warpPerspective(polygonMask, transMatrix.getMat(), fixedImageIn.size(), polygonMaskAligned);

		//Mat polygonMaskAligned(fixedImageIn.size(), polygonMask.type());
		//warpPerspective(polygonMask, polygonMaskAligned, transMatrix, fixedImageIn.size());

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
	double weight,
	int doOverlay,
	cv::OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageFloodFillFixedImage");

	try {

		CV_Assert(seedPts.size() == floodFillTolerances.size());

		if(seedPts.empty()) {
			if(doOverlay) {
				fixedImageIn.copyTo(blendImage);
			}
			else {
				blendImage.create(fixedImageIn.size(), fixedImageIn.type());
				ImageUtils::dimImage(fixedImageIn, weight, blendImage.getMatRef());
			}
			return;
		}

		std::vector<Point> ptsScaled;
		for(const auto &pt : seedPts) {
			ptsScaled.push_back(Point((int)(pt.x * 1.0), (int)(pt.y * 1.0)));
		}

		TMat mask;
		ImageUtils::floodFillMask(fixedImageIn, ptsScaled, floodFillTolerances, mask);

		Mat alignedImage;
		WarperHelper::warpPerspective(movingImageIn, transMatrix.getMat(), fixedImageIn.size(), alignedImage);

		//Mat alignedImage(fixedImageIn.size(), movingImageIn.type());
		//warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());

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
	double weight,
	int doOverlay,
	OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getImageFloodFillMovingImage");

	try {

		CV_Assert(seedPts.size() == floodFillTolerances.size());

		if(seedPts.empty()) {
			if(doOverlay) {
				fixedImageIn.copyTo(blendImage);
			}
			else {
				blendImage.create(fixedImageIn.size(), fixedImageIn.type());
				ImageUtils::dimImage(fixedImageIn, weight, blendImage.getMatRef());
			}
			return;
		}

		std::vector<Point> ptsScaled;
		for(const auto &pt : seedPts) {
			ptsScaled.push_back(Point((int)(pt.x * 1.0), (int)(pt.y * 1.0)));
		}

		std::vector<Point> ptsWarped = WarperHelper::transformPts(ptsScaled, transMatrix.getMat());

		Mat alignedImage;
		WarperHelper::warpPerspective(movingImageIn, transMatrix.getMat(), fixedImageIn.size(), alignedImage);

		//Mat alignedImage(fixedImageIn.size(), movingImageIn.type());
		//warpPerspective(movingImageIn, alignedImage, transMatrix, fixedImageIn.size());

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

void ImgAlign_Impl::getStitchedImage(
	InputArray transMatrix,
	CV_IN_OUT std::vector<Point> &goodMatchesFixedImage,
	double weight,
	OutputArray blendImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::getStitchedImage");

	try {

		std::vector<Point> bBoxPtsMovingImage{
			Point(0, 0),
			Point(movingImageIn.size().width, 0),
			Point(movingImageIn.size().width, movingImageIn.size().height),
			Point(0, movingImageIn.size().height)
		};
		std::vector<Point> bBoxPtsWarpedMovingImage =
			WarperHelper::transformPts(bBoxPtsMovingImage, transMatrix.getMat());

		std::vector<Point> bBoxPtsFixedImage{
			Point(0, 0),
			Point(fixedImageIn.size().width, 0),
			Point(fixedImageIn.size().width, fixedImageIn.size().height),
			Point(0, fixedImageIn.size().height)
		};
		
		const auto iList = {
			bBoxPtsWarpedMovingImage[0], bBoxPtsWarpedMovingImage[1],
			bBoxPtsWarpedMovingImage[2], bBoxPtsWarpedMovingImage[3],
			bBoxPtsFixedImage[0], bBoxPtsFixedImage[1],
			bBoxPtsFixedImage[2], bBoxPtsFixedImage[3]
		};

		auto xMin = std::min(iList,
			[](const Point &pt1, const Point &pt2) {
				return pt1.x < pt2.x;
			});

		auto xMax = std::max(iList,
			[](const Point &pt1, const Point &pt2) {
				return pt1.x < pt2.x;
			});

		auto yMin = std::min(iList,
			[](const Point &pt1, const Point &pt2) {
				return pt1.y < pt2.y;
			});

		auto yMax = std::max(iList,
			[](const Point &pt1, const Point &pt2) {
				return pt1.y < pt2.y;
			});	
	
		auto tx = -std::min(0, xMin.x);
		auto ty = -std::min(0, yMin.y);

		Mat tTransMat;
		transMatrix.getMat().copyTo(tTransMat);
		if(xMin.x < 0 || yMin.y < 0) {

			Mat tM = Mat::eye(3,3,CV_64F);
			tM.at<double>(0,2) = tx;
			tM.at<double>(1,2)= ty;
			tTransMat = tM * transMatrix.getMat();
		}

		Mat fixedImageExpanded = Mat::zeros(Size(xMax.x - xMin.x, yMax.y - yMin.y), fixedImageIn.type());
		Mat fixedImageExpandedRoi = fixedImageExpanded(Rect(tx, ty, fixedImageIn.size().width, fixedImageIn.size().height));
		fixedImageIn.copyTo(fixedImageExpandedRoi);
		
		Mat alignedImage;
		WarperHelper::warpPerspective(movingImageIn, tTransMat, fixedImageExpanded.size(), alignedImage);

		//Mat alignedImage(fixedImageExpanded.size(), fixedImageExpanded.type());
		//warpPerspective(movingImageIn, alignedImage, tTransMat, fixedImageExpanded.size());

		if(goodMatchesFixedImage.empty()) {

			blendImage.create(fixedImageExpanded.size(), fixedImageExpanded.type());
			//ImageUtils::blendImagesCenterDistance(fixedImageExpanded, alignedImage, blendDistPx, blendImage.getMatRef());
			ImageUtils::blendImagesAlphaWhereOverlap(alignedImage, fixedImageExpanded, weight, blendImage.getMatRef());
		}
		else {
			Mat tempBlendImage(fixedImageExpanded.size(), fixedImageExpanded.type());
			ImageUtils::blendImagesAlphaWhereOverlap(alignedImage, fixedImageExpanded, weight, tempBlendImage);

			std::vector<TConstMat> images;
			images.push_back(tempBlendImage);

			auto display = std::unique_ptr<Display>(new Display(images, fixedImageExpanded.size(), 0, 0, fixedImageExpanded.type()));
			display->generateSideBySideImage();
			
			std::vector<Point> pts;
			for(auto it = goodMatchesFixedImage.begin(); it != goodMatchesFixedImage.end(); ++it) {
				pts.push_back(Point(it->x + tx, it->y + ty));
			}
			display->drawPoints(pts, 0);

			blendImage.create(display->getDestImage().size(), display->getDestImage().type());
			display->getDestImage().copyTo(blendImage);
		}
	}
	catch(std::exception &e) {
		LogUtils::getLog() << e.what() << std::endl;
		throw e;
	}
}

double ImgAlign_Impl::stitch(
	cv::InputArray /*transMatrix*/,
	int projectionType1,
	int projectionType2,
	int blend,
	int transferColors,
	double fieldOfViewFixedImage,
	double fieldOfViewMovingImage,
	int calcYaw2,
  int calcPitch2,
	double yaw1, double pitch1,
	double yaw2, double pitch2,
	OutputArray stitchedImage)
{
	FUNCLOGTIMEL("ImgAlign_Impl::stitch");

	LogUtils::getLog() << "blend: " << blend << std::endl;
	LogUtils::getLog() << "projectionType1: " << projectionType1 << std::endl;
	LogUtils::getLog() << "projectionType2: " << projectionType2 << std::endl;
	LogUtils::getLog() << "fieldOfViewFixedImage: " << fieldOfViewFixedImage << std::endl;
	LogUtils::getLog() << "fieldOfViewMovingImage: " << fieldOfViewMovingImage << std::endl;
	LogUtils::getLog() << "calcYaw2: " << calcYaw2 << std::endl;
	LogUtils::getLog() << "calcPitch2: " << calcPitch2 << std::endl;
	LogUtils::getLog() << "yaw1: " << yaw1 << std::endl;
	LogUtils::getLog() << "pitch1: " << pitch1 << std::endl;
	LogUtils::getLog() << "yaw2: " << yaw2 << std::endl;
	LogUtils::getLog() << "pitch2: " << pitch2 << std::endl;
	

	try {
		if(spMatcher == nullptr) {
			throw std::logic_error("Matcher instance is null");
		}
		
		if(fieldOfViewFixedImage <= 0) {
			throw std::logic_error("Field of view of template image must be a positive value");
		}
		if(fieldOfViewMovingImage <= 0) {
			throw std::logic_error("Field of view of moving image must be a positive value");
		}

		TMat movingImageIn1 = movingImageIn;
		if(transferColors) {
			movingImageIn1 = ImageUtils::colorTransfer(fixedImageIn, movingImageIn1);
		}

		TMat homography;
		spMatcher->getHomography(
			fieldOfViewFixedImage,
			fieldOfViewMovingImage,
			calcYaw2,
  		calcPitch2,
			yaw1, pitch1,
			yaw2, pitch2,
			projectionType1,
			projectionType2,
			homography);

		// LogUtils::logMat("original Transform", spMatcher->getTransform());
		// LogUtils::logMat("other Transform", homography);


		// spMatcher->getTransform().copyTo(homography);

		TMat fixedImageProjected;
		if((ParamType)projectionType1 != eStitch_projectionTypeNone) {
			
			TMat rotMat1;
			WarperHelper::getMatR(yaw1, pitch1, 0, rotMat1);

			WarperHelper::warpImage(
				projectionType1, fixedImageIn, fixedImageProjected, fieldOfViewFixedImage, rotMat1);

			// WarperHelper::warpImage(
			// 	projectionType1, fixedImageIn, fixedImageProjected, fieldOfViewFixedImage, yaw1, pitch1, 0);
		}
		else { 
			fixedImageProjected = fixedImageIn;
		}
	
		TMat movingImageProjected;
		if((ParamType)projectionType2 != eStitch_projectionTypeNone || (yaw1 + yaw2) != 0 || (pitch1 + pitch2) != 0) {

			TMat rotMat2;
			WarperHelper::getMatR(yaw1 + yaw2, pitch1 + pitch2, 0, rotMat2);

			WarperHelper::warpImage(
				projectionType2, movingImageIn1, movingImageProjected, fieldOfViewMovingImage, rotMat2);

			// WarperHelper::warpImage(
			// 	projectionType2, movingImageIn1, movingImageProjected, fieldOfViewMovingImage, yaw1 + yaw2, pitch1 + pitch2, 0);
		}
		else { 
			movingImageProjected = movingImageIn1;
		}


		int fw = fixedImageProjected.size().width;
		int fh = fixedImageProjected.size().height;
		int mw = movingImageProjected.size().width;
		int mh = movingImageProjected.size().height;

		double tx, ty, t, r, b, l;
		WarperHelper::getBox(
			fw, fh, mw, mh,
			homography,
			tx, ty, t, r, b, l);

		Mat fixedImageExpanded = Mat::zeros(Size((int)(r - l), (int)(b - t)), fixedImageProjected.type());
		Mat fixedImageExpandedRoi = fixedImageExpanded(Rect((int)tx, (int)ty, fw, fh));
		fixedImageProjected.copyTo(fixedImageExpandedRoi);

		Mat alignedImage;
		WarperHelper::warpPerspective(movingImageProjected, homography, fixedImageExpanded.size(), alignedImage);

		//Mat alignedImage(fixedImageExpanded.size(), fixedImageExpanded.type());
		//warpPerspective(movingImageProjected, alignedImage, homography, fixedImageExpanded.size());

		double outFieldOfViewStichedImage = WarperHelper::fieldOfView(fieldOfViewFixedImage, fieldOfViewMovingImage, yaw2);
		LogUtils::getLog() << "outFieldOfViewStichedImage: " << outFieldOfViewStichedImage << std::endl;
		
		TMat stitchedImageTemp;
		ImageUtils::stitch(fixedImageExpanded, alignedImage, blend, false, stitchedImageTemp);
		//stitchedImage.create(fixedImageExpanded.size(), fixedImageExpanded.type());
		ImageUtils::crop(stitchedImageTemp, stitchedImage.getMatRef());

		//stitchedImage.create(fixedImageExpanded.size(), fixedImageExpanded.type());
		//ImageUtils::stitch(fixedImageExpanded, alignedImage, blend, stitchedImage.getMatRef());

		return outFieldOfViewStichedImage;
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
		cvtColor(srcImageStretched, srcImageGray, CV_RGBA2GRAY);
		
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