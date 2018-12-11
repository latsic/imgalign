

#include "ImageUtils.h"
#include "../precomp.hpp"
#include <opencv2/imgproc.hpp>
#include "opencv2/imgproc/types_c.h"
#include "LogUtils.h"

namespace imgalign
{

void ImageUtils::blendImages(
  TConstMat& imageSrc,
  TConstMat& imageDst,
  float blendValueSrc,
  int doOverlay,
  TConstPoints& polygonMask,
  TMat& blendedMat)
{
  FUNCLOGTIMEL("ImageUtils::blendImages");

  cv::Mat mask = cv::Mat::zeros(imageDst.size(), CV_8UC1);
	if(polygonMask.size() > 0) {
		auto *pts = &polygonMask[0];
		int ptsN = (int)polygonMask.size();
		fillPoly(mask, &pts, &ptsN, 1, cv::Scalar::all(255));
	}

  blendImages(imageSrc, imageDst, blendValueSrc, doOverlay, mask, blendedMat);
}
void ImageUtils::blendImages(
  TConstMat& imageSrc,
  TConstMat& imageDst,
  float blendValueSrc,
  int doOverlay,
  TMat& mask,
  TMat& blendedMat)
{
  FUNCLOGTIMEL("ImageUtils::blendImages");

  CV_Assert(imageSrc.size() == imageDst.size());
  CV_Assert(imageSrc.type() == imageDst.type());
  CV_Assert(blendedMat.size() == imageDst.size());
  CV_Assert(blendedMat.type() == imageDst.type());
  CV_Assert(mask.size() == imageDst.size());
  CV_Assert(imageSrc.channels() == 4);

  auto blendValueDst = 1.0 - blendValueSrc; 

  cv::MatConstIterator_<cv::Vec4b> itSrc = imageSrc.begin<cv::Vec4b>();
  cv::MatConstIterator_<cv::Vec4b> itDst = imageDst.begin<cv::Vec4b>();
  cv::MatIterator_<cv::Vec4b> itBlend = blendedMat.begin<cv::Vec4b>();
  cv::MatConstIterator_<uint8_t> itMask = mask.begin<uint8_t>();
  
  for(; itSrc != imageSrc.end<cv::Vec3b>(); ++itSrc, ++itDst, ++itMask, ++itBlend) {
    if((*itMask) > 0) {
      (*itBlend)[0] = blendValueDst * (*itDst)[0] + blendValueSrc * (*itSrc)[0];
      (*itBlend)[1] = blendValueDst * (*itDst)[1] + blendValueSrc * (*itSrc)[1];
      (*itBlend)[2] = blendValueDst * (*itDst)[2] + blendValueSrc * (*itSrc)[2];
      (*itBlend)[3] = blendValueDst * (*itDst)[3] + blendValueSrc * (*itSrc)[3];
    }
    else {
      (*itBlend)[0] = doOverlay ? (*itDst)[0] : blendValueDst * (*itDst)[0];
      (*itBlend)[1] = doOverlay ? (*itDst)[1] : blendValueDst * (*itDst)[1];
      (*itBlend)[2] = doOverlay ? (*itDst)[2] : blendValueDst * (*itDst)[2];
      (*itBlend)[3] = doOverlay ? (*itDst)[3] : blendValueDst * (*itDst)[3];
    }
  }
}

void ImageUtils::clearPolygonInvFromImage(
  TConstMat& imageSrc,
  TConstPoints& polygonMask,
  TMat& outPolygonImage,
  TMat& outPolygonMask)
{
  FUNCLOGTIMEL("ImageUtils::clearPolygonInvFromImage");

  outPolygonMask = cv::Mat::zeros(imageSrc.size(), CV_8UC1);
  outPolygonImage = cv::Mat::zeros(imageSrc.size(), imageSrc.type());

	if(polygonMask.size() > 0) {
		auto *pts = &polygonMask[0];
		int ptsN = (int)polygonMask.size();
		fillPoly(outPolygonMask, &pts, &ptsN, 1, cv::Scalar::all(255));
	}

  imageSrc.copyTo(outPolygonImage, outPolygonMask);
}

void ImageUtils::floodFillMask(
  TConstMat& imageSrc,
  cv::Point ptSeed,
  int floodFillTolerance,
  TMat& outMask)
{
  FUNCLOGTIMEL("ImageUtils::floodFillMask");

  if( ptSeed.x >= imageSrc.size().width
   || ptSeed.y >= imageSrc.size().height
   || ptSeed.x < 0
   || ptSeed.y < 0) {

     outMask = cv::Mat::zeros(imageSrc.size(), CV_8UC1);
     return;
  }
  CV_Assert(imageSrc.channels() == 4);

  cv::Mat imageSrcRgb;
  cvtColor(imageSrc, imageSrcRgb, CV_RGBA2RGB);

  cv::Mat mask = cv::Mat::zeros(
    cv::Size(
      imageSrcRgb.size().width + 2,
      imageSrcRgb.size().height + 2),
    CV_8UC1);

  floodFill(
    imageSrcRgb,
    mask,
    ptSeed,
    cv::Scalar::all(255),
    nullptr,
    cv::Scalar::all(floodFillTolerance),
    cv::Scalar::all(floodFillTolerance),
    CV_FLOODFILL_MASK_ONLY);

    cv::Rect roi(1, 1, mask.size().width - 2, mask.size().height - 2);
    outMask = mask(roi);
}

void ImageUtils::floodFillMask(
  TConstMat& imageSrc,
  std::vector<cv::Point> ptsSeed,
  std::vector<int> &floodFillTolerances,
  TMat& outMask)
{
  FUNCLOGTIMEL("ImageUtils::floodFillMask");

  CV_Assert(ptsSeed.size() == floodFillTolerances.size());

  std::vector<TMat> masks(ptsSeed.size());
  std::vector<TMat>::iterator itMask = masks.begin();
  std::vector<int>::iterator itTolerance = floodFillTolerances.begin();
  for(const auto &pt : ptsSeed) {

    CV_Assert(*itTolerance < 256);
    floodFillMask(imageSrc, pt, *itTolerance, *itMask);
    ++itTolerance;
    ++itMask;
  }

  outMask = cv::Mat::zeros(imageSrc.size(), CV_8UC1);
  for(itMask = masks.begin(); itMask != masks.end(); ++itMask) {

    CV_Assert((*itMask).size().width == outMask.size().width);
    CV_Assert((*itMask).size().height == outMask.size().height);

    for(int y = 0; y < itMask->rows; ++y){
      for(int x = 0; x < itMask->cols; ++x){
        

        if(itMask->at<uint8_t>(y, x) > 0) {
          outMask.at<uint8_t>(y, x) = itMask->at<uint8_t>(y, x);
        }
      }
    }
  }
}

std::vector<cv::Point> ImageUtils::transformPts(
  const std::vector<cv::Point> &pts,
  TConstMat &transMatrix)
{
  FUNCLOGTIMEL("ImageUtils::transformPts");

  CV_Assert(pts.size() > 0);

  std::vector<cv::Point2f> ptsSrcF;
  for(const auto &pt : pts) {
    ptsSrcF.push_back(cv::Point2f(pt.x, pt.y));
  }
  std::vector<cv::Point2f> ptsDstF(ptsSrcF.size());
  perspectiveTransform(ptsSrcF, ptsDstF, transMatrix);

  std::vector<cv::Point> ptsDst;
  for(const auto &pt : ptsDstF) {
    ptsDst.push_back(cv::Point((int)pt.x, (int)pt.y));
  }

  return ptsDst;
}

double ImageUtils::resize(
	const cv::Mat &image,
  cv::Mat &matOut,
  double maxPixelsN)
{
  FUNCLOGTIMEL("ImageUtils::resize");

  if(image.cols * image.rows <= maxPixelsN)
  {
    image.copyTo(matOut);
    return 1.0;
  }

  double resizeFactor = std::sqrt(maxPixelsN / (image.cols * image.rows));
  cv::resize(
    image,
    matOut,
    cv::Size(0, 0),
    resizeFactor,
    resizeFactor,
    cv::INTER_AREA);

	return resizeFactor;
}

} // imgalign