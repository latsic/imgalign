
#ifndef IMGALIGN_MATCHERH
#define IMGALIGN_MATCHERH

#include <string>
#include <vector>

#include "CommonTypes.h"
#include "EnumTypes.h"
#include "../precomp.hpp"

namespace cv
{
	class Feature2D;
}

namespace imgalign
{

class Settings;

class Matcher
{
	public:
		class Type{

			public:
				Type(DetType inDetType, DesType inDesType, TransformFinderType inTransformFinderType, MatcherType inMatcherType);
			
				const DetType				detType;
				const DesType				desType;
				const MatcherType			matcherType;
				const TransformFinderType	transformFinderType;
		};

	public:

		static void compare(
			cv::InputArray image,
			DetType detType,
			const Settings &settings,
			TKeyPoints &outKeyPoints,
			TConstPoints &maskPolygon);

		Matcher(
			const cv::Mat& inFixedImage,
			const cv::Mat& inMovingImage,
			double scaleFactorFixedImage,
			double scaleFactorMovingImage,
			const Settings &settings,
			TConstPoints &fixedImageMaskPolygon,
			TConstPoints &movingImageMaskPolygon);
		
		int					match();
		void				detect(cv::Feature2D* feature2d, TConstMat& inImage, TKeyPoints& outKeyPoints, TConstPoints &maskPolygon, long& outTimeMs) const;
		void				compute(cv::Feature2D* feature2d, TConstMat& inImage, TKeyPoints& inKeyPoints, TMat& outMat, long& outTimeMs) const;
		void        detectAndCompute(cv::Feature2D* feature2dDet, cv::Feature2D* feature2dDes, TConstMat& inImage, TKeyPoints& outKeyPoints, TConstPoints &maskPolygon, TMat& outMat, long& outTimeDMs, long& outTimeCMs, bool isFixedImage) const;
		void				match(TConstMat& inDescriptors1, TConstMat& inDecsriptors2, TMatches& outMatches, long& outTimeMs) const;
		bool				getHomography(TConstPoints2f& inSrcPoints, TConstPoints2f& inDstPoints, TBools& outBools, TMat& outTransform, long& outTimeMs) const;


		TConstKeyPoints&	getFixedImageKeyPts() const;
		TConstKeyPoints&	getMovingImageKeyPts() const;
		TConstKeyPoints   getFixedImageInlierKeyPoints() const;
		TConstKeyPoints   getMovingImageInlierKeyPoints() const;

		TPoints2f			getGoodMatchesFixedImagePts2f() const;
		TPoints2f			getGoodMatchesMovingImageKeyPts2f() const;

		TConstMat&	getTransform() const;
		TMat				getTransformInverse() const;

		TConstMatchInfos&	getMatchesAll() const;
		TConstMatchInfos&	getMatchesFiltered() const;
		TConstMatchInfos&	getMatchesInliers() const;
		TConstMatchInfos&	getMatchesOutliers() const;

		MatcherType	        getMatcherType() const;
		const Type&         getType() const;

		std::string         getDetectorName() const;
		std::string         getDescriptorName() const;
		std::string         getMatcherName() const;
		std::string         getTransformFinderName() const;

		double				getMinDist() const;
		double				getMaxDist() const;
		double				getMaxDistFactorUsed() const;
		double        getMaxDistUsed() const;

		long				getMsDetectFixed() const;
		long				getMsDetectMoving() const;
		long				getMsComputeFixed() const;
		long				getMsComputeMoving() const;
		long				getMsMatching() const;
		long				getMsFindTransform() const;
		long				getMsTotal() const;

		long                getFixedImageWidth() const;
		long                getFixedImageHeight() const;
		long                getMovingImageWidth() const;
		long                getMovingImageHeight() const;

		double              getInliersDistMin() const;
		double              getInliersDistMax() const;
		double              getInliersDistAverage() const;
		double              getInliersDistMedian() const;
		std::vector<double> getInliersDists() const;

		double              getOutliersDistMin() const;
		double              getOutliersDistMax() const;
		double              getOutliersDistAverage() const;
		double              getOutliersDistMedian() const;
		std::vector<double> getOutliersDists() const;

		double              getAllMatchesDistMin() const;
		double              getAllMatchesDistMax() const;
		double              getAllMatchesDistAverage() const;
		double              getAllMatchesDistMedian() const;
		std::vector<double> getAllMatchesDists() const;

		double              getGoodMatchesDistMin() const;
		double              getGoodMatchesDistMax() const;
		double              getGoodMatchesDistAverage() const;
		double              getGoodMatchesDistMedian() const;
		std::vector<double> getGoodMatchesDists() const;

		double              getDistMin(TMatchInfos matchInfos) const;
		double              getDistMax(TMatchInfos matchInfos) const;
		double              getDistAverage(TMatchInfos matchInfos) const;
		double              getDistMedian(TMatchInfos matchInfos) const;
		std::vector<double> getDists(TMatchInfos matchInfos) const;

		double							getRansacThresh() const;
		double							getRansacConfidence() const;

		TConstPoints2f			getGoodMatchesFixedImagePtsOrdered() const;
		TConstPoints2f			getGoodMatchesMovingImagePtsOrdered() const;
		
		TConstPoints2f			getGoodInlierMatchesFixedImagePtsOrdered() const;
		TConstPoints2f			getGoodInlierMatchesMovingImagePtsOrdered() const;

		void getHomography(
			double fieldOfViewFixedImage,
			double fieldOfViewMovingImage,
			bool calcYaw2,
  		bool calcPitch2,
			double yaw1, double pitch1,
			double &yaw2, double &pitch2,
			int projectionType1,
			int projectionType2,
			TMat &outHomography);

	private:
		void            detectGFTT(TConstMat& inImage, TKeyPoints& outKeyPoints) const;
		void						scaleKeyPoints();

		TKeyPoints			fixedKeyPoints;
		TKeyPoints			movingKeyPoints;

		TMatchInfos			allMatches;
		TMatchInfos			filteredMatches;

		TMatchInfos			matchesInliers;
		TMatchInfos			matchesOutliers;

		TMat						transform;

		long						msDetectFixed, msDetectMoving, msComputeFixed, msComputeMoving, msMatching, msFindTransform, msTotal;
		double					minDist, maxDist, maxDistUsed, maxDistFactorUsed;

		Type						type;

		TMat						fixedImage;
		TMat						movingImage;

		double          ransacThresh, ransacConfidence;

		TPoints2f				goodMatchesFixedImagePtsOrdered;
		TPoints2f				goodMatchesMovingImagePtsOrdered;
		
		TPoints2f				goodInlierMatchesFixedImagePtsOrdered;
		TPoints2f				goodInlierMatchesMovingImagePtsOrdered;

		std::vector<cv::Point> _fixedImageMaskPolygon;
		std::vector<cv::Point> _movingImageMaskPolygon;

		double					_scaleFactorFixedImage = 1.0;
		double					_scaleFactorMovingImage = 1.0;

};


} // imgalign


#endif