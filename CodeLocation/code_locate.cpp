// CodeLocation.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "ryucv.h"

#define  _DEBUG_CODE_LOCATE_
#ifdef   _DEBUG_CODE_LOCATE_
#include "image_ui.h"
#include "general_utils.h"
#endif

#include "code_locate.h"

#define _METHOD_GRADIENT_FEATURE_

int gnCodeLoc_isInit = 0;
CvSize gszCodeLoc_initSize;

#ifdef _METHOD_GRADIENT_FEATURE_
IplImage * giGradient = 0, * giGradfilter = 0;
IplImage * giIntegral = 0, * giFeature = 0;
CvPoint * gptPatchCoords = 0;
#endif _METHOD_GRADIENT_FEATURE_

int CodeLocating_process(IplImage * im, char * imfile_path)
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
	IplImage * dbgIm = cvCreateImage(cvSize(im->width>>2, im->height>>2), 8, 1);
	cvResize(im, dbgIm);
	cvNamedWindow("dbgIm");
	cvShowImage("dbgIm", dbgIm);
	cvReleaseImage(&dbgIm);
#endif _DEBUG_CODE_LOCATE_


#ifdef _METHOD_GRADIENT_FEATURE_
	int i = 0;
	int gradThre = 10;
	IplImage * iGrad = 0, * iGradFiltered = 0;
	IplImage * iInteg = 0, * iFmap = 0;

	CvPoint * ptCoords = gptPatchCoords;

	CvSize sw_size = cvSize(21, 21);
	CvSize sw_step = cvSize(16, 16);

	float swThreRatio = 0.25;
	int patchCnt = 0;

	iGrad = cvCreateImageHeader(cvSize(im->width/3, im->height/3), RYU_DEPTH_8C, 1);
	iGrad->imageData = giGradient->imageData;
	iGradFiltered = cvCreateImageHeader(cvSize(im->width/3, im->height/3), RYU_DEPTH_8C, 1);
	iGradFiltered->imageData = giGradfilter->imageData;

	iInteg = cvCreateImageHeader(cvSize(im->width/3, im->height/3), RYU_DEPTH_32N, 1);
	iInteg->imageData = giIntegral->imageData;
	iFmap = cvCreateImageHeader(cvSize(im->width/3, im->height/3), RYU_DEPTH_32N, 1);
	iFmap->imageData = giFeature->imageData;

	ryuTimerStart();

	// 提取梯度
	GradientFeature_w3s3(im, iGrad, &gradThre, 0.025);
	// 过滤梯度点
	GradientFeature_filtering(iGrad, iGradFiltered, gradThre, 1);
	// 构建梯度积分图
	GetGradientFeatureIntegral(iGradFiltered, gradThre, iInteg);
	// 获取滑动窗梯度特征
	FoldGradientFeatureInSlideWindow(iInteg, iFmap, sw_size, sw_step);
	// 特征patch坐标提取
	patchCnt = FeaturePatchCoords2Arr(iFmap, ptCoords, sw_size, sw_step, swThreRatio);
	for(i = 0; i < patchCnt; i++) {
		ptCoords[i].x *= 3;
		ptCoords[i].y *= 3;
	}
	// 坐标写入文件
	WritePatchCoords2File(ptCoords, patchCnt, imfile_path);

	ryuTimerStop();
	ryuTimerPrint();

#ifdef _DEBUG_CODE_LOCATE_
	printf("[CodeLocating_process]- grad_thre = %d\n", gradThre);
	cvNamedWindow("CodeLocating_grad_0");
	cvShowImage("CodeLocating_grad_0", iGrad);

	IplImage * Grad3C_dbg = cvCreateImage(cvGetSize(iGrad), IPL_DEPTH_8U, 3);
	cvCvtColor(iGrad, Grad3C_dbg, CV_GRAY2BGR);
	unsigned char * plGrad_dbg = (unsigned char *)Grad3C_dbg->imageData, * pGrad_dbg = 0;
	for(int i_dbg = 0; i_dbg < Grad3C_dbg->height; i_dbg++) {
		pGrad_dbg = plGrad_dbg;
		for(int j_dbg = 0; j_dbg < Grad3C_dbg->width; j_dbg++) {
			if(*pGrad_dbg > gradThre) {
				pGrad_dbg[1] = 255;
			}
			pGrad_dbg += 3;
		}
		plGrad_dbg += Grad3C_dbg->widthStep;
	}
	cvNamedWindow("CodeLocating_grad_1");
	cvShowImage("CodeLocating_grad_1", Grad3C_dbg);

	cvCvtColor(iGradFiltered, Grad3C_dbg, CV_GRAY2BGR);
	plGrad_dbg = (unsigned char *)Grad3C_dbg->imageData, * pGrad_dbg = 0;
	for(int i_dbg = 0; i_dbg < Grad3C_dbg->height; i_dbg++) {
		pGrad_dbg = plGrad_dbg;
		for(int j_dbg = 0; j_dbg < Grad3C_dbg->width; j_dbg++) {
			if(*pGrad_dbg > gradThre) {
				pGrad_dbg[1] = 255;
			}
			pGrad_dbg += 3;
		}
		plGrad_dbg += Grad3C_dbg->widthStep;
	}
	cvNamedWindow("CodeLocating_grad_2");
	cvShowImage("CodeLocating_grad_2", Grad3C_dbg);

	printf("[CodeLocating_process]- sw_size = (%d, %d), sw_step = (%d, %d)\n", 
		sw_size.width, sw_size.height, sw_step.width, sw_step.height);
	printf("[CodeLocating_process]- h_step = %d, v_step = %d\n", 
		iFmap->width, iFmap->height);

	int thre_dbg = sw_size.width * sw_size.height / 4;
	IplImage * FmapUc_dbg = cvCreateImage(cvGetSize(iFmap), IPL_DEPTH_8U, 1);
	unsigned char * plFmapUc_dbg = (unsigned char *)FmapUc_dbg->imageData, * pFmapUc_dbg = 0;
	unsigned int * plFmap_dbg = (unsigned int *)iFmap->imageData, * pFmap_dbg = 0;
	int sw_total = sw_size.width * sw_size.height;
	for(int i_dbg = 0; i_dbg < FmapUc_dbg->height; i_dbg++) {
		pFmapUc_dbg = plFmapUc_dbg;
		pFmap_dbg = plFmap_dbg;
		for(int j_dbg = 0; j_dbg < FmapUc_dbg->width; j_dbg++) {
			*(pFmapUc_dbg++) = (*(pFmap_dbg++) * 255 / sw_total);
		}
		plFmapUc_dbg += FmapUc_dbg->widthStep;
		plFmap_dbg += iFmap->width;
	}
	cvNamedWindow("CodeLocating_Fmap_3");
	cvShowImage("CodeLocating_Fmap_3", FmapUc_dbg);
	cvThreshold(FmapUc_dbg, FmapUc_dbg, thre_dbg, 255, CV_THRESH_BINARY);
	cvNamedWindow("CodeLocating_Fmap_4");
	cvShowImage("CodeLocating_Fmap_4", FmapUc_dbg);
	cvReleaseImage(&FmapUc_dbg);

	int roiCount_dbg = 0;
	plFmap_dbg = (unsigned int *)iFmap->imageData;
	for(int i_dbg = 0; i_dbg < iFmap->height; i_dbg++) {
		pFmap_dbg = plFmap_dbg;
		for(int j_dbg = 0; j_dbg < iFmap->width; j_dbg++) {
			if(*pFmap_dbg > thre_dbg) {
				roiCount_dbg++;
				CvPoint ptLine_dbg = cvPoint(j_dbg * sw_step.width + 1, i_dbg * sw_step.height + sw_size.height/2 + 1);
				cvLine(Grad3C_dbg, ptLine_dbg, cvPoint(ptLine_dbg.x + sw_size.width - 1, ptLine_dbg.y), 
					CV_RGB(255, 0, 0), 1);
				ptLine_dbg = cvPoint(j_dbg * sw_step.width + sw_size.width/2 + 1, i_dbg * sw_step.height + 1);
				cvLine(Grad3C_dbg, ptLine_dbg, cvPoint(ptLine_dbg.x, ptLine_dbg.y + sw_size.height - 1),
					CV_RGB(255, 0, 0), 1);
			}
			pFmap_dbg++;
		}
		plFmap_dbg += iFmap->width;
	}
	printf("[CodeLocating_process]- feature_thre = %d, roiCount_dbg = %d\n", 
		thre_dbg, roiCount_dbg);
	cvNamedWindow("CodeLocating_Gmap_0");
	cvShowImage("CodeLocating_Gmap_0", Grad3C_dbg);

	cvWaitKey();
	cvReleaseImage(&Grad3C_dbg);
#endif
#endif 

nExit:

#ifdef _METHOD_GRADIENT_FEATURE_
	if(iGrad)
		cvReleaseImageHeader(&iGrad);
	if(iGradFiltered)
		cvReleaseImageHeader(&iGradFiltered);
	if(iInteg)
		cvReleaseImageHeader(&iInteg);
	if(iFmap)
		cvReleaseImageHeader(&iFmap);
#endif _METHOD_GRADIENT_FEATURE_

	return ret_val;
}


int CodeLocating_init(int width, int height)
{
	int ret_val = 0;

	gszCodeLoc_initSize = cvSize(width, height);

#ifdef _METHOD_GRADIENT_FEATURE_
	giGradient = cvCreateImage(cvSize(width/3, height/3), IPL_DEPTH_8U, 1);
	if(NULL == giGradient) {
		ret_val = -1;
		goto nExit;
	}

	giGradfilter = cvCreateImage(cvSize(width/3, height/3), IPL_DEPTH_8U, 1);
	if(NULL == giGradfilter) {
		ret_val = -1;
		goto nExit;
	}

	giIntegral = cvCreateImage(cvSize(width/3, height/3), IPL_DEPTH_32S, 1);
	if(NULL == giIntegral) {
		ret_val = -1;
		goto nExit;
	}

	giFeature = cvCreateImage(cvSize(width/3, height/3), IPL_DEPTH_32S, 1);
	if(NULL == giFeature) {
		ret_val = -1;
		goto nExit;
	}

	gptPatchCoords = (CvPoint *)malloc((width * height / 9) * sizeof(CvPoint));
	if(NULL == gptPatchCoords) {
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

#ifdef _METHOD_GRADIENT_FEATURE_
	if(giGradient)
		cvReleaseImage(&giGradient);
	if(giGradfilter)
		cvReleaseImage(&giGradfilter);
	if(giIntegral)
		cvReleaseImage(&giIntegral);
	if(giFeature)
		cvReleaseImage(&giFeature);

	if(gptPatchCoords) {
		free(gptPatchCoords);
		gptPatchCoords = 0;
	}
#endif _METHOD_GRADIENT_FEATURE_

	return;
}

