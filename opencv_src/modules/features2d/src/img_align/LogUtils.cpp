
#include <iomanip>

#include "LogUtils.h"
#include "Matcher.h"


using namespace cv;

namespace imgalign
{

std::ostream&
LogUtils::getLog(){
	return std::cout;
}

long locTime = 0;

FuncLogTime::FuncLogTime(std::string inStr, long& outTime)
	: str(inStr)
	, time(outTime)
{
	LogUtils::getLog() << "s: " << inStr << std::endl;
	sTime = milliseconds();
}
FuncLogTime::FuncLogTime(std::string inStr)
	: str(inStr)
	, time(locTime)
{
	LogUtils::getLog() << "s: " << inStr << std::endl;
	sTime = milliseconds();
}

FuncLogTime::~FuncLogTime(){
	time = milliseconds() - sTime;
	LogUtils::getLog() << "e: " << str << " time [ms]: " << time << std::endl;
}


FuncLog::FuncLog(std::string inStr)
	:str(inStr)
{
	LogUtils::getLog() << "s: " << inStr << std::endl;
}

FuncLog::~FuncLog()
{
	LogUtils::getLog() << "e: " << str << std::endl;
	LogUtils::getLog() << "----------" << std::endl;
}



void
LogUtils::doLogMatches(TConstMatchInfos& matchInfos, int maxMatches)
{
	for(TConstMatchInfos::const_iterator it = matchInfos.begin(); it != matchInfos.end(); ++it){
		
		const int matchIndex = it - matchInfos.begin();

		TConstMatchInfo& matchInfo = *it;

		TConstKeyPoint& kP1 = std::get<0>(std::get<0>(matchInfo));   
		TConstKeyPoint& kP2 = std::get<1>(std::get<0>(matchInfo));
		const DMatch&   match  = std::get<1>(matchInfo);

		getLog() << "-------------------------------" << std::endl;

		getLog() << std::setfill('0') << std::setw(3) << std::setprecision(5)
			     << matchIndex << " angle "    << kP1.angle    << " octave " << kP1.octave
			                   << " response " << kP1.response << " size "   << kP1.size << std::endl;

		getLog()  << std::setfill('0')  << std::setw(3) << std::setprecision(5)
			      << matchIndex  << " angle "    << kP2.angle    << " octave " << kP2.octave
			                     << " response " << kP2.response << " size "   << kP2.size << std::endl;

		getLog() << "Matching distance: " << match.distance << std::endl;

		if(matchIndex > maxMatches){
			break;
		}
	}

	getLog() << "-------------------------------" << std::endl;
}

void
LogUtils::doLog(const Matcher& matcher, const std::string& inFileNameImage1, const std::string& inFileNameImage2)
{	
	// const int kMaxPtsInliers = 10;
	// const int kMaxPtsOutliers = 5;

	// getLog() << "Outliers:----------------------" << std::endl;
	// doLogMatches(matcher.getMatchesOutliers(), kMaxPtsOutliers);

	// getLog() << "Inliers:-----------------------" << std::endl;
	// doLogMatches(matcher.getMatchesInliers(), kMaxPtsInliers);

	// getLog() << "-------------------------------" << std::endl;
	
	if(!inFileNameImage1.empty()) {getLog() << "FixedImage:  " << inFileNameImage1 << std::endl;}
	if(!inFileNameImage2.empty()) {getLog() << "MovingImage: " << inFileNameImage2 << std::endl;}
	getLog() << "FixedImage  Size: " << matcher.getFixedImageWidth() << "/" << matcher.getFixedImageHeight() << std::endl;
	getLog() << "MovingImage Size: " << matcher.getMovingImageWidth() << "/" << matcher.getMovingImageHeight() << std::endl;
	getLog() << "number of key points on fixed image: "  << matcher.getFixedImageKeyPts().size()  << std::endl;
	getLog() << "number of key points on moving image: " << matcher.getMovingImageKeyPts().size() << std::endl;
	getLog() << "total matches: " << matcher.getMatchesAll().size() << std::endl;
	getLog() << "good matches: " << matcher.getMatchesFiltered().size() << std::endl;
	getLog() << "matches minDist/maxDist: " << matcher.getMinDist() << "/" << matcher.getMaxDist() << std::endl;
	getLog() << "maxDistFactor/maxDistUsed: " << matcher.getMaxDistFactorUsed() << "/" << matcher.getMaxDistUsed() << std::endl;
	getLog() << "Number of inliers: "  << matcher.getMatchesInliers().size()  << std::endl;
	getLog() << "number of outliers: " << matcher.getMatchesOutliers().size() << std::endl;
	getLog() << "AllMatches  minD/maxD/avrgD/medianD: "  <<  matcher.getAllMatchesDistMin()      << " / "  <<  matcher.getAllMatchesDistMax()     << " / "
		                                                 <<  matcher.getAllMatchesDistAverage()  << " / "  <<  matcher.getAllMatchesDistMedian()  << std::endl;
	getLog() << "GoodMatches minD/maxD/avrgD/medianD: "  <<  matcher.getGoodMatchesDistMin()     << " / "  <<  matcher.getGoodMatchesDistMax()    << " / "
		                                                 <<  matcher.getGoodMatchesDistAverage() << " / "  <<  matcher.getGoodMatchesDistMedian() << std::endl;
	getLog() << "Outliers    minD/maxD/avrgD/medianD: "  <<  matcher.getOutliersDistMin()        << " / "  <<  matcher.getOutliersDistMax()       << " / "
		                                                 <<  matcher.getOutliersDistAverage()    << " / "  <<  matcher.getOutliersDistMedian()    << std::endl;
	getLog() << "Inliers     minD/maxD/avrgD/medianD: "  <<  matcher.getInliersDistMin()         << " / "  <<  matcher.getInliersDistMax()        << " / "
		                                                 <<  matcher.getInliersDistAverage()     << " / "  <<  matcher.getInliersDistMedian()     << std::endl;



	getLog() << "-------------------------------" << std::endl;

	getLog() << "total time ms: " << matcher.getMsTotal() << std::endl;
	getLog() << "detection time fixed image: " << matcher.getMsDetectFixed()  << std::endl;
	getLog() << "detection time moving image: " << matcher.getMsDetectMoving() << std::endl;
	getLog() << "descriptor computing time fixed image: " << matcher.getMsComputeFixed() << std::endl;
	getLog() << "descriptor computing time moving image: " << matcher.getMsComputeMoving() << std::endl;
	getLog() << "matching time: " << matcher.getMsMatching() << std::endl;
	getLog() << "transform find time: " << matcher.getMsFindTransform() << std::endl;
	getLog() << "-------------------------------" << std::endl;
	getLog() << "Detector: " << matcher.getDetectorName() << std::endl;
	getLog() << "Descriptors: " << matcher.getDescriptorName() << std::endl;
	getLog() << "Matcher: " << matcher.getMatcherName() << std::endl;
	getLog() << "TransformFinderType: " << matcher.getTransformFinderName() << std::endl;
	getLog() << "Ransac Input Thresh/Confidence: " << matcher.getRansacThresh() << "/" << matcher.getRansacConfidence() << std::endl;
	getLog() << "-------------------------------" << std::endl;

	if(!matcher.getTransform().empty()){

		TConstMat& transform = matcher.getTransform();

		getLog() << "Transformation matrix: [ ";

		for(int y = 0; y < transform.size().height; ++y){
			
			getLog() << "(";

			for(int x = 0; x < transform.size().width; ++x){

				double v = transform.at<double>(x, y);

				getLog() << std::fixed << std::setprecision(5) << v;
				if(x < transform.size().width - 1) getLog() << ", ";
			}
			getLog() << ")";
			if(y < transform.size().height - 1) getLog() << ", ";

		}
		getLog() << " ]" << std::endl;

	}

	if(matcher.getTransform().empty()){
		getLog() << "-----------------------" << std::endl;
		getLog() << "No Transformation matrix found!" << std::endl;
		getLog() << "-----------------------" << std::endl;
	}
	else if(matcher.getMatchesInliers().size() * 5 < matcher.getMatchesOutliers().size()){
		
		getLog() << "-----------------------" << std::endl;
		getLog() << "Transformation probably incorrect (inlier/outlier relation....)!" << std::endl;
		getLog() << "-----------------------" << std::endl;
	}
	if(!inFileNameImage2.empty()) {getLog() << "MovingImage: " << inFileNameImage2 << std::endl;}
}

} // imgalign