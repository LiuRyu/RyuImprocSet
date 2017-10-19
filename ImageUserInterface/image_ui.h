#ifndef __IMAGE_USER_INTERFACE_H__
#define __IMAGE_USER_INTERFACE_H__

#ifdef IMAGEUSERINTERFACE_EXPORTS
#define IMAGEUSERINTERFACE_API __declspec(dllexport)
#else
#define IMAGEUSERINTERFACE_API __declspec(dllimport)
#endif

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <opencv2/opencv.hpp>

#include "types.h"

/****************************************************************************************\
*								Label rect in image window								 *
\****************************************************************************************/
// 控制台打印开关
#define _LABEL_IM_SAMPLES_PRINT_	(1)

IMAGEUSERINTERFACE_API int ryuLabelImageInWindow(IplImage * im, double hscale, double vscale, 
							 CvPoint rect[4], int mode RYU_DEFAULT(RYU_LABELIM_POINT), 
							 int mask_w RYU_DEFAULT(256), int mask_h RYU_DEFAULT(256));

IMAGEUSERINTERFACE_API float getLabelRectAngel();

IMAGEUSERINTERFACE_API CvSize getLableRectSize();


/****************************************************************************************\
*										Resize image									 *
\****************************************************************************************/
IMAGEUSERINTERFACE_API CvSize ryuSizeFitScreen(CvSize sz, double &hscale, double &vscale, 
											   double s2bratio RYU_DEFAULT(0.9));


/****************************************************************************************\
*										Get image info									 *
\****************************************************************************************/
IMAGEUSERINTERFACE_API int ryuGetPointLum(IplImage* im, CvPoint pt);


/****************************************************************************************\
*										Draw Diagram									 *
\****************************************************************************************/
IMAGEUSERINTERFACE_API IplImage * ryuDrawDiagramFrame(int xrange, int yrange_pos, int yrange_neg, 
							double hscl, double vscl, CvScalar color);

IMAGEUSERINTERFACE_API IplImage * ryuDrawDiagram1D(int * arr, int arr_len, 
							double hscl RYU_DEFAULT(1.0), double vscl RYU_DEFAULT(1.0), 
							int val_form RYU_DEFAULT(0), int diag_type RYU_DEFAULT(0),
							CvPoint * origin_pt RYU_DEFAULT(0));


/****************************************************************************************\
*										Ryu Convert										 *
\****************************************************************************************/
IMAGEUSERINTERFACE_API RyuImage * iplImage2ryuImage(IplImage * ipl);

IMAGEUSERINTERFACE_API IplImage * ryuImage2iplImage(RyuImage * ryu);

IMAGEUSERINTERFACE_API int ryuShowImage(const char * name, RyuImage * ryu);


#endif  __IMAGE_USER_INTERFACE_H__
