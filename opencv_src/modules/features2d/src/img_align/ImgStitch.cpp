
#include "../precomp.hpp"
#include "MultiStitcher.h"
#include "ImageUtils.h"
#include "WarperHelper.h"
#include "Display.h"
#include "LogUtils.h"
#include "Settings.h"
#include "EnumTypes.h"
#include "TimeUtils.h"

#include <opencv2/imgproc.hpp>
#include "opencv2/imgproc/types_c.h"

#include <stdio.h>
#include <ostream>
#include <iostream>
#include <memory>
#include <limits>
#include <iomanip>


namespace cv
{

ImgStitch::~ImgStitch() {}

void ImgStitch::set(
	CV_IN_OUT std::vector<int> &/*valueTypes*/,
	CV_IN_OUT std::vector<float> &/*values*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

int ImgStitch::stitch(
  CV_IN_OUT std::vector<float> &/*fieldsOfView*/,
  OutputArray /*stitchedImage*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgStitch::stitchStart(
  CV_IN_OUT std::vector<float> &/*fieldsOfView*/,
  OutputArray /*stitchedImage*/,
  CV_IN_OUT std::vector<int> &/*stitchIndices/*)
{
  CV_Error(Error::StsNotImplemented, "");
}

int ImgStitch::stitchNext(OutputArray /*stitchedImage*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

} // namespace cv


namespace imgalign
{

class ImgStitch_Impl : public cv::ImgStitch
{
	public:

    ImgStitch_Impl(cv::InputArrayOfArrays images);

    void set(
			CV_IN_OUT std::vector<int> &valueTypes,
			CV_IN_OUT std::vector<float> &values) CV_OVERRIDE;


    int stitch(
      CV_IN_OUT std::vector<float> &fieldsOfView,
      cv::OutputArray stitchedImage) CV_OVERRIDE;

    void stitchStart(
      CV_IN_OUT std::vector<float> &fieldsOfView,
      cv::OutputArray stitchedImage,
      CV_IN_OUT std::vector<int> &stitchIndices) CV_OVERRIDE;

    int stitchNext(cv::OutputArray stitchedImage) CV_OVERRIDE;
  
  private:
    void createStitcher();

    std::unique_ptr<MultiStitcher> spMultiStitcher;
    std::vector<TMat> srcImages;
    Settings settings;

    int currentStitchIndex = 0;
};

} // imgalign

namespace cv
{

Ptr<ImgStitch> ImgStitch::create(InputArrayOfArrays images)
{	
	return makePtr<imgalign::ImgStitch_Impl>(images);
}

} // cv

using namespace cv;

namespace imgalign
{

ImgStitch_Impl::ImgStitch_Impl(InputArrayOfArrays images)
{
	FUNCLOGTIMEL("ImgStitch_Impl::ImgStitch_Impl");
	try {

    LogUtils::getLog() << std::setprecision(8);

    auto imagesN = (int)images.total();  
    for(auto i = 0; i < imagesN; ++i) {
      TMat image;
      images.getMat(i).copyTo(image);
      srcImages.push_back(image);
    }
  }
	catch(std::exception &e) {
		LogUtils::getLogUserError() << e.what() << std::endl;
		throw e;
	}
}

void ImgStitch_Impl::set(
	CV_IN_OUT std::vector<int> &valueTypes,
	CV_IN_OUT std::vector<float> &values)
{
	FUNCLOGTIMEL("ImgStitch_Impl::set");

	try {
		settings.setValues(valueTypes, values);

    LogUtils::isDebug = false;
    if(settings.getValue(eLogInfoEnabled) > 0.0) {
      LogUtils::isDebug = true;
    }
	}
	catch(std::exception &e) {
		LogUtils::getLogUserError() << e.what() << std::endl;
		throw e;
	}
}

int ImgStitch_Impl::stitch(
  CV_IN_OUT std::vector<float> &fieldsOfView,
  OutputArray stitchedImage)
{
  FUNCLOGTIMEL("ImgStitch_Impl::stitch");

  try {

    createStitcher();
    std::vector<double> fieldsOfViewdD;
    for(auto it = fieldsOfView.begin(); it != fieldsOfView.end(); ++it) {
      fieldsOfViewdD.push_back(*it);
    }
    std::vector<int> stitchIndices;
    spMultiStitcher->initStiching(fieldsOfViewdD, stitchIndices);
    int stitchCount = spMultiStitcher->stitchAll();
    spMultiStitcher->getStitchedImage().copyTo(stitchedImage.getMatRef());
    return stitchCount;
  }
  catch(std::exception &e) {
		LogUtils::getLogUserError() << e.what() << std::endl;
		throw e;
	}
}

void ImgStitch_Impl::stitchStart(
  CV_IN_OUT std::vector<float> &fieldsOfView,
  OutputArray stitchedImage,
  CV_IN_OUT std::vector<int> &stitchIndices)
{
  FUNCLOGTIMEL("ImgStitch_Impl::stitchStart");
  try {
    createStitcher();
    stitchIndices.clear();
    std::vector<double> fieldsOfViewdD;
    for(auto it = fieldsOfView.begin(); it != fieldsOfView.end(); ++it) {
      fieldsOfViewdD.push_back(*it);
    }
    
    if(spMultiStitcher->initStiching(fieldsOfViewdD, stitchIndices)) {
      currentStitchIndex = 0;
      spMultiStitcher->getStitchedImageCurrent().copyTo(stitchedImage.getMatRef());
    }
  }
  catch(std::exception &e) {
		LogUtils::getLogUserError() << e.what() << std::endl;
		throw e;
	}
}

int ImgStitch_Impl::stitchNext(OutputArray stitchedImage)
{
  FUNCLOGTIMEL("ImgStitch_Impl::stitchNext");
  
  if(spMultiStitcher == nullptr) return 0;
  
  try {

    const auto &stitchOrder = spMultiStitcher->getStitchOrder();

    if(currentStitchIndex >= (int)stitchOrder.size() - 1) {
      return 0;
    }

    const StitchInfo &stitchInfo = *stitchOrder[++currentStitchIndex];
    if(!spMultiStitcher->stitchNext(stitchInfo)) {
      return -1;
    }

    auto allStitchesDone = (int)(stitchOrder.size() - 1) == currentStitchIndex;
    TConstMat &stitchedImageRef = allStitchesDone
      ? spMultiStitcher->getStitchedImage()
      : spMultiStitcher->getStitchedImageCurrent();

    if(allStitchesDone) {
      spMultiStitcher->releaseStitchedData();
    }

    if(!stitchedImageRef.empty()) {
      stitchedImageRef.copyTo(stitchedImage.getMatRef());
    }

    if(currentStitchIndex == (int)stitchOrder.size() - 1) {
      LogUtils::getLogUserInfo() << "Finalizing" << std::endl;
    }
    else {
      LogUtils::getLogUserInfo() << "Stitching "
      << stitchInfo.srcImageIndex << "->" << stitchInfo.dstImageIndex << ", "
      << currentStitchIndex << "/" << stitchOrder.size() << ", "
      << "w/h: " << stitchedImage.getMatRef().size().width << "/" << stitchedImage.getMatRef().size().height
      << std::endl;
    }

    spMultiStitcher->releaseStitchedImage();

    return currentStitchIndex + 1;
  }
  catch(std::exception &e) {
		LogUtils::getLogUserError() << e.what() << std::endl;
		throw e;
	}
}

void ImgStitch_Impl::createStitcher()
{
  FUNCLOGTIMEL("ImgStitch_Impl::createStitcher");
  
  if(spMultiStitcher != nullptr) return;
  spMultiStitcher = std::unique_ptr<MultiStitcher>(
    new MultiStitcher(srcImages, settings));
}

}