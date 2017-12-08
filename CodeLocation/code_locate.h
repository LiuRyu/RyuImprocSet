#ifndef __CODE_LOCATION_H__
#define __CODE_LOCATION_H__

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "types.h"

#ifdef  CODELOCATION_EXPORTS
#define CODELOCATION_API __declspec(dllexport)
#else
#define CODELOCATION_API __declspec(dllimport)
#endif

/*++++++++++++++++++++++++++++++++++++++++EXPORT++++++++++++++++++++++++++++++++++++++++*/

/****************************************************************************************\
*											XXX									         *
\****************************************************************************************/
CODELOCATION_API int CodeLocating_init(int width, int height);

CODELOCATION_API int CodeLocating_process(IplImage * im, char * imfile_path);

CODELOCATION_API void CodeLocating_release();



/*++++++++++++++++++++++++++++++++++++++++INLINE++++++++++++++++++++++++++++++++++++++++*/

/****************************************************************************************\
*									Gradient feature								     *
\****************************************************************************************/
CODELOCATION_API int CodeLocFnc_GradientFeatureSample4(RyuImage * im, int * grad_thre,
									  RyuImage * gradient, RyuImage * gradorie);

CODELOCATION_API int CodeLocFnc_GradientExtract(RyuImage * im, RyuImage * gradient, 
							   float extract_ratio RYU_DEFAULT(0.025), int * grad_thre RYU_DEFAULT(0));

CODELOCATION_API int ryuGradientFeature_w3s3(IplImage * im, IplImage * gradient, int * grad_thre, 
						 float grad_thre_ratio RYU_DEFAULT(0.1));

int ryuGradientFeatureFilter(IplImage * grad_in, IplImage * grad_out, 
							  int grad_thresh, int denoise_level RYU_DEFAULT(0));

CODELOCATION_API int ryuIntegrateGradientFeature(IplImage * gradient, int grad_thre, IplImage * integral);

CODELOCATION_API int ryuFoldGradientFeatureInPatch(IplImage * integral, IplImage * feature_map,
							  CvSize sw_size RYU_DEFAULT(cvSize(16, 16)), 
							  CvSize sw_step RYU_DEFAULT(cvSize(8, 8)));

CODELOCATION_API int ryuFeaturePatchCoords2Arr(IplImage * feature_map, CvPoint * patch_coords, 
						   CvSize sw_size RYU_DEFAULT(cvSize(16, 16)), 
						   CvSize sw_step RYU_DEFAULT(cvSize(8, 8)),
						   float thresh_ratio RYU_DEFAULT(0.25));

CODELOCATION_API int ryuWritePatchCoords2File(CvPoint * patch_coords, int patch_cnt, char * imfile_path);


#endif  __CODE_LOCATION_H__
