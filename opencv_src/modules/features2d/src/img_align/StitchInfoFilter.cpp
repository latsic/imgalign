
#include "StitchInfoFilter.h"
#include "LogUtils.h"
#include "WarperHelper.h"

namespace imgalign
{

namespace
{
  bool
  deltaAngleSumInRange(const StitchInfo &stitchInfo)
  {
    if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
      return false;
    }
    
    double deltaSum = std::abs(stitchInfo.deltaH) + std::abs(stitchInfo.deltaV);

    return deltaSum < 3500.0;
  }

  bool
  fulfillsMinCriterias(const StitchInfo &stitchInfo, double cf)
  {
    return stitchInfo.matchInfo.isHomographyGood()
      && stitchInfo.matchInfo.confidence >= cf
      && deltaAngleSumInRange(stitchInfo);
  }
}

StitchInfoFilter::StitchInfoFilter(double confidenceThresh)
  : cf(confidenceThresh)
{
}

void StitchInfoFilter::setIteration(int i)
{
  iteration = i;
}

bool StitchInfoFilter::done() const
{
  return false;
}

SIF_Std::SIF_Std(double confidenceThresh)
  : StitchInfoFilter(confidenceThresh)
{
}

bool SIF_Std::pass(const StitchInfo &stitchInfo) const
{
  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    return false;
  }

  return fulfillsMinCriterias(stitchInfo, cf);
}

bool SIF_Std::done() const
{
  return iteration > 0;
}

SIF_IgnoreEdgesConfidence::SIF_IgnoreEdgesConfidence(
  double confidenceThresh, const std::vector<const StitchInfo *> &stitchInfos)
  : StitchInfoFilter(confidenceThresh)
{
  FUNCLOGTIMEL("SIF_IgnoreEdgesConfidence::SIF_IgnoreEdgesConfidence");

  for(const auto *pStitchInfo : stitchInfos) {
    if(pStitchInfo != nullptr && fulfillsMinCriterias(*pStitchInfo, cf)) {
      _stitchInfos.push_back(pStitchInfo);
    }
  }

  std::sort(_stitchInfos.begin(), _stitchInfos.end(),
    [](const StitchInfo *s1, const StitchInfo *s2) {
      return s1->matchInfo.confidence > s2->matchInfo.confidence;
  });

  LogUtils::getLogUserInfo() << "SIF_IgnoreEdgesConfidence " << std::endl;
  for(const auto *pStitchInfo : _stitchInfos) {
    LogUtils::getLogUserInfo() << pStitchInfo->matchInfo.confidence << " ";
  }
  LogUtils::getLogUserInfo() << std::endl;
}

bool SIF_IgnoreEdgesConfidence::pass(const StitchInfo &stitchInfo) const
{
  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    return false;
  }
  if(!fulfillsMinCriterias(stitchInfo, cf)) {
    return false;
  }

  int maxIndex = (int)_stitchInfos.size() - iteration * 2;
  for(int i = 0; i < (int)_stitchInfos.size(); ++i) {

    if(i > maxIndex) return false;

    if(stitchInfo.srcImageIndex == _stitchInfos[i]->srcImageIndex &&
       stitchInfo.dstImageIndex == _stitchInfos[i]->dstImageIndex) {
      
      return true;
    }
  }
  return false;
}

bool SIF_IgnoreEdgesConfidence::done() const
{
  int maxIndex = (int)_stitchInfos.size() - iteration * 2;
  return maxIndex <= 1;
}

SIF_IgnoreImagesConfidence::SIF_IgnoreImagesConfidence(
  double confidenceThresh, const std::vector<const StitchInfo *> &stitchInfos)
  : StitchInfoFilter(confidenceThresh)
{
  FUNCLOGTIMEL("SIF_IgnoreImagesConfidence::SIF_IgnoreImagesConfidence");

  std::map<size_t, std::vector<double>> srcImageConfidencesV;
  
  for(const auto *pStitchInfo : stitchInfos) {
    
    if(pStitchInfo == nullptr || !fulfillsMinCriterias(*pStitchInfo, cf)) {
      continue;
    }
    
    size_t srcIndex = pStitchInfo->srcImageIndex;
    auto it = srcImageConfidencesV.find(srcIndex);
    if(it == srcImageConfidencesV.end()) {
      srcImageConfidencesV.insert(std::make_pair(srcIndex, std::vector<double>()));
    }
    auto &srcConfs = srcImageConfidencesV[srcIndex];
    srcConfs.push_back(pStitchInfo->matchInfo.confidence);
  }

  std::vector<std::pair<int, double>> srcImagesConfidenceSum;
  for(auto &pair : srcImageConfidencesV) {
    double cofidencesSum = 0;
    for(auto c : pair.second) {
      cofidencesSum += c;
    }
    srcImagesConfidenceSum.push_back(
      std::make_pair(pair.first, cofidencesSum / pair.second.size()));
  }

  std::sort(srcImagesConfidenceSum.begin(), srcImagesConfidenceSum.end(), 
    [](std::pair<int, double> &p1, std::pair<int, double> &p2) {

      return p1.second > p2.second;
  });

  LogUtils::getLogUserInfo() << "SIF_IgnoreImagesConfidence " << std::endl;
  for(const auto &p : srcImagesConfidenceSum) {
    imageIndices.push_back(p.first);
    LogUtils::getLogUserInfo() << p.second << " ";
  }
  LogUtils::getLogUserInfo() << std::endl;

  LogUtils::getLogUserInfo() << "SIF_IgnoreImagesConfidence " << std::endl;
  for(auto i : imageIndices) {
    LogUtils::getLogUserInfo() << i << " ";
  }
  LogUtils::getLogUserInfo() << std::endl;
}

bool SIF_IgnoreImagesConfidence::pass(const StitchInfo &stitchInfo) const 
{
  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    return false;
  }
  if(!fulfillsMinCriterias(stitchInfo, cf)) {
    return false;
  }

  int maxIndex = (int)imageIndices.size() - iteration;

  for(int i = (int)imageIndices.size() - 1; i >= maxIndex; --i) {
    if((int)stitchInfo.srcImageIndex == imageIndices[i] ||
       (int)stitchInfo.dstImageIndex == imageIndices[i]) {

      return false;
    }
  }
  return true;
}

bool SIF_IgnoreImagesConfidence::done() const
{
  int maxIndex = (int)imageIndices.size() - iteration;
  return maxIndex <= 1;
}

SIF_BestNeighbourOnly::SIF_BestNeighbourOnly(
  double confidenceThresh,
  const std::vector<const StitchInfo *> &stitchOrder)
  : StitchInfoFilter(confidenceThresh)
  , _stitchOrder(stitchOrder)
{
  FUNCLOGTIMEL("SIF_BestNeighbourOnly::SIF_BestNeighbourOnly");
}

bool SIF_BestNeighbourOnly::pass(const StitchInfo &stitchInfo) const
{
  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    return false;
  }
  if(!fulfillsMinCriterias(stitchInfo, cf)) {
    return false;
  }

  for(const auto *pStitchInfo : _stitchOrder) {

    if(stitchInfo.srcImageIndex == pStitchInfo->srcImageIndex &&
       stitchInfo.dstImageIndex == pStitchInfo->dstImageIndex) {
      
      return true;
    }

    if(stitchInfo.srcImageIndex == pStitchInfo->dstImageIndex &&
       stitchInfo.dstImageIndex == pStitchInfo->srcImageIndex) {
      
      return true;
    }
  }
  return false;
}

bool SIF_BestNeighbourOnly::done() const
{
  return iteration > 0;
}

SIF_IgnoreImagesDeltaSumHV::SIF_IgnoreImagesDeltaSumHV(
  double confidenceThresh, const std::vector<const StitchInfo *> &stitchInfos)
  : StitchInfoFilter(confidenceThresh)
{
  FUNCLOGTIMEL("SIF_IgnoreImagesDeltaSumHV::SIF_IgnoreImagesDeltaSumHV");

  std::map<size_t, std::vector<double>> srcImageDeltaHVSumV;
  
  for(const auto *pStitchInfo : stitchInfos) {
    
    if(pStitchInfo == nullptr || !fulfillsMinCriterias(*pStitchInfo, cf)) {
      continue;
    }
    
    size_t srcIndex = pStitchInfo->srcImageIndex;
    auto it = srcImageDeltaHVSumV.find(srcIndex);
    if(it == srcImageDeltaHVSumV.end()) {
      srcImageDeltaHVSumV.insert(std::make_pair(srcIndex, std::vector<double>()));
    }
    auto &srcDeltaHVs = srcImageDeltaHVSumV[srcIndex];
    srcDeltaHVs.push_back(std::abs(pStitchInfo->deltaH) + std::abs(pStitchInfo->deltaV));
  }

  std::vector<std::pair<int, double>> srcImagesDeltaHVSum;
  for(auto &pair : srcImageDeltaHVSumV) {
    double deltaHVSum = 0;
    for(auto d : pair.second) {
      deltaHVSum += d;
    }
    srcImagesDeltaHVSum.push_back(
      std::make_pair(pair.first, deltaHVSum / pair.second.size()));
  }

  std::sort(srcImagesDeltaHVSum.begin(), srcImagesDeltaHVSum.end(), 
    [](std::pair<int, double> &p1, std::pair<int, double> &p2) {

      return p1.second < p2.second;
  });

  LogUtils::getLogUserInfo() << "SIF_IgnoreImagesDeltaSumHV " << std::endl;
  for(const auto &p : srcImagesDeltaHVSum) {
    imageIndices.push_back(p.first);
    LogUtils::getLogUserInfo() << p.second << " ";
  }
  LogUtils::getLogUserInfo() << std::endl;

  LogUtils::getLogUserInfo() << "SIF_IgnoreImagesDeltaSumHV " << std::endl;
  for(auto i : imageIndices) {
    LogUtils::getLogUserInfo() << i << " ";
  }
  LogUtils::getLogUserInfo() << std::endl;
}

bool SIF_IgnoreImagesDeltaSumHV::pass(const StitchInfo &stitchInfo) const
{
  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    return false;
  }
  if(!fulfillsMinCriterias(stitchInfo, cf)) {
    return false;
  }

  int maxIndex = (int)imageIndices.size() - iteration;

  for(int i = (int)imageIndices.size() - 1; i >= maxIndex; --i) {
    if((int)stitchInfo.srcImageIndex == imageIndices[i] ||
       (int)stitchInfo.dstImageIndex == imageIndices[i]) {

      return false;
    }
  }
  return true;
}

bool SIF_IgnoreImagesDeltaSumHV::done() const
{
  int maxIndex = (int)imageIndices.size() - iteration;
  return maxIndex <= 1;
}

SIF_IgnoreEdgesDeltaSumHV::SIF_IgnoreEdgesDeltaSumHV(
  double confidenceThresh, const std::vector<const StitchInfo *> &stitchInfos)
  : StitchInfoFilter(confidenceThresh)
{
  FUNCLOGTIMEL("SIF_IgnoreEdgesDeltaSumHV::SIF_IgnoreEdgesDeltaSumHV");

  for(const auto *pStitchInfo : stitchInfos) {
    if(pStitchInfo != nullptr && fulfillsMinCriterias(*pStitchInfo, cf)) {
      _stitchInfos.push_back(pStitchInfo);
    }
  }

  std::sort(_stitchInfos.begin(), _stitchInfos.end(),
    [](const StitchInfo *s1, const StitchInfo *s2) {

      double delta1 = std::abs(s1->deltaH) + std::abs(s1->deltaV);
      double delta2 = std::abs(s2->deltaH) + std::abs(s2->deltaV);

      return delta1 < delta2;
  });

  LogUtils::getLogUserInfo() << "SIF_IgnoreEdgesDeltaSumHV " << std::endl;
  for(const auto *pStitchInfo : _stitchInfos) {
    double delta = std::abs(pStitchInfo->deltaH) + std::abs(pStitchInfo->deltaV);
    LogUtils::getLogUserInfo() << delta << " ";
  }
  LogUtils::getLogUserInfo() << std::endl;
}

bool SIF_IgnoreEdgesDeltaSumHV::pass(const StitchInfo &stitchInfo) const
{
  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    return false;
  }
  if(!fulfillsMinCriterias(stitchInfo, cf)) {
    return false;
  }

  int maxIndex = (int)_stitchInfos.size() - iteration * 2;
  for(int i = 0; i < (int)_stitchInfos.size(); ++i) {

    if(i > maxIndex) return false;

    if(stitchInfo.srcImageIndex == _stitchInfos[i]->srcImageIndex &&
       stitchInfo.dstImageIndex == _stitchInfos[i]->dstImageIndex) {
      
      return true;
    }
  }
  return false;
}

bool SIF_IgnoreEdgesDeltaSumHV::done() const
{
  int maxIndex = (int)_stitchInfos.size() - iteration * 2;
  return maxIndex <= 1;
}


SIF_IgnoreEdgesDistortion::SIF_IgnoreEdgesDistortion(
  double confidenceThresh,
  const std::vector<const StitchInfo *> &stitchInfos,
  const std::vector<cv::Size> &srcImagesSizes)
  : StitchInfoFilter(confidenceThresh)
{
  FUNCLOGTIMEL("SIF_IgnoreEdgesScale::SIF_IgnoreEdgesScale");

  auto dist = [](cv::Point2f pt1, cv::Point2f pt2) -> double {
    
    auto ptDiff = pt1 - pt2;
    return std::sqrt(ptDiff.x * ptDiff.x + ptDiff.y * ptDiff.y);
  };

  auto getValue = [&](const StitchInfo &stitchInfo) -> double {

    auto w = srcImagesSizes[stitchInfo.srcImageIndex].width;
    auto h = srcImagesSizes[stitchInfo.srcImageIndex].height;

    TPoints2f pts{
      cv::Point2f(0, 0), cv::Point2f(w, 0),
      cv::Point2f(w, h), cv::Point2f(0, h),
    };

    auto ptsWarped = WarperHelper::transformPtsf(pts, stitchInfo.matchInfo.homography);

    double d1 = dist(pts[0], pts[2]);
    double d2 = d1;

    auto d1W = dist(ptsWarped[0], ptsWarped[2]);
    auto d2W = dist(ptsWarped[1], ptsWarped[3]);

    auto v1 = d1 > d1W ? d1W / d1 : d1 / d1W;
    auto v2 = d2 > d2W ? d2W / d2 : d2 / d2W;

    return v1 * v2;
  };

  for(const auto *pStitchInfo : stitchInfos) {
    if(pStitchInfo != nullptr && fulfillsMinCriterias(*pStitchInfo, cf)) {
      _stitchInfos.push_back(std::make_pair(pStitchInfo, getValue(*pStitchInfo)));
    }
  }

  std::sort(_stitchInfos.begin(), _stitchInfos.end(),
    [&](std::pair<const StitchInfo *, double> &s1, std::pair<const StitchInfo *, double> &s2) {

      return s1.second > s2.second;
  });

  LogUtils::getLogUserInfo() << "SIF_IgnoreEdgesDistortion " << std::endl;
  for(const auto &s : _stitchInfos) {
    LogUtils::getLogUserInfo() << s.first->srcImageIndex << "->" << s.first->dstImageIndex << " " << s.second << ", ";
  }
  LogUtils::getLogUserInfo() << std::endl;
}

bool SIF_IgnoreEdgesDistortion::pass(const StitchInfo &stitchInfo) const
{

  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    return false;
  }
  if(!fulfillsMinCriterias(stitchInfo, cf)) {
    return false;
  }

  bool found1 = false;
  bool found2 = false;

  int maxIndex = (int)_stitchInfos.size() - iteration * 2;
  for(int i = 0; i < (int)_stitchInfos.size(); ++i) {

    if(i > maxIndex) {
      return false;
    }

    if(!found1 &&
       stitchInfo.srcImageIndex == _stitchInfos[i].first->srcImageIndex &&
       stitchInfo.dstImageIndex == _stitchInfos[i].first->dstImageIndex) {
      
      found1 = true;
    }
    else if(!found2 &&
       stitchInfo.dstImageIndex == _stitchInfos[i].first->srcImageIndex &&
       stitchInfo.srcImageIndex == _stitchInfos[i].first->dstImageIndex) {
      
      found2 = true;
    }

    if(found1 && found2) {
      return true;
    }
  }

  return false;
}

bool SIF_IgnoreEdgesDistortion::done() const
{
  int maxIndex = (int)_stitchInfos.size() - iteration * 2;
  return maxIndex <= 1;
}

SIF_IgnoreEdgesBlacklist::SIF_IgnoreEdgesBlacklist(double confidenceThresh, int edgesN)
  : StitchInfoFilter(confidenceThresh)
  , _edgesN(edgesN)
{
  FUNCLOGTIMEL("SIF_IgnoreEdgesBlacklist::SIF_IgnoreEdgesBlacklist");  
}

void SIF_IgnoreEdgesBlacklist::addEdge(int srcI, int dstI)
{
  blacklist.push_back(std::make_pair(srcI, dstI));
}

bool SIF_IgnoreEdgesBlacklist::pass(const StitchInfo &stitchInfo) const
{
  int srcI = (int)stitchInfo.srcImageIndex;
  int dstI = (int)stitchInfo.dstImageIndex;

  if(srcI == dstI) {
    return false;
  }
  if(!fulfillsMinCriterias(stitchInfo, cf)) {
    return false;
  }

  for(const auto & edge : blacklist) {
    if(srcI == edge.first &&
       dstI == edge.second) {
    
      return false;
    }
    if(dstI == edge.first &&
       srcI == edge.second) {
      
      return false;
    }
  }
  return true;
}

bool SIF_IgnoreEdgesBlacklist::done() const
{
  return (int)blacklist.size() >= _edgesN || doneFlag;
}

SIF_IgnoreEdgesInlierDistance::SIF_IgnoreEdgesInlierDistance(
  double confidenceThresh, const std::vector<const StitchInfo *> &stitchInfos)
  : StitchInfoFilter(confidenceThresh)
{
  FUNCLOGTIMEL("SIF_IgnoreEdgesInlierDistance::SIF_IgnoreEdgesInlierDistance");

  auto calcDistanceFromPts = [](TConstPoints2f &pts) -> double {
    double l, r, t, b;
    l = t = std::numeric_limits<double>::max();
    r = b = 0;
    for(const auto &pt : pts) {
      if(pt.x < l) l = pt.x;
      if(pt.x > r) r = pt.x;
      if(pt.y < t) t = pt.y;
      if(pt.y > b) b = pt.y;
    }
    return std::min(r - l, b - t);
  };

  auto calcDistance = [&](const StitchInfo &stitchInfo) {
    
    auto d1 = calcDistanceFromPts(stitchInfo.matchInfo.inlierPts1);
    auto d2 = calcDistanceFromPts(stitchInfo.matchInfo.inlierPts2);

    return std::min(d1, d2);
  };

  for(const auto *stitchInfo : stitchInfos) {

    if(stitchInfo->srcImageIndex == stitchInfo->dstImageIndex) {
      continue;
    }
    if(!fulfillsMinCriterias(*stitchInfo, cf)) {
      continue;
    }

    _stitchInfos.push_back(std::make_pair(stitchInfo, calcDistance(*stitchInfo)));
  }

  std::sort(_stitchInfos.begin(), _stitchInfos.end(),
    [](const std::pair<const StitchInfo *, double> &p1, const std::pair<const StitchInfo *, double> &p2) {

      return p1.second > p2.second;
  });

  LogUtils::getLogUserInfo() << "SIF_IgnoreEdgesInlierDistance " << std::endl;
  for(const auto &s : _stitchInfos) {
    LogUtils::getLogUserInfo() << s.first->srcImageIndex << "->" << s.first->dstImageIndex << " " << s.second << ", ";
  }
  LogUtils::getLogUserInfo() << std::endl;

}

bool SIF_IgnoreEdgesInlierDistance::pass(const StitchInfo &stitchInfo) const
{

  if(stitchInfo.srcImageIndex == stitchInfo.dstImageIndex) {
    return false;
  }
  if(!fulfillsMinCriterias(stitchInfo, cf)) {
    return false;
  }

  bool found1 = false;
  bool found2 = false;

  int maxIndex = (int)_stitchInfos.size() - iteration * 2;
  for(int i = 0; i < (int)_stitchInfos.size(); ++i) {

    if(i > maxIndex) {
      return false;
    }

    if(!found1 &&
       stitchInfo.srcImageIndex == _stitchInfos[i].first->srcImageIndex &&
       stitchInfo.dstImageIndex == _stitchInfos[i].first->dstImageIndex) {
      
      found1 = true;
    }
    else if(!found2 &&
       stitchInfo.dstImageIndex == _stitchInfos[i].first->srcImageIndex &&
       stitchInfo.srcImageIndex == _stitchInfos[i].first->dstImageIndex) {
      
      found2 = true;
    }

    if(found1 && found2) {
      return true;
    }

  }

  return false;
}

bool SIF_IgnoreEdgesInlierDistance::done() const
{
  int maxIndex = (int)_stitchInfos.size() - iteration * 2;
  return maxIndex <= 1;
}

} // imgalign