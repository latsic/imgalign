
#include "CameraParams.h"

using namespace cv;

namespace imgalign
{
namespace bundle
{

CameraParams::CameraParams(double focalIn) : focal(focalIn), aspect(1), ppx(0), ppy(0),
                               R(Mat::eye(3, 3, CV_64F)), t(Mat::zeros(3, 1, CV_64F)) {}

CameraParams::CameraParams(const CameraParams &other) { *this = other; }

CameraParams& CameraParams::operator =(const CameraParams &other)
{
    focal = other.focal;
    ppx = other.ppx;
    ppy = other.ppy;
    aspect = other.aspect;
    R = other.R.clone();
    t = other.t.clone();
    return *this;
}

Mat CameraParams::K() const
{
    Mat_<double> k = Mat::eye(3, 3, CV_64F);
    k(0,0) = focal; k(0,2) = ppx;
    k(1,1) = focal * aspect; k(1,2) = ppy;
    return k;
}

}
}