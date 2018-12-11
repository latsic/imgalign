
#include "Display.h"

#include <iostream>
#include <sstream>
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/imgproc_c.h" // for CV_AA
#include "LogUtils.h"

using namespace cv;

namespace imgalign
{

Display*
Display::display = NULL;

Display*
Display::createDisplay(TConstImages& images, const cv::Size& targetSizePerImage, int borderX, int borderY, int type)
{
	if(display != NULL){
		display->destroyDisplay();
		delete display;
	}
	display = new Display(images, targetSizePerImage, borderX, borderY, type);
	return display;
}

void
Display::destroyDisplay()
{
}

Display*
Display::getDisplay()
{
	return display;
}

Display::Display(TConstImages& images, const Size& targetSizePerImage, int borderX, int borderY, int type)
	: images(images)
	, targetSizePerImage(targetSizePerImage)
	, borderX(borderX)
	, borderY(borderY)
{
	destImage = Mat(Size(images.size() * targetSizePerImage.width + borderX * 2, targetSizePerImage.height + borderY * 2), type);
}

Display::~Display()
{
}

void
Display::clear()
{
	destImage = Mat::zeros(destImage.size(), destImage.type());
}

cv::Mat &Display::getDestImage()
{
	return destImage;
}

double
Display::lastImageScaleFactor() const
{
	const double currScalFactorX = static_cast<double>(targetSizePerImage.width) / images.back().cols;
	const double currScalFactorY = static_cast<double>(targetSizePerImage.height) / images.back().rows;

	double maxScaleFactor = 1.0;

	if(maxScaleFactor > currScalFactorX) maxScaleFactor = currScalFactorX;
	if(maxScaleFactor > currScalFactorY) maxScaleFactor = currScalFactorY;

	return maxScaleFactor;
}

double
Display::findMaximizedScalFactor() const
{

	double maxScaleFactor = 1;

	for(std::vector<const Mat>::const_iterator it = images.begin(); it != images.end(); ++it){

		const double currScalFactorX = static_cast<double>(targetSizePerImage.width) / it->cols;
		const double currScalFactorY = static_cast<double>(targetSizePerImage.height) / it->rows;

		if(maxScaleFactor > currScalFactorX) maxScaleFactor = currScalFactorX;
		if(maxScaleFactor > currScalFactorY) maxScaleFactor = currScalFactorY;
	}

	return maxScaleFactor;
}

TRect
Display::lastDestRect(double inScaleFactor) const
{
	const int x0 = (images.size() - 1) * targetSizePerImage.width;

	const int width  = static_cast<int>(images.back().cols * inScaleFactor);
	const int height = static_cast<int>(images.back().rows * inScaleFactor);

	TRect rect;

	rect.x     = x0 + std::abs(targetSizePerImage.width - width) / 2;
	rect.width = width;

	rect.y      = std::abs(targetSizePerImage.height- height) / 2;
	rect.height = height;

	return rect;
}

TRects
Display::findDestRects(double inScaleFactor) const
{
	TRects rects(images.size());
	
	TRects::iterator itRects = rects.begin();
	int currentX = 0;
	for(std::vector<const Mat>::const_iterator it = images.begin(); it != images.end(); ++it, ++itRects, currentX += targetSizePerImage.width){

		const int width  = static_cast<int>(it->cols * inScaleFactor);
		const int height = static_cast<int>(it->rows * inScaleFactor);

		itRects->x     = currentX + std::abs(targetSizePerImage.width - width) / 2;
		itRects->width = width;

		itRects->y      = std::abs(targetSizePerImage.height- height) / 2;
		itRects->height = height;

	}

	return rects;
}

void
Display::generateSideBySideImage()
{	
	const std::vector<Rect> rects = findDestRects(findMaximizedScalFactor());

	std::vector<Rect>::const_iterator itRects = rects.begin();
	size_t index = 0;
	for(std::vector<const Mat>::const_iterator it = images.begin(); it != images.end(); ++it, ++itRects, ++index){

		TRect currentRect;	
		if(index == images.size() - 1){
			currentRect = lastDestRect(lastImageScaleFactor());
		}
		else{
			currentRect = *itRects;
		}

		TRect destRect(borderX + currentRect.x, borderY + currentRect.y, currentRect.width, currentRect.height);
		Mat destImageRoi(destImage, destRect);

		Mat srcImageResized;
		cv::resize(*it, srcImageResized, Size(currentRect.width, currentRect.height), INTER_LANCZOS4);
		
		if(srcImageResized.type() != destImage.type()){

			Mat colorMat(srcImageResized.size(), destImage.type());
			cvtColor(srcImageResized, colorMat, CV_GRAY2BGR);
			srcImageResized = colorMat;
		}

		srcImageResized.copyTo(destImageRoi);
	}		

}

void
Display::drawTargetRects()
{
	for(size_t i = 0; i < images.size(); ++i){

		Point tl(borderX + i * targetSizePerImage.width, borderY);
		Point br(borderX + (i + 1) * targetSizePerImage.width -1, borderY + targetSizePerImage.height - 1);

		rectangle(destImage, tl, br,  Scalar( 255, 255, 0, 255 ));
	}
}

void
Display::drawDestRects()
{
	TConstRects rects = findDestRects(findMaximizedScalFactor());
	
	for(size_t i = 0; i < images.size(); ++i){

		if(i == images.size() - 1){
			
			rectangle(destImage, lastDestRect(lastImageScaleFactor()) + Point(borderX, borderY), Scalar( 0, 0, 255, 255 ));
		}
		else{
			rectangle(destImage, rects.at(i) + Point(borderX, borderY), Scalar( 0, 0, 255, 255 ));
		}
	}
}

Point
Display::convert(const Point& pt, size_t imageIndex) const
{
	if(imageIndex == images.size() - 1){
		
		const double scaleFactor = lastImageScaleFactor();
		const Rect rect = lastDestRect(scaleFactor);
		return Point(static_cast<int>(rect.x + pt.x * scaleFactor) + borderX, static_cast<int>(rect.y + pt.y * scaleFactor) + borderY);
	}
	else{
		const double scaleFactor = findMaximizedScalFactor();
		const Rect rect = findDestRects(scaleFactor).at(imageIndex);
		return Point(static_cast<int>(rect.x + pt.x * scaleFactor) + borderX, static_cast<int>(rect.y + pt.y * scaleFactor) + borderY);
	}
}

void
Display::drawLines(TConstPoints2f& pts1, TConstPoints2f& pts2, size_t i1, size_t i2, TColor /*color*/)
{
	if(i1 >= images.size() || i2 >= images.size()){
		LogUtils::getLog() << "Display::drawLines: Wrong images index" << std::endl;
		return;
	}
	if(pts1.size() == pts2.size())
	{
		LogUtils::getLog() << "Display::drawLines: Wrong number of points" << std::endl;
		return;
	}

	uint8_t a = 0;
	float step = 256.0 / pts1.size();
	for(uint32_t i = 0; i < pts1.size(); ++i, ++a)
	{
		Point pt1 = convert(pts1[i], i1);
		Point pt2 = convert(pts2[i], i2);

		line(destImage, pt1, pt2, Scalar((uint8_t)a, 255, 0, 255), 1, CV_AA);
		a += step;
	}
}
void
Display::drawLines(TConstPoints& pts1, TConstPoints& pts2, size_t i1, size_t i2, TColor /*color*/)
{
	if(i1 >= images.size() || i2 >= images.size()){
		LogUtils::getLog() << "Display::drawLines: Wrong images index" << std::endl;
		return;
	}
	if(pts1.size() != pts2.size())
	{
		LogUtils::getLog() << "Display::drawLines: Wrong number of points" << std::endl;
		return;
	}

	uint8_t a = 0;
	float step = 256.0 / pts1.size();
	for(uint32_t i = 0; i < pts1.size(); ++i, ++a)
	{
		Point pt1 = convert(pts1[i], i1);
		Point pt2 = convert(pts2[i], i2);

		line(destImage, pt1, pt2, Scalar((uint8_t)a, 255, 0, 255), 1, CV_AA);
		a += step;
	}
}

void
Display::drawLines(
	const TKeyPoints& keyPoints1, const TKeyPoints& keyPoints2,
	const TMatches& matches, size_t i1, size_t i2, const Mat& mask)
{
	if(i1 >= images.size() || i2 >= images.size()){
		LogUtils::getLog() << "Wrong images index";
	}

	int counter = 1;
	for(TMatches::const_iterator it = matches.begin(); it != matches.end(); ++it, ++counter){

		Point pt1 = convert(keyPoints1.at(it->queryIdx).pt, i1);
		Point pt2 = convert(keyPoints2.at(it->trainIdx).pt, i2);

		uchar colorChannelValue1 = static_cast<uchar>(static_cast<double>(counter) / matches.size() * 255.0);

		uchar colorChannelValue2 = 255;
		const uchar val = mask.at<uchar>(Point(0, counter - 1));
		line(destImage, pt1, pt2, Scalar( 120, 120, 120 ), 1);
		if(val == static_cast<uchar>(1)){
			colorChannelValue2 = 0;
			line(destImage, pt1, pt2, Scalar( colorChannelValue2, 0, 255 - colorChannelValue1, 255 ), 1, CV_AA);
		}
	}
}

void
Display::drawLines(TConstMatchInfos& inliers, TConstMatchInfos& outliers, size_t i1, size_t i2)
{
	int index = 0;
	for(TConstMatchInfos::const_iterator it = outliers.begin(); it != outliers.end(); ++it, ++index){

		double minDist = outliers.front().second.distance;
		double maxDist = outliers.back().second.distance;

		double distSpan = maxDist - minDist;

		uchar col = static_cast<uchar>(255 - 255 * (it->second.distance - minDist) / distSpan);

		TConstMatchInfo& matchInfo = *it;
		TConstKeyPoint& kP1 = std::get<0>(std::get<0>(matchInfo));   
		TConstKeyPoint& kP2 = std::get<1>(std::get<0>(matchInfo));
		
		Point pt1 = convert(kP1.pt, i1);
		Point pt2 = convert(kP2.pt, i2);

		line(destImage, pt1, pt2, Scalar(col, 0, 0, 255), 1, CV_AA);
	}

	index = 0;
	for(TConstMatchInfos::const_iterator it = inliers.begin(); it != inliers.end(); ++it, ++index){

		double minDist = inliers.front().second.distance;
		double maxDist = inliers.back().second.distance;

		double distSpan = maxDist - minDist;

		uchar col = static_cast<uchar>(255 * (it->second.distance - minDist) / distSpan);		
		
		
		TConstMatchInfo& matchInfo = *it;
		TConstKeyPoint& kP1 = std::get<0>(std::get<0>(matchInfo));   
		TConstKeyPoint& kP2 = std::get<1>(std::get<0>(matchInfo));
		
		Point pt1 = convert(kP1.pt, i1);
		Point pt2 = convert(kP2.pt, i2);

		line(destImage, pt1, pt2, Scalar(col, col, 255, 255), 1, CV_AA);
	}
}

void
Display::drawPoint(const cv::Point& point, size_t imageIndex, int sideLen, TColor color)
{
	const Point centerPt = convert(point, imageIndex);

	const Point tl(centerPt.x - sideLen, centerPt.y - sideLen);
	const Point br(centerPt.x + sideLen, centerPt.y + sideLen);

	rectangle(destImage, tl, br,  color);
}

void
Display::drawPoint(const cv::Point2f& point, size_t imageIndex, int sideLen, TColor color)
{
	const Point centerPt = convert(point, imageIndex);

	const Point tl(centerPt.x - sideLen, centerPt.y - sideLen);
	const Point br(centerPt.x + sideLen, centerPt.y + sideLen);

	rectangle(destImage, tl, br, color);
}

void
Display::drawPoints(TConstPoints2f& pts, size_t imageIndex, TColor color1, TColor color2)
{
	for(TConstPoints2f::const_iterator it = pts.begin(); it != pts.end(); ++it){

		drawPoint(*it, imageIndex, 2, color1);
		drawPoint(*it, imageIndex, 1, color2);
	}
}

void
Display::drawPoints(TConstPoints& pts, size_t imageIndex, TColor color1, TColor color2)
{
	for(TConstPoints::const_iterator it = pts.begin(); it != pts.end(); ++it){

		Point2f p(it->x, it->y);
		
		drawPoint(p, imageIndex, 2, color1);
		drawPoint(p, imageIndex, 1, color2);
	}
}

void
Display::drawKeyPoints(TConstKeyPoints& pts, size_t imageIndex, TColor colorS, TColor colorE)
{
	int index = 0;
	for(TConstKeyPoints::const_iterator it = pts.begin(); it != pts.end(); ++it, ++ index){

		uchar colorChannel1 = static_cast<uchar>(colorS[0] + (colorE[0] - colorS[0]) * (double)index / pts.size());
		uchar colorChannel2 = static_cast<uchar>(colorS[1] + (colorE[1] - colorS[1]) * (double)index / pts.size());
		uchar colorChannel3 = static_cast<uchar>(colorS[2] + (colorE[2] - colorS[2]) * (double)index / pts.size());

		drawPoint(it->pt, imageIndex, 2, TColor(colorChannel3, colorChannel2, colorChannel1, 255));
		drawPoint(it->pt, imageIndex, 1, TColor(colorChannel1, colorChannel2, colorChannel3, 255));
		
	}
}

} //imgalign