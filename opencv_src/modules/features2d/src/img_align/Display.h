

#ifndef IMGALIGN_DISPLAYH
#define IMGALIGN_DISPLAYH

#include "CommonTypes.h"
#include "../precomp.hpp"

namespace imgalign
{

class Display{
		
	public:
						
		static Display* createDisplay(TConstImages& images, const cv::Size& targetSizePerImage, int borderX, int borderY, int type);
		static void     destroyDisplay();
		static Display* getDisplay();

										Display(TConstImages& images, const cv::Size& targetSizePerImage, int borderX, int borderY, int type);
										~Display();

    void						generateSideBySideImage();
		void						drawTargetRects();
		void						drawDestRects();
		void            drawLines(const TKeyPoints& keyPoints1, const TKeyPoints& keyPoints2, const TMatches& matches, size_t i1, size_t i2, const cv::Mat& mask);
		void            drawLines(TConstMatchInfos& inliers, TConstMatchInfos& outliers, size_t i1, size_t i2);
		void            drawLines(TConstPoints2f& pts1, TConstPoints2f& pts2, size_t i1, size_t i2, TColor color = TColor(255, 255, 0, 255));
		void            drawLines(TConstPoints& pts1, TConstPoints& pts2, size_t i1, size_t i2, TColor color = TColor(255, 255, 0, 255));
		void						drawLines(const TKeyPoints& keyPoints1, const TKeyPoints& keyPoints2, size_t i1, size_t i2);
		void            drawPoint(const cv::Point& point, size_t imageIndex, int sideLen = 1, TColor color = TColor(255, 255, 0, 255));
		void            drawPoint(const cv::Point2f& point, size_t imageIndex, int sideLen = 1, TColor color = TColor(255, 255, 0, 255));
		void            drawPoints(TConstPoints2f& pts, size_t imageIndex, TColor color1 = TColor(0, 255, 0, 255), TColor color2 = TColor(255, 255, 0, 255));
		void            drawPoints(TConstPoints& pts, size_t imageIndex, TColor color1 = TColor(0, 255, 0, 255), TColor color2 = TColor(255, 255, 0, 255));
		void            drawKeyPoints(TConstKeyPoints& pts, size_t imageIndex, TColor colorS = TColor(0, 255, 0, 255), TColor colorE = TColor(255, 255, 0, 255));

		void            clear();

		cv::Mat					&getDestImage();

	private:

		static Display  *display;

		TConstImages&   images;
		const cv::Size	targetSizePerImage;
		const int		borderX;
		const int		borderY;
		
		cv::Mat         destImage;

		double          lastImageScaleFactor() const;
		TRect           lastDestRect(double inScaleFactor) const;
		double					findMaximizedScalFactor() const;
		TRects					findDestRects(double inScaleFactor) const;
		cv::Point       convert(const cv::Point& pt, size_t imageIndex) const;

		std::string     windowName;

};

} // imgalign


#endif