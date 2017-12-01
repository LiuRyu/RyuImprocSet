#include "stdafx.h"

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "ryucv.h"
#include "general_utils.h"

//#define _DEBUG_GRADIENT_FEATURE_
#ifdef  _DEBUG_GRADIENT_FEATURE_
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


int GradientFeature_w3s3(IplImage * im, IplImage * gradient, int * grad_thre, float grad_thre_ratio)
{
	int nRet = 0;
	int i = 0, j = 0, nstep = 0;

	int dx = 0, dy = 0, val = 0, idx = 0, cnt = 0;

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

	nthre = RYUMAX(0, *grad_thre);

	//////////////////////////////////////////////////////////////////////////
	// 获取(w/3)*(h/3)梯度图像
	nstep = im->widthStep * 3;
	plGrad = (unsigned char *)gradient->imageData;

	plOffset_n1	= (unsigned char *)im->imageData;
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

			if(val > nthre) {
				*pGrad = val;
				hist[val]++;
				cnt++;
			} else {
				*pGrad = 0;
			}

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
	val = (int)(grad_thre_ratio * gradient->width * gradient->height);
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
	cvNamedWindow("GradFeature_w3s3_0");
	cvShowImage("GradFeature_w3s3_0", gradient);
	IplImage * Grad3C_dbg = cvCreateImage(cvGetSize(gradient), 8, 3);
	cvCvtColor(gradient, Grad3C_dbg, CV_GRAY2BGR);
	unsigned char * plGrad_dbg = (unsigned char *)Grad3C_dbg->imageData, * pGrad_dbg = 0;
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
	cvNamedWindow("GradFeature_w3s3_1");
	cvShowImage("GradFeature_w3s3_1", Grad3C_dbg);
	cvWaitKey();
	cvReleaseImage(&Grad3C_dbg);
#endif

nExit:
	return nRet;
}

int GradientFeature_filtering(IplImage * grad_in, IplImage * grad_out, 
							  int grad_thresh, int denoise_level)
{
	int nRet = 0;
	int i = 0, j = 0, n = 0, level = 0;
	int nstep = grad_in->widthStep;
	int offset[8] = {-1 - nstep, -nstep, 1 - nstep,
					 -1, 1,
					 -1 + nstep, nstep, 1 + nstep};

	unsigned char * pGrad = 0, * plGrad = 0;
	unsigned char * pGrad_out = 0, * plGrad_out = 0;

	plGrad = (unsigned char *)grad_in->imageData;
	plGrad_out = (unsigned char *)grad_out->imageData;

	memset(plGrad_out, 0, nstep * sizeof(unsigned char));

	plGrad += nstep;
	plGrad_out += nstep;

	for(i = 1; i < grad_in->height; i++) {
		pGrad = plGrad + 1;
		pGrad_out = plGrad_out + 1;
		*plGrad_out = 0;
		for(j = 1; j < grad_in->width; j++) {
			level = 0;
			if(*pGrad > grad_thresh) {
				for(n = 0; n < 8; n++) {
					level += (*(pGrad + offset[n]) > grad_thresh);
				}
			}
			*pGrad_out = (level > denoise_level) ? *pGrad : 0;
			pGrad++;
			pGrad_out++;
		}
		*pGrad_out = 0;
		plGrad += nstep;
		plGrad_out += nstep;
	}

	nRet = 1;

nExit:
	return nRet;
}


int GetGradientFeatureIntegral(IplImage * gradient, int grad_thre, IplImage * integral)
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
	
	if(IPL_DEPTH_32F != integral->depth && IPL_DEPTH_32S != integral->depth) {
		nRet = -3;
		goto nExit;
	}

	if(gradient->width != integral->width || gradient->height != integral->height) {
		nRet = -4;
		goto nExit;
	}

	// 建立阈值过滤后的梯度（幅值/个数）积分图
	plGrad	= (unsigned char *)gradient->imageData;
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

	for(i = 1; i < gradient->height; i++) {
		*plInteg = *plInteg_n1 +  (*pGrad > grad_thre ? 1 : 0);		// 行首元素
		pGrad = plGrad + 1;
		pInteg = plInteg + 1;
		pInteg_n1 = plInteg_n1 + 1;
		for(j = 1; j < gradient->width; j++) {
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

int FoldGradientFeatureInSlideWindow(IplImage * integral, IplImage * feature_map,
							  CvSize sw_size, CvSize sw_step)
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

	if(IPL_DEPTH_32F != integral->depth && IPL_DEPTH_32S != integral->depth) {
		nRet = -3;
		goto nExit;
	}

	if(IPL_DEPTH_32F != feature_map->depth && IPL_DEPTH_32S != integral->depth) {
		nRet = -4;
		goto nExit;
	}

	if(sw_size.width <= 0 || sw_size.height <= 0
			|| sw_step.width <= 0 || sw_step.height <= 0) {
		nRet = -5;
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
		plInteg += (sw_step.height * integral->width);
		plInteg_diag += (sw_step.height * integral->width);
		plFmap += h_step;
	}

	nRet = 1;

#ifdef  _DEBUG_GRADIENT_FEATURE_
	printf("[FoldGradInSlWindow]- sw_size = (%d, %d), sw_step = (%d, %d)\n", 
		sw_size.width, sw_size.height, sw_step.width, sw_step.height);
	printf("[FoldGradInSlWindow]- h_step = %d, v_step = %d\n", 
		h_step, v_step);
	IplImage * FmapUc_dbg = cvCreateImage(cvGetSize(feature_map), IPL_DEPTH_8U, 1);
	unsigned char * plFmapUc_dbg = (unsigned char *)FmapUc_dbg->imageData, * pFmapUc_dbg = 0;
	unsigned int * plFmap_dbg = (unsigned int *)feature_map->imageData, * pFmap_dbg = 0;
	int sw_total = sw_size.width * sw_size.height;
	for(int i_dbg = 0; i_dbg < FmapUc_dbg->height; i_dbg++) {
		pFmapUc_dbg = plFmapUc_dbg;
		pFmap_dbg = plFmap_dbg;
		for(int j_dbg = 0; j_dbg < FmapUc_dbg->width; j_dbg++) {
			*(pFmapUc_dbg++) = (*(pFmap_dbg++) * 255 / sw_total);
		}
		plFmapUc_dbg += FmapUc_dbg->widthStep;
		plFmap_dbg += feature_map->width;
	}
	cvNamedWindow("FoldGradInSlWindow_0");
	cvShowImage("FoldGradInSlWindow_0", FmapUc_dbg);
	cvThreshold(FmapUc_dbg, FmapUc_dbg, 64, 255, CV_THRESH_BINARY);
	cvNamedWindow("FoldGradInSlWindow_1");
	cvShowImage("FoldGradInSlWindow_1", FmapUc_dbg);
	cvWaitKey();
	cvReleaseImage(&FmapUc_dbg);
#endif

nExit:
	return nRet;
}

int FeaturePatchCoords2Arr(IplImage * feature_map, CvPoint * patch_coords, 
						   CvSize sw_size, CvSize sw_step, float thresh_ratio)
{
	int nRet = 0;
	int i = 0, j = 0;

	int nthre = (int)(sw_size.width * sw_size.height * thresh_ratio);
	int count = 0;

	unsigned int * plFmap = 0, * pFmap = 0;

	if(NULL == feature_map || NULL == patch_coords) {
		nRet = -1;
		goto nExit;
	}

	plFmap = (unsigned int *)feature_map->imageData;
	for(i = 0; i < feature_map->height; i++) {
		pFmap = plFmap;
		for(j = 0; j < feature_map->width; j++) {
			if(*pFmap > nthre) {
				patch_coords[count].x = j * sw_step.width;
				patch_coords[count].y = i * sw_step.height;
				count++;
			}
			pFmap++;
		}
		plFmap += feature_map->width;
	}

	nRet = count;
nExit:
	return nRet;
}


int WritePatchCoords2File(CvPoint * patch_coords, int patch_cnt, char * imfile_path)
{
	int nRet = 0;
	int i = 0, status = 0;

	char txtPath[MAX_PATH * 2];
	char fileName[MAX_PATH];
	char fileExt[] = ".txt";
	char childPath[] = "pacth_w64s48";
	char sp = '';

	int nLen = 0;
	FILE *pFile = NULL; 

	char wrText[32];

	if(NULL == patch_coords || NULL == imfile_path) {
		nRet = -1;
		goto nExit;
	}

	sp = getPathSeparator(imfile_path);

	status = getParentPath(imfile_path, txtPath);

	corrPathSeparator(txtPath, sp);

	status = getFileName(imfile_path, fileName, 0);
	strcat(fileName, fileExt);

	nLen = strlen(txtPath);
	if(sp != txtPath[nLen-1]) {
		txtPath[nLen] = sp;
		txtPath[nLen+1] = '\0';
	}
	strcat(txtPath, childPath);
	nLen = strlen(txtPath);
	txtPath[nLen] = sp;
	txtPath[nLen+1] = '\0';

	if(1 != isDirExist(txtPath)) {
		status = creatFlieDir(txtPath);
	}

	strcat(txtPath, fileName);

	if(1 != isFileExist(txtPath)) {
		status = creatFile(txtPath, 0);
	}

	pFile = fopen(txtPath, "a+"); 
	if(NULL == pFile) 
	{ 
		return -1; 
	} 
	sprintf(wrText, "%04x%04x", patch_coords[0].x, patch_coords[0].y); 
	fwrite(wrText, 1, strlen(wrText), pFile);

	fclose(pFile);


nExit:
	return nRet;
}