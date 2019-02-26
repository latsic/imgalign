
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
#include <exception>
#include <stdexcept>


namespace cv
{

ImgStitch::~ImgStitch()
{
  
}

void ImgStitch::setImages(InputArrayOfArrays /*images*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

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

int ImgStitch::stitchStart(
  CV_IN_OUT std::vector<float> &/*fieldsOfView*/,
  OutputArray /*stitchedImage*/,
  CV_IN_OUT std::vector<int> &/*stitchIndices*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

int ImgStitch::stitchNext(
  OutputArray /*stitchedImage*/,
  OutputArray /*stitchedImageSmall*/)
{
  CV_Error(Error::StsNotImplemented, "");
}

void ImgStitch::signalAbort()
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
    virtual ~ImgStitch_Impl();

    void setImages(cv::InputArrayOfArrays images) CV_OVERRIDE;

    void set(
			CV_IN_OUT std::vector<int> &valueTypes,
			CV_IN_OUT std::vector<float> &values) CV_OVERRIDE;


    int stitch(
      CV_IN_OUT std::vector<float> &fieldsOfView,
      cv::OutputArray stitchedImage) CV_OVERRIDE;

    int stitchStart(
      CV_IN_OUT std::vector<float> &fieldsOfView,
      cv::OutputArray stitchedImage,
      CV_IN_OUT std::vector<int> &stitchIndices) CV_OVERRIDE;

    int stitchNext(
      cv::OutputArray stitchedImage,
      cv::OutputArray stitchedImageSmall) CV_OVERRIDE;

    void signalAbort() CV_OVERRIDE;
  
  private:
    void createStitcher();
    void handleException(std::exception_ptr eptr);

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
  LogUtils::getLog() << std::setprecision(5);

  setImages(images);
}

ImgStitch_Impl::~ImgStitch_Impl()
{
  FUNCLOGTIMEL("ImgStitch_Impl::~ImgStitch_Impl");
}

void ImgStitch_Impl::setImages(InputArrayOfArrays images)
{
  FUNCLOGTIMEL("ImgStitch_Impl::setImages");
  try {
    auto imagesN = (int)images.total();
    srcImages.clear();
    for(auto i = 0; i < imagesN; ++i) {
      TMat image;
      images.getMat(i).copyTo(image);
      srcImages.push_back(image);
    }
  }
  catch(...) {
    handleException(std::current_exception());
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
	catch(...) {
    handleException(std::current_exception());
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
  catch(...) {
    handleException(std::current_exception());
    return 0;
  }
}

int ImgStitch_Impl::stitchStart(
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
      //spMultiStitcher->getStitchedImageCurrent().copyTo(stitchedImage.getMatRef());
      stitchedImage.getMatRef() = spMultiStitcher->getStitchedImageCurrent();
      return 1;
    }
  }
  catch(...) {
    handleException(std::current_exception());
    stitchIndices.clear();
  }
  return -1;
}

int ImgStitch_Impl::stitchNext(
  OutputArray stitchedImage,
  OutputArray stitchedImageSmall)
{
  FUNCLOGTIMEL("ImgStitch_Impl::stitchNext");
  
  if(spMultiStitcher == nullptr) return 0;
  
  try {

    const auto &stitchOrder = spMultiStitcher->getStitchOrder();

    if(currentStitchIndex >= (int)stitchOrder.size() - 1) {
      return 0;
    }

    int oldStitchIndex = currentStitchIndex;

    int liveUpdateCyle = (int)settings.getValue(eMultiStitch_liveUpdateCycle);
    if(liveUpdateCyle < 1) liveUpdateCyle = 1;
    
    int step = liveUpdateCyle;
    if(currentStitchIndex >= (int)stitchOrder.size() - 3) {
      ++currentStitchIndex;
    }
    else{
      if(currentStitchIndex + step > (int)stitchOrder.size() - 2) {
        currentStitchIndex = (int)stitchOrder.size() - 2;
      }
      else {
        currentStitchIndex += step;
      }
    }

    for(size_t i = oldStitchIndex + 1; i <= (size_t)currentStitchIndex; ++i) {

      const StitchInfo &stitchInfo = *stitchOrder[i];

      LogUtils::getLogUserInfo() << "Warping "
        << stitchInfo.srcImageIndex << "->" << stitchInfo.dstImageIndex << ", "
        << i << "/" << stitchOrder.size() << ", "
        << "w/h: " << srcImages[stitchInfo.srcImageIndex].size().width << "/" << srcImages[stitchInfo.srcImageIndex].size().height
        << std::endl;

      if(!spMultiStitcher->stitchNext(stitchInfo)) {
        return -1;
      }
    }

    auto allStitchesDone = (int)(stitchOrder.size() - 1) == currentStitchIndex;
    const StitchInfo &stitchInfo = *stitchOrder[currentStitchIndex];
    auto &imgSize = spMultiStitcher->getStitchImageCurrentOrigSize();
    LogUtils::getLogUserInfo() << "Stitching "
      << stitchInfo.srcImageIndex << "->" << stitchInfo.dstImageIndex << ", "
      << currentStitchIndex << "/" << stitchOrder.size() << ", "
      << "w/h: " << imgSize.width << "/" << imgSize.height
      << std::endl;

    TConstMat &stitchedImageRef = allStitchesDone
      ? spMultiStitcher->getStitchedImage()
      : spMultiStitcher->getStitchedImageCurrent(true);

    if(!stitchedImageRef.empty()) {
      //stitchedImageRef.copyTo(stitchedImage.getMatRef());
      stitchedImage.getMatRef() = stitchedImageRef;
    }

    if(allStitchesDone) {

      LogUtils::getLogUserInfo() << "Finalizing, "
        << "w/h: " << imgSize.width << "/" << imgSize.height
        << std::endl;

      spMultiStitcher->releaseStitchedData();

      int maxPixelsN = (int)settings.getValue(eMultiStitch_limitResultPreview);

      if(  maxPixelsN > 0
        && maxPixelsN < stitchedImageRef.size().width * stitchedImageRef.size().height) {

        ImageUtils::resize(stitchedImageRef, stitchedImageSmall.getMatRef(), maxPixelsN);
      }

      spMultiStitcher->releaseStitchedImage();
    }

    return currentStitchIndex + 1;
  }
  catch(...) {
    handleException(std::current_exception());
  }
  return -1;
}

void ImgStitch_Impl::signalAbort()
{
  FUNCLOGTIMEL("ImgStitch_Impl::signalAbort");
  if(spMultiStitcher != nullptr) {
    spMultiStitcher->signalAbort();
  }
}

void ImgStitch_Impl::createStitcher()
{
  FUNCLOGTIMEL("ImgStitch_Impl::createStitcher");
  
  if(spMultiStitcher != nullptr) return;
  spMultiStitcher = std::unique_ptr<MultiStitcher>(
    new MultiStitcher(srcImages, settings));
}

void ImgStitch_Impl::handleException(std::exception_ptr eptr)
{
  if(eptr != nullptr) {
    try {
      std::rethrow_exception(eptr);
    }
    catch(cv::Exception &e) {
      if(e.code == CV_StsNoMem) {
        LogUtils::getLogUserError() << "Insufficent memory" << std::endl;
      }
      LogUtils::getLog() << "Error code: " << e.code << ",  " << e.what() << std::endl;
      //throw e;
    }
    catch(std::exception &e) {
      LogUtils::getLogUserError() << e.what() << std::endl;
      //throw e;
    }
  }
}

}