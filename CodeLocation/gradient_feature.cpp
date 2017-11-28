#include "stdafx.h"

#include "ryucv.h"

#define _DEBUG_GRADIENT_FEATURE_
#ifdef  _DEBUG_GRADIENT_FEATURE_
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "image_ui.h"
#endif

#include "code_locate.h"


const float gfGradThreRatio = 0.025;
const int gnGradThreMaxVal = 255;

int CodeLocFnc_GradientFeature_w4s4(RyuImage * im, int * grad_thre,
									  RyuImage * gradient, RyuImage * gradorie)
{
	int nRet = 0;
	int i = 0, j = 0, nstep = 0;

	int dx[4] = {0}, dy[4] = {0}, t[4] = {0};

	int val = 0, val1 = 0, val2 = 0, idx = 0, idx1 = 0, idx2 = 0;

	int nsum = 0, nthre = *grad_thre;

	int hist[256] = {0};

	unsigned char * pOrie	= 0;
	unsigned char * pGrad	= 0;

	unsigned char * lOrie	= 0;
	unsigned char * lGrad	= 0;

	unsigned char * loffset_1, * loffset_2, * loffset_3, * loffset_4;
	unsigned char * poffset_1, * poffset_2, * poffset_3, * poffset_4;

	if(NULL == im || NULL == gradient || NULL == gradorie) {
		nRet = -10000012;
		goto nExit;
	}

	loffset_1 = loffset_2 = loffset_3 = loffset_4 = 0;
	poffset_1 = poffset_2 = poffset_3 = poffset_4 = 0;

	nstep = im->widthStep << 2;
	lOrie	= gradorie->imageData;
	lGrad	= gradient->imageData;

	loffset_1	= im->imageData;
	loffset_2	= loffset_1 + im->widthStep;
	loffset_3	= loffset_2 + im->widthStep;
	loffset_4	= loffset_3 + im->widthStep;

	for(i = gradient->height; i > 0; i--) {
		poffset_1 = loffset_1;
		poffset_2 = loffset_2;
		poffset_3 = loffset_3;
		poffset_4 = loffset_4;

		pGrad = lGrad;
		pOrie = lOrie;

		for(j = gradient->width; j > 0; j--) {
			dx[0] = poffset_2[2] - poffset_2[0];
			dy[0] = poffset_3[1] - poffset_1[1];
			dx[1] = poffset_2[3] - poffset_2[1];
			dy[1] = poffset_3[2] - poffset_1[2];
			dx[2] = poffset_3[2] - poffset_3[0];
			dy[2] = poffset_4[1] - poffset_2[1];
			dx[3] = poffset_3[3] - poffset_3[1];
			dy[3] = poffset_4[2] - poffset_2[2];

			t[0] = abs(dx[0]) > abs(dy[0]) ? abs(dx[0]) : abs(dy[0]);
			t[1] = abs(dx[1]) > abs(dy[1]) ? abs(dx[1]) : abs(dy[1]);
			t[2] = abs(dx[2]) > abs(dy[2]) ? abs(dx[2]) : abs(dy[2]);
			t[3] = abs(dx[3]) > abs(dy[3]) ? abs(dx[3]) : abs(dy[3]);

			val1 = (t[0] > t[1]) ? t[0] : t[1];
			idx1 = (t[0] > t[1]) ? 0 : 1;
			val2 = (t[2] > t[3]) ? t[2] : t[3];
			idx2 = (t[2] > t[3]) ? 2 : 3;

			idx = (val1 > val2) ? idx1 : idx2;
			val = (val1 > val2) ? val1 : val2;

			*pGrad = (val < nthre) ? 0 : val;
			*pOrie = (*pGrad) ? ryuAtan180_10SH(dy[idx], dx[idx]) : 0;

			hist[val]++;

			poffset_1 += 4;
			poffset_2 += 4;
			poffset_3 += 4;
			poffset_4 += 4;
			pGrad++;
			pOrie++;
		}
		loffset_1 += nstep;
		loffset_2 += nstep;
		loffset_3 += nstep;
		loffset_4 += nstep;
		lOrie += gradient->widthStep;
		lGrad += gradient->widthStep;
	}

	nsum = idx = 0;
	val = gradient->width * gradient->height * gfGradThreRatio;
	for(i = 255; i >= 0; i--) {
		nsum += hist[i];
		if(nsum >= val) {
			idx = i - 1;
			break;
		}
	}
	nthre = *grad_thre = RYUMIN(gnGradThreMaxVal, RYUMAX(idx, nthre));

	lOrie	= gradorie->imageData;
	lGrad	= gradient->imageData;
	for(i = gradient->height; i > 0; i--) {
		pGrad = lGrad;
		pOrie = lOrie;
		for(j = gradient->width; j > 0; j--) {
			*pGrad = (*pGrad < nthre) ? 0 : *pGrad;
			*pOrie = (*pGrad) ? *pOrie : 0;
			pGrad++;
			pOrie++;
		}
		lOrie += gradient->widthStep;
		lGrad += gradient->widthStep;
	}

	nRet = 1;
nExit:
	return nRet;
}


int CodeLocFnc_GradientExtract(RyuImage * im, RyuImage * gradient, float extract_ratio, int * grad_thre)
{
	int ret_val = 0;
	int hist[256] = {0};

	float fZoomRatioW = 0.0;
	float fZoomRatioH = 0.0;
	float fZoomAccW = 0.0, fZoomAccH = 0.0;

	int i = 0, j = 0;
	unsigned char * pIn = 0, * pOut = 0, * pOutL = 0;
	unsigned char * pInUppr = 0, * pInUndr = 0;

	int dx = 0, dy = 0;
	int nThre = 0, curIndex = 0;

	if( !im || !gradient ) {
		printf( "ERROR! Invalid input of ryuSetImage, img_in = 0x%x, img_out = 0x%x\n",
			im, gradient );
		return -1;
	}

	if( !im->imageData || !gradient->imageData ) {
		printf( "ERROR! Bad address of ryuSetImage, img_in data = 0x%x, img_out data = 0x%x\n",
			im->imageData, gradient->imageData );
		return -1;
	}

	if(1 >= gradient->width || 1 >= gradient->height) {
		printf( "ERROR! Bad value of gradient w/h, width = %d, width = %d\n",
			gradient->width, gradient->height );
		return -1;
	}

	if(grad_thre) {
		 nThre = *grad_thre;
	}

	fZoomRatioW = (im->width - 3) * 1.0 / (gradient->width - 1);
	fZoomRatioH = (im->height - 3) * 1.0 / (gradient->height - 1);

	fZoomAccH = 1.0;
	pOutL = gradient->imageData;
	for(j = 0; j < gradient->height; j++) {
		fZoomAccW = 1.0;
		pIn = im->imageData + (int)(fZoomAccH+0.5) * im->widthStep;
		pInUppr = pIn - im->widthStep;
		pInUndr = pIn + im->widthStep;
		pOut = pOutL;
		for(i = 0; i < gradient->width; i++) {
			curIndex = (int)(fZoomAccW+0.5);
			dx = abs(pIn[curIndex+1] - pIn[curIndex-1]);
			dy = abs(pInUndr[curIndex] - pInUppr[curIndex]);
			pOut[i] = curIndex = RYUMAX(dx, dy);
			hist[curIndex]++;

			fZoomAccW = fZoomAccW + fZoomRatioW;
		}
		fZoomAccH = fZoomAccH + fZoomRatioH;
		pOutL += gradient->widthStep;
	}

	dx = 0;
	dy = gradient->width * gradient->height * extract_ratio;
	for(i = 255; i >= 0; i--) {
		dx += hist[i];
		if(dx >= dy) {
			curIndex = i - 1;
			break;
		}
	}
	nThre = RYUMAX(curIndex, nThre);

	pOutL = gradient->imageData;
	for(j = 0; j < gradient->height; j++) {
		for(i = 0; i < gradient->width; i++) {
			pOut[i] = (pOut[i] >= nThre) ? pOut[i] : 0;
		}
		pOutL += gradient->widthStep;
	}

	if(grad_thre) {
		*grad_thre = nThre;
	}

	ret_val = 1;

nExit:

	return ret_val;
}


int GradientFeature_w3s3(RyuImage * im, RyuImage * gradient, int * grad_thre, float grad_thre_ratio)
{
	int nRet = 0;
	int i = 0, j = 0, nstep = 0;

	int dx = 0, dy = 0, val = 0, idx = 0;

	int nsum = 0, nthre = 0;

	int hist[256] = {0};

	unsigned char * pGrad = 0, * plGrad = 0;

	unsigned char * plOffset_n1 = 0, * plOffset_0 = 0, * plOffset_p1 = 0;
	unsigned char * pOffset_n1 = 0, * pOffset_0 = 0, * pOffset_p1 = 0;

	if(NULL == im || NULL == gradient || NULL == grad_thre) {
		nRet = -1;
		goto nExit;
	}

	if(NULL == im->imageData || NULL == gradient->imageData) {
		nRet = -2;
		goto nExit;
	}

	if(im->width/3 != gradient->width || im->height/3 != gradient->height) {
		nRet = -3;
		goto nExit;
	}

	nthre = RYUMAX(*grad_thre, 10);

	//////////////////////////////////////////////////////////////////////////
	// 获取(w/3)*(h/3)梯度图像
	nstep = im->widthStep * 3;
	plGrad	= gradient->imageData;

	plOffset_n1	= im->imageData;
	plOffset_0	= plOffset_n1 + im->widthStep;
	plOffset_p1	= plOffset_0 + im->widthStep;
	for(i = gradient->height; i > 0; i--) {
		pOffset_n1 = plOffset_n1;
		pOffset_0 = plOffset_0;
		pOffset_p1 = plOffset_p1;
		pGrad = plGrad;

		for(j = gradient->width; j > 0; j--) {
			dx = abs(pOffset_0[2] - pOffset_0[0]);
			dy = abs(pOffset_p1[1] - pOffset_n1[1]);
			val = (dx > dy) ? dx : dy;

			*pGrad = val;
			hist[val]++;

			pOffset_n1 += 3;
			pOffset_0 += 3;
			pOffset_p1 += 3;
			pGrad++;
		}
		plOffset_n1 += nstep;
		plOffset_0 += nstep;
		plOffset_p1 += nstep;
		plGrad += gradient->widthStep;
	}

	//////////////////////////////////////////////////////////////////////////
	// 确定有效梯度阈值
	nsum = idx = 0;
	val = gradient->width * gradient->height * gfGradThreRatio;
	for(i = 255; i >= 0; i--) {
		nsum += hist[i];
		if(nsum >= val) {
			idx = i - 1;
			break;
		}
	}

	*grad_thre = RYUMIN(36, RYUMAX(idx, nthre));
	nRet = 1;

#ifdef  _DEBUG_GRADIENT_FEATURE_
	printf("[GradFeature_w3s3]- ratio = %.5f, grad_thre = %d\n", 
		grad_thre_ratio, *grad_thre);
	ryuShowImage("GradFeature_w3s3_0", gradient);
	RyuImage * Grad3C_dbg = ryuCreateImage(ryuGetSize(gradient), 8, 3);
	ryuCvtColor(gradient, Grad3C_dbg, RYU_GRAY2BGR);
	unsigned char * plGrad_dbg = Grad3C_dbg->imageData, * pGrad_dbg = 0;
	for(int i_dbg = 0; i_dbg < Grad3C_dbg->height; i_dbg++) {
		pGrad_dbg = plGrad_dbg;
		for(int j_dbg = 0; j_dbg < Grad3C_dbg->width; j_dbg++) {
			if(*pGrad_dbg > *grad_thre) {
				pGrad_dbg[1] = 255;
			}
			pGrad_dbg += 3;
		}
		plGrad_dbg += Grad3C_dbg->widthStep;
	}
	ryuShowImage("GradFeature_w3s3_1", Grad3C_dbg);
	cvWaitKey();
	ryuReleaseImage(&Grad3C_dbg);
#endif

nExit:
	return nRet;
}


int GetGradientFeatureIntegral(RyuImage * gradient, int grad_thre, RyuImage * integral)
{
	int nRet = 0;
	int i = 0, j = 0;

	unsigned char * pGrad = 0, * plGrad = 0;
	unsigned int * pInteg = 0, * plInteg = 0;
	unsigned int * pInteg_n1 = 0, * plInteg_n1 = 0;

	if(NULL == gradient || NULL == integral) {
		nRet = -1;
		goto nExit;
	}

	if(NULL == gradient->imageData || NULL == integral->imageData) {
		nRet = -2;
		goto nExit;
	}

	if(RYU_DEPTH_32N != integral->depth) {
		nRet = -3;
		goto nExit;
	}

	if(gradient->width != integral->width || gradient->height != integral->height) {
		nRet = -4;
		goto nExit;
	}

	// 建立阈值过滤后的梯度（幅值/个数）积分图
	plGrad	= gradient->imageData;
	plInteg = plInteg_n1 = (unsigned int *)integral->imageData;
	// 建立首行积分图
	*plInteg = (*plGrad > grad_thre ? 1 : 0);		// 行首元素
	pGrad = plGrad + 1;
	pInteg = plInteg + 1;
	for(i = 1; i < gradient->width; i++) {
		*pInteg = *(pInteg-1) + (*pGrad > grad_thre ? 1 : 0);
		pGrad++;
		pInteg++;
	}
	plGrad += gradient->widthStep;
	plInteg += gradient->width;

	for(i = gradient->height; i > 0; i--) {
		*plInteg = *plInteg_n1 +  (*pGrad > grad_thre ? 1 : 0);		// 行首元素
		pGrad = plGrad + 1;
		pInteg = plInteg + 1;
		pInteg_n1 = plInteg_n1 + 1;
		for(j = gradient->width; j > 0; j--) {
			*pInteg =  (*pGrad > grad_thre ? 1 : 0) + *(pInteg-1) + *pInteg_n1 - *(pInteg_n1-1);
			pGrad++;
			pInteg++;
			pInteg_n1++;
		}
		plGrad += gradient->widthStep;
		plInteg_n1 = plInteg;
		plInteg += gradient->width;
	}

	nRet = 1;

nExit:
	return nRet;
}

int FoldGradientFeatureInSlideWindow(RyuImage * integral, RyuImage * feature_map,
							  RyuSize sw_size, RyuSize sw_step)
{
	int nRet = 0;
	int i = 0, j = 0;
	int h_step = 0, v_step = 0;

	unsigned int * plInteg = 0, * pInteg = 0;
	unsigned int * plInteg_diag = 0, * pInteg_diag = 0;
	unsigned int * plFmap = 0, * pFmap = 0;

	if(NULL == integral || NULL == feature_map) {
		nRet = -1;
		goto nExit;
	}

	if(NULL == integral->imageData || NULL == feature_map->imageData) {
		nRet = -2;
		goto nExit;
	}

	if(RYU_DEPTH_32N != integral->depth || RYU_DEPTH_32N != feature_map->depth) {
		nRet = -3;
		goto nExit;
	}

	if(sw_size.width <= 0 || sw_size.height <= 0
			|| sw_step.width <= 0 || sw_step.height <= 0) {
		nRet = -4;
		goto nExit;
	}

	// 忽略第一行/列元素
	h_step = (integral->width - sw_size.width - 1) / sw_step.width + 1;
	v_step = (integral->height - sw_size.height - 1) / sw_step.height + 1;

	feature_map->width = h_step;
	feature_map->height = v_step;

	plInteg = (unsigned int *)integral->imageData;
	plInteg_diag = plInteg + (sw_size.height) * integral->width;
	plFmap = (unsigned int *)feature_map->imageData;
	for(i = v_step; i > 0; i--) {
		pInteg = plInteg;
		pInteg_diag = plInteg_diag;
		pFmap = plFmap;
		for(j = h_step; j > 0; j--) {
			*pFmap = pInteg_diag[sw_size.width] - pInteg_diag[0] - pInteg[sw_size.width] + pInteg[0];
			pInteg += sw_step.width;
			pInteg_diag += sw_step.width;
			pFmap++;
		}
		plInteg += sw_step.height;
		plInteg_diag += sw_step.height;
		plFmap += h_step;
	}

	nRet = 1;
nExit:
	return nRet;
}