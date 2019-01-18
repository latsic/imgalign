#ifndef IMGALIGN_BUNDLE_IMAGEFEATURESH
#define IMGALIGN_BUNDLE_IMAGEFEATURESH

#include "../CommonTypes.h"
#include <vector>
#include "../../precomp.hpp"

namespace imgalign
{
namespace bundle
{

struct ImageFeatures
{
  int img_idx;
  cv::Size img_size;
  std::vector<cv::KeyPoint> keypoints;
  cv::UMat descriptors;
};

}

}

#endif
