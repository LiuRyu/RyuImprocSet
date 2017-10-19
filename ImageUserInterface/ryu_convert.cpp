#include "stdafx.h" 

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <opencv2/opencv.hpp>

#include "types.h"

#include "general_utils.h"
#include "ryucv.h"
#include "image_ui.h"

RyuImage * iplImage2ryuImage(IplImage * ipl)
{
	RyuImage * ryu = ryuCreateImageHeader(ryuSize(ipl->width, ipl->height),
		ipl->depth, ipl->nChannels);

	ryu->imageData = (unsigned char *)ipl->imageData;

	return ryu;
}

IplImage * ryuImage2iplImage(RyuImage * ryu)
{
	IplImage * ipl = cvCreateImageHeader(cvSize(ryu->width, ryu->height), 
		ryu->depth, ryu->nChannels);

	ipl->imageData = ipl->imageDataOrigin = (char *)ryu->imageData;

	return ipl;
}

int ryuShowImage(const char * name, RyuImage * ryu)
{
	IplImage * ipl = ryuImage2iplImage(ryu);
	if(NULL == ipl)
		return 0;

	cvNamedWindow(name);
	cvShowImage(name, ipl);

	cvReleaseImageHeader(&ipl);

	return 1;
}