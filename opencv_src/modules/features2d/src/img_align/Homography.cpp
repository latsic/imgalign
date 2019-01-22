#include "Homography.h"
#include <opencv2/calib3d.hpp>
#include "Settings.h"
#include "WarperHelper.h"
#include "LogUtils.h"

#include <math.h>

using namespace cv;

namespace imgalign
{

bool Homography::getHomography(
  TConstPoints2f& inDstPoints,
  TConstPoints2f& inSrcPoints,
  TMat& outTransform)
{
  FUNCLOGTIMEL("Homography::getHomography");
  outTransform = cv::findHomography(inSrcPoints, inDstPoints, 0);
  return !outTransform.empty();
}

bool Homography::getHomography(
  TransformFinderType tfType,
  TConstPoints2f& inDstPoints,
  TConstPoints2f& inSrcPoints,
  TBools& outMask,
  TMat& outTransform,
  double ransacThresh,
  double ransacConfidence)
{
  FUNCLOGTIMEL("Homography::getHomography");

  Mat mask(1, inSrcPoints.size(), CV_8UC1);
  mask = Mat::zeros(mask.size(), CV_8UC1);

  TMat transform;
   
  switch(tfType){

    case TransformFinderType::TFT_RANSAC:
      transform = cv::findHomography(inSrcPoints, inDstPoints, RANSAC, ransacThresh, mask, 2000, ransacConfidence);
      break;

    case TransformFinderType::TFT_RHO:
      transform = cv::findHomography(inSrcPoints, inDstPoints, RHO, ransacThresh, mask, 2000, ransacConfidence);
      break;

    case TransformFinderType::TFT_LMEDS:
      transform = cv::findHomography(inSrcPoints, inDstPoints, LMEDS, ransacThresh, mask);
      break;
  }

  for(size_t matchIndex = 0; matchIndex < inSrcPoints.size(); ++ matchIndex){

    if(mask.at<uchar>(matchIndex) == 1) outMask.push_back(true);
    else                                outMask.push_back(false);
  }

  transform.convertTo(outTransform, CV_64F);

   return !outTransform.empty();
}

bool Homography::getHomography(
  TConstPoints2f &pts1, TConstPoints2f &pts2,
  const cv::Size &s1, const cv::Size &s2,
  TConstMat *pMatK1, TConstMat *pMatK2,
  double fieldOfView1, double fieldOfView2,
  TConstMat &rotMat1,
  TConstMat &rotMat2,
  int projType1, int projType2,
  TransformFinderType tfType,
  TMat &outHomography,
  double *globalScale,
  TPoints2f *warpedPts1,
  TPoints2f *warpedPts2)
{

  FUNCLOGTIMEL("Homography::getHomography");

  
  if(pts1.empty()) return false;
  if(pts2.empty()) return false;

  TPoints2f points1(pts1.begin(), pts1.end());
  TPoints2f points2(pts2.begin(), pts2.end());

  if((ParamType)projType1 != eStitch_projectionTypeNone)
  {
    if(pMatK1 != nullptr) {
      WarperHelper::warpPoints(
      s1.width, s1.height, *pMatK1, rotMat1, projType1, points1, globalScale);
    }
    else {
      WarperHelper::warpPoints(
        s1.width, s1.height, fieldOfView1, rotMat1, projType1, points1);
    }
  }
  if((ParamType)projType2 != eStitch_projectionTypeNone)
  {
    if(pMatK2 != nullptr) {
      WarperHelper::warpPoints(
        s2.width, s2.height, *pMatK2, rotMat2, projType2, points2, globalScale);
    }
    else {
      WarperHelper::warpPoints(
        s2.width, s2.height, fieldOfView2, rotMat2, projType2, points2);
    }
  }

  TBools mask;
  if(!getHomography(tfType, points1, points2, mask, outHomography)) {
    return false;
  }

  if(warpedPts1 != nullptr)
  {
    warpedPts1->assign(points1.begin(), points1.end());
  }
  if(warpedPts2 != nullptr)
  {
    warpedPts2->assign(points2.begin(), points2.end());
  }

  return true;
}

double Homography::determinant(TConstMat &homography)
{
  FUNCLOGTIMEL("Homography::determinant");

  // cv::Rect r(0, 0, 2, 2);
  // TMat mRoi(homography, r);
  // return cv::determinant(mRoi);

  return cv::determinant(homography);
}

bool Homography::isSvdConditionNumberSane(TConstMat &homography, double &conditionNumber, double threshHold)
{
  FUNCLOGTIMEL("Homography::svdConditionNumber");

  TMat wValues;

  if(homography.type() == CV_64F) {
    cv::SVD::compute(homography, wValues, cv::SVD::NO_UV);
  }
  else {
    TMat h;
    homography.convertTo(h, CV_64F);
    cv::SVD::compute(h, wValues, cv::SVD::NO_UV);
  }

  double minValue = std::numeric_limits<double>::max();
  double maxValue = -std::numeric_limits<double>::max();
  for(auto it = wValues.begin<double>(); it != wValues.end<double>(); ++it) {
    double w = *it;
    
    if(w < minValue) minValue = w;
    if(w > maxValue) maxValue = w;
  }

  conditionNumber = maxValue / minValue;
  
  if(isinf(conditionNumber)) {
    return false;
  }

  if(conditionNumber > threshHold) {
    return false;
  }

  return true;

}

}