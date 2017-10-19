#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"


int ryuImageGradient(RyuImage * im, RyuImage * grad, RyuImage * orie, int thresh)
{
	int ret_val = 0;

	int i = 0, j = 0;
	int dx = 0, dy = 0;
	int val = 0;

	unsigned char * pOrie	= 0;
	unsigned char * pGrad	= 0;

	unsigned char * lOrie	= 0;
	unsigned char * lGrad	= 0;

	unsigned char * loffset_1, * loffset_2, * loffset_3, * loffset_4;
	unsigned char * poffset_1, * poffset_2, * poffset_3, * poffset_4;

	if(NULL == im) {
		ret_val = -1;
		goto nExit;
	}
	if(NULL == im->imageData || RYU_DEPTH_8C != im->depth || 1 != im->nChannels) {
		ret_val = -1;
		goto nExit;
	}

	if(NULL == grad) {
		ret_val = -1;
		goto nExit;
	}
	if(NULL == grad->imageData || RYU_DEPTH_8C != grad->depth || 1 != grad->nChannels) {
		ret_val = -1;
		goto nExit;
	}
	if(im->width != grad->width || im->height != grad->height) {
		ret_val = -1;
		goto nExit;
	}

	if(NULL == orie) {
		ret_val = -1;
		goto nExit;
	}
	if(NULL == orie->imageData || RYU_DEPTH_8C != orie->depth || 1 != orie->nChannels) {
		ret_val = -1;
		goto nExit;
	}
	if(im->width != orie->width || im->height != orie->height) {
		ret_val = -1;
		goto nExit;
	}

	loffset_1 = loffset_2 = loffset_3 = 0;
	poffset_1 = poffset_2 = poffset_3 = 0;

	lOrie	= (unsigned char *)orie->imageData;
	lGrad	= (unsigned char *)grad->imageData;

	loffset_1	= (unsigned char *)im->imageData;
	loffset_2	= loffset_1 + im->widthStep;
	loffset_3	= loffset_2 + im->widthStep;

	// 首行处理
	memset(lOrie, 0, im->width * sizeof(unsigned char));
	memset(lGrad, 0, im->width * sizeof(unsigned char));
	lOrie += orie->widthStep;
	lGrad += grad->widthStep;

	// 中间行处理
	for(i = im->height-2; i > 0; i--) {
		*lGrad = *lOrie = 0;

		pGrad = lGrad + 1;
		pOrie = lOrie + 1;
		poffset_1 = loffset_1;
		poffset_2 = loffset_2;
		poffset_3 = loffset_3;

		for(j = im->width-2; j > 0; j--) {
			dx = poffset_2[2] - poffset_2[0];
			dy = poffset_3[1] - poffset_1[1];

			val = RYUMAX(abs(dx), abs(dy));
			*pGrad = val < thresh ? 0 : val;
			*pOrie = *pGrad ? (76 + ryuAtan180_10SH(dy, dx)) : 0;

			poffset_1++;
			poffset_2++;
			poffset_3++;
			pGrad++;
			pOrie++;
		}
		lOrie += orie->widthStep;
		lGrad += grad->widthStep;
		loffset_1 += im->widthStep;
		loffset_2 += im->widthStep;
		loffset_3 += im->widthStep;
	}

	// 末行处理
	memset(lOrie, 0, im->width * sizeof(unsigned char));
	memset(lGrad, 0, im->width * sizeof(unsigned char));

	ret_val = 1;

nExit:

	return ret_val;
}