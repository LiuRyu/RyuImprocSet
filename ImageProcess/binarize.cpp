#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"


int ryuImageIntegrogram(RyuImage * im, RyuImage * out)
{
	if(NULL == im || NULL == out) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Invalid input pointer(s). [ryuImageIntegrogram]\n");
#endif
		return -1;
	}

	if(NULL == im->imageData || NULL == out->imageData) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Invalid imageData. [ryuImageIntegrogram]\n");
#endif
		return -1;
	}

// 	if(CV_8UC1 != cvGetElemType(im)) {
// #if _IMAGEPROCESSING_PRINT_
// 		printf("Error. Wrong type of im, should be CV_8UC1. [ryuImageIntegrogram]\n");
// #endif
// 		return -1;
// 	}

	if(RYU_DEPTH_8C != im->depth || 1 != im->nChannels) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Wrong type of im, should be CV_8UC1. [ryuImageIntegrogram]\n");
#endif
		return -1;
	}

// 	if(CV_32SC1 != cvGetElemType(out)) {
// #if _IMAGEPROCESSING_PRINT_
// 		printf("Error. Wrong type of out, should be CV_32SC1. [ryuImageIntegrogram]\n");
// #endif
// 		return -1;
// 	}

	if(RYU_DEPTH_32N != out->depth || 1 != out->nChannels) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Wrong type of out, should be CV_32SC1. [ryuImageIntegrogram]\n");
#endif
		return -1;
	}

	if(im->width != out->width || im->height != out->height) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Size cannot match. [ryuImageIntegrogram]\n");
#endif
		return -1;
	}

	unsigned char * pIm = 0, * pImL = (unsigned char *)im->imageData;
	unsigned int * pOut = 0, * pOutL = (unsigned int *)out->imageData;

	int i = 0, j = 0;
	// 建立首行积分图
	*pOutL = (unsigned int)(*pImL);		// 行首元素
	pIm = pImL + 1;
	pOut = pOutL + 1;
	for(i = 1; i < im->width; i++) {
		*pOut = *(pOut-1) + *pIm;
		pIm++;
		pOut++;
	}
	pImL += im->widthStep;
	pOutL += im->width;
	// 建立积分图
	for(j = 1; j < im->height; j++) {
		*pOutL = *(pOutL-im->width) + *pImL;		// 行首元素
		pIm = pImL + 1;
		pOut = pOutL + 1;
		for(i = 1; i < im->width; i++) {
			*pOut = *(pOut-im->width) + *(pOut-1) + *pIm - *(pOut-im->width-1);
			pIm++;
			pOut++;
		}
		pImL += im->widthStep;
		pOutL += im->width;
	}
	return 1;
}


static int getImrectLuminanceSum(RyuImage * integrogram, RyuRect rc)
{
	unsigned int * pData1 = (unsigned int *)integrogram->imageData + (rc.y-1) * integrogram->width + rc.x - 1;
	unsigned int * pData2 = pData1 + rc.height * integrogram->width;
	int sum = ((0 == rc.x || 0 == rc.y) ? 0 : pData1[0]) 
			+ pData2[rc.width] 
			- ((0 == rc.y) ? 0 : pData1[rc.width]) 
			- ((0 == rc.x) ? 0 : pData2[0]);

	return sum;
}


int ryuCentralLocalBinarize(RyuImage * im, RyuImage * integrogram, RyuImage * binary,
							RyuSize mask_sz, double alpha, int reverse)
{
	if(NULL == binary) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Invalid input pointer:binary. [ryuCentralLocalBinarize]\n");
#endif
		return -1;
	}

	if(NULL == binary->imageData) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Invalid imageData:binary. [ryuCentralLocalBinarize]\n");
#endif
		return -1;
	}

// 	if(CV_8UC1 != cvGetElemType(binary)) {
// #if _IMAGEPROCESSING_PRINT_
// 		printf("Error. Wrong type of binary, should be CV_8UC1. [ryuCentralLocalBinarize]\n");
// #endif
// 		return -1;
// 	}

	if(im->width != binary->width || im->height != binary->height) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Size cannot match. [ryuCentralLocalBinarize]\n");
#endif
		return -1;
	}

	if(0 >= mask_sz.width || 0 >= mask_sz.height) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Bad value of mask_sz. [ryuCentralLocalBinarize] mask_sz=(%d, %d)\n",
			mask_sz.width, mask_sz.height);
#endif
		return -1;
	}

	int status = ryuImageIntegrogram(im, integrogram);
	if(1 != status) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Unexpected return of ryuImageIntegrogram. [ryuCentralLocalBinarize]\n");
#endif
		return -1;
	}
	
	int i = 0, j = 0, thre = 0;
	unsigned char * pIm = 0, * pImL = (unsigned char *)im->imageData;
	unsigned char * pBina = 0, * pBinaL = (unsigned char *)binary->imageData;
	RyuRect rc;
	RyuPoint pt;
	RyuSize radius = ryuSize(mask_sz.width>>1, mask_sz.height>>1);

	int W = 255, B = 0;
	if(reverse) {
		W = 0;
		B = 255;
	}
	// 二值化操作
	for(j = 0; j < im->height; j++) {
		pIm = pImL;
		pBina = pBinaL;
		// 二值化阈值计算区域范围
		rc.y = j - radius.height;
		pt.y = j + radius.height;
		if(0 > rc.y)
			rc.y = 0;
		if(im->height - 1 < pt.y)
			pt.y = im->height - 1;
		rc.height = pt.y - rc.y + 1;
		for(i = 0; i < im->width; i++) {
			rc.x = i - radius.width;
			pt.x = i + radius.width;
			if(0 > rc.x)
				rc.x = 0;
			if(im->width - 1 < pt.x)
				pt.x = im->width - 1;
			rc.width = pt.x - rc.x + 1;
			thre = getImrectLuminanceSum(integrogram, rc);
			if(0 > thre) {
#if _IMAGEPROCESSING_PRINT_
				printf("Error. Unexpected return of getImrectLuminanceSum. [ryuCentralLocalBinarize]\n");
#endif
				return -1;
			}
			thre /= (rc.width * rc.height);
			thre = (int)((1.0 - alpha) * thre);
			*pBina = (*pIm < thre) ? B : W;
			pIm++;
			pBina++;
		}
		pImL += im->widthStep;
		pBinaL += im->widthStep;
	}

	return 1;
}

int ryuCentralLocalBinarizeFast(RyuImage * im, RyuImage * integrogram, RyuImage * binary,
							RyuSize mask_sz, double alpha, int reverse)
{
	if(NULL == binary) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Invalid input pointer:binary. [ryuCentralLocalBinarize]\n");
#endif
		return -1;
	}

	if(NULL == binary->imageData) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Invalid imageData:binary. [ryuCentralLocalBinarize]\n");
#endif
		return -1;
	}

// 	if(CV_8UC1 != cvGetElemType(binary)) {
// #if _IMAGEPROCESSING_PRINT_
// 		printf("Error. Wrong type of binary, should be CV_8UC1. [ryuCentralLocalBinarize]\n");
// #endif
// 		return -1;
// 	}

	if(im->width != binary->width || im->height != binary->height) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Size cannot match. [ryuCentralLocalBinarize]\n");
#endif
		return -1;
	}

	if(0 >= mask_sz.width || 0 >= mask_sz.height || mask_sz.width >= im->width) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Bad value of mask_sz. [ryuCentralLocalBinarize] mask_sz=(%d, %d)\n",
			mask_sz.width, mask_sz.height);
#endif
		return -1;
	}

	int status = ryuImageIntegrogram(im, integrogram);
	if(1 != status) {
#if _IMAGEPROCESSING_PRINT_
		printf("Error. Unexpected return of ryuImageIntegrogram. [ryuCentralLocalBinarize]\n");
#endif
		return -1;
	}

	int i = 0, j = 0, thre = 0;
	int falpha = (int)(1024 - alpha * 1024);
	unsigned char * pIm = 0, * pImL = (unsigned char *)im->imageData;
	unsigned char * pBina = 0, * pBinaL = (unsigned char *)binary->imageData;
	
	RyuSize radius = ryuSize(mask_sz.width>>1, mask_sz.height>>1);
	int rctAreas = 0, increment = radius.height;

	unsigned int * pIntr1_f = 0, * pIntr1_b = 0, * pIntrL1 = (unsigned int *)integrogram->imageData;
	unsigned int * pIntr2_f = 0, * pIntr2_b = 0, * pIntrL2 = (unsigned int *)integrogram->imageData + radius.height * im->width;

	int W = 255, B = 0;
	if(reverse) {
		W = 0;
		B = 255;
	}

	// 小高度图像特别处理
	if(im->height <= 2 * radius.height + 1) {
		RyuRect rc;
		RyuPoint pt;
		// 二值化操作
		for(j = 0; j < im->height; j++) {
			pIm = pImL;
			pBina = pBinaL;
			// 二值化阈值计算区域范围
			rc.y = j - radius.height;
			pt.y = j + radius.height;
			if(0 > rc.y)
				rc.y = 0;
			if(im->height - 1 < pt.y)
				pt.y = im->height - 1;
			rc.height = pt.y - rc.y + 1;
			for(i = 0; i < im->width; i++) {
				rc.x = i - radius.width;
				pt.x = i + radius.width;
				if(0 > rc.x)
					rc.x = 0;
				if(im->width - 1 < pt.x)
					pt.x = im->width - 1;
				rc.width = pt.x - rc.x + 1;
				thre = getImrectLuminanceSum(integrogram, rc);
				thre /= (rc.width * rc.height);
				thre = (falpha * thre) >> 10;
				*pBina = (*pIm < thre) ? B : W;
				pIm++;
				pBina++;
			}
			pImL += im->widthStep;
			pBinaL += im->widthStep;
		}
		return 1;
	}

	// Y上界区域
	for(j = 0; j <= radius.height; j++) {
		pIm = pImL;
		pBina = pBinaL;
		pIntr2_f = pIntrL2;
		pIntr2_b = pIntrL2 + radius.width;
		++increment;
		rctAreas = radius.width * increment;
		// X左界区域
		for(i = 0; i <= radius.width; i++) {
			rctAreas += increment;
			thre = *(pIntr2_b++) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
		}
		// X中间区域
		for( ; i < im->width-radius.width-1; i++) {
			thre = (*(pIntr2_b++) - *(pIntr2_f++)) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
		}
		// X右界区域
		for( ; i < im->width; i++) {
			thre = (*pIntr2_b - *(pIntr2_f++)) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
			rctAreas -= increment;
		}
		pImL += im->widthStep;
		pBinaL += im->widthStep;
		pIntrL2 += im->width;
	}
	
	// Y中间区域
	rctAreas = radius.width * increment;
	for( ; j < im->height-radius.height-1; j++) {
		pIm = pImL;
		pBina = pBinaL;
		pIntr1_f = pIntrL1;
		pIntr1_b = pIntrL1 + radius.width;
		pIntr2_f = pIntrL2;
		pIntr2_b = pIntrL2 + radius.width;
		// X左界区域
		for(i = 0; i <= radius.width; i++) {
			rctAreas += increment;
			thre = (*(pIntr2_b++) - *(pIntr1_b++)) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
		}
		// X中间区域
		for( ; i < im->width-radius.width-1; i++) {
			thre = (*(pIntr2_b++) - *(pIntr2_f++) - *(pIntr1_b++) + *(pIntr1_f++)) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
		}
		// X右界区域
		for( ; i < im->width; i++) {
			thre = (*pIntr2_b - *(pIntr2_f++) - *pIntr1_b + *(pIntr1_f++)) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
			rctAreas -= increment;
		}
		pImL += im->widthStep;
		pBinaL += im->widthStep;
		pIntrL1 += im->width;
		pIntrL2 += im->width;
	}

	// Y下界区域
	for( ; j < im->height; j++) {
		pIm = pImL;
		pBina = pBinaL;
		pIntr1_f = pIntrL1;
		pIntr1_b = pIntrL1 + radius.width;
		pIntr2_f = pIntrL2;
		pIntr2_b = pIntrL2 + radius.width;
		rctAreas = radius.width * increment;
		// X左界区域
		for(i = 0; i <= radius.width; i++) {
			rctAreas += increment;
			thre = (*(pIntr2_b++) - *(pIntr1_b++)) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
		}
		// X中间区域
		for( ; i < im->width-radius.width-1; i++) {
			thre = (*(pIntr2_b++) - *(pIntr2_f++) - *(pIntr1_b++) + *(pIntr1_f++)) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
		}
		// X右界区域
		for( ; i < im->width; i++) {
			thre = (*pIntr2_b - *(pIntr2_f++) - *pIntr1_b + *(pIntr1_f++)) / rctAreas;
			thre = (falpha * thre) >> 10;
			*(pBina++) = (*(pIm++) < thre) ? B : W;
			rctAreas -= increment;
		}
		pImL += im->widthStep;
		pBinaL += im->widthStep;
		pIntrL1 += im->width;
		increment--;
	}

	return 1;
}

