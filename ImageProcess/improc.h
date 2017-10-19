#ifndef __IMAGE_PROCESSING_H__
#define __IMAGE_PROCESSING_H__

#include "types.h"

#define _IMAGEPROCESSING_PRINT_		(1)


/****************************************************************************************\
*									Image binarize								         *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuImageIntegrogram(RyuImage * im, RyuImage * out);

IMAGEPROCESSING_API int ryuCentralLocalBinarize(RyuImage * im, 
												RyuImage * integrogram, 
												RyuImage * binary,
												RyuSize mask_sz, 
												double alpha RYU_DEFAULT(0.15), 
												int reverse RYU_DEFAULT(0));

IMAGEPROCESSING_API int ryuCentralLocalBinarizeFast(RyuImage * im, 
													RyuImage * integrogram, 
													RyuImage * binary,
													RyuSize mask_sz, 
													double alpha RYU_DEFAULT(0.15), 
													int reverse RYU_DEFAULT(0));


/****************************************************************************************\
*								Perspective transform								     *
\****************************************************************************************/
IMAGEPROCESSING_API int getPerspectiveTransformMat(const int * src, 
												   const int * dst, 
												   double * mat);

IMAGEPROCESSING_API int warpPerspectiveTransformFixed(RyuImage * src, 
													  RyuImage * dst, 
													  double * matrix);


/****************************************************************************************\
*										Color										     *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuCopyRoi2Image(RyuImage * src, RyuRect roi, RyuImage * dst);


/****************************************************************************************\
*								Gaussian blur										     *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuGaussianBlur3x3(RyuImage * src, RyuImage * dst);

IMAGEPROCESSING_API int ryuGaussianBlur5x5(RyuImage * src, RyuImage * dst);

IMAGEPROCESSING_API int ryuGaussianBlur5x5_Fast(RyuImage * src, RyuImage * dst);


/****************************************************************************************\
*									Sharpening										     *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuUsmSharpening(RyuImage * src, RyuImage * dst, 
										 RyuImage * buff RYU_DEFAULT(0), 
										 int amount RYU_DEFAULT(160), 
										 int thresh RYU_DEFAULT(0), 
										 int radius RYU_DEFAULT(1));


/****************************************************************************************\
*									UnionFind										     *
\****************************************************************************************/
void ryuUnionFindReset(int * arr, int arr_size);

int ryuUnionFind(int * arr, int arr_size, int idx);

int ryuUnionFindEx(int * arr, int arr_size, int idx);


/****************************************************************************************\
*									Data extract									     *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuExtractImdataLine_8UC1(RyuImage * im, 
												  RyuPoint pt0, RyuPoint pt1, 
												  int * data);


/****************************************************************************************\
*									Image Contrast									     *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuImageContrastAnalyze(unsigned char * img, int width, int height, int widthstep, 
							int * hist, float low_ratio RYU_DEFAULT(0.005), int * low_scale RYU_DEFAULT(0), 
							float high_ratio RYU_DEFAULT(0.005), int * high_scale RYU_DEFAULT(0), 
							int * min_scale RYU_DEFAULT(0), int * max_scale RYU_DEFAULT(0), 
							int * avg_scale RYU_DEFAULT(0), int * mid_scale RYU_DEFAULT(0), int * grav_scale RYU_DEFAULT(0));

IMAGEPROCESSING_API int ryuAutoContrast(RyuImage * in, RyuImage * out, float thre_ratio RYU_DEFAULT(0.005));


/****************************************************************************************\
*										Morpholopy									     *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuDilate(RyuImage * src, RyuImage * dst);

IMAGEPROCESSING_API int ryuErode(RyuImage * src, RyuImage * dst);


/****************************************************************************************\
*										Flood Fill									     *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuFloodFill(RyuImage * im_in, RyuPoint seed, int gl_thre, int lo_diff, int up_diff, 
				 RyuImage * im_lable, int label_val, int conn_flag, RyuImage * seeds);

IMAGEPROCESSING_API int ryuFloodFillWholeIm(RyuImage * im_in, RyuImage * im_lable, RyuImage * seeds,
						int gl_thre, int lo_diff, int up_diff, int conn_flag);


/****************************************************************************************\
*										Gradient									     *
\****************************************************************************************/
IMAGEPROCESSING_API int ryuImageGradient(RyuImage * im, RyuImage * grad, RyuImage * orie, int thresh RYU_DEFAULT(0));



#endif  __IMAGE_PROCESSING_H__