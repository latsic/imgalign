#ifndef IMGALIGN_STITCHINFOFILTERH
#define IMGALIGN_STITCHINFOFILTERH

#include "CommonTypes.h"
#include "EnumTypes.h"
#include "MultiStitcher.h"
#include "../precomp.hpp"

namespace imgalign {

struct StitchInfo;

class InputImagesReach {
  public:
    InputImagesReach(int inReach, int inRange);
    virtual ~InputImagesReach() {}
    bool insideReach(int srcI, int dstI) const;
  private:
    int reach;
    int rangeStart;
    int rangeEnd;
};

class StitchInfoFilter {

  public:
    StitchInfoFilter(double confidenceThresh, const InputImagesReach *pReach);
    virtual ~StitchInfoFilter() {}
    virtual bool pass(const StitchInfo &stitchInfo) const = 0;
    virtual bool done() const;
    virtual void addEdge(int /*srcI*/, int /*dstI*/) {};
    virtual void setDone() {}
    void setIteration(int i);
    
  protected:
    int iteration = 0;
    double cf = 0.4;
    const InputImagesReach *reach = nullptr;
};

class SIF_Std : public StitchInfoFilter {
  public:
    SIF_Std(double confidenceThresh, const InputImagesReach *pReach);
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;
};

class SIF_IgnoreEdgesConfidence : public StitchInfoFilter {
  public:
    SIF_IgnoreEdgesConfidence(
      double confidenceThresh,
      const InputImagesReach *pReach,
      const std::vector<const StitchInfo *> &stitchInfos);
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;
  
  private:
    std::vector<const StitchInfo *> _stitchInfos;
};

class SIF_IgnoreImagesConfidence : public StitchInfoFilter {
  public:
    SIF_IgnoreImagesConfidence(
      double confidenceThresh,
      const InputImagesReach *pReach,
      const std::vector<const StitchInfo *> &stitchInfos);
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;
  
  private:
    std::vector<int> imageIndices;
};

class SIF_BestNeighbourOnly : public StitchInfoFilter {
  public:
    SIF_BestNeighbourOnly(
      double confidenceThresh,
      const InputImagesReach *pReach,
      const std::vector<const StitchInfo *> &stitchOrder);
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;
  
  private:
    const std::vector<const StitchInfo *> &_stitchOrder;
};

class SIF_IgnoreImagesDeltaSumHV : public StitchInfoFilter {
  public:
    SIF_IgnoreImagesDeltaSumHV(
      double confidenceThresh,
      const InputImagesReach *pReach,
      const std::vector<const StitchInfo *> &stitchInfos);
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;

  private:
    std::vector<int> imageIndices;
};

class SIF_IgnoreEdgesDeltaSumHV : public StitchInfoFilter {
  public:
    SIF_IgnoreEdgesDeltaSumHV(
      double confidenceThresh,
      const InputImagesReach *pReach,
      const std::vector<const StitchInfo *> &stitchInfos);
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;

  private:
    std::vector<const StitchInfo *> _stitchInfos;
};

class SIF_IgnoreEdgesDistortion : public StitchInfoFilter {
  public:
    SIF_IgnoreEdgesDistortion(
      double confidenceThresh,
      const InputImagesReach *pReach,
      const std::vector<const StitchInfo *> &stitchInfos,
      const std::vector<cv::Size> &srcImagesSizes);
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;

  private:
    std::vector<std::pair<const StitchInfo *, double>> _stitchInfos;
};

class SIF_IgnoreEdgesBlacklist : public StitchInfoFilter {
  public:
    SIF_IgnoreEdgesBlacklist(double confidenceThresh, const InputImagesReach *pReach, int edgesN);
    virtual void addEdge(int srcI, int dstI) override;
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;
    virtual void setDone() override { doneFlag = true; }
  private:
    std::vector<std::pair<int, int>> blacklist;
    int _edgesN;
    bool doneFlag = false;
};

class SIF_IgnoreEdgesInlierDistance : public StitchInfoFilter {
  public:
    SIF_IgnoreEdgesInlierDistance(
      double confidenceThresh,
      const InputImagesReach *pReach,
      const std::vector<const StitchInfo *> &stitchInfos);
    virtual bool pass(const StitchInfo &stitchInfo) const override;
    virtual bool done() const override;
  private:
    std::vector<std::pair<const StitchInfo *, double>> _stitchInfos;
};

}

#endif