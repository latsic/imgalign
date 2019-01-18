#ifndef IMGALIGN_BUNDLE_MATCHESINFOH
#define IMGALIGN_BUNDLE_MATCHESINFOH

#include "../CommonTypes.h"
#include <vector>
#include "../../precomp.hpp"

namespace imgalign
{
namespace bundle
{

struct MatchesInfo
{
  MatchesInfo();
  MatchesInfo(const MatchesInfo &other);
  MatchesInfo& operator =(const MatchesInfo &other);

  int src_img_idx, dst_img_idx;       //!< Images indices (optional)
  std::vector<cv::DMatch> matches;
  std::vector<uchar> inliers_mask;    //!< Geometrically consistent matches mask
  int num_inliers;                    //!< Number of geometrically consistent matches
  cv::Mat H;                          //!< Estimated transformation
  double confidence;                  //!< Confidence two images are from the same panorama
};

}

}

#endif
