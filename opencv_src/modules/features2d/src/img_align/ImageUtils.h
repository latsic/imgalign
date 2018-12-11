
#ifndef IMGALIGN_IMAGEUTILSH
#define IMGALIGN_IMAGEUTILSH


#include "CommonTypes.h"

namespace imgalign
{

class ImageUtils{
	public:
	
		static void blendImages(
			TConstMat& imageSrc,
  		TConstMat& imageDst,
  		float blendValueSrc,
			int doOverlay,
  		TConstPoints& polygonMask,
			TMat& blendedMat);

		static void blendImages(
			TConstMat& imageSrc,
  		TConstMat& imageDst,
  		float blendValueSrc,
			int doOverlay,
  		TMat& mask,
			TMat& blendedMat);

		static void clearPolygonInvFromImage(
			TConstMat& imageSrc,
  		TConstPoints& polygonMask,
  		TMat& inOutPolygonImage,
			TMat& outPolygonMask);

		static void floodFillMask(
			TConstMat& imageSrc,
			cv::Point ptSeed,
			int floodFillTolerance,
			TMat& outMask);
		
		static void floodFillMask(
			TConstMat& imageSrc,
			std::vector<cv::Point> ptsSeed,
			std::vector<int> &floodFillTolerances,
			TMat& outMask);
		
		static std::vector<cv::Point> transformPts(
  		const std::vector<cv::Point> &pts,
  		TConstMat &transMatrix);

		static double resize(
			const cv::Mat& image,
			cv::Mat& matOut,
			double maxPixelsN);
};

} //imgalign


#endif