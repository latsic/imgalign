/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef OPENCV_FEATURES_2D_HPP
#define OPENCV_FEATURES_2D_HPP

#include "opencv2/opencv_modules.hpp"
#include "opencv2/core.hpp"

#ifdef HAVE_OPENCV_FLANN
#include "opencv2/flann/miniflann.hpp"
#endif

/**
  @defgroup features2d 2D Features Framework
  @{
    @defgroup features2d_main Feature Detection and Description
    @defgroup features2d_match Descriptor Matchers

Matchers of keypoint descriptors in OpenCV have wrappers with a common interface that enables you to
easily switch between different algorithms solving the same problem. This section is devoted to
matching descriptors that are represented as vectors in a multidimensional space. All objects that
implement vector descriptor matchers inherit the DescriptorMatcher interface.

@note
   -   An example explaining keypoint matching can be found at
        opencv_source_code/samples/cpp/descriptor_extractor_matcher.cpp
    -   An example on descriptor matching evaluation can be found at
        opencv_source_code/samples/cpp/detector_descriptor_matcher_evaluation.cpp
    -   An example on one to many image matching can be found at
        opencv_source_code/samples/cpp/matching_to_many_images.cpp

    @defgroup features2d_draw Drawing Function of Keypoints and Matches
    @defgroup features2d_category Object Categorization

This section describes approaches based on local 2D features and used to categorize objects.

@note
   -   A complete Bag-Of-Words sample can be found at
        opencv_source_code/samples/cpp/bagofwords_classification.cpp
    -   (Python) An example using the features2D framework to perform object categorization can be
        found at opencv_source_code/samples/python/find_obj.py

  @}
 */

namespace cv
{

//! @addtogroup features2d
//! @{

// //! writes vector of keypoints to the file storage
// CV_EXPORTS void write(FileStorage& fs, const String& name, const std::vector<KeyPoint>& keypoints);
// //! reads vector of keypoints from the specified file storage node
// CV_EXPORTS void read(const FileNode& node, CV_OUT std::vector<KeyPoint>& keypoints);

/** @brief A class filters a vector of keypoints.

 Because now it is difficult to provide a convenient interface for all usage scenarios of the
 keypoints filter class, it has only several needed by now static methods.
 */
class CV_EXPORTS KeyPointsFilter
{
public:
    KeyPointsFilter(){}

    /*
     * Remove keypoints within borderPixels of an image edge.
     */
    static void runByImageBorder( std::vector<KeyPoint>& keypoints, Size imageSize, int borderSize );
    /*
     * Remove keypoints of sizes out of range.
     */
    static void runByKeypointSize( std::vector<KeyPoint>& keypoints, float minSize,
                                   float maxSize=FLT_MAX );
    /*
     * Remove keypoints from some image by mask for pixels of this image.
     */
    static void runByPixelsMask( std::vector<KeyPoint>& keypoints, const Mat& mask );
    /*
     * Remove duplicated keypoints.
     */
    static void removeDuplicated( std::vector<KeyPoint>& keypoints );
    /*
     * Remove duplicated keypoints and sort the remaining keypoints
     */
    static void removeDuplicatedSorted( std::vector<KeyPoint>& keypoints );

    /*
     * Retain the specified number of the best keypoints (according to the response)
     */
    static void retainBest( std::vector<KeyPoint>& keypoints, int npoints );
};


class CV_EXPORTS_W ImgStitch
{
  public:
    virtual ~ImgStitch();
    CV_WRAP virtual void setImages(InputArrayOfArrays images);
    CV_WRAP virtual void set(
        CV_IN_OUT std::vector<int> &valueTypes,
		CV_IN_OUT std::vector<float> &values);
    CV_WRAP virtual int stitch(
        CV_IN_OUT std::vector<float> &fieldsOfView,
        OutputArray stitchedImage);
    CV_WRAP virtual void stitchStart(
      CV_IN_OUT std::vector<float> &fieldsOfView,
      OutputArray stitchedImage,
      CV_IN_OUT std::vector<int> &stitchIndices);
    CV_WRAP virtual int stitchNext(
        OutputArray stitchedImage,
        OutputArray stitchedImageSmall);

    CV_WRAP static Ptr<ImgStitch> create(InputArrayOfArrays images);
};

class CV_EXPORTS_W ImgAlign
{
  public:
    virtual ~ImgAlign();
    
    CV_WRAP virtual void set(
        CV_IN_OUT std::vector<int> &valueTypes,
		CV_IN_OUT std::vector<float> &values);

    CV_WRAP virtual int match();

    CV_WRAP virtual int match_getAlignedImage(
        OutputArray alignedImage);

    CV_WRAP virtual int match_getTransMatrix(
        OutputArray transMat);

    CV_WRAP virtual int match_getExtData(
        CV_IN_OUT std::vector<Point> &fixedImageMaskPolygon,
		CV_IN_OUT std::vector<Point> &movingImageMaskPolygon,
        OutputArray transMat,
        CV_OUT std::vector<Point> &goodMatchesFixedImage,
        CV_OUT std::vector<Point> &goodMatchesMovingImage,
	    CV_OUT std::vector<Point> &goodInlierMatchesFixedImage,
        CV_OUT std::vector<Point> &goodInlierMatchesMovingImage);

    CV_WRAP virtual void getImageAligned(
        InputArray transMatrix,
        OutputArray alignedImage);

    CV_WRAP virtual void getImageBlended(
        InputArray transMatrix,
        double weight,
        int doOverlay,
        OutputArray blendImage);

    CV_WRAP virtual void getImageBlendedPolygonFixedImage(
        InputArray transMatrix,
        double weight,
        int doOverlay,
        CV_IN_OUT std::vector<Point> &polygon,
        OutputArray blendImage);

    CV_WRAP virtual void getImageBlendedPolygonMovingImage(
        InputArray transMatrix,
        double weight,
        int doOverlay,
        CV_IN_OUT std::vector<Point> &polygon,
        OutputArray blendImage);

    CV_WRAP virtual void getImageFloodFillFixedImage(
        InputArray transMatrix,
        CV_IN_OUT std::vector<Point> &seedPts,
        CV_IN_OUT std::vector<int> &floodFillTolerances,
        double weight,
        int doOverlay,
        OutputArray blendImage);

	CV_WRAP virtual void getImageFloodFillMovingImage(
        InputArray transMatrix,
        CV_IN_OUT std::vector<Point> &seedPts,
        CV_IN_OUT std::vector<int> &floodFillTolerances,
        double weight,
        int doOverlay,
        OutputArray blendImage);

    CV_WRAP virtual void getStitchedImage(
        InputArray transMatrix,
        CV_IN_OUT std::vector<Point> &goodMatchesFixedImage,
        double weight,
        OutputArray blendImage);

    CV_WRAP virtual void getFixedImageWithMatchedPoints(
        CV_IN_OUT std::vector<Point> &points,
        OutputArray fixedImageWithMatchedPoints);

    CV_WRAP virtual void getMovingImageWithMatchedPoints(
        CV_IN_OUT std::vector<Point> &points,
        OutputArray movingImageWithMatchedPoints);

    CV_WRAP virtual void getSideBySideImage(
        CV_IN_OUT std::vector<Point> &matchedPointsFixedImage,
        CV_IN_OUT std::vector<Point> &matchedPointsMovingImage,
        OutputArray sideBySideImage);

    CV_WRAP virtual void compare(
        int imageType, 
        CV_IN_OUT std::vector<int> &detectorTypes,
        OutputArray sideBySideImage,
        CV_IN_OUT std::vector<int> &timeUsedMs,
        CV_IN_OUT std::vector<int> &keyPointsN,
        CV_IN_OUT std::vector<cv::Point> &fixedImageMaskPolygon,
		CV_IN_OUT std::vector<cv::Point> &movingImageMaskPolygon);

    CV_WRAP virtual double stitch(
        InputArray transMatrix,
        int projectionType1,
        int projectionType2,
        int blend,
        int transferColors,
        double fieldOfViewFixedImage,
        double fieldOfViewMovingImage,
        int calcYaw2,
        int calcPitch2,
        double yaw1, double pitch1,
        double yaw2, double pitch2,
        OutputArray stitchedImage);

    CV_WRAP static Ptr<ImgAlign> create(
        InputArray fixedImage,
        InputArray movingImage);
};

/************************************ Base Classes ************************************/

/** @brief Abstract base class for 2D image feature detectors and descriptor extractors
*/
#ifdef __EMSCRIPTEN__
class CV_EXPORTS_W Feature2D : public Algorithm
#else
class CV_EXPORTS_W Feature2D : public virtual Algorithm
#endif
{
public:
    virtual ~Feature2D();

    /** @brief Detects keypoints in an image (first variant) or image set (second variant).

    @param image Image.
    @param keypoints The detected keypoints. In the second variant of the method keypoints[i] is a set
    of keypoints detected in images[i] .
    @param mask Mask specifying where to look for keypoints (optional). It must be a 8-bit integer
    matrix with non-zero values in the region of interest.
     */
    CV_WRAP virtual void detect( InputArray image,
                                 CV_OUT std::vector<KeyPoint>& keypoints,
                                 InputArray mask=noArray() );

    /** @overload
    @param images Image set.
    @param keypoints The detected keypoints. In the second variant of the method keypoints[i] is a set
    of keypoints detected in images[i] .
    @param masks Masks for each input image specifying where to look for keypoints (optional).
    masks[i] is a mask for images[i].
    */
    CV_WRAP virtual void detect( InputArrayOfArrays images,
                         CV_OUT std::vector<std::vector<KeyPoint> >& keypoints,
                         InputArrayOfArrays masks=noArray() );

    /** @brief Computes the descriptors for a set of keypoints detected in an image (first variant) or image set
    (second variant).

    @param image Image.
    @param keypoints Input collection of keypoints. Keypoints for which a descriptor cannot be
    computed are removed. Sometimes new keypoints can be added, for example: SIFT duplicates keypoint
    with several dominant orientations (for each orientation).
    @param descriptors Computed descriptors. In the second variant of the method descriptors[i] are
    descriptors computed for a keypoints[i]. Row j is the keypoints (or keypoints[i]) is the
    descriptor for keypoint j-th keypoint.
     */
    CV_WRAP virtual void compute( InputArray image,
                                  CV_OUT CV_IN_OUT std::vector<KeyPoint>& keypoints,
                                  OutputArray descriptors );

    /** @overload

    @param images Image set.
    @param keypoints Input collection of keypoints. Keypoints for which a descriptor cannot be
    computed are removed. Sometimes new keypoints can be added, for example: SIFT duplicates keypoint
    with several dominant orientations (for each orientation).
    @param descriptors Computed descriptors. In the second variant of the method descriptors[i] are
    descriptors computed for a keypoints[i]. Row j is the keypoints (or keypoints[i]) is the
    descriptor for keypoint j-th keypoint.
    */
    CV_WRAP virtual void compute( InputArrayOfArrays images,
                          CV_OUT CV_IN_OUT std::vector<std::vector<KeyPoint> >& keypoints,
                          OutputArrayOfArrays descriptors );

    /** Detects keypoints and computes the descriptors */
    CV_WRAP virtual void detectAndCompute( InputArray image, InputArray mask,
                                           CV_OUT std::vector<KeyPoint>& keypoints,
                                           OutputArray descriptors,
                                           bool useProvidedKeypoints=false );

    CV_WRAP virtual int descriptorSize() const;
    CV_WRAP virtual int descriptorType() const;
    CV_WRAP virtual int defaultNorm() const;

    CV_WRAP void write( const String& fileName ) const;

    CV_WRAP void read( const String& fileName );

    virtual void write( FileStorage&) const CV_OVERRIDE;

    // see corresponding cv::Algorithm method
    CV_WRAP virtual void read( const FileNode&) CV_OVERRIDE;

    //! Return true if detector object is empty
    CV_WRAP virtual bool empty() const CV_OVERRIDE;
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;

    // see corresponding cv::Algorithm method
    CV_WRAP inline void write(const Ptr<FileStorage>& fs, const String& name = String()) const { Algorithm::write(fs, name); }
};

/** Feature detectors in OpenCV have wrappers with a common interface that enables you to easily switch
between different algorithms solving the same problem. All objects that implement keypoint detectors
inherit the FeatureDetector interface. */
typedef Feature2D FeatureDetector;

/** Extractors of keypoint descriptors in OpenCV have wrappers with a common interface that enables you
to easily switch between different algorithms solving the same problem. This section is devoted to
computing descriptors represented as vectors in a multidimensional space. All objects that implement
the vector descriptor extractors inherit the DescriptorExtractor interface.
 */
typedef Feature2D DescriptorExtractor;

//! @addtogroup features2d_main
//! @{

/** @brief Class implementing the BRISK keypoint detector and descriptor extractor, described in @cite LCS11 .
 */
class CV_EXPORTS_W BRISK : public Feature2D
{
public:
    /** @brief The BRISK constructor

    @param thresh AGAST detection threshold score.
    @param octaves detection octaves. Use 0 to do single scale.
    @param patternScale apply this scale to the pattern used for sampling the neighbourhood of a
    keypoint.
     */
    CV_WRAP static Ptr<BRISK> create(int thresh=30, int octaves=3, float patternScale=1.0f);

    /** @brief The BRISK constructor for a custom pattern

    @param radiusList defines the radii (in pixels) where the samples around a keypoint are taken (for
    keypoint scale 1).
    @param numberList defines the number of sampling points on the sampling circle. Must be the same
    size as radiusList..
    @param dMax threshold for the short pairings used for descriptor formation (in pixels for keypoint
    scale 1).
    @param dMin threshold for the long pairings used for orientation determination (in pixels for
    keypoint scale 1).
    @param indexChange index remapping of the bits. */
    CV_WRAP static Ptr<BRISK> create(const std::vector<float> &radiusList, const std::vector<int> &numberList,
        float dMax=5.85f, float dMin=8.2f, const std::vector<int>& indexChange=std::vector<int>());

    /** @brief The BRISK constructor for a custom pattern, detection threshold and octaves

    @param thresh AGAST detection threshold score.
    @param octaves detection octaves. Use 0 to do single scale.
    @param radiusList defines the radii (in pixels) where the samples around a keypoint are taken (for
    keypoint scale 1).
    @param numberList defines the number of sampling points on the sampling circle. Must be the same
    size as radiusList..
    @param dMax threshold for the short pairings used for descriptor formation (in pixels for keypoint
    scale 1).
    @param dMin threshold for the long pairings used for orientation determination (in pixels for
    keypoint scale 1).
    @param indexChange index remapping of the bits. */
    CV_WRAP static Ptr<BRISK> create(int thresh, int octaves, const std::vector<float> &radiusList,
        const std::vector<int> &numberList, float dMax=5.85f, float dMin=8.2f,
        const std::vector<int>& indexChange=std::vector<int>());
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

/** @brief Class implementing the ORB (*oriented BRIEF*) keypoint detector and descriptor extractor

described in @cite RRKB11 . The algorithm uses FAST in pyramids to detect stable keypoints, selects
the strongest features using FAST or Harris response, finds their orientation using first-order
moments and computes the descriptors using BRIEF (where the coordinates of random point pairs (or
k-tuples) are rotated according to the measured orientation).
 */
class CV_EXPORTS_W ORB : public Feature2D
{
public:
    enum ScoreType { HARRIS_SCORE=0, FAST_SCORE=1 };
    static const int kBytes = 32;

    /** @brief The ORB constructor

    @param nfeatures The maximum number of features to retain.
    @param scaleFactor Pyramid decimation ratio, greater than 1. scaleFactor==2 means the classical
    pyramid, where each next level has 4x less pixels than the previous, but such a big scale factor
    will degrade feature matching scores dramatically. On the other hand, too close to 1 scale factor
    will mean that to cover certain scale range you will need more pyramid levels and so the speed
    will suffer.
    @param nlevels The number of pyramid levels. The smallest level will have linear size equal to
    input_image_linear_size/pow(scaleFactor, nlevels - firstLevel).
    @param edgeThreshold This is size of the border where the features are not detected. It should
    roughly match the patchSize parameter.
    @param firstLevel The level of pyramid to put source image to. Previous layers are filled
    with upscaled source image.
    @param WTA_K The number of points that produce each element of the oriented BRIEF descriptor. The
    default value 2 means the BRIEF where we take a random point pair and compare their brightnesses,
    so we get 0/1 response. Other possible values are 3 and 4. For example, 3 means that we take 3
    random points (of course, those point coordinates are random, but they are generated from the
    pre-defined seed, so each element of BRIEF descriptor is computed deterministically from the pixel
    rectangle), find point of maximum brightness and output index of the winner (0, 1 or 2). Such
    output will occupy 2 bits, and therefore it will need a special variant of Hamming distance,
    denoted as NORM_HAMMING2 (2 bits per bin). When WTA_K=4, we take 4 random points to compute each
    bin (that will also occupy 2 bits with possible values 0, 1, 2 or 3).
    @param scoreType The default HARRIS_SCORE means that Harris algorithm is used to rank features
    (the score is written to KeyPoint::score and is used to retain best nfeatures features);
    FAST_SCORE is alternative value of the parameter that produces slightly less stable keypoints,
    but it is a little faster to compute.
    @param patchSize size of the patch used by the oriented BRIEF descriptor. Of course, on smaller
    pyramid layers the perceived image area covered by a feature will be larger.
    @param fastThreshold
     */
    CV_WRAP static Ptr<ORB> create(int nfeatures=500, float scaleFactor=1.2f, int nlevels=8, int edgeThreshold=31,
        int firstLevel=0, int WTA_K=2, ORB::ScoreType scoreType=ORB::HARRIS_SCORE, int patchSize=31, int fastThreshold=20);

    CV_WRAP virtual void setMaxFeatures(int maxFeatures) = 0;
    CV_WRAP virtual int getMaxFeatures() const = 0;

    CV_WRAP virtual void setScaleFactor(double scaleFactor) = 0;
    CV_WRAP virtual double getScaleFactor() const = 0;

    CV_WRAP virtual void setNLevels(int nlevels) = 0;
    CV_WRAP virtual int getNLevels() const = 0;

    CV_WRAP virtual void setEdgeThreshold(int edgeThreshold) = 0;
    CV_WRAP virtual int getEdgeThreshold() const = 0;

    CV_WRAP virtual void setFirstLevel(int firstLevel) = 0;
    CV_WRAP virtual int getFirstLevel() const = 0;

    CV_WRAP virtual void setWTA_K(int wta_k) = 0;
    CV_WRAP virtual int getWTA_K() const = 0;

    CV_WRAP virtual void setScoreType(ORB::ScoreType scoreType) = 0;
    CV_WRAP virtual ORB::ScoreType getScoreType() const = 0;

    CV_WRAP virtual void setPatchSize(int patchSize) = 0;
    CV_WRAP virtual int getPatchSize() const = 0;

    CV_WRAP virtual void setFastThreshold(int fastThreshold) = 0;
    CV_WRAP virtual int getFastThreshold() const = 0;
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

/** @brief Maximally stable extremal region extractor

The class encapsulates all the parameters of the %MSER extraction algorithm (see [wiki
article](http://en.wikipedia.org/wiki/Maximally_stable_extremal_regions)).

- there are two different implementation of %MSER: one for grey image, one for color image

- the grey image algorithm is taken from: @cite nister2008linear ;  the paper claims to be faster
than union-find method; it actually get 1.5~2m/s on my centrino L7200 1.2GHz laptop.

- the color image algorithm is taken from: @cite forssen2007maximally ; it should be much slower
than grey image method ( 3~4 times ); the chi_table.h file is taken directly from paper's source
code which is distributed under GPL.

- (Python) A complete example showing the use of the %MSER detector can be found at samples/python/mser.py
*/
class CV_EXPORTS_W MSER : public Feature2D
{
public:
    /** @brief Full consturctor for %MSER detector

    @param _delta it compares \f$(size_{i}-size_{i-delta})/size_{i-delta}\f$
    @param _min_area prune the area which smaller than minArea
    @param _max_area prune the area which bigger than maxArea
    @param _max_variation prune the area have similar size to its children
    @param _min_diversity for color image, trace back to cut off mser with diversity less than min_diversity
    @param _max_evolution  for color image, the evolution steps
    @param _area_threshold for color image, the area threshold to cause re-initialize
    @param _min_margin for color image, ignore too small margin
    @param _edge_blur_size for color image, the aperture size for edge blur
     */
    CV_WRAP static Ptr<MSER> create( int _delta=5, int _min_area=60, int _max_area=14400,
          double _max_variation=0.25, double _min_diversity=.2,
          int _max_evolution=200, double _area_threshold=1.01,
          double _min_margin=0.003, int _edge_blur_size=5 );

    /** @brief Detect %MSER regions

    @param image input image (8UC1, 8UC3 or 8UC4, must be greater or equal than 3x3)
    @param msers resulting list of point sets
    @param bboxes resulting bounding boxes
    */
    CV_WRAP virtual void detectRegions( InputArray image,
                                        CV_OUT std::vector<std::vector<Point> >& msers,
                                        CV_OUT std::vector<Rect>& bboxes ) = 0;

    CV_WRAP virtual void setDelta(int delta) = 0;
    CV_WRAP virtual int getDelta() const = 0;

    CV_WRAP virtual void setMinArea(int minArea) = 0;
    CV_WRAP virtual int getMinArea() const = 0;

    CV_WRAP virtual void setMaxArea(int maxArea) = 0;
    CV_WRAP virtual int getMaxArea() const = 0;

    CV_WRAP virtual void setPass2Only(bool f) = 0;
    CV_WRAP virtual bool getPass2Only() const = 0;
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

//! @} features2d_main

//! @addtogroup features2d_main
//! @{

/** @brief Wrapping class for feature detection using the FAST method. :
 */
class CV_EXPORTS_W FastFeatureDetector : public Feature2D
{
public:
    enum DetectorType
    {
        TYPE_5_8 = 0, TYPE_7_12 = 1, TYPE_9_16 = 2
    };
    enum
    {
        THRESHOLD = 10000, NONMAX_SUPPRESSION=10001, FAST_N=10002
    };


    CV_WRAP static Ptr<FastFeatureDetector> create( int threshold=10,
                                                    bool nonmaxSuppression=true,
                                                    FastFeatureDetector::DetectorType type=FastFeatureDetector::TYPE_9_16 );

    CV_WRAP virtual void setThreshold(int threshold) = 0;
    CV_WRAP virtual int getThreshold() const = 0;

    CV_WRAP virtual void setNonmaxSuppression(bool f) = 0;
    CV_WRAP virtual bool getNonmaxSuppression() const = 0;

    CV_WRAP virtual void setType(FastFeatureDetector::DetectorType type) = 0;
    CV_WRAP virtual FastFeatureDetector::DetectorType getType() const = 0;
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

/** @overload */
CV_EXPORTS void FAST( InputArray image, CV_OUT std::vector<KeyPoint>& keypoints,
                      int threshold, bool nonmaxSuppression=true );

/** @brief Detects corners using the FAST algorithm

@param image grayscale image where keypoints (corners) are detected.
@param keypoints keypoints detected on the image.
@param threshold threshold on difference between intensity of the central pixel and pixels of a
circle around this pixel.
@param nonmaxSuppression if true, non-maximum suppression is applied to detected corners
(keypoints).
@param type one of the three neighborhoods as defined in the paper:
FastFeatureDetector::TYPE_9_16, FastFeatureDetector::TYPE_7_12,
FastFeatureDetector::TYPE_5_8

Detects corners using the FAST algorithm by @cite Rosten06 .

@note In Python API, types are given as cv2.FAST_FEATURE_DETECTOR_TYPE_5_8,
cv2.FAST_FEATURE_DETECTOR_TYPE_7_12 and cv2.FAST_FEATURE_DETECTOR_TYPE_9_16. For corner
detection, use cv2.FAST.detect() method.
 */
CV_EXPORTS void FAST( InputArray image, CV_OUT std::vector<KeyPoint>& keypoints,
                      int threshold, bool nonmaxSuppression, FastFeatureDetector::DetectorType type );

//! @} features2d_main

//! @addtogroup features2d_main
//! @{

/** @brief Wrapping class for feature detection using the AGAST method. :
 */
class CV_EXPORTS_W AgastFeatureDetector : public Feature2D
{
public:
    enum DetectorType
    {
        AGAST_5_8 = 0, AGAST_7_12d = 1, AGAST_7_12s = 2, OAST_9_16 = 3,
    };

    enum
    {
        THRESHOLD = 10000, NONMAX_SUPPRESSION = 10001,
    };

    CV_WRAP static Ptr<AgastFeatureDetector> create( int threshold=10,
                                                     bool nonmaxSuppression=true,
                                                     AgastFeatureDetector::DetectorType type = AgastFeatureDetector::OAST_9_16);

    CV_WRAP virtual void setThreshold(int threshold) = 0;
    CV_WRAP virtual int getThreshold() const = 0;

    CV_WRAP virtual void setNonmaxSuppression(bool f) = 0;
    CV_WRAP virtual bool getNonmaxSuppression() const = 0;

    CV_WRAP virtual void setType(AgastFeatureDetector::DetectorType type) = 0;
    CV_WRAP virtual AgastFeatureDetector::DetectorType getType() const = 0;
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

/** @overload */
CV_EXPORTS void AGAST( InputArray image, CV_OUT std::vector<KeyPoint>& keypoints,
                      int threshold, bool nonmaxSuppression=true );

/** @brief Detects corners using the AGAST algorithm

@param image grayscale image where keypoints (corners) are detected.
@param keypoints keypoints detected on the image.
@param threshold threshold on difference between intensity of the central pixel and pixels of a
circle around this pixel.
@param nonmaxSuppression if true, non-maximum suppression is applied to detected corners
(keypoints).
@param type one of the four neighborhoods as defined in the paper:
AgastFeatureDetector::AGAST_5_8, AgastFeatureDetector::AGAST_7_12d,
AgastFeatureDetector::AGAST_7_12s, AgastFeatureDetector::OAST_9_16

For non-Intel platforms, there is a tree optimised variant of AGAST with same numerical results.
The 32-bit binary tree tables were generated automatically from original code using perl script.
The perl script and examples of tree generation are placed in features2d/doc folder.
Detects corners using the AGAST algorithm by @cite mair2010_agast .

 */
CV_EXPORTS void AGAST( InputArray image, CV_OUT std::vector<KeyPoint>& keypoints,
                      int threshold, bool nonmaxSuppression, AgastFeatureDetector::DetectorType type );

/** @brief Wrapping class for feature detection using the goodFeaturesToTrack function. :
 */
class CV_EXPORTS_W GFTTDetector : public Feature2D
{
public:
    CV_WRAP static Ptr<GFTTDetector> create( int maxCorners=1000, double qualityLevel=0.01, double minDistance=1,
                                             int blockSize=3, bool useHarrisDetector=false, double k=0.04 );
    CV_WRAP static Ptr<GFTTDetector> create( int maxCorners, double qualityLevel, double minDistance,
                                             int blockSize, int gradiantSize, bool useHarrisDetector=false, double k=0.04 );
    CV_WRAP virtual void setMaxFeatures(int maxFeatures) = 0;
    CV_WRAP virtual int getMaxFeatures() const = 0;

    CV_WRAP virtual void setQualityLevel(double qlevel) = 0;
    CV_WRAP virtual double getQualityLevel() const = 0;

    CV_WRAP virtual void setMinDistance(double minDistance) = 0;
    CV_WRAP virtual double getMinDistance() const = 0;

    CV_WRAP virtual void setBlockSize(int blockSize) = 0;
    CV_WRAP virtual int getBlockSize() const = 0;

    CV_WRAP virtual void setHarrisDetector(bool val) = 0;
    CV_WRAP virtual bool getHarrisDetector() const = 0;

    CV_WRAP virtual void setK(double k) = 0;
    CV_WRAP virtual double getK() const = 0;
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

/** @brief Class for extracting blobs from an image. :

The class implements a simple algorithm for extracting blobs from an image:

1.  Convert the source image to binary images by applying thresholding with several thresholds from
    minThreshold (inclusive) to maxThreshold (exclusive) with distance thresholdStep between
    neighboring thresholds.
2.  Extract connected components from every binary image by findContours and calculate their
    centers.
3.  Group centers from several binary images by their coordinates. Close centers form one group that
    corresponds to one blob, which is controlled by the minDistBetweenBlobs parameter.
4.  From the groups, estimate final centers of blobs and their radiuses and return as locations and
    sizes of keypoints.

This class performs several filtrations of returned blobs. You should set filterBy\* to true/false
to turn on/off corresponding filtration. Available filtrations:

-   **By color**. This filter compares the intensity of a binary image at the center of a blob to
blobColor. If they differ, the blob is filtered out. Use blobColor = 0 to extract dark blobs
and blobColor = 255 to extract light blobs.
-   **By area**. Extracted blobs have an area between minArea (inclusive) and maxArea (exclusive).
-   **By circularity**. Extracted blobs have circularity
(\f$\frac{4*\pi*Area}{perimeter * perimeter}\f$) between minCircularity (inclusive) and
maxCircularity (exclusive).
-   **By ratio of the minimum inertia to maximum inertia**. Extracted blobs have this ratio
between minInertiaRatio (inclusive) and maxInertiaRatio (exclusive).
-   **By convexity**. Extracted blobs have convexity (area / area of blob convex hull) between
minConvexity (inclusive) and maxConvexity (exclusive).

Default values of parameters are tuned to extract dark circular blobs.
 */
class CV_EXPORTS_W SimpleBlobDetector : public Feature2D
{
public:
  struct CV_EXPORTS_W_SIMPLE Params
  {
      CV_WRAP Params();
      CV_PROP_RW float thresholdStep;
      CV_PROP_RW float minThreshold;
      CV_PROP_RW float maxThreshold;
      CV_PROP_RW size_t minRepeatability;
      CV_PROP_RW float minDistBetweenBlobs;

      CV_PROP_RW bool filterByColor;
      CV_PROP_RW uchar blobColor;

      CV_PROP_RW bool filterByArea;
      CV_PROP_RW float minArea, maxArea;

      CV_PROP_RW bool filterByCircularity;
      CV_PROP_RW float minCircularity, maxCircularity;

      CV_PROP_RW bool filterByInertia;
      CV_PROP_RW float minInertiaRatio, maxInertiaRatio;

      CV_PROP_RW bool filterByConvexity;
      CV_PROP_RW float minConvexity, maxConvexity;

      void read( const FileNode& fn );
      void write( FileStorage& fs ) const;
  };

  CV_WRAP static Ptr<SimpleBlobDetector>
    create(const SimpleBlobDetector::Params &parameters = SimpleBlobDetector::Params());
  CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

//! @} features2d_main

//! @addtogroup features2d_main
//! @{

/** @brief Class implementing the KAZE keypoint detector and descriptor extractor, described in @cite ABD12 .

@note AKAZE descriptor can only be used with KAZE or AKAZE keypoints .. [ABD12] KAZE Features. Pablo
F. Alcantarilla, Adrien Bartoli and Andrew J. Davison. In European Conference on Computer Vision
(ECCV), Fiorenze, Italy, October 2012.
*/
class CV_EXPORTS_W KAZE : public Feature2D
{
public:
    enum DiffusivityType
    {
        DIFF_PM_G1 = 0,
        DIFF_PM_G2 = 1,
        DIFF_WEICKERT = 2,
        DIFF_CHARBONNIER = 3
    };

    /** @brief The KAZE constructor

    @param extended Set to enable extraction of extended (128-byte) descriptor.
    @param upright Set to enable use of upright descriptors (non rotation-invariant).
    @param threshold Detector response threshold to accept point
    @param nOctaves Maximum octave evolution of the image
    @param nOctaveLayers Default number of sublevels per scale level
    @param diffusivity Diffusivity type. DIFF_PM_G1, DIFF_PM_G2, DIFF_WEICKERT or
    DIFF_CHARBONNIER
     */
    CV_WRAP static Ptr<KAZE> create(bool extended=false, bool upright=false,
                                    float threshold = 0.001f,
                                    int nOctaves = 4, int nOctaveLayers = 4,
                                    KAZE::DiffusivityType diffusivity = KAZE::DIFF_PM_G2);

    CV_WRAP virtual void setExtended(bool extended) = 0;
    CV_WRAP virtual bool getExtended() const = 0;

    CV_WRAP virtual void setUpright(bool upright) = 0;
    CV_WRAP virtual bool getUpright() const = 0;

    CV_WRAP virtual void setThreshold(double threshold) = 0;
    CV_WRAP virtual double getThreshold() const = 0;

    CV_WRAP virtual void setNOctaves(int octaves) = 0;
    CV_WRAP virtual int getNOctaves() const = 0;

    CV_WRAP virtual void setNOctaveLayers(int octaveLayers) = 0;
    CV_WRAP virtual int getNOctaveLayers() const = 0;

    CV_WRAP virtual void setDiffusivity(KAZE::DiffusivityType diff) = 0;
    CV_WRAP virtual KAZE::DiffusivityType getDiffusivity() const = 0;
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

/** @brief Class implementing the AKAZE keypoint detector and descriptor extractor, described in @cite ANB13.

@details AKAZE descriptors can only be used with KAZE or AKAZE keypoints. This class is thread-safe.

@note When you need descriptors use Feature2D::detectAndCompute, which
provides better performance. When using Feature2D::detect followed by
Feature2D::compute scale space pyramid is computed twice.

@note AKAZE implements T-API. When image is passed as UMat some parts of the algorithm
will use OpenCL.

@note [ANB13] Fast Explicit Diffusion for Accelerated Features in Nonlinear
Scale Spaces. Pablo F. Alcantarilla, Jesús Nuevo and Adrien Bartoli. In
British Machine Vision Conference (BMVC), Bristol, UK, September 2013.

*/
class CV_EXPORTS_W AKAZE : public Feature2D
{
public:
    // AKAZE descriptor type
    enum DescriptorType
    {
        DESCRIPTOR_KAZE_UPRIGHT = 2, ///< Upright descriptors, not invariant to rotation
        DESCRIPTOR_KAZE = 3,
        DESCRIPTOR_MLDB_UPRIGHT = 4, ///< Upright descriptors, not invariant to rotation
        DESCRIPTOR_MLDB = 5
    };

    /** @brief The AKAZE constructor

    @param descriptor_type Type of the extracted descriptor: DESCRIPTOR_KAZE,
    DESCRIPTOR_KAZE_UPRIGHT, DESCRIPTOR_MLDB or DESCRIPTOR_MLDB_UPRIGHT.
    @param descriptor_size Size of the descriptor in bits. 0 -\> Full size
    @param descriptor_channels Number of channels in the descriptor (1, 2, 3)
    @param threshold Detector response threshold to accept point
    @param nOctaves Maximum octave evolution of the image
    @param nOctaveLayers Default number of sublevels per scale level
    @param diffusivity Diffusivity type. DIFF_PM_G1, DIFF_PM_G2, DIFF_WEICKERT or
    DIFF_CHARBONNIER
     */
    CV_WRAP static Ptr<AKAZE> create(AKAZE::DescriptorType descriptor_type = AKAZE::DESCRIPTOR_MLDB,
                                     int descriptor_size = 0, int descriptor_channels = 3,
                                     float threshold = 0.001f, int nOctaves = 4,
                                     int nOctaveLayers = 4, KAZE::DiffusivityType diffusivity = KAZE::DIFF_PM_G2);

    CV_WRAP virtual void setDescriptorType(AKAZE::DescriptorType dtype) = 0;
    CV_WRAP virtual AKAZE::DescriptorType getDescriptorType() const = 0;

    CV_WRAP virtual void setDescriptorSize(int dsize) = 0;
    CV_WRAP virtual int getDescriptorSize() const = 0;

    CV_WRAP virtual void setDescriptorChannels(int dch) = 0;
    CV_WRAP virtual int getDescriptorChannels() const = 0;

    CV_WRAP virtual void setThreshold(double threshold) = 0;
    CV_WRAP virtual double getThreshold() const = 0;

    CV_WRAP virtual void setNOctaves(int octaves) = 0;
    CV_WRAP virtual int getNOctaves() const = 0;

    CV_WRAP virtual void setNOctaveLayers(int octaveLayers) = 0;
    CV_WRAP virtual int getNOctaveLayers() const = 0;

    CV_WRAP virtual void setDiffusivity(KAZE::DiffusivityType diff) = 0;
    CV_WRAP virtual KAZE::DiffusivityType getDiffusivity() const = 0;
    CV_WRAP virtual String getDefaultName() const CV_OVERRIDE;
};

//! @} features2d_main

/****************************************************************************************\
*                                      Distance                                          *
\****************************************************************************************/

template<typename T>
struct CV_EXPORTS Accumulator
{
    typedef T Type;
};

template<> struct Accumulator<unsigned char>  { typedef float Type; };
template<> struct Accumulator<unsigned short> { typedef float Type; };
template<> struct Accumulator<char>   { typedef float Type; };
template<> struct Accumulator<short>  { typedef float Type; };

/*
 * Squared Euclidean distance functor
 */
template<class T>
struct CV_EXPORTS SL2
{
    static const NormTypes normType = NORM_L2SQR;
    typedef T ValueType;
    typedef typename Accumulator<T>::Type ResultType;

    ResultType operator()( const T* a, const T* b, int size ) const
    {
        return normL2Sqr<ValueType, ResultType>(a, b, size);
    }
};

/*
 * Euclidean distance functor
 */
template<class T>
struct L2
{
    static const NormTypes normType = NORM_L2;
    typedef T ValueType;
    typedef typename Accumulator<T>::Type ResultType;

    ResultType operator()( const T* a, const T* b, int size ) const
    {
        return (ResultType)std::sqrt((double)normL2Sqr<ValueType, ResultType>(a, b, size));
    }
};

/*
 * Manhattan distance (city block distance) functor
 */
template<class T>
struct L1
{
    static const NormTypes normType = NORM_L1;
    typedef T ValueType;
    typedef typename Accumulator<T>::Type ResultType;

    ResultType operator()( const T* a, const T* b, int size ) const
    {
        return normL1<ValueType, ResultType>(a, b, size);
    }
};

/****************************************************************************************\
*                                  DescriptorMatcher                                     *
\****************************************************************************************/

//! @addtogroup features2d_match
//! @{

/** @brief Abstract base class for matching keypoint descriptors.

It has two groups of match methods: for matching descriptors of an image with another image or with
an image set.
 */
class CV_EXPORTS_W DescriptorMatcher : public Algorithm
{
public:
   enum MatcherType
    {
        FLANNBASED            = 1,
        BRUTEFORCE            = 2,
        BRUTEFORCE_L1         = 3,
        BRUTEFORCE_HAMMING    = 4,
        BRUTEFORCE_HAMMINGLUT = 5,
        BRUTEFORCE_SL2        = 6
    };

    virtual ~DescriptorMatcher();

    /** @brief Adds descriptors to train a CPU(trainDescCollectionis) or GPU(utrainDescCollectionis) descriptor
    collection.

    If the collection is not empty, the new descriptors are added to existing train descriptors.

    @param descriptors Descriptors to add. Each descriptors[i] is a set of descriptors from the same
    train image.
     */
    CV_WRAP virtual void add( InputArrayOfArrays descriptors );

    /** @brief Returns a constant link to the train descriptor collection trainDescCollection .
     */
    CV_WRAP const std::vector<Mat>& getTrainDescriptors() const;

    /** @brief Clears the train descriptor collections.
     */
    CV_WRAP virtual void clear() CV_OVERRIDE;

    /** @brief Returns true if there are no train descriptors in the both collections.
     */
    CV_WRAP virtual bool empty() const CV_OVERRIDE;

    /** @brief Returns true if the descriptor matcher supports masking permissible matches.
     */
    CV_WRAP virtual bool isMaskSupported() const = 0;

    /** @brief Trains a descriptor matcher

    Trains a descriptor matcher (for example, the flann index). In all methods to match, the method
    train() is run every time before matching. Some descriptor matchers (for example, BruteForceMatcher)
    have an empty implementation of this method. Other matchers really train their inner structures (for
    example, FlannBasedMatcher trains flann::Index ).
     */
    CV_WRAP virtual void train();

    /** @brief Finds the best match for each descriptor from a query set.

    @param queryDescriptors Query set of descriptors.
    @param trainDescriptors Train set of descriptors. This set is not added to the train descriptors
    collection stored in the class object.
    @param matches Matches. If a query descriptor is masked out in mask , no match is added for this
    descriptor. So, matches size may be smaller than the query descriptors count.
    @param mask Mask specifying permissible matches between an input query and train matrices of
    descriptors.

    In the first variant of this method, the train descriptors are passed as an input argument. In the
    second variant of the method, train descriptors collection that was set by DescriptorMatcher::add is
    used. Optional mask (or masks) can be passed to specify which query and training descriptors can be
    matched. Namely, queryDescriptors[i] can be matched with trainDescriptors[j] only if
    mask.at\<uchar\>(i,j) is non-zero.
     */
    CV_WRAP void match( InputArray queryDescriptors, InputArray trainDescriptors,
                CV_OUT std::vector<DMatch>& matches, InputArray mask=noArray() ) const;

    /** @brief Finds the k best matches for each descriptor from a query set.

    @param queryDescriptors Query set of descriptors.
    @param trainDescriptors Train set of descriptors. This set is not added to the train descriptors
    collection stored in the class object.
    @param mask Mask specifying permissible matches between an input query and train matrices of
    descriptors.
    @param matches Matches. Each matches[i] is k or less matches for the same query descriptor.
    @param k Count of best matches found per each query descriptor or less if a query descriptor has
    less than k possible matches in total.
    @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
    false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
    the matches vector does not contain matches for fully masked-out query descriptors.

    These extended variants of DescriptorMatcher::match methods find several best matches for each query
    descriptor. The matches are returned in the distance increasing order. See DescriptorMatcher::match
    for the details about query and train descriptors.
     */
    CV_WRAP void knnMatch( InputArray queryDescriptors, InputArray trainDescriptors,
                   CV_OUT std::vector<std::vector<DMatch> >& matches, int k,
                   InputArray mask=noArray(), bool compactResult=false ) const;

    /** @brief For each query descriptor, finds the training descriptors not farther than the specified distance.

    @param queryDescriptors Query set of descriptors.
    @param trainDescriptors Train set of descriptors. This set is not added to the train descriptors
    collection stored in the class object.
    @param matches Found matches.
    @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
    false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
    the matches vector does not contain matches for fully masked-out query descriptors.
    @param maxDistance Threshold for the distance between matched descriptors. Distance means here
    metric distance (e.g. Hamming distance), not the distance between coordinates (which is measured
    in Pixels)!
    @param mask Mask specifying permissible matches between an input query and train matrices of
    descriptors.

    For each query descriptor, the methods find such training descriptors that the distance between the
    query descriptor and the training descriptor is equal or smaller than maxDistance. Found matches are
    returned in the distance increasing order.
     */
    CV_WRAP void radiusMatch( InputArray queryDescriptors, InputArray trainDescriptors,
                      CV_OUT std::vector<std::vector<DMatch> >& matches, float maxDistance,
                      InputArray mask=noArray(), bool compactResult=false ) const;

    /** @overload
    @param queryDescriptors Query set of descriptors.
    @param matches Matches. If a query descriptor is masked out in mask , no match is added for this
    descriptor. So, matches size may be smaller than the query descriptors count.
    @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
    descriptors and stored train descriptors from the i-th image trainDescCollection[i].
    */
    CV_WRAP void match( InputArray queryDescriptors, CV_OUT std::vector<DMatch>& matches,
                        InputArrayOfArrays masks=noArray() );
    /** @overload
    @param queryDescriptors Query set of descriptors.
    @param matches Matches. Each matches[i] is k or less matches for the same query descriptor.
    @param k Count of best matches found per each query descriptor or less if a query descriptor has
    less than k possible matches in total.
    @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
    descriptors and stored train descriptors from the i-th image trainDescCollection[i].
    @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
    false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
    the matches vector does not contain matches for fully masked-out query descriptors.
    */
    CV_WRAP void knnMatch( InputArray queryDescriptors, CV_OUT std::vector<std::vector<DMatch> >& matches, int k,
                           InputArrayOfArrays masks=noArray(), bool compactResult=false );
    /** @overload
    @param queryDescriptors Query set of descriptors.
    @param matches Found matches.
    @param maxDistance Threshold for the distance between matched descriptors. Distance means here
    metric distance (e.g. Hamming distance), not the distance between coordinates (which is measured
    in Pixels)!
    @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
    descriptors and stored train descriptors from the i-th image trainDescCollection[i].
    @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
    false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
    the matches vector does not contain matches for fully masked-out query descriptors.
    */
    CV_WRAP void radiusMatch( InputArray queryDescriptors, CV_OUT std::vector<std::vector<DMatch> >& matches, float maxDistance,
                      InputArrayOfArrays masks=noArray(), bool compactResult=false );


    CV_WRAP void write( const String& fileName ) const
    {
        FileStorage fs(fileName, FileStorage::WRITE);
        write(fs);
    }

    CV_WRAP void read( const String& fileName )
    {
        FileStorage fs(fileName, FileStorage::READ);
        read(fs.root());
    }
    // Reads matcher object from a file node
    // see corresponding cv::Algorithm method
    CV_WRAP virtual void read( const FileNode& ) CV_OVERRIDE;
    // Writes matcher object to a file storage
    virtual void write( FileStorage& ) const CV_OVERRIDE;

    /** @brief Clones the matcher.

    @param emptyTrainData If emptyTrainData is false, the method creates a deep copy of the object,
    that is, copies both parameters and train data. If emptyTrainData is true, the method creates an
    object copy with the current parameters but with empty train data.
     */
    CV_WRAP virtual Ptr<DescriptorMatcher> clone( bool emptyTrainData=false ) const = 0;

    /** @brief Creates a descriptor matcher of a given type with the default parameters (using default
    constructor).

    @param descriptorMatcherType Descriptor matcher type. Now the following matcher types are
    supported:
    -   `BruteForce` (it uses L2 )
    -   `BruteForce-L1`
    -   `BruteForce-Hamming`
    -   `BruteForce-Hamming(2)`
    -   `FlannBased`
     */
    CV_WRAP static Ptr<DescriptorMatcher> create( const String& descriptorMatcherType );

    CV_WRAP static Ptr<DescriptorMatcher> create( const DescriptorMatcher::MatcherType& matcherType );


    // see corresponding cv::Algorithm method
    CV_WRAP inline void write(const Ptr<FileStorage>& fs, const String& name = String()) const { Algorithm::write(fs, name); }

protected:
    /**
     * Class to work with descriptors from several images as with one merged matrix.
     * It is used e.g. in FlannBasedMatcher.
     */
    class CV_EXPORTS DescriptorCollection
    {
    public:
        DescriptorCollection();
        DescriptorCollection( const DescriptorCollection& collection );
        virtual ~DescriptorCollection();

        // Vector of matrices "descriptors" will be merged to one matrix "mergedDescriptors" here.
        void set( const std::vector<Mat>& descriptors );
        virtual void clear();

        const Mat& getDescriptors() const;
        const Mat getDescriptor( int imgIdx, int localDescIdx ) const;
        const Mat getDescriptor( int globalDescIdx ) const;
        void getLocalIdx( int globalDescIdx, int& imgIdx, int& localDescIdx ) const;

        int size() const;

    protected:
        Mat mergedDescriptors;
        std::vector<int> startIdxs;
    };

    //! In fact the matching is implemented only by the following two methods. These methods suppose
    //! that the class object has been trained already. Public match methods call these methods
    //! after calling train().
    virtual void knnMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, int k,
        InputArrayOfArrays masks=noArray(), bool compactResult=false ) = 0;
    virtual void radiusMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, float maxDistance,
        InputArrayOfArrays masks=noArray(), bool compactResult=false ) = 0;

    static bool isPossibleMatch( InputArray mask, int queryIdx, int trainIdx );
    static bool isMaskedOut( InputArrayOfArrays masks, int queryIdx );

    static Mat clone_op( Mat m ) { return m.clone(); }
    void checkMasks( InputArrayOfArrays masks, int queryDescriptorsCount ) const;

    //! Collection of descriptors from train images.
    std::vector<Mat> trainDescCollection;
    std::vector<UMat> utrainDescCollection;
};

/** @brief Brute-force descriptor matcher.

For each descriptor in the first set, this matcher finds the closest descriptor in the second set
by trying each one. This descriptor matcher supports masking permissible matches of descriptor
sets.
 */
class CV_EXPORTS_W BFMatcher : public DescriptorMatcher
{
public:
    /** @brief Brute-force matcher constructor (obsolete). Please use BFMatcher.create()
     *
     *
    */
    CV_WRAP BFMatcher( int normType=NORM_L2, bool crossCheck=false );

    virtual ~BFMatcher() {}

    virtual bool isMaskSupported() const CV_OVERRIDE { return true; }

    /** @brief Brute-force matcher create method.
    @param normType One of NORM_L1, NORM_L2, NORM_HAMMING, NORM_HAMMING2. L1 and L2 norms are
    preferable choices for SIFT and SURF descriptors, NORM_HAMMING should be used with ORB, BRISK and
    BRIEF, NORM_HAMMING2 should be used with ORB when WTA_K==3 or 4 (see ORB::ORB constructor
    description).
    @param crossCheck If it is false, this is will be default BFMatcher behaviour when it finds the k
    nearest neighbors for each query descriptor. If crossCheck==true, then the knnMatch() method with
    k=1 will only return pairs (i,j) such that for i-th query descriptor the j-th descriptor in the
    matcher's collection is the nearest and vice versa, i.e. the BFMatcher will only return consistent
    pairs. Such technique usually produces best results with minimal number of outliers when there are
    enough matches. This is alternative to the ratio test, used by D. Lowe in SIFT paper.
     */
    CV_WRAP static Ptr<BFMatcher> create( int normType=NORM_L2, bool crossCheck=false ) ;

    virtual Ptr<DescriptorMatcher> clone( bool emptyTrainData=false ) const CV_OVERRIDE;
protected:
    virtual void knnMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, int k,
        InputArrayOfArrays masks=noArray(), bool compactResult=false ) CV_OVERRIDE;
    virtual void radiusMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, float maxDistance,
        InputArrayOfArrays masks=noArray(), bool compactResult=false ) CV_OVERRIDE;

    int normType;
    bool crossCheck;
};

#if defined(HAVE_OPENCV_FLANN) || defined(CV_DOXYGEN)

/** @brief Flann-based descriptor matcher.

This matcher trains cv::flann::Index on a train descriptor collection and calls its nearest search
methods to find the best matches. So, this matcher may be faster when matching a large train
collection than the brute force matcher. FlannBasedMatcher does not support masking permissible
matches of descriptor sets because flann::Index does not support this. :
 */
class CV_EXPORTS_W FlannBasedMatcher : public DescriptorMatcher
{
public:
    CV_WRAP FlannBasedMatcher( const Ptr<flann::IndexParams>& indexParams=makePtr<flann::KDTreeIndexParams>(),
                       const Ptr<flann::SearchParams>& searchParams=makePtr<flann::SearchParams>() );

    virtual void add( InputArrayOfArrays descriptors ) CV_OVERRIDE;
    virtual void clear() CV_OVERRIDE;

    // Reads matcher object from a file node
    virtual void read( const FileNode& ) CV_OVERRIDE;
    // Writes matcher object to a file storage
    virtual void write( FileStorage& ) const CV_OVERRIDE;

    virtual void train() CV_OVERRIDE;
    virtual bool isMaskSupported() const CV_OVERRIDE;

    CV_WRAP static Ptr<FlannBasedMatcher> create();

    virtual Ptr<DescriptorMatcher> clone( bool emptyTrainData=false ) const CV_OVERRIDE;
protected:
    static void convertToDMatches( const DescriptorCollection& descriptors,
                                   const Mat& indices, const Mat& distances,
                                   std::vector<std::vector<DMatch> >& matches );

    virtual void knnMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, int k,
        InputArrayOfArrays masks=noArray(), bool compactResult=false ) CV_OVERRIDE;
    virtual void radiusMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, float maxDistance,
        InputArrayOfArrays masks=noArray(), bool compactResult=false ) CV_OVERRIDE;

    Ptr<flann::IndexParams> indexParams;
    Ptr<flann::SearchParams> searchParams;
    Ptr<flann::Index> flannIndex;

    DescriptorCollection mergedDescriptors;
    int addedDescCount;
};

#endif

//! @} features2d_match

/****************************************************************************************\
*                                   Drawing functions                                    *
\****************************************************************************************/

//! @addtogroup features2d_draw
//! @{

enum struct DrawMatchesFlags
{
  DEFAULT = 0, //!< Output image matrix will be created (Mat::create),
               //!< i.e. existing memory of output image may be reused.
               //!< Two source image, matches and single keypoints will be drawn.
               //!< For each keypoint only the center point will be drawn (without
               //!< the circle around keypoint with keypoint size and orientation).
  DRAW_OVER_OUTIMG = 1, //!< Output image matrix will not be created (Mat::create).
                        //!< Matches will be drawn on existing content of output image.
  NOT_DRAW_SINGLE_POINTS = 2, //!< Single keypoints will not be drawn.
  DRAW_RICH_KEYPOINTS = 4 //!< For each keypoint the circle around keypoint with keypoint size and
                          //!< orientation will be drawn.
};
CV_ENUM_FLAGS(DrawMatchesFlags)

/** @brief Draws keypoints.

@param image Source image.
@param keypoints Keypoints from the source image.
@param outImage Output image. Its content depends on the flags value defining what is drawn in the
output image. See possible flags bit values below.
@param color Color of keypoints.
@param flags Flags setting drawing features. Possible flags bit values are defined by
DrawMatchesFlags. See details above in drawMatches .

@note
For Python API, flags are modified as cv2.DRAW_MATCHES_FLAGS_DEFAULT,
cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS, cv2.DRAW_MATCHES_FLAGS_DRAW_OVER_OUTIMG,
cv2.DRAW_MATCHES_FLAGS_NOT_DRAW_SINGLE_POINTS
 */
CV_EXPORTS_W void drawKeypoints( InputArray image, const std::vector<KeyPoint>& keypoints, InputOutputArray outImage,
                               const Scalar& color=Scalar::all(-1), DrawMatchesFlags flags=DrawMatchesFlags::DEFAULT );

/** @brief Draws the found matches of keypoints from two images.

@param img1 First source image.
@param keypoints1 Keypoints from the first source image.
@param img2 Second source image.
@param keypoints2 Keypoints from the second source image.
@param matches1to2 Matches from the first image to the second one, which means that keypoints1[i]
has a corresponding point in keypoints2[matches[i]] .
@param outImg Output image. Its content depends on the flags value defining what is drawn in the
output image. See possible flags bit values below.
@param matchColor Color of matches (lines and connected keypoints). If matchColor==Scalar::all(-1)
, the color is generated randomly.
@param singlePointColor Color of single keypoints (circles), which means that keypoints do not
have the matches. If singlePointColor==Scalar::all(-1) , the color is generated randomly.
@param matchesMask Mask determining which matches are drawn. If the mask is empty, all matches are
drawn.
@param flags Flags setting drawing features. Possible flags bit values are defined by
DrawMatchesFlags.

This function draws matches of keypoints from two images in the output image. Match is a line
connecting two keypoints (circles). See cv::DrawMatchesFlags.
 */
CV_EXPORTS_W void drawMatches( InputArray img1, const std::vector<KeyPoint>& keypoints1,
                             InputArray img2, const std::vector<KeyPoint>& keypoints2,
                             const std::vector<DMatch>& matches1to2, InputOutputArray outImg,
                             const Scalar& matchColor=Scalar::all(-1), const Scalar& singlePointColor=Scalar::all(-1),
                             const std::vector<char>& matchesMask=std::vector<char>(), DrawMatchesFlags flags=DrawMatchesFlags::DEFAULT );

/** @overload */
CV_EXPORTS_AS(drawMatchesKnn) void drawMatches( InputArray img1, const std::vector<KeyPoint>& keypoints1,
                             InputArray img2, const std::vector<KeyPoint>& keypoints2,
                             const std::vector<std::vector<DMatch> >& matches1to2, InputOutputArray outImg,
                             const Scalar& matchColor=Scalar::all(-1), const Scalar& singlePointColor=Scalar::all(-1),
                             const std::vector<std::vector<char> >& matchesMask=std::vector<std::vector<char> >(), DrawMatchesFlags flags=DrawMatchesFlags::DEFAULT );

//! @} features2d_draw

/****************************************************************************************\
*   Functions to evaluate the feature detectors and [generic] descriptor extractors      *
\****************************************************************************************/

CV_EXPORTS void evaluateFeatureDetector( const Mat& img1, const Mat& img2, const Mat& H1to2,
                                         std::vector<KeyPoint>* keypoints1, std::vector<KeyPoint>* keypoints2,
                                         float& repeatability, int& correspCount,
                                         const Ptr<FeatureDetector>& fdetector=Ptr<FeatureDetector>() );

CV_EXPORTS void computeRecallPrecisionCurve( const std::vector<std::vector<DMatch> >& matches1to2,
                                             const std::vector<std::vector<uchar> >& correctMatches1to2Mask,
                                             std::vector<Point2f>& recallPrecisionCurve );

CV_EXPORTS float getRecall( const std::vector<Point2f>& recallPrecisionCurve, float l_precision );
CV_EXPORTS int getNearestPoint( const std::vector<Point2f>& recallPrecisionCurve, float l_precision );

/****************************************************************************************\
*                                     Bag of visual words                                *
\****************************************************************************************/

//! @addtogroup features2d_category
//! @{

/** @brief Abstract base class for training the *bag of visual words* vocabulary from a set of descriptors.

For details, see, for example, *Visual Categorization with Bags of Keypoints* by Gabriella Csurka,
Christopher R. Dance, Lixin Fan, Jutta Willamowski, Cedric Bray, 2004. :
 */
class CV_EXPORTS_W BOWTrainer
{
public:
    BOWTrainer();
    virtual ~BOWTrainer();

    /** @brief Adds descriptors to a training set.

    @param descriptors Descriptors to add to a training set. Each row of the descriptors matrix is a
    descriptor.

    The training set is clustered using clustermethod to construct the vocabulary.
     */
    CV_WRAP void add( const Mat& descriptors );

    /** @brief Returns a training set of descriptors.
    */
    CV_WRAP const std::vector<Mat>& getDescriptors() const;

    /** @brief Returns the count of all descriptors stored in the training set.
    */
    CV_WRAP int descriptorsCount() const;

    CV_WRAP virtual void clear();

    /** @overload */
    CV_WRAP virtual Mat cluster() const = 0;

    /** @brief Clusters train descriptors.

    @param descriptors Descriptors to cluster. Each row of the descriptors matrix is a descriptor.
    Descriptors are not added to the inner train descriptor set.

    The vocabulary consists of cluster centers. So, this method returns the vocabulary. In the first
    variant of the method, train descriptors stored in the object are clustered. In the second variant,
    input descriptors are clustered.
     */
    CV_WRAP virtual Mat cluster( const Mat& descriptors ) const = 0;

protected:
    std::vector<Mat> descriptors;
    int size;
};

/** @brief kmeans -based class to train visual vocabulary using the *bag of visual words* approach. :
 */
class CV_EXPORTS_W BOWKMeansTrainer : public BOWTrainer
{
public:
    /** @brief The constructor.

    @see cv::kmeans
    */
    CV_WRAP BOWKMeansTrainer( int clusterCount, const TermCriteria& termcrit=TermCriteria(),
                      int attempts=3, int flags=KMEANS_PP_CENTERS );
    virtual ~BOWKMeansTrainer();

    // Returns trained vocabulary (i.e. cluster centers).
    CV_WRAP virtual Mat cluster() const CV_OVERRIDE;
    CV_WRAP virtual Mat cluster( const Mat& descriptors ) const CV_OVERRIDE;

protected:

    int clusterCount;
    TermCriteria termcrit;
    int attempts;
    int flags;
};

/** @brief Class to compute an image descriptor using the *bag of visual words*.

Such a computation consists of the following steps:

1.  Compute descriptors for a given image and its keypoints set.
2.  Find the nearest visual words from the vocabulary for each keypoint descriptor.
3.  Compute the bag-of-words image descriptor as is a normalized histogram of vocabulary words
encountered in the image. The i-th bin of the histogram is a frequency of i-th word of the
vocabulary in the given image.
 */
class CV_EXPORTS_W BOWImgDescriptorExtractor
{
public:
    /** @brief The constructor.

    @param dextractor Descriptor extractor that is used to compute descriptors for an input image and
    its keypoints.
    @param dmatcher Descriptor matcher that is used to find the nearest word of the trained vocabulary
    for each keypoint descriptor of the image.
     */
    CV_WRAP BOWImgDescriptorExtractor( const Ptr<DescriptorExtractor>& dextractor,
                               const Ptr<DescriptorMatcher>& dmatcher );
    /** @overload */
    BOWImgDescriptorExtractor( const Ptr<DescriptorMatcher>& dmatcher );
    virtual ~BOWImgDescriptorExtractor();

    /** @brief Sets a visual vocabulary.

    @param vocabulary Vocabulary (can be trained using the inheritor of BOWTrainer ). Each row of the
    vocabulary is a visual word (cluster center).
     */
    CV_WRAP void setVocabulary( const Mat& vocabulary );

    /** @brief Returns the set vocabulary.
    */
    CV_WRAP const Mat& getVocabulary() const;

    /** @brief Computes an image descriptor using the set visual vocabulary.

    @param image Image, for which the descriptor is computed.
    @param keypoints Keypoints detected in the input image.
    @param imgDescriptor Computed output image descriptor.
    @param pointIdxsOfClusters Indices of keypoints that belong to the cluster. This means that
    pointIdxsOfClusters[i] are keypoint indices that belong to the i -th cluster (word of vocabulary)
    returned if it is non-zero.
    @param descriptors Descriptors of the image keypoints that are returned if they are non-zero.
     */
    void compute( InputArray image, std::vector<KeyPoint>& keypoints, OutputArray imgDescriptor,
                  std::vector<std::vector<int> >* pointIdxsOfClusters=0, Mat* descriptors=0 );
    /** @overload
    @param keypointDescriptors Computed descriptors to match with vocabulary.
    @param imgDescriptor Computed output image descriptor.
    @param pointIdxsOfClusters Indices of keypoints that belong to the cluster. This means that
    pointIdxsOfClusters[i] are keypoint indices that belong to the i -th cluster (word of vocabulary)
    returned if it is non-zero.
    */
    void compute( InputArray keypointDescriptors, OutputArray imgDescriptor,
                  std::vector<std::vector<int> >* pointIdxsOfClusters=0 );
    // compute() is not constant because DescriptorMatcher::match is not constant

    CV_WRAP_AS(compute) void compute2( const Mat& image, std::vector<KeyPoint>& keypoints, CV_OUT Mat& imgDescriptor )
    { compute(image,keypoints,imgDescriptor); }

    /** @brief Returns an image descriptor size if the vocabulary is set. Otherwise, it returns 0.
    */
    CV_WRAP int descriptorSize() const;

    /** @brief Returns an image descriptor type.
     */
    CV_WRAP int descriptorType() const;

protected:
    Mat vocabulary;
    Ptr<DescriptorExtractor> dextractor;
    Ptr<DescriptorMatcher> dmatcher;
};

//! @} features2d_category

//! @} features2d



//! @addtogroup xfeatures2d_experiment
//! @{

/** @brief Class implementing the FREAK (*Fast Retina Keypoint*) keypoint descriptor, described in @cite AOV12 .

The algorithm propose a novel keypoint descriptor inspired by the human visual system and more
precisely the retina, coined Fast Retina Key- point (FREAK). A cascade of binary strings is
computed by efficiently comparing image intensities over a retinal sampling pattern. FREAKs are in
general faster to compute with lower memory load and also more robust than SIFT, SURF or BRISK.
They are competitive alternatives to existing keypoints in particular for embedded applications.

@note
   -   An example on how to use the FREAK descriptor can be found at
        opencv_source_code/samples/cpp/freak_demo.cpp
 */
class CV_EXPORTS_W FREAK : public Feature2D
{
public:

    static const int    NB_SCALES        = 64;
    static const int    NB_PAIRS         = 512;
    static const int    NB_ORIENPAIRS    = 45;

    /**
    @param orientationNormalized Enable orientation normalization.
    @param scaleNormalized Enable scale normalization.
    @param patternScale Scaling of the description pattern.
    @param nOctaves Number of octaves covered by the detected keypoints.
    @param selectedPairs (Optional) user defined selected pairs indexes,
     */
    CV_WRAP static Ptr<FREAK> create(bool orientationNormalized = true,
                             bool scaleNormalized = true,
                             float patternScale = 22.0f,
                             int nOctaves = 4,
                             const std::vector<int>& selectedPairs = std::vector<int>());
};


/** @brief The class implements the keypoint detector introduced by @cite Agrawal08, synonym of StarDetector. :
 */
class CV_EXPORTS_W StarDetector : public Feature2D
{
public:
    //! the full constructor
    CV_WRAP static Ptr<StarDetector> create(int maxSize=45, int responseThreshold=30,
                         int lineThresholdProjected=10,
                         int lineThresholdBinarized=8,
                         int suppressNonmaxSize=5);
};

/*
 * BRIEF Descriptor
 */

/** @brief Class for computing BRIEF descriptors described in @cite calon2010 .

@param bytes legth of the descriptor in bytes, valid values are: 16, 32 (default) or 64 .
@param use_orientation sample patterns using keypoints orientation, disabled by default.

 */
class CV_EXPORTS_W BriefDescriptorExtractor : public Feature2D
{
public:
    CV_WRAP static Ptr<BriefDescriptorExtractor> create( int bytes = 32, bool use_orientation = false );
};

/** @brief Class implementing the locally uniform comparison image descriptor, described in @cite LUCID

An image descriptor that can be computed very fast, while being
about as robust as, for example, SURF or BRIEF.

@note It requires a color image as input.
 */
class CV_EXPORTS_W LUCID : public Feature2D
{
public:
    /**
     * @param lucid_kernel kernel for descriptor construction, where 1=3x3, 2=5x5, 3=7x7 and so forth
     * @param blur_kernel kernel for blurring image prior to descriptor construction, where 1=3x3, 2=5x5, 3=7x7 and so forth
     */
    CV_WRAP static Ptr<LUCID> create(const int lucid_kernel = 1, const int blur_kernel = 2);
};


/*
* LATCH Descriptor
*/

/** latch Class for computing the LATCH descriptor.
If you find this code useful, please add a reference to the following paper in your work:
Gil Levi and Tal Hassner, "LATCH: Learned Arrangements of Three Patch Codes", arXiv preprint arXiv:1501.03719, 15 Jan. 2015

LATCH is a binary descriptor based on learned comparisons of triplets of image patches.

* bytes is the size of the descriptor - can be 64, 32, 16, 8, 4, 2 or 1
* rotationInvariance - whether or not the descriptor should compansate for orientation changes.
* half_ssd_size - the size of half of the mini-patches size. For example, if we would like to compare triplets of patches of size 7x7x
    then the half_ssd_size should be (7-1)/2 = 3.
* sigma - sigma value for GaussianBlur smoothing of the source image. Source image will be used without smoothing in case sigma value is 0.

Note: the descriptor can be coupled with any keypoint extractor. The only demand is that if you use set rotationInvariance = True then
    you will have to use an extractor which estimates the patch orientation (in degrees). Examples for such extractors are ORB and SIFT.

Note: a complete example can be found under /samples/cpp/tutorial_code/xfeatures2D/latch_match.cpp

*/
class CV_EXPORTS_W LATCH : public Feature2D
{
public:
    CV_WRAP static Ptr<LATCH> create(int bytes = 32, bool rotationInvariance = true, int half_ssd_size = 3, double sigma = 2.0);
};

/** @brief Class implementing DAISY descriptor, described in @cite Tola10

@param radius radius of the descriptor at the initial scale
@param q_radius amount of radial range division quantity
@param q_theta amount of angular range division quantity
@param q_hist amount of gradient orientations range division quantity
@param norm choose descriptors normalization type, where
DAISY::NRM_NONE will not do any normalization (default),
DAISY::NRM_PARTIAL mean that histograms are normalized independently for L2 norm equal to 1.0,
DAISY::NRM_FULL mean that descriptors are normalized for L2 norm equal to 1.0,
DAISY::NRM_SIFT mean that descriptors are normalized for L2 norm equal to 1.0 but no individual one is bigger than 0.154 as in SIFT
@param H optional 3x3 homography matrix used to warp the grid of daisy but sampling keypoints remains unwarped on image
@param interpolation switch to disable interpolation for speed improvement at minor quality loss
@param use_orientation sample patterns using keypoints orientation, disabled by default.

 */
class CV_EXPORTS_W DAISY : public Feature2D
{
public:
    enum NormalizationType
    {
        NRM_NONE = 100, NRM_PARTIAL = 101, NRM_FULL = 102, NRM_SIFT = 103,
    };
    CV_WRAP static Ptr<DAISY> create( float radius = 15, int q_radius = 3, int q_theta = 8,
                int q_hist = 8, DAISY::NormalizationType norm = DAISY::NRM_NONE, InputArray H = noArray(),
                bool interpolation = true, bool use_orientation = false );

    /** @overload
     * @param image image to extract descriptors
     * @param keypoints of interest within image
     * @param descriptors resulted descriptors array
     */
    virtual void compute( InputArray image, std::vector<KeyPoint>& keypoints, OutputArray descriptors ) CV_OVERRIDE = 0;

    virtual void compute( InputArrayOfArrays images,
                          std::vector<std::vector<KeyPoint> >& keypoints,
                          OutputArrayOfArrays descriptors ) CV_OVERRIDE;

    /** @overload
     * @param image image to extract descriptors
     * @param roi region of interest within image
     * @param descriptors resulted descriptors array for roi image pixels
     */
    virtual void compute( InputArray image, Rect roi, OutputArray descriptors ) = 0;

    /**@overload
     * @param image image to extract descriptors
     * @param descriptors resulted descriptors array for all image pixels
     */
    virtual void compute( InputArray image, OutputArray descriptors ) = 0;

    /**
     * @param y position y on image
     * @param x position x on image
     * @param orientation orientation on image (0->360)
     * @param descriptor supplied array for descriptor storage
     */
    virtual void GetDescriptor( double y, double x, int orientation, float* descriptor ) const = 0;

    /**
     * @param y position y on image
     * @param x position x on image
     * @param orientation orientation on image (0->360)
     * @param descriptor supplied array for descriptor storage
     * @param H homography matrix for warped grid
     */
    virtual bool GetDescriptor( double y, double x, int orientation, float* descriptor, double* H ) const = 0;

    /**
     * @param y position y on image
     * @param x position x on image
     * @param orientation orientation on image (0->360)
     * @param descriptor supplied array for descriptor storage
     */
    virtual void GetUnnormalizedDescriptor( double y, double x, int orientation, float* descriptor ) const = 0;

    /**
     * @param y position y on image
     * @param x position x on image
     * @param orientation orientation on image (0->360)
     * @param descriptor supplied array for descriptor storage
     * @param H homography matrix for warped grid
     */
    virtual bool GetUnnormalizedDescriptor( double y, double x, int orientation, float* descriptor , double *H ) const = 0;

};

/** @brief Class implementing the MSD (*Maximal Self-Dissimilarity*) keypoint detector, described in @cite Tombari14.

The algorithm implements a novel interest point detector stemming from the intuition that image patches
which are highly dissimilar over a relatively large extent of their surroundings hold the property of
being repeatable and distinctive. This concept of "contextual self-dissimilarity" reverses the key
paradigm of recent successful techniques such as the Local Self-Similarity descriptor and the Non-Local
Means filter, which build upon the presence of similar - rather than dissimilar - patches. Moreover,
it extends to contextual information the local self-dissimilarity notion embedded in established
detectors of corner-like interest points, thereby achieving enhanced repeatability, distinctiveness and
localization accuracy.

*/

class CV_EXPORTS_W MSDDetector : public Feature2D {

public:

    static Ptr<MSDDetector> create(int m_patch_radius = 3, int m_search_area_radius = 5,
            int m_nms_radius = 5, int m_nms_scale_radius = 0, float m_th_saliency = 250.0f, int m_kNN = 4,
            float m_scale_factor = 1.25f, int m_n_scales = -1, bool m_compute_orientation = false);
};

/** @brief Class implementing VGG (Oxford Visual Geometry Group) descriptor trained end to end
using "Descriptor Learning Using Convex Optimisation" (DLCO) aparatus described in @cite Simonyan14.

@param desc type of descriptor to use, VGG::VGG_120 is default (120 dimensions float)
Available types are VGG::VGG_120, VGG::VGG_80, VGG::VGG_64, VGG::VGG_48
@param isigma gaussian kernel value for image blur (default is 1.4f)
@param img_normalize use image sample intensity normalization (enabled by default)
@param use_orientation sample patterns using keypoints orientation, enabled by default
@param scale_factor adjust the sampling window of detected keypoints to 64.0f (VGG sampling window)
6.25f is default and fits for KAZE, SURF detected keypoints window ratio
6.75f should be the scale for SIFT detected keypoints window ratio
5.00f should be the scale for AKAZE, MSD, AGAST, FAST, BRISK keypoints window ratio
0.75f should be the scale for ORB keypoints ratio

@param dsc_normalize clamp descriptors to 255 and convert to uchar CV_8UC1 (disabled by default)

 */
// class CV_EXPORTS_W VGG : public Feature2D
// {
// public:

//     CV_WRAP enum
//     {
//         VGG_120 = 100, VGG_80 = 101, VGG_64 = 102, VGG_48 = 103,
//     };

//     CV_WRAP static Ptr<VGG> create( int desc = VGG::VGG_120, float isigma = 1.4f,
//                                     bool img_normalize = true, bool use_scale_orientation = true,
//                                     float scale_factor = 6.25f, bool dsc_normalize = false );

//     CV_WRAP virtual void setSigma(const float isigma) = 0;
//     CV_WRAP virtual float getSigma() const = 0;

//     CV_WRAP virtual void setUseNormalizeImage(const bool img_normalize) = 0;
//     CV_WRAP virtual bool getUseNormalizeImage() const = 0;

//     CV_WRAP virtual void setUseScaleOrientation(const bool use_scale_orientation) = 0;
//     CV_WRAP virtual bool getUseScaleOrientation() const = 0;

//     CV_WRAP virtual void setScaleFactor(const float scale_factor) = 0;
//     CV_WRAP virtual float getScaleFactor() const = 0;

//     CV_WRAP virtual void setUseNormalizeDescriptor(const bool dsc_normalize) = 0;
//     CV_WRAP virtual bool getUseNormalizeDescriptor() const = 0;
// };

/** @brief Class implementing BoostDesc (Learning Image Descriptors with Boosting), described in
@cite Trzcinski13a and @cite Trzcinski13b.

@param desc type of descriptor to use, BoostDesc::BINBOOST_256 is default (256 bit long dimension)
Available types are: BoostDesc::BGM, BoostDesc::BGM_HARD, BoostDesc::BGM_BILINEAR, BoostDesc::LBGM,
BoostDesc::BINBOOST_64, BoostDesc::BINBOOST_128, BoostDesc::BINBOOST_256
@param use_orientation sample patterns using keypoints orientation, enabled by default
@param scale_factor adjust the sampling window of detected keypoints
6.25f is default and fits for KAZE, SURF detected keypoints window ratio
6.75f should be the scale for SIFT detected keypoints window ratio
5.00f should be the scale for AKAZE, MSD, AGAST, FAST, BRISK keypoints window ratio
0.75f should be the scale for ORB keypoints ratio
1.50f was the default in original implementation

@note BGM is the base descriptor where each binary dimension is computed as the output of a single weak learner.
BGM_HARD and BGM_BILINEAR refers to same BGM but use different type of gradient binning. In the BGM_HARD that
use ASSIGN_HARD binning type the gradient is assigned to the nearest orientation bin. In the BGM_BILINEAR that use
ASSIGN_BILINEAR binning type the gradient is assigned to the two neighbouring bins. In the BGM and all other modes that use
ASSIGN_SOFT binning type the gradient is assigned to 8 nearest bins according to the cosine value between the gradient
angle and the bin center. LBGM (alias FP-Boost) is the floating point extension where each dimension is computed
as a linear combination of the weak learner responses. BINBOOST and subvariants are the binary extensions of LBGM
where each bit is computed as a thresholded linear combination of a set of weak learners.
BoostDesc header files (boostdesc_*.i) was exported from original binaries with export-boostdesc.py script from
samples subfolder.

*/

// class CV_EXPORTS_W BoostDesc : public Feature2D
// {
// public:

//     CV_WRAP enum
//     {
//        BGM = 100, BGM_HARD = 101, BGM_BILINEAR = 102, LBGM = 200,
//        BINBOOST_64 = 300, BINBOOST_128 = 301, BINBOOST_256 = 302
//     };

//     CV_WRAP static Ptr<BoostDesc> create( int desc = BoostDesc::BINBOOST_256,
//                     bool use_scale_orientation = true, float scale_factor = 6.25f );

//     CV_WRAP virtual void setUseScaleOrientation(const bool use_scale_orientation) = 0;
//     CV_WRAP virtual bool getUseScaleOrientation() const = 0;

//     CV_WRAP virtual void setScaleFactor(const float scale_factor) = 0;
//     CV_WRAP virtual float getScaleFactor() const = 0;
// };


/*
* Position-Color-Texture signatures
*/

/**
* @brief Class implementing PCT (position-color-texture) signature extraction
*       as described in @cite KrulisLS16.
*       The algorithm is divided to a feature sampler and a clusterizer.
*       Feature sampler produces samples at given set of coordinates.
*       Clusterizer then produces clusters of these samples using k-means algorithm.
*       Resulting set of clusters is the signature of the input image.
*
*       A signature is an array of SIGNATURE_DIMENSION-dimensional points.
*       Used dimensions are:
*       weight, x, y position; lab color, contrast, entropy.
* @cite KrulisLS16
* @cite BeecksUS10
*/
class CV_EXPORTS_W PCTSignatures : public Algorithm
{
public:
    /**
    * @brief Lp distance function selector.
    */
    enum DistanceFunction
    {
        L0_25, L0_5, L1, L2, L2SQUARED, L5, L_INFINITY
    };

    /**
    * @brief Point distributions supported by random point generator.
    */
    enum PointDistribution
    {
        UNIFORM,    //!< Generate numbers uniformly.
        REGULAR,    //!< Generate points in a regular grid.
        NORMAL      //!< Generate points with normal (gaussian) distribution.
    };

    /**
    * @brief Similarity function selector.
    * @see
    *       Christian Beecks, Merih Seran Uysal, Thomas Seidl.
    *       Signature quadratic form distance.
    *       In Proceedings of the ACM International Conference on Image and Video Retrieval, pages 438-445.
    *       ACM, 2010.
    * @cite BeecksUS10
    * @note For selected distance function: \f[ d(c_i, c_j) \f]  and parameter: \f[ \alpha \f]
    */
    enum SimilarityFunction
    {
        MINUS,      //!< \f[ -d(c_i, c_j) \f]
        GAUSSIAN,   //!< \f[ e^{ -\alpha * d^2(c_i, c_j)} \f]
        HEURISTIC   //!< \f[ \frac{1}{\alpha + d(c_i, c_j)} \f]
    };


    /**
    * @brief Creates PCTSignatures algorithm using sample and seed count.
    *       It generates its own sets of sampling points and clusterization seed indexes.
    * @param initSampleCount Number of points used for image sampling.
    * @param initSeedCount Number of initial clusterization seeds.
    *       Must be lower or equal to initSampleCount
    * @param pointDistribution Distribution of generated points. Default: UNIFORM.
    *       Available: UNIFORM, REGULAR, NORMAL.
    * @return Created algorithm.
    */
    CV_WRAP static Ptr<PCTSignatures> create(
        const int initSampleCount = 2000,
        const int initSeedCount = 400,
        const int pointDistribution = 0);

    /**
    * @brief Creates PCTSignatures algorithm using pre-generated sampling points
    *       and number of clusterization seeds. It uses the provided
    *       sampling points and generates its own clusterization seed indexes.
    * @param initSamplingPoints Sampling points used in image sampling.
    * @param initSeedCount Number of initial clusterization seeds.
    *       Must be lower or equal to initSamplingPoints.size().
    * @return Created algorithm.
    */
    CV_WRAP static Ptr<PCTSignatures> create(
        const std::vector<Point2f>& initSamplingPoints,
        const int initSeedCount);

    /**
    * @brief Creates PCTSignatures algorithm using pre-generated sampling points
    *       and clusterization seeds indexes.
    * @param initSamplingPoints Sampling points used in image sampling.
    * @param initClusterSeedIndexes Indexes of initial clusterization seeds.
    *       Its size must be lower or equal to initSamplingPoints.size().
    * @return Created algorithm.
    */
    CV_WRAP static Ptr<PCTSignatures> create(
        const std::vector<Point2f>& initSamplingPoints,
        const std::vector<int>& initClusterSeedIndexes);



    /**
    * @brief Computes signature of given image.
    * @param image Input image of CV_8U type.
    * @param signature Output computed signature.
    */
    CV_WRAP virtual void computeSignature(
        InputArray image,
        OutputArray signature) const = 0;

    /**
    * @brief Computes signatures for multiple images in parallel.
    * @param images Vector of input images of CV_8U type.
    * @param signatures Vector of computed signatures.
    */
    CV_WRAP virtual void computeSignatures(
        const std::vector<Mat>& images,
        std::vector<Mat>& signatures) const = 0;

    /**
    * @brief Draws signature in the source image and outputs the result.
    *       Signatures are visualized as a circle
    *       with radius based on signature weight
    *       and color based on signature color.
    *       Contrast and entropy are not visualized.
    * @param source Source image.
    * @param signature Image signature.
    * @param result Output result.
    * @param radiusToShorterSideRatio Determines maximal radius of signature in the output image.
    * @param borderThickness Border thickness of the visualized signature.
    */
    CV_WRAP static void drawSignature(
        InputArray source,
        InputArray signature,
        OutputArray result,
        float radiusToShorterSideRatio = 1.0 / 8,
        int borderThickness = 1);

    /**
    * @brief Generates initial sampling points according to selected point distribution.
    * @param initPoints Output vector where the generated points will be saved.
    * @param count Number of points to generate.
    * @param pointDistribution Point distribution selector.
    *       Available: UNIFORM, REGULAR, NORMAL.
    * @note Generated coordinates are in range [0..1)
    */
    CV_WRAP static void generateInitPoints(
        std::vector<Point2f>& initPoints,
        const int count,
        int pointDistribution);


    /**** sampler ****/

    /**
    * @brief Number of initial samples taken from the image.
    */
    CV_WRAP virtual int getSampleCount() const = 0;

    /**
    * @brief Color resolution of the greyscale bitmap represented in allocated bits
    *       (i.e., value 4 means that 16 shades of grey are used).
    *       The greyscale bitmap is used for computing contrast and entropy values.
    */
    CV_WRAP virtual int getGrayscaleBits() const = 0;
    /**
    * @brief Color resolution of the greyscale bitmap represented in allocated bits
    *       (i.e., value 4 means that 16 shades of grey are used).
    *       The greyscale bitmap is used for computing contrast and entropy values.
    */
    CV_WRAP virtual void setGrayscaleBits(int grayscaleBits) = 0;

    /**
    * @brief Size of the texture sampling window used to compute contrast and entropy
    *       (center of the window is always in the pixel selected by x,y coordinates
    *       of the corresponding feature sample).
    */
    CV_WRAP virtual int getWindowRadius() const = 0;
    /**
    * @brief Size of the texture sampling window used to compute contrast and entropy
    *       (center of the window is always in the pixel selected by x,y coordinates
    *       of the corresponding feature sample).
    */
    CV_WRAP virtual void setWindowRadius(int radius) = 0;


    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual float getWeightX() const = 0;
    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual void setWeightX(float weight) = 0;

    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual float getWeightY() const = 0;
    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual void setWeightY(float weight) = 0;

    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual float getWeightL() const = 0;
    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual void setWeightL(float weight) = 0;

    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual float getWeightA() const = 0;
    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual void setWeightA(float weight) = 0;

    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual float getWeightB() const = 0;
    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual void setWeightB(float weight) = 0;

    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual float getWeightContrast() const = 0;
    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual void setWeightContrast(float weight) = 0;

    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual float getWeightEntropy() const = 0;
    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space
    *       (x,y = position; L,a,b = color in CIE Lab space; c = contrast. e = entropy)
    */
    CV_WRAP virtual void setWeightEntropy(float weight) = 0;

    /**
    * @brief Initial samples taken from the image.
    *       These sampled features become the input for clustering.
    */
    CV_WRAP virtual std::vector<Point2f> getSamplingPoints() const = 0;



    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space.
    * @param idx ID of the weight
    * @param value Value of the weight
    * @note
    *       WEIGHT_IDX = 0;
    *       X_IDX = 1;
    *       Y_IDX = 2;
    *       L_IDX = 3;
    *       A_IDX = 4;
    *       B_IDX = 5;
    *       CONTRAST_IDX = 6;
    *       ENTROPY_IDX = 7;
    */
    CV_WRAP virtual void setWeight(int idx, float value) = 0;
    /**
    * @brief Weights (multiplicative constants) that linearly stretch individual axes of the feature space.
    * @param weights Values of all weights.
    * @note
    *       WEIGHT_IDX = 0;
    *       X_IDX = 1;
    *       Y_IDX = 2;
    *       L_IDX = 3;
    *       A_IDX = 4;
    *       B_IDX = 5;
    *       CONTRAST_IDX = 6;
    *       ENTROPY_IDX = 7;
    */
    CV_WRAP virtual void setWeights(const std::vector<float>& weights) = 0;

    /**
    * @brief Translations of the individual axes of the feature space.
    * @param idx ID of the translation
    * @param value Value of the translation
    * @note
    *       WEIGHT_IDX = 0;
    *       X_IDX = 1;
    *       Y_IDX = 2;
    *       L_IDX = 3;
    *       A_IDX = 4;
    *       B_IDX = 5;
    *       CONTRAST_IDX = 6;
    *       ENTROPY_IDX = 7;
    */
    CV_WRAP virtual void setTranslation(int idx, float value) = 0;
    /**
    * @brief Translations of the individual axes of the feature space.
    * @param translations Values of all translations.
    * @note
    *       WEIGHT_IDX = 0;
    *       X_IDX = 1;
    *       Y_IDX = 2;
    *       L_IDX = 3;
    *       A_IDX = 4;
    *       B_IDX = 5;
    *       CONTRAST_IDX = 6;
    *       ENTROPY_IDX = 7;
    */
    CV_WRAP virtual void setTranslations(const std::vector<float>& translations) = 0;

    /**
    * @brief Sets sampling points used to sample the input image.
    * @param samplingPoints Vector of sampling points in range [0..1)
    * @note Number of sampling points must be greater or equal to clusterization seed count.
    */
    CV_WRAP virtual void setSamplingPoints(std::vector<Point2f> samplingPoints) = 0;



    /**** clusterizer ****/
    /**
    * @brief Initial seeds (initial number of clusters) for the k-means algorithm.
    */
    CV_WRAP virtual std::vector<int> getInitSeedIndexes() const = 0;
    /**
    * @brief Initial seed indexes for the k-means algorithm.
    */
    CV_WRAP virtual void setInitSeedIndexes(std::vector<int> initSeedIndexes) = 0;
    /**
    * @brief Number of initial seeds (initial number of clusters) for the k-means algorithm.
    */
    CV_WRAP virtual int getInitSeedCount() const = 0;

    /**
    * @brief Number of iterations of the k-means clustering.
    *       We use fixed number of iterations, since the modified clustering is pruning clusters
    *       (not iteratively refining k clusters).
    */
    CV_WRAP virtual int getIterationCount() const = 0;
    /**
    * @brief Number of iterations of the k-means clustering.
    *       We use fixed number of iterations, since the modified clustering is pruning clusters
    *       (not iteratively refining k clusters).
    */
    CV_WRAP virtual void setIterationCount(int iterationCount) = 0;

    /**
    * @brief Maximal number of generated clusters. If the number is exceeded,
    *       the clusters are sorted by their weights and the smallest clusters are cropped.
    */
    CV_WRAP virtual int getMaxClustersCount() const = 0;
    /**
    * @brief Maximal number of generated clusters. If the number is exceeded,
    *       the clusters are sorted by their weights and the smallest clusters are cropped.
    */
    CV_WRAP virtual void setMaxClustersCount(int maxClustersCount) = 0;

    /**
    * @brief This parameter multiplied by the index of iteration gives lower limit for cluster size.
    *       Clusters containing fewer points than specified by the limit have their centroid dismissed
    *       and points are reassigned.
    */
    CV_WRAP virtual int getClusterMinSize() const = 0;
    /**
    * @brief This parameter multiplied by the index of iteration gives lower limit for cluster size.
    *       Clusters containing fewer points than specified by the limit have their centroid dismissed
    *       and points are reassigned.
    */
    CV_WRAP virtual void setClusterMinSize(int clusterMinSize) = 0;

    /**
    * @brief Threshold euclidean distance between two centroids.
    *       If two cluster centers are closer than this distance,
    *       one of the centroid is dismissed and points are reassigned.
    */
    CV_WRAP virtual float getJoiningDistance() const = 0;
    /**
    * @brief Threshold euclidean distance between two centroids.
    *       If two cluster centers are closer than this distance,
    *       one of the centroid is dismissed and points are reassigned.
    */
    CV_WRAP virtual void setJoiningDistance(float joiningDistance) = 0;

    /**
    * @brief Remove centroids in k-means whose weight is lesser or equal to given threshold.
    */
    CV_WRAP virtual float getDropThreshold() const = 0;
    /**
    * @brief Remove centroids in k-means whose weight is lesser or equal to given threshold.
    */
    CV_WRAP virtual void setDropThreshold(float dropThreshold) = 0;

    /**
    * @brief Distance function selector used for measuring distance between two points in k-means.
    */
    CV_WRAP virtual int getDistanceFunction() const = 0;
    /**
    * @brief Distance function selector used for measuring distance between two points in k-means.
    *       Available: L0_25, L0_5, L1, L2, L2SQUARED, L5, L_INFINITY.
    */
    CV_WRAP virtual void setDistanceFunction(int distanceFunction) = 0;

};

/**
* @brief Class implementing Signature Quadratic Form Distance (SQFD).
* @see Christian Beecks, Merih Seran Uysal, Thomas Seidl.
*   Signature quadratic form distance.
*   In Proceedings of the ACM International Conference on Image and Video Retrieval, pages 438-445.
*   ACM, 2010.
* @cite BeecksUS10
*/
class CV_EXPORTS_W PCTSignaturesSQFD : public Algorithm
{
public:

    /**
    * @brief Creates the algorithm instance using selected distance function,
    *       similarity function and similarity function parameter.
    * @param distanceFunction Distance function selector. Default: L2
    *       Available: L0_25, L0_5, L1, L2, L2SQUARED, L5, L_INFINITY
    * @param similarityFunction Similarity function selector. Default: HEURISTIC
    *       Available: MINUS, GAUSSIAN, HEURISTIC
    * @param similarityParameter Parameter of the similarity function.
    */
    CV_WRAP static Ptr<PCTSignaturesSQFD> create(
        const int distanceFunction = 3,
        const int similarityFunction = 2,
        const float similarityParameter = 1.0f);

    /**
    * @brief Computes Signature Quadratic Form Distance of two signatures.
    * @param _signature0 The first signature.
    * @param _signature1 The second signature.
    */
    CV_WRAP virtual float computeQuadraticFormDistance(
        InputArray _signature0,
        InputArray _signature1) const = 0;

    /**
    * @brief Computes Signature Quadratic Form Distance between the reference signature
    *       and each of the other image signatures.
    * @param sourceSignature The signature to measure distance of other signatures from.
    * @param imageSignatures Vector of signatures to measure distance from the source signature.
    * @param distances Output vector of measured distances.
    */
    CV_WRAP virtual void computeQuadraticFormDistances(
        const Mat& sourceSignature,
        const std::vector<Mat>& imageSignatures,
        std::vector<float>& distances) const = 0;

};

/**
* @brief Elliptic region around an interest point.
*/
class CV_EXPORTS Elliptic_KeyPoint : public KeyPoint
{
public:
    Size_<float> axes; //!< the lengths of the major and minor ellipse axes
    float si;  //!< the integration scale at which the parameters were estimated
    Matx23f transf; //!< the transformation between image space and local patch space
    Elliptic_KeyPoint();
    Elliptic_KeyPoint(Point2f pt, float angle, Size axes, float size, float si);
    virtual ~Elliptic_KeyPoint();
};

/**
 * @brief Class implementing the Harris-Laplace feature detector as described in @cite Mikolajczyk2004.
 */
class CV_EXPORTS_W HarrisLaplaceFeatureDetector : public Feature2D
{
public:
    /**
     * @brief Creates a new implementation instance.
     *
     * @param numOctaves the number of octaves in the scale-space pyramid
     * @param corn_thresh the threshold for the Harris cornerness measure
     * @param DOG_thresh the threshold for the Difference-of-Gaussians scale selection
     * @param maxCorners the maximum number of corners to consider
     * @param num_layers the number of intermediate scales per octave
     */
    CV_WRAP static Ptr<HarrisLaplaceFeatureDetector> create(
            int numOctaves=6,
            float corn_thresh=0.01f,
            float DOG_thresh=0.01f,
            int maxCorners=5000,
            int num_layers=4);
};

/**
 * @brief Class implementing affine adaptation for key points.
 *
 * A @ref FeatureDetector and a @ref DescriptorExtractor are wrapped to augment the
 * detected points with their affine invariant elliptic region and to compute
 * the feature descriptors on the regions after warping them into circles.
 *
 * The interface is equivalent to @ref Feature2D, adding operations for
 * @ref Elliptic_KeyPoint "Elliptic_KeyPoints" instead of @ref KeyPoint "KeyPoints".
 */
class CV_EXPORTS AffineFeature2D : public Feature2D
{
public:
    /**
     * @brief Creates an instance wrapping the given keypoint detector and
     * descriptor extractor.
     */
    static Ptr<AffineFeature2D> create(
        Ptr<FeatureDetector> keypoint_detector,
        Ptr<DescriptorExtractor> descriptor_extractor);

    /**
     * @brief Creates an instance where keypoint detector and descriptor
     * extractor are identical.
     */
    static Ptr<AffineFeature2D> create(
        Ptr<FeatureDetector> keypoint_detector)
    {
        return create(keypoint_detector, keypoint_detector);
    }

    using Feature2D::detect; // overload, don't hide
    /**
     * @brief Detects keypoints in the image using the wrapped detector and
     * performs affine adaptation to augment them with their elliptic regions.
     */
    virtual void detect(
        InputArray image,
        CV_OUT std::vector<Elliptic_KeyPoint>& keypoints,
        InputArray mask=noArray() ) = 0;

    using Feature2D::detectAndCompute; // overload, don't hide
    /**
     * @brief Detects keypoints and computes descriptors for their surrounding
     * regions, after warping them into circles.
     */
    virtual void detectAndCompute(
        InputArray image,
        InputArray mask,
        CV_OUT std::vector<Elliptic_KeyPoint>& keypoints,
        OutputArray descriptors,
        bool useProvidedKeypoints=false ) = 0;
};


/** @brief Estimates cornerness for prespecified KeyPoints using the FAST algorithm

@param image grayscale image where keypoints (corners) are detected.
@param keypoints keypoints which should be tested to fit the FAST criteria. Keypoints not beeing
detected as corners are removed.
@param threshold threshold on difference between intensity of the central pixel and pixels of a
circle around this pixel.
@param nonmaxSuppression if true, non-maximum suppression is applied to detected corners
(keypoints).
@param type one of the three neighborhoods as defined in the paper:
FastFeatureDetector::TYPE_9_16, FastFeatureDetector::TYPE_7_12,
FastFeatureDetector::TYPE_5_8

Detects corners using the FAST algorithm by @cite Rosten06 .
 */
CV_EXPORTS void FASTForPointSet( InputArray image, CV_IN_OUT std::vector<KeyPoint>& keypoints,
                      int threshold, bool nonmaxSuppression=true, cv::FastFeatureDetector::DetectorType type=FastFeatureDetector::TYPE_9_16);


//! @}


//! @addtogroup xfeatures2d_match
//! @{

/** @brief GMS  (Grid-based Motion Statistics) feature matching strategy by @cite Bian2017gms .
    @param size1 Input size of image1.
    @param size2 Input size of image2.
    @param keypoints1 Input keypoints of image1.
    @param keypoints2 Input keypoints of image2.
    @param matches1to2 Input 1-nearest neighbor matches.
    @param matchesGMS Matches returned by the GMS matching strategy.
    @param withRotation Take rotation transformation into account.
    @param withScale Take scale transformation into account.
    @param thresholdFactor The higher, the less matches.
    @note
        Since GMS works well when the number of features is large, we recommend to use the ORB feature and set FastThreshold to 0 to get as many as possible features quickly.
        If matching results are not satisfying, please add more features. (We use 10000 for images with 640 X 480).
        If your images have big rotation and scale changes, please set withRotation or withScale to true.
 */

CV_EXPORTS_W void matchGMS( const Size& size1, const Size& size2, const std::vector<KeyPoint>& keypoints1, const std::vector<KeyPoint>& keypoints2,
                          const std::vector<DMatch>& matches1to2, CV_OUT std::vector<DMatch>& matchesGMS, const bool withRotation = false,
                          const bool withScale = false, const double thresholdFactor = 6.0 );


//! @addtogroup xfeatures2d_nonfree
//! @{

/** @brief Class for extracting keypoints and computing descriptors using the Scale Invariant Feature Transform
(SIFT) algorithm by D. Lowe @cite Lowe04 .
 */
class CV_EXPORTS_W SIFT : public Feature2D
{
public:
    /**
    @param nfeatures The number of best features to retain. The features are ranked by their scores
    (measured in SIFT algorithm as the local contrast)

    @param nOctaveLayers The number of layers in each octave. 3 is the value used in D. Lowe paper. The
    number of octaves is computed automatically from the image resolution.

    @param contrastThreshold The contrast threshold used to filter out weak features in semi-uniform
    (low-contrast) regions. The larger the threshold, the less features are produced by the detector.

    @param edgeThreshold The threshold used to filter out edge-like features. Note that the its meaning
    is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
    filtered out (more features are retained).

    @param sigma The sigma of the Gaussian applied to the input image at the octave \#0. If your image
    is captured with a weak camera with soft lenses, you might want to reduce the number.
     */
    CV_WRAP static Ptr<SIFT> create( int nfeatures = 0, int nOctaveLayers = 3,
                                    double contrastThreshold = 0.04, double edgeThreshold = 10,
                                    double sigma = 1.6);
};

typedef SIFT SiftFeatureDetector;
typedef SIFT SiftDescriptorExtractor;

/** @brief Class for extracting Speeded Up Robust Features from an image @cite Bay06 .

The algorithm parameters:
-   member int extended
    -   0 means that the basic descriptors (64 elements each) shall be computed
    -   1 means that the extended descriptors (128 elements each) shall be computed
-   member int upright
    -   0 means that detector computes orientation of each feature.
    -   1 means that the orientation is not computed (which is much, much faster). For example,
if you match images from a stereo pair, or do image stitching, the matched features
likely have very similar angles, and you can speed up feature extraction by setting
upright=1.
-   member double hessianThreshold
Threshold for the keypoint detector. Only features, whose hessian is larger than
hessianThreshold are retained by the detector. Therefore, the larger the value, the less
keypoints you will get. A good default value could be from 300 to 500, depending from the
image contrast.
-   member int nOctaves
The number of a gaussian pyramid octaves that the detector uses. It is set to 4 by default.
If you want to get very large features, use the larger value. If you want just small
features, decrease it.
-   member int nOctaveLayers
The number of images within each octave of a gaussian pyramid. It is set to 2 by default.
@note
   -   An example using the SURF feature detector can be found at
        opencv_source_code/samples/cpp/generic_descriptor_match.cpp
    -   Another example using the SURF feature detector, extractor and matcher can be found at
        opencv_source_code/samples/cpp/matcher_simple.cpp
 */
class CV_EXPORTS_W SURF : public Feature2D
{
public:
    /**
    @param hessianThreshold Threshold for hessian keypoint detector used in SURF.
    @param nOctaves Number of pyramid octaves the keypoint detector will use.
    @param nOctaveLayers Number of octave layers within each octave.
    @param extended Extended descriptor flag (true - use extended 128-element descriptors; false - use
    64-element descriptors).
    @param upright Up-right or rotated features flag (true - do not compute orientation of features;
    false - compute orientation).
     */
    CV_WRAP static Ptr<SURF> create(double hessianThreshold=100,
                  int nOctaves = 4, int nOctaveLayers = 3,
                  bool extended = false, bool upright = false);

    CV_WRAP virtual void setHessianThreshold(double hessianThreshold) = 0;
    CV_WRAP virtual double getHessianThreshold() const = 0;

    CV_WRAP virtual void setNOctaves(int nOctaves) = 0;
    CV_WRAP virtual int getNOctaves() const = 0;

    CV_WRAP virtual void setNOctaveLayers(int nOctaveLayers) = 0;
    CV_WRAP virtual int getNOctaveLayers() const = 0;

    CV_WRAP virtual void setExtended(bool extended) = 0;
    CV_WRAP virtual bool getExtended() const = 0;

    CV_WRAP virtual void setUpright(bool upright) = 0;
    CV_WRAP virtual bool getUpright() const = 0;
};

typedef SURF SurfFeatureDetector;
typedef SURF SurfDescriptorExtractor;

} /* namespace cv */

#endif
