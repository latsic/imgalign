#ifndef IMGALIGN_MULTISTITCHERH
#define IMGALIGN_MULTISTITCHERH

#include "CommonTypes.h"
#include "EnumTypes.h"
#include "DesMatcher.h"
#include <map>

namespace imgalign
{
  class Settings;
  struct MatchInfo;

  struct StitchInfo {

    StitchInfo(size_t srcI, size_t dstI);
    void resetCamData();

    size_t srcImageIndex;
    size_t dstImageIndex;

    MatchInfo matchInfo;
    MatchInfo matchInfoInverse;

    mutable double deltaH = 0.0;
    mutable double deltaV = 0.0;

    mutable double absH = 0.0;
    mutable double absV = 0.0;

    mutable TMat matR;
    mutable TMat matK;
  };

  struct StitchedImage {

    struct StitchedInfo {
      StitchedInfo();
      TMat homography;
      double tx, ty;
      TMat rMat, kMat;
      TMat warpedImage, warpedMask;
      cv::Point tlCorner;  
    };

    void init(
      const TMat &inImage,
      int inProjType,
      std::vector<const StitchInfo *> &stitchOrder,
      double *globalScale = nullptr);

    void addTranslation(double tx, double ty);
    void createMaskFor(size_t imageIndex);
    void setHomographyFor(size_t imageIndex, TConstMat &h);
    void setKMatFor(size_t imageIndex, TConstMat &k);
    void setRMatFor(size_t imageIndex, TConstMat &r);
    void setXTranslationFor(size_t imageIndex, double tx);
    void setYTranslationFor(size_t imageIndex, double ty);
    void setCornerXFor(size_t imageIndex, double x);
    void setCornerYFor(size_t imageIndex, double y);
    TMat& warpedImage(size_t imageIndex);

    TPoints2f getTransformedPts(
      TConstPoints2f &pts,
      const cv::Size &size,
      //double fieldOfView,
      size_t index,
      double *globalScale = nullptr) const;

    void stitch(bool fastAndUgly, bool seamBlend, bool compensateExposure, bool rectify);
   
    TMat image;
    cv::Size imageSize;
    int projType;
    std::vector<size_t> imageIndices;
    std::map<size_t, StitchedInfo> stitchedInfos;
  };

  class MultiStitcher
  {
    public:
      using TStitchOrder = std::vector<const StitchInfo *>;
      using TConstIndices = const std::vector<size_t>;
      using TIndices = std::vector<size_t>;

      MultiStitcher(
        const std::vector<TConstMat> &inSrcImages,
        const Settings &inSettings);
      
      bool initStiching(
        const std::vector<double> &inFieldsOfView,
        std::vector<int> &outStitchIndices);
      int stitchAll();      
      TConstMat &getStitchedImage();
      TConstMat &getStitchedImageCurrent();

      const TStitchOrder &getStitchOrder() const;
      
      bool stitchNext(const StitchInfo &stitchInfo);

    private:
      void computeKeyPoints();
      size_t getCenterImage();
      const StitchInfo *findNextMatch(const TStitchOrder &rStitchOrder);
      const StitchInfo *findNextMatch(TConstIndices &dstI, TConstIndices &srcI, const TStitchOrder *pStitchOrder = nullptr);
      bool stitch(const StitchInfo &stitchInfo);
      const StitchInfo *getStitchInfo(size_t dstI, size_t srcI);

      TStitchOrder computeStitchOrder(size_t startIndex);
      TStitchOrder computeStitchOrder();
      const StitchInfo *computeStitchInfoFirstLast(TStitchOrder &rStitchOrder);
      bool camEstimateAndBundleAdjust(TStitchOrder &rStitchOrder, const StitchInfo *pStitchInfoFirstLast, double &rGlobalScale);
      void computeRelativeRotation(TStitchOrder &rStitchOrder);
      void computeRelativeRotation(const StitchInfo &stitchInfo);
      void computeAbsRotation(TStitchOrder &rStitchOrder);
      void setCamMatrices(TStitchOrder &rStitchOrder);
      void waveCorrection(TStitchOrder &rStitchOrder);

      const std::vector<TConstMat> &srcImages;
      std::vector<TMat> srcImagesScaled;
      std::vector<double> scaleFactors;
      std::vector<TKeyPoints> keyPoints;
      std::vector<TPoints2f> points;
      std::vector<TMat> descriptors;
      StitchedImage stitchedImage;
      std::vector<std::shared_ptr<StitchInfo>> stitchInfos;
      const Settings &settings;
      std::vector<double> fieldsOfView;
      int projectionType;
      bool rectify = true;
      bool camEstimate = true;
      BundleAdjustType bundleAdjustType;
      double globalScale = 0.0;
      bool colorTransfer;
      bool seamBlend;
      bool compensateExposure;
      TransformFinderType tfType;
      bool calcCenterImage = true;
      bool calcImageOrder = true;
      double confidenceThresh = 0.2;
      WaveCorrectType wcType = WaveCorrectType::WCT_AUTO;
      size_t centerImageIndex = 0;
      bool keyPointsComputed = false;
      DesMatcher matcher;
      TStitchOrder stitchOrder;
      const StitchInfo *stitchInfoFirstLast = nullptr;
      double estimatedFieldOfViewH = 0.0;
      double estimatesFieldOfViewV = 0.0;
  };

}

#endif