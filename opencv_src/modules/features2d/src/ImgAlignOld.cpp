// #include "precomp.hpp"
// #include "img_align/Matcher.h"
// #include "img_align/ImageUtils.h"
// #include "img_align/Display.h"
// #include "img_align/LogUtils.h"

// //#include "opencv2/calib3d.hpp"
// //#include "opencv2/video/tracking.hpp"
// #include "opencv2/imgproc.hpp"
// // #include "opencv2/calib3d/calib3d.hpp"
// // #include "opencv2/video/tracking.hpp"
// // #include "opencv2/imgproc/imgproc.hpp"

// //#include "opencv2/imgproc.hpp"
// #include "opencv2/imgproc/types_c.h"
// //#include "opencv2/core/types.hpp"

// #include <stdio.h>
// #include <ostream>
// #include <iostream>

// namespace {
// void whiten(TMat& image)
// {
	
// 	long pixelSum = 0;

// 	for(int x = 0; x < image.size().width; ++x){
// 		for(int y = 0; y < image.size().height; ++y){

// 			pixelSum += image.at<uchar>(cv::Point(x, y));
// 		}
// 	}

// 	uchar stdPixel = static_cast<uchar>(pixelSum / (image.size().width * image.size().height) - 20);

// 	for(int x = 0; x < image.size().width; ++x){
// 		for(int y = 0; y < image.size().height; ++y){

// 			if(image.at<uchar>(cv::Point(x, y)) >= stdPixel){
// 				image.at<uchar>(cv::Point(x, y)) = 255;
// 			}
// 		}
// 	}
// }

// void grayImageToBinary(TMat& image)
// {
// 	//whiten(image);
	
// 	TMat dstImage1(image.size(), image.type());

// 	TMat dstImage2(image.size(), image.type());

// 	//threshold(image, dstImage2, 150, 255, THRESH_BINARY);

// 	adaptiveThreshold(image, dstImage2, 255.0, (int)cv::ADAPTIVE_THRESH_MEAN_C, (int)cv::THRESH_BINARY, 151, 1.0);
	
// 	for(int x = 0; x < dstImage2.size().width; ++x){
// 		for(int y = 0; y < dstImage2.size().height; ++y){

// 			if(dstImage2.at<uchar>(cv::Point(x, y)) < 254){
// 				dstImage2.at<uchar>(cv::Point(x, y)) = image.at<uchar>(cv::Point(x, y));
// 			}
// 		}
// 	}
	
// 	image = dstImage2;

// }


// void resizeToFixedImage(TMat fixedImage, TMat& movingImage)
// {
// 	const double fixedImagePixelsN  = fixedImage.cols * fixedImage.rows;
// 	const double movingImagePixelsN = movingImage.cols * movingImage.rows;
	
// 	double resizeFactor = std::sqrt(fixedImagePixelsN / movingImagePixelsN);

	

// 	cv::Mat movingImageSmall;
// 	resize(movingImage, movingImageSmall, cv::Size(0, 0), resizeFactor, resizeFactor, cv::INTER_AREA);
// 	//movingImageSmall.assignTo(movingImage);
// 	movingImage = movingImageSmall;
// }

// void resizeToNoStretch(const TMat& image, cv::Mat &matOut, int cols, int rows)
// {


// 	const double srcImagePixelsN  = image.cols * image.rows;
// 	const double dstImagePixelsN =  cols * rows;

// 	const double resizeFactor = std::sqrt(dstImagePixelsN / srcImagePixelsN);

// 	cv::Mat newImage;
// 	resize(image, newImage, cv::Size(0, 0), resizeFactor, resizeFactor, cv::INTER_AREA);
// 	matOut = newImage;
//   //image = newImage;

// }

// TMat RGBcolorImageDiff(const TMat& inImage1, const TMat& inImage2)
// {
// 	LogUtils::getLog() << "RGBcolorImageDiff 1" << std::endl;

// 	cv::Mat imageGs1(inImage1.size(), CV_8UC1);
// 	cv::Mat imageGs2(inImage2.size(), CV_8UC1);

// 	LogUtils::getLog() << "RGBcolorImageDiff 2" << std::endl;
	
// 	cvtColor(inImage1, imageGs1, CV_BGR2GRAY);
// 	cvtColor(inImage2, imageGs2, CV_RGB2GRAY);

// 	LogUtils::getLog() << "RGBcolorImageDiff 3" << std::endl;
	
// 	TMat imageGsWhite(imageGs1.size(), imageGs1.type(), cv::Scalar::all(255));

// 	LogUtils::getLog() << "RGBcolorImageDiff 4" << std::endl;

// 	TMat imageDiffInv = imageGsWhite - (imageGs1 - imageGs2);

// 	LogUtils::getLog() << "RGBcolorImageDiff 5" << std::endl;

// 	whiten(imageDiffInv);

// 	LogUtils::getLog() << "RGBcolorImageDiff 6" << std::endl;

// 	TMat imageGSBin(imageGs1.size(), imageGs1.type());

// 	LogUtils::getLog() << "RGBcolorImageDiff 7" << std::endl;

// 	//adaptiveThreshold(imageDiffInv, imageGSBin, 255.0, (int)ADAPTIVE_THRESH_MEAN_C, (int)THRESH_BINARY, 3, 1.0);
// 	threshold(imageDiffInv, imageGSBin, 110, 255, cv::THRESH_BINARY);
// 	//TMat diffImageGs = imageGSBin - (imageGsWhite - imageGs1);

// 	LogUtils::getLog() << "RGBcolorImageDiff 8" << std::endl;

// 	TMat diffImage(inImage1.size(), inImage1.type());
	
// 	LogUtils::getLog() << "RGBcolorImageDiff 9" << std::endl;


// 	//cvtColor(diffImageGs, diffImage, CV_GRAY2BGR);
	

// 	//inImage2.copyTo(diffImage, imageGSBin);
	
// 	//cv::Mat const structure_elem = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
// 	//Mat diffGs(imageGSBin.size(), imageGSBin.type());
// 	//cv::morphologyEx(imageGSBin, diffGs, cv::MORPH_OPEN, structure_elem);

// 	cvtColor(imageGSBin, diffImage, CV_GRAY2BGR);

// 	LogUtils::getLog() << "RGBcolorImageDiff 10" << std::endl;

// 	return diffImage;
// }
// }

// namespace cv
// {

// ImgAlign::~ImgAlign() {}

// void ImgAlign::match(
// 		InputArray fixedImageIn,
// 		InputArray movingImageIn,
// 		OutputArrayOfArrays _transformedImages)
// {
// 	CV_Error(Error::StsNotImplemented, "");
// }
// void ImgAlign::match1(
// 		InputArray fixedImageIn,
// 		InputArray movingImageIn,
// 		CV_OUT std::vector<int> &widths,
// 		CV_OUT std::vector<int> &heights,
// 		CV_OUT std::vector<int> &types)
// {
// 	CV_Error(Error::StsNotImplemented, "");
// }
// void ImgAlign::getResults(CV_OUT std::vector<Mat> &mats)
// {
// 	CV_Error(Error::StsNotImplemented, "");
// }
// void ImgAlign::getResults2(OutputArrayOfArrays mats)
// {
// 	CV_Error(Error::StsNotImplemented, "");
// }
// void ImgAlign::test(InputArray inImage, OutputArray outImage)
// {
// 	CV_Error(Error::StsNotImplemented, "");
// }


// class ImgAlign_Impl : public ImgAlign
// {
// 	public:
// 		void test(InputArray inImage, OutputArray outImage);
// 		void match(
// 			InputArray fixedImageIn,
// 			InputArray movingImageIn,
// 			OutputArrayOfArrays _transformedImages) CV_OVERRIDE;
// 		void match1(
// 			InputArray fixedImageIn,
// 			InputArray movingImageIn,
// 			CV_OUT std::vector<int> &widths,
// 			CV_OUT std::vector<int> &heights,
// 			CV_OUT std::vector<int> &types) CV_OVERRIDE;
// 		void getResults(CV_OUT std::vector<Mat> &mats) CV_OVERRIDE;
// 		void getResults2(OutputArrayOfArrays mats) CV_OVERRIDE;
// 		void getResults3(CV_IN_OUT std::vector<Mat> &mats) CV_OVERRIDE;
// 	private:
// 		std::vector<Mat> m_transformedImages;
// };


// Ptr<ImgAlign> ImgAlign::create()
// {		
// 		LogUtils::getLog() << "create" << std::endl;
// 		printf("Hello World ImgAlign::create\n");
//     return makePtr<ImgAlign_Impl>();
// }


// void ImgAlign_Impl::test(InputArray inImage, OutputArray outImage)
// {
// 	outImage.create(inImage.size(), inImage.type());
// 	Mat &mat = outImage.getMatRef();
// 	inImage.copyTo(mat);
// }

// void ImgAlign_Impl::getResults2(OutputArrayOfArrays mats)
// {
// 	try {

// 	mats.clear();

// 	if(m_transformedImages.size() == 0) {
// 		LogUtils::getLog() << "ImgAlign_Impl::getResults2 No Data" << std::endl;
// 		return;
// 	}

// 	mats.create(m_transformedImages.size(), 1, m_transformedImages[0].depth());
// 		//m_transformedImages[0].depth());

// 	for(auto i = 0; i < m_transformedImages.size(); ++i) {
// 		mats.create(
// 			m_transformedImages[i].size(),
// 			m_transformedImages[i].type(), i);
// 	}

// 	std::vector<Mat> dst;
//   mats.getMatVector(dst);

// 	for(auto i = 0; i < m_transformedImages.size(); ++i) {
// 		m_transformedImages[i].copyTo(dst[i]); 
// 		LogUtils::getLog() << "getResults2 Copy Image " << i << std::endl;
// 		LogUtils::getLog() << "type " << dst[i].type() << std::endl;
// 	}

// 	}
// 	catch(std::exception &e) {
// 		LogUtils::getLog() << "exception: " << e.what() << std::endl;
// 	}
// 	catch(...)
// 	{
// 		LogUtils::getLog() << "exception: " << "unknown" << std::endl;
// 	}

// }

// void ImgAlign_Impl::getResults(CV_OUT std::vector<Mat> &mats)
// {
// 	if(mats.size() != m_transformedImages.size())
// 	{
// 		LogUtils::getLog()
// 			<< "ImgAlign_Impl::getResults: sizes of array not matching, "
// 			<< "m_transformedImages size " << m_transformedImages.size()
// 			<< "size input " << mats.size() << std::endl;
// 		return;
// 	}

// 	for(auto i = 0; i < m_transformedImages.size(); ++i) {
// 		m_transformedImages[i].copyTo(mats[i]); 
// 		LogUtils::getLog() << "getResults Copy Image " << i << std::endl;
// 	}
// }
// void ImgAlign_Impl::getResults3(CV_IN_OUT std::vector<Mat> &mats)
// {
// 	if(mats.size() != m_transformedImages.size())
// 	{
// 		LogUtils::getLog()
// 			<< "ImgAlign_Impl::getResults3: sizes of array not matching, "
// 			<< "m_transformedImages size " << m_transformedImages.size()
// 			<< "size input " << mats.size() << std::endl;
// 		return;
// 	}

// 	for(auto i = 0; i < m_transformedImages.size(); ++i) {
// 		m_transformedImages[i].copyTo(mats[i]); 
// 		LogUtils::getLog() << "getResults Copy Image " << i << std::endl;
// 	}
// }

// void ImgAlign_Impl::match(
// 	InputArray fixedImageIn,
// 	InputArray movingImageIn,
// 	OutputArrayOfArrays _transformedImages)
// {
// 	std::vector<Mat> transformedImages;
// 	_transformedImages.getMatVector(transformedImages);

// 	if(transformedImages.size() < 3) {
// 		transformedImages.resize(3);
// 	}

// 	try
// 	{
	
	
	
		

// 	LogUtils::getLog() << "match start" << std::endl;
	
// 	//resizeTo(fixedImage, 800, 600);
// 	//resizeTo(movingImage, 800, 600);

//   Mat fixedImage, movingImage; 

// 	LogUtils::getLog() << "match start1" << std::endl;

// 	CV_Assert(!fixedImageIn.empty());
// 	CV_Assert(!movingImageIn.empty());

// 	resizeToNoStretch(fixedImageIn.getMat(), fixedImage, 1000, 700);
// 	LogUtils::getLog() << "match resizeToNoStretch1" << std::endl;
// 	resizeToNoStretch(movingImageIn.getMat(), movingImage, 1000, 700);
// 	LogUtils::getLog() << "match resizeToNoStretch2" << std::endl;

// 	CV_Assert(!fixedImage.empty());
// 	CV_Assert(!movingImage.empty());

// 	//resizeTo(fixedImage, 1200, 840);
// 	//resizeTo(movingImage, 1200, 840);

// 	//resizeTo(fixedImage, 1420, 1000);
// 	//resizeTo(movingImage, 1420, 1000);

// 	Mat fixedImageGs(fixedImage.size(), CV_8UC1);
// 	Mat movingImageGs(movingImage.size(), CV_8UC1);
// 	Mat fixedImageGs2(fixedImage.size(), fixedImage.type());
// 	Mat movingImageGs2(movingImage.size(), movingImage.type());

// 	LogUtils::getLog() << "match 3" << std::endl;

	
// 	cvtColor(fixedImage, fixedImageGs, CV_BGR2GRAY);
// 	LogUtils::getLog() << "match 3.5" << std::endl;
// 	cvtColor(movingImage, movingImageGs, CV_RGB2GRAY);
	

// 	LogUtils::getLog() << "match 4" << std::endl;

// 	cvtColor(fixedImageGs, fixedImageGs2, CV_GRAY2BGR);
// 	cvtColor(movingImageGs, movingImageGs2, CV_GRAY2RGB);

// 	LogUtils::getLog() << "match 5" << std::endl;

// 	//grayImageToBinary(fixedImageGs);
// 	//grayImageToBinary(movingImageGs);

// 	Matcher matcher(fixedImageGs, movingImageGs, Matcher::DET_SIFT, Matcher::DES_SIFT, Matcher::TFT_RANSAC, Matcher::AUTO);
// 	LogUtils::getLog() << "match 6" << std::endl;
// 	matcher.match();
// 	LogUtils::getLog() << "match 7" << std::endl;
// 	//matcher.keyPts();


// 	Mat transformedImage(fixedImage.size(), fixedImage.type());
// 	if(!matcher.getTransform().empty()){
// 		warpPerspective(movingImage, transformedImage, matcher.getTransform(), fixedImage.size());
// 	}

// 	LogUtils::getLog() << "match 8" << std::endl;

// 	Mat transformedImageRaster(fixedImage.size(), fixedImage.type());
// 	TMat rasterMovingImage = ImageUtils::createRasterImage(movingImage.size(), 100, 100, 1, TColor(155, 0, 0, 255), TColor(0, 0, 155, 255));
// 	if(!matcher.getTransform().empty()){
// 		warpPerspective(rasterMovingImage, transformedImageRaster, matcher.getTransform(), fixedImage.size());
// 	}

// 	LogUtils::getLog() << "match 9" << std::endl;
    
//   TMat rasterFixedImage = ImageUtils::createRasterImage(fixedImage.size(), 100, 100, 1, TColor(255, 0, 0, 255), TColor(0, 0, 255, 255));

// 	LogUtils::getLog() << "match 10" << std::endl;

// 	double alpha = 0.5; 
// 	Mat overlayedImage(fixedImage.size(), fixedImage.type());
// 	addWeighted(fixedImage, alpha, transformedImage, 1.0 - alpha, 0.0, overlayedImage);

// 	LogUtils::getLog() << "match 11" << std::endl;

// 	Mat overlayedImageWithRaster(transformedImage.size(), transformedImage.type());
// 	Mat overlayedImageWithRaster2(transformedImage.size(), transformedImage.type());
// 	ImageUtils::overlayImage(transformedImage, transformedImageRaster, overlayedImageWithRaster, cv::Point2i(0, 0));
// 	ImageUtils::overlayImage(overlayedImageWithRaster, rasterFixedImage, overlayedImageWithRaster2, cv::Point2i(0, 0));

// 	LogUtils::getLog() << "match 12" << std::endl;


// 	std::vector<TConstMat> images;
// 	images.push_back(fixedImage);
// 	images.push_back(movingImage);
// 	//images.push_back(transformedImage);
// 	images.push_back(overlayedImageWithRaster2);
// 	images.push_back(overlayedImage);

// 	LogUtils::getLog() << "match 13" << std::endl;
	
// 	//const Size kTargetSizePerImage(1000, 1300);
// 	//const Size kTargetSizePerImage(500, 720);
// 	const Size kTargetSizePerImage(300, 720);
// 	const int  kBorderX = 5;
// 	const int  kBorderY = 5;

// 	Display* display = Display::getDisplay();

// 	if(!display){
// 		display = Display::createDisplay(images, kTargetSizePerImage, kBorderX, kBorderY, fixedImage.type());
// 	}

// 	LogUtils::getLog() << "match 14" << std::endl;

// 	display->clear();
// 	display->generateSideBySideImage();

	
// 	//display->drawTargetRects();
// 	display->drawDestRects();
	

// 	display->drawKeyPoints(matcher.getFixedImageKeyPts(), 0);
// 	display->drawKeyPoints(matcher.getMovingImageKeyPts(), 1);
// 	display->drawKeyPoints(matcher.getFixedImageInlierKeyPoints(), 2, TColor(0, 0, 255), TColor(200, 200, 255));

// 	LogUtils::getLog() << "match 15" << std::endl;

// 	display->drawLines(matcher.getMatchesInliers(), matcher.getMatchesOutliers(), 0, 1);

// 	transformedImages[0] = display->getDestImage().clone();
//   //transformedImages.push_back(display->getDestImage());

// 	display->clear();
// 	display->generateSideBySideImage();
// 	display->drawLines(matcher.getMatchesInliers(), matcher.getMatchesOutliers(), 0, 1);
// 	display->drawKeyPoints(matcher.getFixedImageInlierKeyPoints(), 2, TColor(0, 0, 255), TColor(200, 200, 255));
// 	display->drawDestRects();

// 	LogUtils::getLog() << "match 16" << std::endl;

// 	transformedImages[1] = display->getDestImage().clone();
// 	//transformedImages.push_back(display->getDestImage());

// 	// display->clear();
// 	// images.pop_back();
// 	// images.push_back(RGBcolorImageDiff(fixedImage, transformedImage));
// 	// display->generateSideBySideImage();
// 	// LogUtils::getLog() << "match 16.1" << std::endl;
// 	// display->drawLines(matcher.getMatchesInliers(), matcher.getMatchesOutliers(), 0, 1);
// 	// LogUtils::getLog() << "match 16.2" << std::endl;
// 	// //display->drawKeyPoints(matcher.getFixedImageInlierKeyPoints(), 2, TColor(0, 0, 255), TColor(200, 200, 255));
// 	// display->drawDestRects();

// 	LogUtils::getLog() << "match 17" << std::endl;

// 	//transformedImages.push_back(display->getDestImage());

// 	images.pop_back();
// 	images.push_back(transformedImage);
// 	display->clear();
// 	display->generateSideBySideImage();
// 	display->drawDestRects();

// 	LogUtils::getLog() << "match 18" << std::endl;

// 	transformedImages[2] = display->getDestImage().clone();
// 	//transformedImages.push_back(display->getDestImage());

	

// 	LogUtils::getLog() << "match 19" << std::endl;

// 	LogUtils::getLog() << "images size" << transformedImages.size() << std::endl;


// 	LogUtils::doLog(matcher);

// 	}
// 	catch(std::exception &e) {
// 		LogUtils::getLog() << "exception: " << e.what() << std::endl;
// 	}
// 	catch(...)
// 	{
// 		LogUtils::getLog() << "exception: " << "unknown" << std::endl;
// 	}

// }
// void ImgAlign_Impl::match1(
// 	InputArray fixedImageIn,
// 	InputArray movingImageIn,
// 	CV_OUT std::vector<int> &widths,
// 	CV_OUT std::vector<int> &heights,
// 	CV_OUT std::vector<int> &types)
// {
// 	try
// 	{
	

	
		

// 	LogUtils::getLog() << "match start" << std::endl;
	
// 	//resizeTo(fixedImage, 800, 600);
// 	//resizeTo(movingImage, 800, 600);

// 	m_transformedImages.clear();
// 	m_transformedImages.push_back(fixedImageIn.getMat().clone());
// 	widths.push_back(m_transformedImages.back().size().width);
// 	heights.push_back(m_transformedImages.back().size().height);
// 	types.push_back(m_transformedImages.back().type());

//   Mat fixedImage, movingImage; 

// 	LogUtils::getLog() << "match start1" << std::endl;

// 	CV_Assert(!fixedImageIn.empty());
// 	CV_Assert(!movingImageIn.empty());

// 	resizeToNoStretch(fixedImageIn.getMat(), fixedImage, 1000, 700);
// 	LogUtils::getLog() << "match resizeToNoStretch1" << std::endl;
// 	resizeToNoStretch(movingImageIn.getMat(), movingImage, 1000, 700);
// 	LogUtils::getLog() << "match resizeToNoStretch2" << std::endl;

// 	CV_Assert(!fixedImage.empty());
// 	CV_Assert(!movingImage.empty());

// 	//resizeTo(fixedImage, 1200, 840);
// 	//resizeTo(movingImage, 1200, 840);

// 	//resizeTo(fixedImage, 1420, 1000);
// 	//resizeTo(movingImage, 1420, 1000);

// 	Mat fixedImageGs(fixedImage.size(), CV_8UC1);
// 	Mat movingImageGs(movingImage.size(), CV_8UC1);
// 	Mat fixedImageGs2(fixedImage.size(), fixedImage.type());
// 	Mat movingImageGs2(movingImage.size(), movingImage.type());

// 	LogUtils::getLog() << "match 3" << std::endl;

	
// 	cvtColor(fixedImage, fixedImageGs, CV_BGR2GRAY);
// 	LogUtils::getLog() << "match 3.5" << std::endl;
// 	cvtColor(movingImage, movingImageGs, CV_RGB2GRAY);
	

// 	LogUtils::getLog() << "match 4" << std::endl;

// 	cvtColor(fixedImageGs, fixedImageGs2, CV_GRAY2BGR);
// 	cvtColor(movingImageGs, movingImageGs2, CV_GRAY2RGB);

// 	LogUtils::getLog() << "match 5" << std::endl;

// 	//grayImageToBinary(fixedImageGs);
// 	//grayImageToBinary(movingImageGs);

// 	Matcher matcher(fixedImageGs, movingImageGs, Matcher::DET_SIFT, Matcher::DES_SIFT, Matcher::TFT_RANSAC, Matcher::AUTO);
// 	LogUtils::getLog() << "match 6" << std::endl;
// 	matcher.match();
// 	LogUtils::getLog() << "match 7" << std::endl;
// 	//matcher.keyPts();


// 	Mat transformedImage(fixedImage.size(), fixedImage.type());
// 	if(!matcher.getTransform().empty()){
// 		warpPerspective(movingImage, transformedImage, matcher.getTransform(), fixedImage.size());
// 	}

// 	LogUtils::getLog() << "match 8" << std::endl;

// 	Mat transformedImageRaster(fixedImage.size(), fixedImage.type());
// 	TMat rasterMovingImage = ImageUtils::createRasterImage(movingImage.size(), 100, 100, 1, TColor(155, 0, 0, 255), TColor(0, 0, 155, 255));
// 	if(!matcher.getTransform().empty()){
// 		warpPerspective(rasterMovingImage, transformedImageRaster, matcher.getTransform(), fixedImage.size());
// 	}

// 	LogUtils::getLog() << "match 9" << std::endl;
    
//   TMat rasterFixedImage = ImageUtils::createRasterImage(fixedImage.size(), 100, 100, 1, TColor(255, 0, 0, 255), TColor(0, 0, 255, 255));

// 	LogUtils::getLog() << "match 10" << std::endl;

// 	double alpha = 0.5; 
// 	Mat overlayedImage(fixedImage.size(), fixedImage.type());
// 	addWeighted(fixedImage, alpha, transformedImage, 1.0 - alpha, 0.0, overlayedImage);

// 	LogUtils::getLog() << "match 11" << std::endl;

// 	Mat overlayedImageWithRaster(transformedImage.size(), transformedImage.type());
// 	Mat overlayedImageWithRaster2(transformedImage.size(), transformedImage.type());
// 	ImageUtils::overlayImage(transformedImage, transformedImageRaster, overlayedImageWithRaster, cv::Point2i(0, 0));
// 	ImageUtils::overlayImage(overlayedImageWithRaster, rasterFixedImage, overlayedImageWithRaster2, cv::Point2i(0, 0));

// 	LogUtils::getLog() << "match 12" << std::endl;


// 	std::vector<TConstMat> images;
// 	images.push_back(fixedImage);
// 	images.push_back(movingImage);
// 	//images.push_back(transformedImage);
// 	images.push_back(overlayedImageWithRaster2);
// 	images.push_back(overlayedImage);

// 	LogUtils::getLog() << "match 13" << std::endl;
	
// 	//const Size kTargetSizePerImage(1000, 1300);
// 	//const Size kTargetSizePerImage(500, 720);
// 	const Size kTargetSizePerImage(300, 720);
// 	const int  kBorderX = 5;
// 	const int  kBorderY = 5;

// 	Display* display = Display::getDisplay();

// 	if(!display){
// 		display = Display::createDisplay(images, kTargetSizePerImage, kBorderX, kBorderY, fixedImage.type());
// 	}

// 	LogUtils::getLog() << "match 14" << std::endl;

// 	display->clear();
// 	display->generateSideBySideImage();

	
// 	//display->drawTargetRects();
// 	display->drawDestRects();
	

// 	display->drawKeyPoints(matcher.getFixedImageKeyPts(), 0);
// 	display->drawKeyPoints(matcher.getMovingImageKeyPts(), 1);
// 	display->drawKeyPoints(matcher.getFixedImageInlierKeyPoints(), 2, TColor(0, 0, 255), TColor(200, 200, 255));

// 	LogUtils::getLog() << "match 15" << std::endl;

// 	display->drawLines(matcher.getMatchesInliers(), matcher.getMatchesOutliers(), 0, 1);


//   m_transformedImages.push_back(display->getDestImage().clone());
// 	widths.push_back(m_transformedImages.back().size().width);
// 	heights.push_back(m_transformedImages.back().size().height);
// 	types.push_back(m_transformedImages.back().type());


// 	display->clear();
// 	display->generateSideBySideImage();
// 	display->drawLines(matcher.getMatchesInliers(), matcher.getMatchesOutliers(), 0, 1);
// 	display->drawKeyPoints(matcher.getFixedImageInlierKeyPoints(), 2, TColor(0, 0, 255), TColor(200, 200, 255));
// 	display->drawDestRects();

// 	LogUtils::getLog() << "match 16" << std::endl;

// 	m_transformedImages.push_back(display->getDestImage().clone());
// 	widths.push_back(m_transformedImages.back().size().width);
// 	heights.push_back(m_transformedImages.back().size().height);
// 	types.push_back(m_transformedImages.back().type());

// 	// display->clear();
// 	// images.pop_back();
// 	// images.push_back(RGBcolorImageDiff(fixedImage, transformedImage));
// 	// display->generateSideBySideImage();
// 	// LogUtils::getLog() << "match 16.1" << std::endl;
// 	// display->drawLines(matcher.getMatchesInliers(), matcher.getMatchesOutliers(), 0, 1);
// 	// LogUtils::getLog() << "match 16.2" << std::endl;
// 	// //display->drawKeyPoints(matcher.getFixedImageInlierKeyPoints(), 2, TColor(0, 0, 255), TColor(200, 200, 255));
// 	// display->drawDestRects();

// 	LogUtils::getLog() << "match 17" << std::endl;

// 	//transformedImages.push_back(display->getDestImage());

// 	images.pop_back();
// 	images.push_back(transformedImage);
// 	display->clear();
// 	display->generateSideBySideImage();
// 	display->drawDestRects();

// 	LogUtils::getLog() << "match 18" << std::endl;

// 	m_transformedImages.push_back(display->getDestImage().clone());
// 	widths.push_back(m_transformedImages.back().size().width);
// 	heights.push_back(m_transformedImages.back().size().height);
// 	types.push_back(m_transformedImages.back().type());
	

// 	LogUtils::getLog() << "match 19" << std::endl;

// 	LogUtils::getLog() << "images size" << m_transformedImages.size() << std::endl;


// 	LogUtils::doLog(matcher);

// 	}
// 	catch(std::exception &e) {
// 		LogUtils::getLog() << "exception: " << e.what() << std::endl;
// 	}
// 	catch(...)
// 	{
// 		LogUtils::getLog() << "exception: " << "unknown" << std::endl;
// 	}

// }


// }