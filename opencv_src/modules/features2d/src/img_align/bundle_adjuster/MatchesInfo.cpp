
#include "MatchesInfo.h"

using namespace cv;

namespace imgalign
{
namespace bundle
{

MatchesInfo::MatchesInfo()
    : src_img_idx(-1)
    , dst_img_idx(-1)
    , num_inliers(0)
    , confidence(0)
    , num_filtered(0)
    , num_all(0)
    , num_outlier(0)
    , sumDeltaHV(0)
{

}

MatchesInfo::MatchesInfo(const MatchesInfo &other) { *this = other; }

MatchesInfo& MatchesInfo::operator =(const MatchesInfo &other)
{
    src_img_idx = other.src_img_idx;
    dst_img_idx = other.dst_img_idx;
    matches = other.matches;
    inliers_mask = other.inliers_mask;
    num_inliers = other.num_inliers;
    H = other.H.clone();
    confidence = other.confidence;

    num_filtered = other.num_filtered;
    num_all = other.num_all;
    num_outlier = other.num_outlier;
    sumDeltaHV = other.sumDeltaHV;

    return *this;
}

}
}