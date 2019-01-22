

#include "WarperHelper.h"
#include "Settings.h"
#include "LogUtils.h"
#include "warpers/warpers.hpp"
#include <array>
#include <limits>

using namespace cv;
using namespace cv::detail;

namespace imgalign
{

  Ptr<RotationWarper> WarperHelper::getWarper(int warperType, float scale)
  {
    FUNCLOGTIMEL("WarperHelper::getWarper");
    
    cv::Ptr<cv::detail::RotationWarper> rotWraper = nullptr;
    
	  switch((ParamType)warperType) {
      case eStitch_projectionTypePlane: {
        cv::PlaneWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeAffine: {
        cv::AffineWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeCylindrical: {
        cv::CylindricalWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeSpherical: {
        cv::SphericalWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeFisheye: {
        cv::FisheyeWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeStereographic: {
        cv::StereographicWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeRectilinear: {
        cv::CompressedRectilinearWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeRectilinearPortrait: {
        cv::CompressedRectilinearPortraitWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypePanini: {
        cv::PaniniWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeMercator: {
        cv::MercatorWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeTransverseMercator: {
        cv::TransverseMercatorWarper warper;
        rotWraper = warper.create(scale);
        break;
      }
      case eStitch_projectionTypeNone:
        break;
      default: {
        throw std::logic_error("WarperHelper::getWarper: Unknown type");
      }
    }

    return rotWraper;
  }

  void
  WarperHelper::getMatR(double yaw, double pitch, double roll, TMat &outMat)
  {
    TMat r(3, 3, CV_64F);

    double x = pitch * CV_PI / 180.0;
    double y = -yaw * CV_PI / 180.0;
    double z = roll * CV_PI / 180.0;

    // Assuming the angles are in radians.
    double ch = cos(z);
    double sh = sin(z);
    double ca = cos(y);
    double sa = sin(y);
    double cb = cos(x);
    double sb = sin(x);

    double m00, m01, m02, m10, m11, m12, m20, m21, m22;

    m00 = ch * ca;
    m01 = sh*sb - ch*sa*cb;
    m02 = ch*sa*sb + sh*cb;
    m10 = sa;
    m11 = ca*cb;
    m12 = -ca*sb;
    m20 = -sh*ca;
    m21 = sh*sa*cb + ch*sb;
    m22 = -sh*sa*sb + ch*cb;

    r.at<double>(0,0) = m00;
    r.at<double>(0,1) = m01;
    r.at<double>(0,2) = m02;
    r.at<double>(1,0) = m10;
    r.at<double>(1,1) = m11;
    r.at<double>(1,2) = m12;
    r.at<double>(2,0) = m20;
    r.at<double>(2,1) = m21;
    r.at<double>(2,2) = m22;

    r.copyTo(outMat);

    // float rX = pitch * CV_PI / 180.0;
    // float rY = -yaw * CV_PI / 180.0;
    // float rZ = roll * CV_PI / 180.0;

    // float dataRx[] = {
    //   1, 0, 0,
    //   0, cosf(rX), -sinf(rX),
    //   0, sinf(rX), cosf(rX)};
    // TMat R_x(3, 3, CV_32FC1, dataRx);

    // float dataRy[] = {
    //   cosf(rY), 0, sinf(rY),
    //   0, 1, 0,
    //   -sinf(rY), 0, cosf(rY)};
    // TMat R_y(3, 3, CV_32FC1, dataRy);

    // float dataRz[] = {
    //   cosf(rZ), -sinf(rZ), 0,
    //   sinf(rZ), cosf(rZ), 0,
    //   0, 0, 1};
    // TMat R_z(3, 3, CV_32FC1, dataRz);

    // return R_z * R_y * R_x;
  }

  void
  WarperHelper::getMatK(double width, double height, double focalLengthPx, TMat &outMat)
  {
    //LogUtils::getLog() << "getMatK focalLengthPx: " << focalLengthPx << std::endl;
    
    double dataK[] = {
      focalLengthPx, 0.0, width * 0.5,
      0.0, focalLengthPx, height * 0.5,
      0.0, 0.0, 1.0
    };
    
    TMat(3, 3, CV_64F, dataK).copyTo(outMat);
  }

double
WarperHelper::getFocalLengthPx(double width, double /*height*/, double fieldOfView)
{
  FUNCLOGTIMEL("WarperHelper::getFocalLengthPx");
  return (width * 0.5) / tan(fieldOfView * 0.5 * CV_PI / 180);
}

double
WarperHelper::getFieldOfView(double width, double /*height*/, double fLenPx)
{
  FUNCLOGTIMEL("WarperHelper::getFieldOfView");
  return atan(width * 0.5 / fLenPx) * 180.0 / (CV_PI * 0.5);
}

std::vector<cv::Point> WarperHelper::transformPts(
  const std::vector<cv::Point> &pts,
  TConstMat &homography)
{
  FUNCLOGTIMEL("WarperHelper::transformPts");

  CV_Assert(pts.size() > 0);

  std::vector<cv::Point2f> ptsSrcF;
  for(const auto &pt : pts) {
    ptsSrcF.push_back(cv::Point2f(pt.x, pt.y));
  }
  std::vector<cv::Point2f> ptsDstF(ptsSrcF.size());
  perspectiveTransform(ptsSrcF, ptsDstF, homography);

  std::vector<cv::Point> ptsDst;
  for(const auto &pt : ptsDstF) {
    ptsDst.push_back(cv::Point((int)pt.x, (int)pt.y));
  }

  return ptsDst;
}
std::vector<cv::Point2f> WarperHelper::transformPtsf(
  const std::vector<cv::Point2f> &pts,
  TConstMat &homography)
{
  FUNCLOGTIMEL("WarperHelper::transformPtsf");

  CV_Assert(pts.size() > 0);

  std::vector<cv::Point2f> ptsDstF(pts.size());
  perspectiveTransform(pts, ptsDstF, homography);

  return ptsDstF;
}

cv::Point2f WarperHelper::getCenter(const std::initializer_list<cv::Point2f> &pts) 
{
  FUNCLOGTIMEL("WarperHelper::getCenter");

  auto xMin = std::min(pts,
    [](const Point2f &pt1, const Point2f &pt2) {
      return pt1.x < pt2.x;
    });
  auto xMax = std::max(pts,
    [](const Point2f &pt1, const Point2f &pt2) {
      return pt1.x < pt2.x;
    });

  auto yMin = std::min(pts,
    [](const Point2f &pt1, const Point2f &pt2) {
      return pt1.y < pt2.y;
    });
  auto yMax = std::max(pts,
    [](const Point2f &pt1, const Point2f &pt2) {
      return pt1.y < pt2.y;
    });

  return Point2f(
    0.5 * (xMax.x + xMin.x),
    0.5 * (yMax.y + yMin.y));
}

void WarperHelper::getRelativeRotation(
  double w1, double h1, double w2, double h2,
  double fieldOfViewX1,
  TConstMat &homography,
  double &outYaw, double &outPitch)
{
  FUNCLOGTIMEL("WarperHelper::getRelativeRotation");

  const auto bBox1Array = std::array<Point2f, 4>{
    Point2f(0.0, 0.0), Point2f(w1, 0.0), Point2f(w1, h1), Point2f(0.0, h1)
  };
  const auto bBox1 = {
    bBox1Array[0], bBox1Array[1], bBox1Array[2], bBox1Array[3]
  };
  const auto bBox2 = {
    Point2f(0.0, 0.0), Point2f(w2, 0.0), Point2f(w2, h2), Point2f(0.0, h2)
  };

  const auto bBox2Warped = WarperHelper::transformPtsf(bBox2, homography);
  const auto bBox2WarpedI = {
    bBox2Warped[0], bBox2Warped[1], bBox2Warped[2], bBox2Warped[3] 
  };
  
  const auto ct1 = getCenter(bBox1);
  const auto ct2 = getCenter(bBox2WarpedI);
  
  const auto tx = ct2.x - ct1.x;
  const auto ty = ct2.y - ct1.y;

  outYaw = tx * fieldOfViewX1 / w1;
  outPitch = -(ty * fieldOfViewX1 / w1);
}

void WarperHelper::warpPoints(
  double w, double h, double fieldOfView,
  TConstMat &rotMat,
  int warperType,
  TPoints2f &ioPts)
{
  FUNCLOGTIMEL("WarperHelper::warpPoints with fieldOfView");

  if(fieldOfView <= 0) {
    throw std::logic_error("WarperHelper::warpPoints: Invalid field of view");
  }

  double fLenPx = getFocalLengthPx(w, h, fieldOfView);
  double scale = w * 180 / (fieldOfView  * CV_PI);

  auto warper = WarperHelper::getWarper(warperType, scale);
  if(warper == nullptr) return;

  TMat K;
  WarperHelper::getMatK(w, h, fLenPx, K);
	auto R = rotMat;

  TMat k32, r32;
  K.convertTo(k32, CV_32F);
  R.convertTo(r32, CV_32F);

  //auto bBoxWarped = warper->warpRoi(Size(w, h), k32, r32);
  cv::Point2f tl, br;
  warper->warpRoif(Size(w, h), k32, r32, tl, br);
 
  for(auto it = ioPts.begin(); it != ioPts.end(); ++it) {
		
    *it = warper->warpPoint(*it, k32, r32);

    it->x = it->x - tl.x;
    it->y = it->y - tl.y;
    // it->x = it->x - bBoxWarped.tl().x;
    // it->y = it->y - bBoxWarped.tl().y;
  }
}


void WarperHelper::warpImage(
  int warperType,
  TConstMat& srcMat,
  TMat &outMat,
  double fieldOfView,
  TConstMat &rotMat)
{
  FUNCLOGTIMEL("WarperHelper::warpImage  with fieldOfView");

  double fLenPx = getFocalLengthPx(srcMat.size().width, srcMat.size().height, fieldOfView);
  double scale = srcMat.size().width * (360 / fieldOfView) / (2 * CV_PI);

  auto warper = WarperHelper::getWarper(warperType, (float)scale);
  if(warper == nullptr) {
    srcMat.copyTo(outMat);
  }
  else {

    auto R = rotMat;
    TMat K;
    WarperHelper::getMatK(srcMat.size().width, srcMat.size().height, fLenPx, K);

    TMat k32, r32;
    K.convertTo(k32, CV_32F);
    R.convertTo(r32, CV_32F);

    warper->warp(srcMat, k32, r32, cv::INTER_LINEAR, cv::BORDER_CONSTANT, outMat);
  }
}

void WarperHelper::warpPoints(
  double w, double h,
  TConstMat &kMat,
  TConstMat &rotMat,
  int warperType,
  TPoints2f &ioPts,
  double *globalScale)
{
  FUNCLOGTIMEL("WarperHelper::warpPoints");

  CV_Assert(kMat.type() == CV_64F);
  CV_Assert(rotMat.type() == CV_64F);

  double scale;
  if(globalScale != nullptr) {
    scale = *globalScale;
    
  }
  else {
    double flenPx = kMat.at<double>(0, 0);
    double fieldOfView = getFieldOfView(w, h, flenPx);
    scale = w * 180 / (fieldOfView  * CV_PI);
  }
  
  auto warper = WarperHelper::getWarper(warperType, (float)scale);
  if(warper == nullptr) return;

  auto K = kMat;
	auto R = rotMat;

  TMat k32, r32;
  K.convertTo(k32, CV_32F);
  R.convertTo(r32, CV_32F);

  cv::Point2f tl, br;
  warper->warpRoif(Size(w, h), k32, r32, tl, br);
  
  for(auto it = ioPts.begin(); it != ioPts.end(); ++it) {
		
    *it = warper->warpPoint(*it, k32, r32);

    it->x = it->x - tl.x;
    it->y = it->y - tl.y;
  }
}

void WarperHelper::warpImage(
  int warperType,
  TConstMat& srcMat,
  TMat &outMat,
  TConstMat &kMat,
  TConstMat &rotMat,
  double *globalScale)
{
  FUNCLOGTIMEL("WarperHelper::warpImage");

  CV_Assert(kMat.type() == CV_64F);
  CV_Assert(rotMat.type() == CV_64F);

  double scale;
  if(globalScale != nullptr) {
    scale = *globalScale;
  }
  else {
    double flenPx = kMat.at<double>(0, 0);
    double fieldOfView = getFieldOfView(srcMat.size().width, srcMat.size().height, flenPx);
    scale = srcMat.size().width * 180 / (fieldOfView  * CV_PI);
  }
  
  auto warper = WarperHelper::getWarper(warperType, (float)scale);
  if(warper == nullptr) {
    srcMat.copyTo(outMat);
  }
  else {

    auto R = rotMat;
    auto K = kMat;
    TMat k32, r32;
    K.convertTo(k32, CV_32F);
    R.convertTo(r32, CV_32F);

    warper->warp(srcMat, k32, r32, cv::INTER_LINEAR, cv::BORDER_CONSTANT, outMat);
  }
}
void WarperHelper::warpImageBackwards(
  int warperType,
  TConstMat& srcMat,
  TMat &outMat,
  double fieldOfView,
  double yaw, double pitch, double roll)
{
  FUNCLOGTIMEL("WarperHelper::warpImageBackwards");

  float focalLengthPx = (srcMat.size().width * 0.5) / tan(fieldOfView * 0.5 * CV_PI / 180);

  float scale = srcMat.size().width * (360 / fieldOfView) / (2 * CV_PI);
  
  TMat R, K;
  WarperHelper::getMatR(yaw, pitch, roll, R);
  WarperHelper::getMatK(srcMat.size().width, srcMat.size().height, focalLengthPx, K);

  auto warper = WarperHelper::getWarper(warperType, (float)scale);
  if(warper == nullptr) {
    srcMat.copyTo(outMat);
  }
  else {
    warper->warpBackward(srcMat, K, R, cv::INTER_LINEAR, cv::BORDER_CONSTANT, outMat.size(), outMat);
  }
}

void WarperHelper::getBox(
  double w1, double h1, double w2, double h2,
  TMat &ioHomography,
  double &outTx, double &outTy,
  double &outT, double &outR, double &outB, double &outL)
{
  FUNCLOGTIMEL("WarperHelper::getBox");

  auto bBox2 = {
    Point2f(0, 0), Point2f(w2, 0), Point2f(w2, h2), Point2f(0, h2)
	};

  auto bBoxWarped2 = WarperHelper::transformPtsf(bBox2, ioHomography);
  
  auto ptsAll = {
    bBoxWarped2[0], bBoxWarped2[1], bBoxWarped2[2], bBoxWarped2[3],
    Point2f(0, 0), Point2f(w1, 0), Point2f(w1, h1), Point2f(0, h1)
  };

  auto xMin = std::min(ptsAll,
			[](const Point &pt1, const Point &pt2) {
				return pt1.x < pt2.x;
			});

		auto xMax = std::max(ptsAll,
			[](const Point &pt1, const Point &pt2) {
				return pt1.x < pt2.x;
			});

		auto yMin = std::min(ptsAll,
			[](const Point &pt1, const Point &pt2) {
				return pt1.y < pt2.y;
			});

		auto yMax = std::max(ptsAll,
			[](const Point &pt1, const Point &pt2) {
				return pt1.y < pt2.y;
			});	
	
		outTx = -std::min(0.0f, xMin.x);
		outTy = -std::min(0.0f, yMin.y);

    if(xMin.x < 0 || yMin.y < 0) {

      Mat tM = Mat::eye(3,3, CV_64F);
      if(xMin.x < 0) {
			  tM.at<double>(0,2) = outTx;
      }
      if(yMin.y < 0) {
			  tM.at<double>(1,2)= outTy;
      }
			ioHomography = tM * ioHomography;
    }

    outL = xMin.x;
    outT = yMin.y;
    outR = xMax.x;
    outB = yMax.y;
}

void WarperHelper::getBox(
  double w, double h,
  TMat &ioHomography,
  double &outTx, double &outTy,
  double &outT, double &outR, double &outB, double &outL)
{
  FUNCLOGTIMEL("WarperHelper::getBox");

  auto bBox = {
    Point2f(0, 0), Point2f(w, 0), Point2f(w, h), Point2f(0, h)
	};

  auto bBoxWarped = WarperHelper::transformPtsf(bBox, ioHomography);

  auto ptsAll = {
    bBoxWarped[0], bBoxWarped[1], bBoxWarped[2], bBoxWarped[3]
  };

  auto xMin = std::min(ptsAll,
			[](const Point &pt1, const Point &pt2) {
				return pt1.x < pt2.x;
			});

		auto xMax = std::max(ptsAll,
			[](const Point &pt1, const Point &pt2) {
				return pt1.x < pt2.x;
			});

		auto yMin = std::min(ptsAll,
			[](const Point &pt1, const Point &pt2) {
				return pt1.y < pt2.y;
			});

		auto yMax = std::max(ptsAll,
			[](const Point &pt1, const Point &pt2) {
				return pt1.y < pt2.y;
			});	
	
		outTx = -xMin.x;
		outTy = -yMin.y;


    Mat tM = Mat::eye(3,3,CV_64F);
    tM.at<double>(0,2) = outTx;
    tM.at<double>(1,2)= outTy;
    ioHomography = tM * ioHomography;
    

    outL = xMin.x;
    outT = yMin.y;
    outR = xMax.x;
    outB = yMax.y;
}

double WarperHelper::fieldOfView(
  double fieldOfView1, double fieldOfView2, double yaw)
{
  FUNCLOGTIMEL("WarperHelper::fieldOfView");

  double min1 = - fieldOfView1 / 2.0;
  double max1 = + fieldOfView1 / 2.0;

  double min2 = yaw - fieldOfView2 / 2.0;
  double max2 = yaw + fieldOfView2 / 2.0;

  double min = std::min(min1, min2);
  double max = std::max(max1, max2);

  return abs(max - min);
}

void WarperHelper::warpPerspective(
  TConstMat &src,
  TConstMat &homography, 
  cv::Size dstSize,
  TMat &dst)
{
  FUNCLOGTIMEL("WarperHelper::warpPerspective");

  dst = TMat::zeros(dstSize, src.type());
  cv::warpPerspective(src, dst, homography, dstSize, INTER_LINEAR, BORDER_CONSTANT, Scalar::all(0));
}


void WarperHelper::waveCorrect(std::vector<TMat> &rmats, bool horizontal)
{
  FUNCLOGTIMEL("WarperHelper::waveCorrect");

    if (rmats.size() <= 1)
    {
        return;
    }

    Mat moment = Mat::zeros(3, 3, CV_32F);
    for (size_t i = 0; i < rmats.size(); ++i)
    {
        Mat col = rmats[i].col(0);
        moment += col * col.t();
    }
    Mat eigen_vals, eigen_vecs;
    eigen(moment, eigen_vals, eigen_vecs);

    Mat rg1;
    if (horizontal)
        rg1 = eigen_vecs.row(2).t();
    else
        rg1 = eigen_vecs.row(0).t();

    Mat img_k = Mat::zeros(3, 1, CV_32F);
    for (size_t i = 0; i < rmats.size(); ++i)
        img_k += rmats[i].col(2);
    Mat rg0 = rg1.cross(img_k);
    double rg0_norm = norm(rg0);

    if( rg0_norm <= DBL_MIN )
    {
        return;
    }

    rg0 /= rg0_norm;

    Mat rg2 = rg0.cross(rg1);

    double conf = 0;
    if (horizontal)
    {
        for (size_t i = 0; i < rmats.size(); ++i)
            conf += rg0.dot(rmats[i].col(0));
        if (conf < 0)
        {
            rg0 *= -1;
            rg1 *= -1;
        }
    }
    else
    {
        for (size_t i = 0; i < rmats.size(); ++i)
            conf -= rg1.dot(rmats[i].col(0));
        if (conf < 0)
        {
            rg0 *= -1;
            rg1 *= -1;
        }
    }

    Mat R = Mat::zeros(3, 3, CV_32F);
    Mat tmp = R.row(0);
    Mat(rg0.t()).copyTo(tmp);
    tmp = R.row(1);
    Mat(rg1.t()).copyTo(tmp);
    tmp = R.row(2);
    Mat(rg2.t()).copyTo(tmp);

    for (size_t i = 0; i < rmats.size(); ++i)
        rmats[i] = R * rmats[i];
}



}