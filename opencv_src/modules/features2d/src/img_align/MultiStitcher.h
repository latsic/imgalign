#ifndef IMGALIGN_MULTISTITCHERH
#define IMGALIGN_MULTISTITCHERH

#include "CommonTypes.h"
#include "EnumTypes.h"
#include "DesMatcher.h"
#include "Settings.h"
#include <map>

namespace imgalign
{
  struct MatchInfo;
  class StitchInfoFilter;

  struct StitchInfo {

    StitchInfo(size_t srcI, size_t dstI);
    void resetCamData();
  
    size_t srcImageIndex;
    size_t dstImageIndex;

    MatchInfo matchInfo;
    // MatchInfo matchInfoInverse;

    mutable double deltaH = 0.0;
    mutable double deltaV = 0.0;

    mutable double absH = 0.0;
    mutable double absV = 0.0;

    mutable TMat matR;
    mutable TMat matK;

    ~StitchInfo();
  };

  struct StitchedImage {

    struct StitchedInfo {
      StitchedInfo();
      void release();
      TMat homography;
      double tx, ty;
      TMat rMat, kMat;
      TMat warpedImage, warpedMask;
      cv::Point tlCorner;
      cv::Point tlCornerRoi; 

      ~StitchedInfo();

    };
    ~StitchedImage();

    void init(
      const TMat &inImage,
      int inProjType,
      std::vector<const StitchInfo *> &stitchOrder,
      bool warpFirst,
      double *globalScale = nullptr);

    void addTranslation(double tx, double ty);
    void createMaskFor(size_t imageIndex);
    void setMaskFor(size_t imageIndex, TMat mat);
    void setHomographyFor(size_t imageIndex, TConstMat &h);
    void setKMatFor(size_t imageIndex, TConstMat &k);
    void setRMatFor(size_t imageIndex, TConstMat &r);
    void setXTranslationFor(size_t imageIndex, double tx);
    void setYTranslationFor(size_t imageIndex, double ty);
    void setCornerXFor(size_t imageIndex, double x);
    void setCornerYFor(size_t imageIndex, double y);
    void setCornerRoiXFor(size_t imageIndex, double x);
    void setCornerRoiYFor(size_t imageIndex, double y);
    TMat& warpedImage(size_t imageIndex);
    TMat& warpedMask(size_t imageIndex);

    TPoints2f getTransformedPts(
      TConstPoints2f &pts,
      const cv::Size &size,
      size_t index,
      double *globalScale = nullptr) const;

    void stitch(
      bool compensateExposure,
      bool rectifyPerspective,
      bool rectifyStretch,
      bool maxRectangle,
      BlendType blendType,
      double blendStrength,
      SeamFinderType seamFinderType,
      bool saveMemory);

    void stitchFast();
    
    TMat image;
    cv::Size imageSize;
    cv::Size imageSizeOrig;
    int projType;
    std::vector<size_t> imageIndices;
    std::map<size_t, StitchedInfo> stitchedInfos;
  };

  struct LastRunData {

    LastRunData(
      const Settings &inSettings,
      const std::vector<double> inFieldsOfView,
      const std::vector<const StitchInfo *> &inStitchOrder,
      double inGlobalScale);

    Settings settings;
    std::vector<double> fieldsOfView;
    std::vector<TMat> stitchOrderMatKs;
    std::vector<TMat> stitchOrderMatRs;
    double globalScale;
    bool aborted = false;
  };

  class MultiStitcher
  {
    public:
      using TStitchOrder = std::vector<const StitchInfo *>;
      using TConstIndices = const std::vector<size_t>;
      using TIndices = std::vector<size_t>;

      MultiStitcher(
        std::vector<TMat> &inSrcImages,
        const Settings &inSettings);

      ~MultiStitcher();
      
      bool initStiching(
        const std::vector<double> &inFieldsOfView,
        std::vector<int> &outStitchIndices);
      int stitchAll();      
      TConstMat &getStitchedImage();
      TConstMat &getStitchedImageCurrent(bool update = true);
      void releaseStitchedImage();
      void releaseStitchedData();
      const cv::Size &getStitchImageCurrentOrigSize();

      const TStitchOrder &getStitchOrder() const;
      
      bool stitchNext(const StitchInfo &stitchInfo);

      void signalAbort();

    private:
      void computeKeyPoints();
      size_t getCenterImage();
      const StitchInfo *findNextMatch(const TStitchOrder &rStitchOrder);
      const StitchInfo *findNextMatch(TConstIndices &dstI, TConstIndices &srcI);
      void calcAndStoreAllMatches();
      bool stitch(const StitchInfo &stitchInfo);
      bool stitch2(const StitchInfo &stitchInfo);
      const StitchInfo *getStitchInfo(size_t dstI, size_t srcI, bool createIf = true);
      bool hasStitchInfo(size_t dstI, size_t srcI);
      
      TStitchOrder computeStitchOrder(size_t startIndex);
      TStitchOrder computeStitchOrder();
      bool camEstimateAndBundleAdjustIf(TStitchOrder &rStitchOrder, double &rGlobalScale);
      

      void computeRelativeRotation(TStitchOrder &rStitchOrder);
      void computeRelativeRotation(std::vector<std::shared_ptr<StitchInfo>> &rStitchInfos);
      void computeRelativeRotation(const StitchInfo &stitchInfo);
      bool deltaAngleSumInRange(const StitchInfo &stitchInfo);
      bool matchesMinCriterias(const StitchInfo &stitchInfo);
      void computeAbsRotation(TStitchOrder &rStitchOrder);
      void setCamMatrices(TStitchOrder &rStitchOrder);
      void waveCorrection(TStitchOrder &rStitchOrder);

      void applyLastRunData();
      bool isCamBasicDataUpToDate();

      std::vector<TMat> &srcImages;
      std::vector<cv::Size> srcImagesSizes;
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
      bool rectifyPerspective = true;
      bool rectifyStretch = false;
      bool maxRectangle = false;
      bool camEstimate = true;
      BundleAdjustType bundleAdjustType;
      double globalScale = 0.0;
      bool colorTransfer;
      bool seamBlend;
      SeamFinderType seamFinderType = SeamFinderType::SFT_VORNOI;
      bool compensateExposure;
      BlendType blendType = BlendType::BT_MULTIBAND;
      double blendStrength = 5.0;
      TransformFinderType tfType;
      bool calcCenterImage = true;
      bool calcImageOrder = true;
      bool warpFirst = false;
      double confidenceThresh = 0.2;
      double confidenceThreshCam = 1.0;
      WaveCorrectType wcType = WaveCorrectType::WCT_AUTO;
      bool preserveAlphaChannelValue = false;
      int featureDetectionMaxPixelsN = 500000;
      int currentStitchedImageMaxPixelsN = 200000;
      size_t centerImageIndex = 0;
      bool keyPointsComputed = false;
      //DesMatcher matcher;
      TStitchOrder stitchOrder;
      double estimatedFieldOfViewH = 0.0;
      double estimatesFieldOfViewV = 0.0;

      bool abort = false;

      std::unique_ptr<LastRunData> lastRunData;
      std::unique_ptr<StitchInfoFilter> sifComputeOrder;
      std::unique_ptr<StitchInfoFilter> sifBundleAdjust;
      std::unique_ptr<StitchInfoFilter> sifCamEstimate;

      std::map<int, DesMatcher> matchers;
      const DesMatcher *getMatcher(int dstIndex);
      const DesMatcher &getMatcherRef(int dstIndex);
      const DesMatcher *createMatcher(int dstIndex, TConstMat& inDescriptors, TConstKeyPoints &inKeyPoints);
      const DesMatcher &getMatcher(int dstIndex, TConstMat& inDescriptors, TConstKeyPoints &inKeyPoints);
  };

}

#endif