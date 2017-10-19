#include "stdafx.h"

#include "ryucv.h"

#define _DEBUG_EXWAYBILL_CUTOUT_
#ifdef  _DEBUG_EXWAYBILL_CUTOUT_
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "image_ui.h"
#endif  _DEBUG_EXWAYBILL_CUTOUT_

#include "Exwaybill_cutout.h"

const int gcExCut_normalizedWidth = 400;

#define gnExCut_imHeapBuffCnt	(3)
RyuImage * giExCut_imHeapBuff[gnExCut_imHeapBuffCnt] = {0};

int gnExCut_initFlag = 0;


int ExwaybillCutout_process(RyuImage * im)
{
	int ret_val = 0;
	int status = 0;

	RyuSize normalized_sz;
	double sampled_rt;

	RyuImage * imRsz = 0, * imDlt = 0;
	int hist[256] = {0};

	RyuImage * grad = 0, * orie = 0;

	RyuImage * imLabel = 0;

	// null pointer
	if(NULL == im) {
		ret_val = -1;
		goto nExit;
	}

	// initialize if not 
	if(1 != gnExCut_initFlag) {
		status = ExwaybillCutout_init(im->width, im->height);
		if(1 != status) {
			ret_val = -1;
			goto nExit;
		}
	}

	// calculate the sample-ratio & normalized size
	sampled_rt = 1.0 * gcExCut_normalizedWidth / im->width;
	normalized_sz = ryuSize(gcExCut_normalizedWidth, 
		(int)(sampled_rt * im->height + 0.5));
	
	if(normalized_sz.height > giExCut_imHeapBuff[0]->height) {
		// re-initialize
		ExwaybillCutout_release();
		status = ExwaybillCutout_init(im->width, im->height);
		if(1 != status) {
			ret_val = -1;
			goto nExit;
		}
	}

	// resize image with normalized size
	imRsz = ryuCreateImageHeader(normalized_sz, 8, 1);
	if(NULL == imRsz) {
		ret_val = -1;
		goto nExit;
	}
	imRsz->imageData = giExCut_imHeapBuff[0]->imageData;

	status = ryuResizeImage(im, imRsz);
	if(1 != status) {
		ret_val = -1;
		goto nExit;
	}

#ifdef  _DEBUG_EXWAYBILL_CUTOUT_
	ryuShowImage("imRsz", imRsz);
#endif _DEBUG_EXWAYBILL_CUTOUT_

	// dilate resized image
	imDlt = ryuCreateImageHeader(normalized_sz, 8, 1);
	if(NULL == imDlt) {
		ret_val = -1;
		goto nExit;
	}
	imDlt->imageData = giExCut_imHeapBuff[1]->imageData;
	status = ryuDilate(imRsz, imDlt);
	if(1 != status) {
		ret_val = -1;
		goto nExit;
	}
#ifdef  _DEBUG_EXWAYBILL_CUTOUT_
	ryuShowImage("imDlt", imDlt);
#endif _DEBUG_EXWAYBILL_CUTOUT_

	// get dilated image's histogram
	ryuImageContrastAnalyze(imDlt->imageData, imDlt->width, imDlt->height, imDlt->widthStep, hist);

#ifdef  _DEBUG_EXWAYBILL_CUTOUT_
	IplImage * dbg_diagram = ryuDrawDiagram1D(hist, 256, 1.0, 0.1);
	cvNamedWindow("dbg_diagram");
	cvShowImage("dbg_diagram", dbg_diagram);
	cvReleaseImage(&dbg_diagram);
#endif _DEBUG_EXWAYBILL_CUTOUT_

	// get image full gradient
	grad = ryuCreateImage(ryuGetSize(imDlt), RYU_DEPTH_8C, 1);
	orie = ryuCreateImage(ryuGetSize(imDlt), RYU_DEPTH_8C, 1);
	ryuImageGradient(imDlt, grad, orie, 36);
#ifdef  _DEBUG_EXWAYBILL_CUTOUT_
	ryuShowImage("grad", grad);
	ryuShowImage("orie", orie);
#endif _DEBUG_EXWAYBILL_CUTOUT_

	// Flood-fill dilated image
	imLabel = ryuCreateImage(ryuGetSize(imDlt), RYU_DEPTH_32N, 1);
	status = ryuFloodFillWholeIm(imDlt, imLabel, 0, 54, 24, 24, 4);

#ifdef  _DEBUG_EXWAYBILL_CUTOUT_
	int * colorPad = (int *)malloc(status * 3 * sizeof(int));
	for(int iid = 0; iid < status; iid++) {
		int randTmp = rand()%3;
		colorPad[iid*3] = (0 == randTmp) ? rand()%128+128 : rand()%256;
		colorPad[iid*3+1] = (1 == randTmp) ? rand()%128+128 : rand()%256;
		colorPad[iid*3+2] = (2 == randTmp) ? rand()%128+128 : rand()%256;
	}
	IplImage * dbg_floodfill = cvCreateImage(cvSize(imDlt->width, imDlt->height), 8, 3);
	cvZero(dbg_floodfill);
	for(int jjd = 0; jjd < imDlt->height; jjd++) {
		unsigned char * dbgImg = (unsigned char *)dbg_floodfill->imageData + jjd * dbg_floodfill->widthStep;
		int * dbgMat = (int *)(imLabel->imageData + jjd * imLabel->widthStep);
		for(int iid = 0; iid < imDlt->width; iid++) {
			if(dbgMat[iid] > 0) {
				dbgImg[iid*3] = colorPad[dbgMat[iid]*3];
				dbgImg[iid*3+1] = colorPad[dbgMat[iid]*3+1];
				dbgImg[iid*3+2] = colorPad[dbgMat[iid]*3+2];
			}
		}
	}
	cvNamedWindow("dbg_floodfill");
	cvShowImage("dbg_floodfill", dbg_floodfill);
	cvWaitKey();
	cvReleaseImage(&dbg_floodfill);
	free(colorPad);
#endif _DEBUG_EXWAYBILL_CUTOUT_

nExit:

	if(imRsz)
		ryuReleaseImageHeader(&imRsz);
	if(imDlt)
		ryuReleaseImageHeader(&imDlt);

	if(grad)
		ryuReleaseImage(&grad);
	if(orie)
		ryuReleaseImage(&orie);
	if(imLabel)
		ryuReleaseImage(&imLabel);

	return ret_val;
}


int ExwaybillCutout_init(int width, int height)
{
	int ret_val = 0;
	int i = 0;

	RyuSize sz = ryuSize(gcExCut_normalizedWidth, gcExCut_normalizedWidth);

	if(width < height) {
		sz.height = (int)(1.0 * height * 400 / width + 0.5);
	}

	for(i = 0; i < gnExCut_imHeapBuffCnt; i++) {
		giExCut_imHeapBuff[i] = ryuCreateImage(sz, 8, 1);
		if(NULL == giExCut_imHeapBuff[i]) {
			ret_val = -1;
			goto nExit;
		}
	}

	ret_val = gnExCut_initFlag = 1;

nExit:

	return ret_val;
}


void ExwaybillCutout_release()
{
	int i = 0;

	gnExCut_initFlag = 0;

	for(i = 0; i < gnExCut_imHeapBuffCnt; i++) {
		if(giExCut_imHeapBuff[i])
			ryuReleaseImage(&giExCut_imHeapBuff[i]);
	}

	return;
}

