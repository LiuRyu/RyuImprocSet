// CodeLocation.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "ryucv.h"

#define  _DEBUG_CODE_LOCATE_
#ifdef   _DEBUG_CODE_LOCATE_
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "image_ui.h"
#include "general_utils.h"
#endif

#include "code_locate.h"

//#define _METHOD_BINARIZE_
#define _METHOD_GRADIENT_FEATURE_

int gnCodeLoc_isInit = 0;
RyuSize gszCodeLoc_initSize;

#ifdef _METHOD_BINARIZE_
RyuImage * giCodeLoc_imbuff = 0;
RyuImage * giCodeLoc_integrogram = 0;
RyuImage * giCodeLoc_binary = 0;
#endif _METHOD_BINARIZE_

#ifdef _METHOD_GRADIENT_FEATURE_
RyuImage * giGradient = 0;
RyuImage * giIntegral = 0;
#endif _METHOD_GRADIENT_FEATURE_

int CodeLocating_process(RyuImage * im)
{
	int ret_val = 0, status = 0;

	if(1 != gnCodeLoc_isInit) {
		status = CodeLocating_init(im->width, im->height);
		if(1 != status) {
			ret_val = -1;
			goto nExit;
		}
	} else if(im->width > gszCodeLoc_initSize.width 
		|| im->height > gszCodeLoc_initSize.height) {
			CodeLocating_release();
			status = CodeLocating_init(im->width, im->height);
			if(1 != status) {
				ret_val = -1;
				goto nExit;
			}
	}

#ifdef _DEBUG_CODE_LOCATE_
	RyuImage * dbgIm = ryuCreateImage(ryuSize(im->width>>2, im->height>>2), 8, 1);
	ryuResizeImage(im, dbgIm);
	ryuShowImage("dbgIm", dbgIm);
	ryuReleaseImage(&dbgIm);
#endif _DEBUG_CODE_LOCATE_

#ifdef _METHOD_BINARIZE_
	RyuImage *iImbuff = 0, * iIntegr = 0, * iBinary = 0;
	iImbuff = ryuCreateImageHeader(
		ryuSize(im->width>>2, im->height>>2), RYU_DEPTH_8C, 1);
	if(NULL == iImbuff) {
		ret_val = -1;
		goto nExit;
	}
	if(iImbuff->imageSize > giCodeLoc_imbuff->imageSize) {
		ret_val = -1;
		goto nExit;
	}
	iImbuff->imageData = giCodeLoc_imbuff->imageData;

	iIntegr = ryuCreateImageHeader(
		ryuSize(im->width>>2, im->height>>2), RYU_DEPTH_32N, 1);
	if(NULL == iIntegr) {
		ret_val = -1;
		goto nExit;
	}
	if(iIntegr->imageSize > giCodeLoc_integrogram->imageSize) {
		ret_val = -1;
		goto nExit;
	}
	iIntegr->imageData = giCodeLoc_integrogram->imageData;

	iBinary = ryuCreateImageHeader(
		ryuSize(im->width>>2, im->height>>2), RYU_DEPTH_8C, 1);
	if(NULL == iBinary) {
		ret_val = -1;
		goto nExit;
	}
	if(iBinary->imageSize > giCodeLoc_binary->imageSize) {
		ret_val = -1;
		goto nExit;
	}
	iBinary->imageData = giCodeLoc_binary->imageData;

	ryuResizeImage(im, iImbuff);

	// 全图二值化处理-基于积分图的局部二值化算法
	ryuCentralLocalBinarizeFast(iImbuff, iIntegr, iBinary, ryuSize(33, 33));

#ifdef _DEBUG_CODE_LOCATE_
	RyuImage *iImbuff = 0
	ryuShowImage("im", iImbuff);
	ryuShowImage("binary", iBinary);
	cvWaitKey();
#endif _DEBUG_CODE_LOCATE_

#endif _METHOD_BINARIZE_


#ifdef _METHOD_GRADIENT_FEATURE_

	int gradThre = 10;
	RyuImage * iGrad = 0, * iInteg = 0;
	iGrad = ryuCreateImageHeader(ryuSize(im->width/3, im->height/3), RYU_DEPTH_8C, 1);
	iGrad->imageData = giGradient->imageData;
	iInteg = ryuCreateImageHeader(ryuSize(im->width/3, im->height/3), RYU_DEPTH_32N, 1);
	iInteg->imageData = giIntegral->imageData;

	ryuTimerStart();
	GradientFeature_w3s3(im, iGrad, &gradThre);
	ryuTimerStop();
	ryuTimerPrint();

#endif _METHOD_GRADIENT_FEATURE_

nExit:

#ifdef _METHOD_BINARIZE_
	if(iImbuff)
		ryuReleaseImageHeader(&iImbuff);

	if(iIntegr)
		ryuReleaseImageHeader(&iIntegr);

	if(iBinary)
		ryuReleaseImageHeader(&iBinary);
#endif _METHOD_BINARIZE_

#ifdef _METHOD_GRADIENT_FEATURE_
	if(iGrad)
		ryuReleaseImageHeader(&iGrad);
	if(iInteg)
		ryuReleaseImageHeader(&iInteg);
#endif _METHOD_GRADIENT_FEATURE_

	return ret_val;
}


int CodeLocating_init(int width, int height)
{
	int ret_val = 0;

	gszCodeLoc_initSize = ryuSize(width, height);

#ifdef _METHOD_BINARIZE_
	giCodeLoc_imbuff = ryuCreateImage(ryuSize(width, height), RYU_DEPTH_8C, 1);
	if(NULL == giCodeLoc_imbuff) {
		ret_val = -1;
		goto nExit;
	}

	giCodeLoc_integrogram = ryuCreateImage(ryuSize(width, height), RYU_DEPTH_32N, 1);
	if(NULL == giCodeLoc_integrogram) {
		ret_val = -1;
		goto nExit;
	}

	giCodeLoc_binary = ryuCreateImage(ryuSize(width, height), RYU_DEPTH_8C, 1);
	if(NULL == giCodeLoc_binary) {
		ret_val = -1;
		goto nExit;
	}
#endif _METHOD_BINARIZE_

#ifdef _METHOD_GRADIENT_FEATURE_
	giGradient = ryuCreateImage(ryuSize(width/3, height/3), RYU_DEPTH_8C, 1);
	if(NULL == giGradient) {
		ret_val = -1;
		goto nExit;
	}

	giIntegral = ryuCreateImage(ryuSize(width/3, height/3), RYU_DEPTH_32N, 1);
	if(NULL == giIntegral) {
		ret_val = -1;
		goto nExit;
	}
#endif _METHOD_GRADIENT_FEATURE_

	ret_val = gnCodeLoc_isInit = 1;

nExit:
	return ret_val;
}


void CodeLocating_release()
{
	gnCodeLoc_isInit = 0;

#ifdef _METHOD_BINARIZE_
	if(giCodeLoc_imbuff)
		ryuReleaseImage(&giCodeLoc_imbuff);

	if(giCodeLoc_integrogram)
		ryuReleaseImage(&giCodeLoc_integrogram);

	if(giCodeLoc_binary)
		ryuReleaseImage(&giCodeLoc_binary);
#endif _METHOD_BINARIZE_

#ifdef _METHOD_GRADIENT_FEATURE_
	if(giGradient)
		ryuReleaseImage(&giGradient);
	if(giIntegral)
		ryuReleaseImage(&giIntegral);
#endif _METHOD_GRADIENT_FEATURE_

	return;
}

