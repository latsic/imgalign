#ifndef IMGALIGN_BUNDLE_CAMERAPARAMSH
#define IMGALIGN_BUNDLE_CAMERAPARAMSH

#include "../CommonTypes.h"
#include <vector>
#include "../../precomp.hpp"

namespace imgalign
{
namespace bundle
{


struct CV_EXPORTS CameraParams
{
  CameraParams(double focalIn = 1.0);
  CameraParams(const CameraParams& other);
  CameraParams& operator =(const CameraParams& other);
  cv::Mat K() const;

  double focal; // Focal length
  double aspect; // Aspect ratio
  double ppx; // Principal point X
  double ppy; // Principal point Y
  cv::Mat R; // Rotation
  cv::Mat t; // Translation
};

}
}

#endif