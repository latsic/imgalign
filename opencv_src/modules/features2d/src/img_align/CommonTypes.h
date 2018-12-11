
#ifndef IMGALIGN_COMMONTYPESH
#define IMGALIGN_COMMONTYPESH

#include <vector>
#include <tuple> 
#include <opencv2/core/types.hpp>

namespace imgalign {

  typedef cv::Mat                                 TMat;
  typedef cv::KeyPoint                            TKeyPoint;
  typedef const TKeyPoint                         TConstKeyPoint;
  typedef const TMat                              TConstMat;
  typedef const std::vector<TConstMat>            TConstImages;
  typedef cv::Rect                                TRect;                                             
  typedef std::vector<TRect>                      TRects;
  typedef const std::vector<TRect>							  TConstRects;
  typedef std::vector<cv::KeyPoint>							  TKeyPoints;
  typedef const std::vector<cv::KeyPoint>					TConstKeyPoints;
  typedef std::vector<cv::DMatch>								  TMatches;
  typedef const std::vector<cv::DMatch>           TConstMatches;
  typedef std::tuple<cv::KeyPoint, cv::KeyPoint>  TKeyPointPair;
  typedef std::vector<TKeyPointPair>							TKeyPointPairs;
  typedef std::pair<TKeyPointPair, cv::DMatch>    TMatchInfo;
  typedef const TMatchInfo                        TConstMatchInfo;
  typedef std::vector<TMatchInfo>                 TMatchInfos;
  typedef const TMatchInfos                       TConstMatchInfos;
  typedef std::tuple<cv::Mat, cv::Mat>            TDescriptorPair;
  typedef std::vector<TDescriptorPair>            TDescriptorPairs;
  typedef std::vector<cv::Point2f>                TPoints2f;
  typedef const std::vector<cv::Point2f>          TConstPoints2f;
  typedef std::vector<cv::Point>                  TPoints;
  typedef const std::vector<cv::Point>            TConstPoints;
  typedef std::vector<bool>                       TBools;
  typedef cv::Scalar                              TColor;
}

#endif