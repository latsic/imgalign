#ifndef IMGALIGN_IMGALIGN_WARPERHELPERH
#define IMGALIGN_IMGALIGN_WARPERHELPERH

#include "CommonTypes.h"


namespace cv
{
  namespace detail
  {
    class RotationWarper;
  }
}

namespace imgalign
{
  class WarperHelper {
    public:
      static cv::Ptr<cv::detail::RotationWarper> getWarper(int warperType, float scale);
      static void getMatR(double yaw, double pitch, double roll, TMat &outMat);
      static void getMatK(double width, double height, double focalLengthPx, TMat &outMat);
      static double getFocalLengthPx(double width, double height, double fieldOfView);
      static double getFieldOfView(double width, double height, double fLenPx);
      static std::vector<cv::Point> transformPts(
        const std::vector<cv::Point> &pts, TConstMat &homography);
      static std::vector<cv::Point2f> transformPtsf(
        const std::vector<cv::Point2f> &pts, TConstMat &homography);

      static cv::Point2f getCenter(const std::initializer_list<cv::Point2f> &pts);

      static void getRelativeRotation(
        double w1, double h1, double w2, double h2,
        double fieldOfViewX1,
        TConstMat &homography,
        double &outYaw, double &outPitch);

      static void warpPoints(
        double w, double h, double fieldOfView,
        TConstMat &rotMat,
        int warperType,
        TPoints2f &ioPts);

      static cv::Point warpImage(
        int warperType,
        TConstMat& srcMat,
        TMat &outMat,
        double fieldOfView,
        TConstMat &rotMat,
        bool useBorderReflect,
        bool useLinear);

      static void warpPoints(
        double w, double h,
        TConstMat &kMat,
        TConstMat &rotMat,
        int warperType,
        TPoints2f &ioPts,
        double *globalScale = nullptr);

      static cv::Point warpImage(
        int warperType,
        TConstMat& srcMat,
        TMat &outMat,
        TConstMat &kMat,
        TConstMat &rotMat,
        bool useBorderReflect,
        bool useLinear,
        double *globalScale = nullptr);

      // static void warpImageBackwards(
      //   int warperType,
      //   TConstMat& srcMat,
      //   TMat &outMat,
      //   double fieldOfView,
      //   double yaw, double pitch, double roll);

      static void getBox(
        double w1, double h1, double w2, double h2,
        TMat &ioHomography,
        double &outTx, double &outTy,
        double &outT, double &outR, double &outB, double &outL);

      static void getBox(
        double w, double h,
        TMat &ioHomography,
        double &outTx, double &outTy,
        double &outT, double &outR, double &outB, double &outL);

      static double fieldOfView(
        double fieldOfView1, double fieldOfView2, double yaw);

      static void warpPerspective(
        TConstMat &src,
        TConstMat &homography, 
        cv::Size dstSize,
        TMat &dst,
        bool useBorderReplicate = false,
        bool useLinear = true);

      static void waveCorrect(
        std::vector<TMat> &rmats, bool horizontal);
  };

}

#endif