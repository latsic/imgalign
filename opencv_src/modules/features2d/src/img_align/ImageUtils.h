
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
  		double blendValueSrc,
			int doOverlay,
  		TConstPoints& polygonMask,
			TMat& blendedMat);

		static void blendImages(
			TConstMat& imageSrc,
  		TConstMat& imageDst,
  		double blendValueSrc,
			int doOverlay,
  		TMat mask,
			TMat& blendedMat);

		static void blendImages(
			TConstMat& imageSrc,
  		TConstMat& imageDst,
  		double blendValueSrc,
			int doOverlay,
			TMat& blendedMat);
		
		static void blendImagesAlphaWhereOverlap(
			TConstMat& imageSrc,
  		TConstMat& imageDst,
  		double blendValueSrc,
			TMat& blendedMat);

		static void dimImage(
			TConstMat& imageSrc,
			double dimValue,
			TMat& dimmedMat);

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

		// static void blendMultiBand(
		// 	TConstMat &src1,
		// 	TConstMat &src2,
		// 	TConstMat &mask1,
		// 	TConstMat &mask2,
		// 	bool blend,
		// 	TMat& outImage);

		static void blendMultiBand(
			const std::vector<TMat> &images,
			const std::vector<TMat> &masks,
			const std::vector<cv::Point> &tlCorners,
			bool blend,
			TMat &outImage);

		static void compensateExposure(
			TConstPoints &corners,
			const std::vector<cv::UMat> &masks,
			std::vector<TMat> &images);

		static double resize(
			const cv::Mat& image,
			cv::Mat& matOut,
			double maxPixelsN);

		static void stitch(
			TConstMat &src1,
			TConstMat &src2,
			bool blend,
			bool exposureCompensate,
			TMat &outDst);

		static void stitch(
			std::vector<TMat> &images,
			std::vector<TMat> &masks,
			const std::vector<cv::Point> &tlCorners,
			bool blend,
			bool exposureCompensate,
			TMat &outDst);

		static void stitchFast(
			const std::vector<TMat> &images,
			const std::vector<TMat> &masks,
			const std::vector<cv::Point> &tlCorners,
			TMat &outDst);

		static cv::Point crop(
			TConstMat &src,
			TMat &outDst);

		static TMat colorTransfer(
			TConstMat &src,
			TConstMat &dst);

		static cv::Size bbox(
			const std::vector<TMat> &images,
  		const std::vector<cv::Point> &tlCorners);

		static bool estimateCorners(
			TConstMat &srcImage,
			cv::Point2f &tl, cv::Point2f &tr, cv::Point2f &br, cv::Point2f &bl);

		static bool rectify(TConstMat &srcImage, TMat &dstImage);
};

} //imgalign


#endif