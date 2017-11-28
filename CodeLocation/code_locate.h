#ifndef __CODE_LOCATION_H__
#define __CODE_LOCATION_H__

#include "ryucv.h"

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

CODELOCATION_API int CodeLocating_process(RyuImage * im);

CODELOCATION_API void CodeLocating_release();



/*++++++++++++++++++++++++++++++++++++++++INLINE++++++++++++++++++++++++++++++++++++++++*/

/****************************************************************************************\
*									Gradient feature								     *
\****************************************************************************************/
CODELOCATION_API int CodeLocFnc_GradientFeatureSample4(RyuImage * im, int * grad_thre,
									  RyuImage * gradient, RyuImage * gradorie);

CODELOCATION_API int CodeLocFnc_GradientExtract(RyuImage * im, RyuImage * gradient, 
							   float extract_ratio RYU_DEFAULT(0.025), int * grad_thre RYU_DEFAULT(0));

CODELOCATION_API int GradientFeature_w3s3(RyuImage * im, RyuImage * gradient, int * grad_thre, 
						 float grad_thre_ratio RYU_DEFAULT(0.1));

CODELOCATION_API int GetGradientFeatureIntegral(RyuImage * gradient, int grad_thre, RyuImage * integral);

CODELOCATION_API int FoldGradientFeatureInSlideWindow(RyuImage * integral, RyuImage * feature_map,
							  RyuSize sw_size RYU_DEFAULT(ryuSize(16, 16)), 
							  RyuSize sw_step RYU_DEFAULT(ryuSize(16, 16)));


#endif  __CODE_LOCATION_H__
