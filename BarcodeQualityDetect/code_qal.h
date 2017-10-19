#ifndef __CODE_QUALITY_DETECT_H__
#define __CODE_QUALITY_DETECT_H__

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#ifdef CODEQUALITYDETECT_EXPORTS
#define CODEQUALITYDETECT_API __declspec(dllexport)
#else
#define CODEQUALITYDETECT_API __declspec(dllimport)
#endif

CODEQUALITYDETECT_API int BarcodeQualityDetect(IplImage * im, CvPoint code_cent, 
							int * contrast, int * luminance, int * definition);


#endif  __CODE_QUALITY_DETECT_H__