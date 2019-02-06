// /*M///////////////////////////////////////////////////////////////////////////////////////
// //
// //  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
// //
// //  By downloading, copying, installing or using the software you agree to this license.
// //  If you do not agree to this license, do not download, install,
// //  copy or use the software.
// //
// //
// //                          License Agreement
// //                For Open Source Computer Vision Library
// //
// // Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// // Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// // Third party copyrights are property of their respective owners.
// //
// // Redistribution and use in source and binary forms, with or without modification,
// // are permitted provided that the following conditions are met:
// //
// //   * Redistribution's of source code must retain the above copyright notice,
// //     this list of conditions and the following disclaimer.
// //
// //   * Redistribution's in binary form must reproduce the above copyright notice,
// //     this list of conditions and the following disclaimer in the documentation
// //     and/or other materials provided with the distribution.
// //
// //   * The name of the copyright holders may not be used to endorse or promote products
// //     derived from this software without specific prior written permission.
// //
// // This software is provided by the copyright holders and contributors "as is" and
// // any express or implied warranties, including, but not limited to, the implied
// // warranties of merchantability and fitness for a particular purpose are disclaimed.
// // In no event shall the Intel Corporation or contributors be liable for any direct,
// // indirect, incidental, special, exemplary, or consequential damages
// // (including, but not limited to, procurement of substitute goods or services;
// // loss of use, data, or profits; or business interruption) however caused
// // and on any theory of liability, whether in contract, strict liability,
// // or tort (including negligence or otherwise) arising in any way out of
// // the use of this software, even if advised of the possibility of such damage.
// //
// //M*/

// #include "../../precomp.hpp"
// #include "blendersT.hpp"
// #include "../util/util.hpp"


// namespace cv {
// namespace detail {

// static const float WEIGHT_EPS = 1e-5f;

// template <typename T>
// Ptr<BlenderT<T>> BlenderT<T>::createDefault(int type, bool try_gpu)
// {
//     if (type == NO)
//         return makePtr<BlenderT<T>>();
//     if (type == NO8)
//         return makePtr<Blender8T<T>>();
//     if (type == FEATHER)
//         return makePtr<FeatherBlenderT<T>>();
//     if (type == MULTI_BAND)
//         return makePtr<MultiBandBlenderT<T>>(try_gpu);
//     CV_Error(Error::StsBadArg, "unsupported blending method");
// }

// template <typename T>
// void BlenderT<T>::prepare(const std::vector<Point> &corners, const std::vector<Size> &sizes)
// {
//     prepare(resultRoi(corners, sizes));
// }

// template <typename T>
// void BlenderT<T>::prepare(Rect dst_roi)
// {
//     static_assert(sizeof(T) == 8 || sizeof(T) == 16, "BlenderT supports only 8 and 16 bit types");

//     if(sizeof(T) == 8) {
//         dstValueType_ = CV_8UC3;
//     }
//     else {
//         dstValueType_ = CV_16SC3;
//     }

//     dst_.create(dst_roi.size(), dstValueType_);
//     dst_.setTo(Scalar::all(0));
//     dst_mask_.create(dst_roi.size(), CV_8U);
//     dst_mask_.setTo(Scalar::all(0));
//     dst_roi_ = dst_roi;
// }

// template <typename T>
// void BlenderT<T>::feed(InputArray _img, InputArray _mask, Point tl)
// {
//     Mat img = _img.getMat();
//     Mat mask = _mask.getMat();
//     Mat dst = dst_.getMat(ACCESS_RW);
//     Mat dst_mask = dst_mask_.getMat(ACCESS_RW);

//     CV_Assert(img.type() == dstValueType_);
//     // CV_Assert(img.type() == CV_16SC3);
//     CV_Assert(mask.type() == CV_8U);
//     int dx = tl.x - BlenderT<T>::dst_roi_.x;
//     int dy = tl.y - BlenderT<T>::dst_roi_.y;

//     for (int y = 0; y < img.rows; ++y)
//     {
//         const Point3_<T> *src_row = img.ptr<Point3_<T> >(y);
//         Point3_<T> *dst_row = dst.ptr<Point3_<T> >(dy + y);
//         const uchar *mask_row = mask.ptr<uchar>(y);
//         uchar *dst_mask_row = dst_mask.ptr<uchar>(dy + y);

//         for (int x = 0; x < img.cols; ++x)
//         {
//             if (mask_row[x])
//                 dst_row[dx + x] = src_row[x];
//             dst_mask_row[dx + x] |= mask_row[x];
//         }
//     }
// }

// template <typename T>
// void BlenderT<T>::blend(InputOutputArray dst, InputOutputArray dst_mask)
// {
//     UMat mask;
//     compare(dst_mask_, 0, mask, CMP_EQ);
//     dst_.setTo(Scalar::all(0), mask);
//     dst.assign(dst_);
//     dst_mask.assign(dst_mask_);
//     dst_.release();
//     dst_mask_.release();
// }

// // template <typename T>
// // void Blender8T<T>::prepare(Rect dst_roi)
// // {
// //     BlenderT<T>::dst_.create(dst_roi.size(), CV_8UC3);
// //     BlenderT<T>::dst_.setTo(Scalar::all(0));
// //     BlenderT<T>::dst_mask_.create(dst_roi.size(), CV_8U);
// //     BlenderT<T>::dst_mask_.setTo(Scalar::all(0));
// //     BlenderT<T>::dst_roi_ = dst_roi;
// // }

// // template <typename T>
// // void Blender8T<T>::feed(InputArray _img, InputArray _mask, Point tl)
// // {
// //     Mat img = _img.getMat();
// //     Mat mask = _mask.getMat();
// //     Mat dst = BlenderT<T>::dst_.getMat(ACCESS_RW);
// //     Mat dst_mask = BlenderT<T>::dst_mask_.getMat(ACCESS_RW);

// //     CV_Assert(img.type() == CV_8UC3);
// //     CV_Assert(mask.type() == CV_8U);
// //     int dx = tl.x - BlenderT<T>::dst_roi_.x;
// //     int dy = tl.y - BlenderT<T>::dst_roi_.y;

// //     for (int y = 0; y < img.rows; ++y)
// //     {
// //         const Point3_<uint8_t> *src_row = img.ptr<Point3_<uint8_t> >(y);
// //         Point3_<uint8_t> *dst_row = dst.ptr<Point3_<uint8_t> >(dy + y);
// //         const uchar *mask_row = mask.ptr<uchar>(y);
// //         uchar *dst_mask_row = dst_mask.ptr<uchar>(dy + y);

// //         for (int x = 0; x < img.cols; ++x)
// //         {
// //             if (mask_row[x])
// //                 dst_row[dx + x] = src_row[x];
// //             dst_mask_row[dx + x] |= mask_row[x];
// //         }
// //     }
// // }

// // template <typename T>
// // void Blender8T<T>::blend(InputOutputArray dst, InputOutputArray dst_mask)
// // {
// //     UMat mask;
// //     compare(BlenderT<T>::dst_mask_, 0, mask, CMP_EQ);
// //     BlenderT<T>::dst_.setTo(Scalar::all(0), mask);
// //     dst.assign(BlenderT<T>::dst_);
// //     dst_mask.assign(BlenderT<T>::dst_mask_);
// //     BlenderT<T>::dst_.release();
// //     BlenderT<T>::dst_mask_.release();
// // }



// template <typename T>
// void FeatherBlenderT<T>::prepare(Rect dst_roi)
// {
//     BlenderT<T>::prepare(dst_roi);
//     dst_weight_map_.create(dst_roi.size(), CV_32F);
//     dst_weight_map_.setTo(0);
// }

// template <typename T>
// void FeatherBlenderT<T>::feed(InputArray _img, InputArray mask, Point tl)
// {
//     Mat img = _img.getMat();
//     Mat dst = BlenderT<T>::dst_.getMat(ACCESS_RW);

//     CV_Assert(img.type() == BlenderT<T>::dstValueType_);
//     // CV_Assert(img.type() == CV_16SC3);
//     CV_Assert(mask.type() == CV_8U);

//     createWeightMapT(mask, sharpness_, weight_map_);
//     Mat weight_map = weight_map_.getMat(ACCESS_READ);
//     Mat dst_weight_map = dst_weight_map_.getMat(ACCESS_RW);

//     int dx = tl.x - BlenderT<T>::dst_roi_.x;
//     int dy = tl.y - BlenderT<T>::dst_roi_.y;

//     for (int y = 0; y < img.rows; ++y)
//     {
//         const Point3_<T>* src_row = img.ptr<Point3_<T> >(y);
//         Point3_<T>* dst_row = dst.ptr<Point3_<T> >(dy + y);
//         const float* weight_row = weight_map.ptr<float>(y);
//         float* dst_weight_row = dst_weight_map.ptr<float>(dy + y);

//         for (int x = 0; x < img.cols; ++x)
//         {
//             dst_row[dx + x].x += static_cast<T>(src_row[x].x * weight_row[x]);
//             dst_row[dx + x].y += static_cast<T>(src_row[x].y * weight_row[x]);
//             dst_row[dx + x].z += static_cast<T>(src_row[x].z * weight_row[x]);
//             dst_weight_row[dx + x] += weight_row[x];
//         }
//     }
// }

// template <typename T>
// void FeatherBlenderT<T>::blend(InputOutputArray dst, InputOutputArray dst_mask)
// {
//     normalizeUsingWeightMapT<T>(dst_weight_map_, BlenderT<T>::dst_);
//     compare(dst_weight_map_, WEIGHT_EPS, BlenderT<T>::dst_mask_, CMP_GT);
//     BlenderT<T>::blend(dst, dst_mask);
// }

// template <typename T>
// Rect FeatherBlenderT<T>::createWeightMaps(const std::vector<UMat> &masks, const std::vector<Point> &corners,
//                                       std::vector<UMat> &weight_maps)
// {
//     weight_maps.resize(masks.size());
//     for (size_t i = 0; i < masks.size(); ++i)
//         createWeightMapT(masks[i], sharpness_, weight_maps[i]);

//     Rect dst_roi = resultRoi(corners, masks);
//     Mat weights_sum(dst_roi.size(), CV_32F);
//     weights_sum.setTo(0);

//     for (size_t i = 0; i < weight_maps.size(); ++i)
//     {
//         Rect roi(corners[i].x - dst_roi.x, corners[i].y - dst_roi.y,
//                  weight_maps[i].cols, weight_maps[i].rows);
//         add(weights_sum(roi), weight_maps[i], weights_sum(roi));
//     }

//     for (size_t i = 0; i < weight_maps.size(); ++i)
//     {
//         Rect roi(corners[i].x - dst_roi.x, corners[i].y - dst_roi.y,
//                  weight_maps[i].cols, weight_maps[i].rows);
//         Mat tmp = weights_sum(roi);
//         tmp.setTo(1, tmp < std::numeric_limits<float>::epsilon());
//         divide(weight_maps[i], tmp, weight_maps[i]);
//     }

//     return dst_roi;
// }

// template <typename T>
// MultiBandBlenderT<T>::MultiBandBlenderT(int try_gpu, int num_bands, int weight_type)
// {
//     num_bands_ = 0;
//     setNumBands(num_bands);

//     CV_UNUSED(try_gpu);
//     can_use_gpu_ = false;

//     CV_Assert(weight_type == CV_32F || weight_type == CV_16S);
//     weight_type_ = weight_type;
// }

// template <typename T>
// void MultiBandBlenderT<T>::prepare(Rect dst_roi)
// {
//     dst_roi_final_ = dst_roi;

//     // Crop unnecessary bands
//     double max_len = static_cast<double>(std::max(dst_roi.width, dst_roi.height));
//     num_bands_ = std::min(actual_num_bands_, static_cast<int>(ceil(std::log(max_len) / std::log(2.0))));

//     // Add border to the final image, to ensure sizes are divided by (1 << num_bands_)
//     dst_roi.width += ((1 << num_bands_) - dst_roi.width % (1 << num_bands_)) % (1 << num_bands_);
//     dst_roi.height += ((1 << num_bands_) - dst_roi.height % (1 << num_bands_)) % (1 << num_bands_);

//     BlenderT<T>::prepare(dst_roi);

//     {
//         dst_pyr_laplace_.resize(num_bands_ + 1);
//         dst_pyr_laplace_[0] = BlenderT<T>::dst_;

//         dst_band_weights_.resize(num_bands_ + 1);
//         dst_band_weights_[0].create(dst_roi.size(), weight_type_);
//         dst_band_weights_[0].setTo(0);

//         for (int i = 1; i <= num_bands_; ++i)
//         {
//             dst_pyr_laplace_[i].create((dst_pyr_laplace_[i - 1].rows + 1) / 2,
//                 (dst_pyr_laplace_[i - 1].cols + 1) / 2, CV_16SC3);
//             dst_band_weights_[i].create((dst_band_weights_[i - 1].rows + 1) / 2,
//                 (dst_band_weights_[i - 1].cols + 1) / 2, weight_type_);
//             dst_pyr_laplace_[i].setTo(Scalar::all(0));
//             dst_band_weights_[i].setTo(0);
//         }
//     }
// }

// template <typename T>
// void MultiBandBlenderT<T>::feed(InputArray _img, InputArray mask, Point tl)
// {
// // #if ENABLE_LOG
// //     int64 t = getTickCount();
// // #endif

//     UMat img;

//     {
//         img = _img.getUMat();
//     }

//     CV_Assert(img.type() == CV_16SC3 || img.type() == CV_8UC3);
//     CV_Assert(mask.type() == CV_8U);

//     // Keep source image in memory with small border
//     int gap = 3 * (1 << num_bands_);
//     Point tl_new(std::max(BlenderT<T>::dst_roi_.x, tl.x - gap),
//                  std::max(BlenderT<T>::dst_roi_.y, tl.y - gap));
//     Point br_new(std::min(BlenderT<T>::dst_roi_.br().x, tl.x + img.cols + gap),
//                  std::min(BlenderT<T>::dst_roi_.br().y, tl.y + img.rows + gap));

//     // Ensure coordinates of top-left, bottom-right corners are divided by (1 << num_bands_).
//     // After that scale between layers is exactly 2.
//     //
//     // We do it to avoid interpolation problems when keeping sub-images only. There is no such problem when
//     // image is bordered to have size equal to the final image size, but this is too memory hungry approach.
//     tl_new.x = BlenderT<T>::dst_roi_.x + (((tl_new.x - BlenderT<T>::dst_roi_.x) >> num_bands_) << num_bands_);
//     tl_new.y = BlenderT<T>::dst_roi_.y + (((tl_new.y - BlenderT<T>::dst_roi_.y) >> num_bands_) << num_bands_);
//     int width = br_new.x - tl_new.x;
//     int height = br_new.y - tl_new.y;
//     width += ((1 << num_bands_) - width % (1 << num_bands_)) % (1 << num_bands_);
//     height += ((1 << num_bands_) - height % (1 << num_bands_)) % (1 << num_bands_);
//     br_new.x = tl_new.x + width;
//     br_new.y = tl_new.y + height;
//     int dy = std::max(br_new.y - BlenderT<T>::dst_roi_.br().y, 0);
//     int dx = std::max(br_new.x - BlenderT<T>::dst_roi_.br().x, 0);
//     tl_new.x -= dx; br_new.x -= dx;
//     tl_new.y -= dy; br_new.y -= dy;

//     int top = tl.y - tl_new.y;
//     int left = tl.x - tl_new.x;
//     int bottom = br_new.y - tl.y - img.rows;
//     int right = br_new.x - tl.x - img.cols;

//     // Create the source image Laplacian pyramid
//     UMat img_with_border;
//     copyMakeBorder(_img, img_with_border, top, bottom, left, right,
//                    BORDER_REFLECT);
   
//     std::vector<UMat> src_pyr_laplace;
//     createLaplacePyrT(img_with_border, num_bands_, src_pyr_laplace);

//     // Create the weight map Gaussian pyramid
//     UMat weight_map;
//     std::vector<UMat> weight_pyr_gauss(num_bands_ + 1);

//     if (weight_type_ == CV_32F)
//     {
//         mask.getUMat().convertTo(weight_map, CV_32F, 1./255.);
//     }
//     else // weight_type_ == CV_16S
//     {
//         mask.getUMat().convertTo(weight_map, CV_16S);
//         UMat add_mask;
//         compare(mask, 0, add_mask, CMP_NE);
//         add(weight_map, Scalar::all(1), weight_map, add_mask);
//     }

//     copyMakeBorder(weight_map, weight_pyr_gauss[0], top, bottom, left, right, BORDER_CONSTANT);

//     for (int i = 0; i < num_bands_; ++i)
//         pyrDown(weight_pyr_gauss[i], weight_pyr_gauss[i + 1]);

//     int y_tl = tl_new.y - BlenderT<T>::dst_roi_.y;
//     int y_br = br_new.y - BlenderT<T>::dst_roi_.y;
//     int x_tl = tl_new.x - BlenderT<T>::dst_roi_.x;
//     int x_br = br_new.x - BlenderT<T>::dst_roi_.x;

//     // Add weighted layer of the source image to the final Laplacian pyramid layer
//     for (int i = 0; i <= num_bands_; ++i)
//     {
//         Rect rc(x_tl, y_tl, x_br - x_tl, y_br - y_tl);

//         {
//             Mat _src_pyr_laplace = src_pyr_laplace[i].getMat(ACCESS_READ);
//             Mat _dst_pyr_laplace = dst_pyr_laplace_[i](rc).getMat(ACCESS_RW);
//             Mat _weight_pyr_gauss = weight_pyr_gauss[i].getMat(ACCESS_READ);
//             Mat _dst_band_weights = dst_band_weights_[i](rc).getMat(ACCESS_RW);
//             if (weight_type_ == CV_32F)
//             {
//                 for (int y = 0; y < rc.height; ++y)
//                 {
//                     const Point3_<short>* src_row = _src_pyr_laplace.ptr<Point3_<short> >(y);
//                     Point3_<short>* dst_row = _dst_pyr_laplace.ptr<Point3_<short> >(y);
//                     const float* weight_row = _weight_pyr_gauss.ptr<float>(y);
//                     float* dst_weight_row = _dst_band_weights.ptr<float>(y);

//                     for (int x = 0; x < rc.width; ++x)
//                     {
//                         dst_row[x].x += static_cast<short>(src_row[x].x * weight_row[x]);
//                         dst_row[x].y += static_cast<short>(src_row[x].y * weight_row[x]);
//                         dst_row[x].z += static_cast<short>(src_row[x].z * weight_row[x]);
//                         dst_weight_row[x] += weight_row[x];
//                     }
//                 }
//             }
//             else // weight_type_ == CV_16S
//             {
//                 for (int y = 0; y < y_br - y_tl; ++y)
//                 {
//                     const Point3_<short>* src_row = _src_pyr_laplace.ptr<Point3_<short> >(y);
//                     Point3_<short>* dst_row = _dst_pyr_laplace.ptr<Point3_<short> >(y);
//                     const short* weight_row = _weight_pyr_gauss.ptr<short>(y);
//                     short* dst_weight_row = _dst_band_weights.ptr<short>(y);

//                     for (int x = 0; x < x_br - x_tl; ++x)
//                     {
//                         dst_row[x].x += short((src_row[x].x * weight_row[x]) >> 8);
//                         dst_row[x].y += short((src_row[x].y * weight_row[x]) >> 8);
//                         dst_row[x].z += short((src_row[x].z * weight_row[x]) >> 8);
//                         dst_weight_row[x] += weight_row[x];
//                     }
//                 }
//             }
//         }

//         x_tl /= 2; y_tl /= 2;
//         x_br /= 2; y_br /= 2;
//     }
// }

// template <typename T>
// void MultiBandBlenderT<T>::blend(InputOutputArray dst, InputOutputArray dst_mask)
// {
//     Rect dst_rc(0, 0, dst_roi_final_.width, dst_roi_final_.height);
//     {
//         cv::UMat dst_band_weights_0;

//         for (int i = 0; i <= num_bands_; ++i)
//             normalizeUsingWeightMapT<T>(dst_band_weights_[i], dst_pyr_laplace_[i]);

//         restoreImageFromLaplacePyrT(dst_pyr_laplace_);

//         BlenderT<T>::dst_ = dst_pyr_laplace_[0](dst_rc);
//         dst_band_weights_0 = dst_band_weights_[0];

//         dst_pyr_laplace_.clear();
//         dst_band_weights_.clear();

//         compare(dst_band_weights_0(dst_rc), WEIGHT_EPS, BlenderT<T>::dst_mask_, CMP_GT);

//         BlenderT<T>::blend(dst, dst_mask);
//     }
// }


// //////////////////////////////////////////////////////////////////////////////
// // Auxiliary functions

// template <typename T>
// void normalizeUsingWeightMapT(InputArray _weight, InputOutputArray _src)
// {
//     Mat src;
//     Mat weight;

//     {
//         src = _src.getMat();
//         weight = _weight.getMat();

//         CV_Assert(src.type() == CV_16SC3);

//         if (weight.type() == CV_32FC1)
//         {
//             for (int y = 0; y < src.rows; ++y)
//             {
//                 Point3_<short> *row = src.ptr<Point3_<short> >(y);
//                 const float *weight_row = weight.ptr<float>(y);

//                 for (int x = 0; x < src.cols; ++x)
//                 {
//                     row[x].x = static_cast<short>(row[x].x / (weight_row[x] + WEIGHT_EPS));
//                     row[x].y = static_cast<short>(row[x].y / (weight_row[x] + WEIGHT_EPS));
//                     row[x].z = static_cast<short>(row[x].z / (weight_row[x] + WEIGHT_EPS));
//                 }
//             }
//         }
//         else
//         {
//             CV_Assert(weight.type() == CV_16SC1);

//             for (int y = 0; y < src.rows; ++y)
//             {
//                 const short *weight_row = weight.ptr<short>(y);
//                 Point3_<short> *row = src.ptr<Point3_<short> >(y);

//                 for (int x = 0; x < src.cols; ++x)
//                 {
//                     int w = weight_row[x] + 1;
//                     row[x].x = static_cast<short>((row[x].x << 8) / w);
//                     row[x].y = static_cast<short>((row[x].y << 8) / w);
//                     row[x].z = static_cast<short>((row[x].z << 8) / w);
//                 }
//             }
//         }
//     }
// }


// void createWeightMapT(InputArray mask, float sharpness, InputOutputArray weight)
// {
//     CV_Assert(mask.type() == CV_8U);
//     distanceTransform(mask, weight, DIST_L1, 3);
//     UMat tmp;
//     multiply(weight, sharpness, tmp);
//     threshold(tmp, weight, 1.f, 1.f, THRESH_TRUNC);
// }


// void createLaplacePyrT(InputArray img, int num_levels, std::vector<UMat> &pyr)
// {
//     pyr.resize(num_levels + 1);

//     if(img.depth() == CV_8U)
//     {
//         if(num_levels == 0)
//         {
//             img.getUMat().convertTo(pyr[0], CV_16S);
//             return;
//         }

//         UMat downNext;
//         UMat current = img.getUMat();
//         pyrDown(img, downNext);

//         for(int i = 1; i < num_levels; ++i)
//         {
//             UMat lvl_up;
//             UMat lvl_down;

//             pyrDown(downNext, lvl_down);
//             pyrUp(downNext, lvl_up, current.size());
//             subtract(current, lvl_up, pyr[i-1], noArray(), CV_16S);

//             current = downNext;
//             downNext = lvl_down;
//         }

//         {
//             UMat lvl_up;
//             pyrUp(downNext, lvl_up, current.size());
//             subtract(current, lvl_up, pyr[num_levels-1], noArray(), CV_16S);

//             downNext.convertTo(pyr[num_levels], CV_16S);
//         }
//     }
//     else
//     {
//         pyr[0] = img.getUMat();
//         for (int i = 0; i < num_levels; ++i)
//             pyrDown(pyr[i], pyr[i + 1]);
//         UMat tmp;
//         for (int i = 0; i < num_levels; ++i)
//         {
//             pyrUp(pyr[i + 1], tmp, pyr[i].size());
//             subtract(pyr[i], tmp, pyr[i]);
//         }
//     }
// }


// void restoreImageFromLaplacePyrT(std::vector<UMat> &pyr)
// {
//     if (pyr.empty())
//         return;
//     UMat tmp;
//     for (size_t i = pyr.size() - 1; i > 0; --i)
//     {
//         pyrUp(pyr[i], tmp, pyr[i - 1].size());
//         add(tmp, pyr[i - 1], pyr[i - 1]);
//     }
// }


// } // namespace detail
// } // namespace cv
