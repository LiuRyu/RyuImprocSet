#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"


int ryuCopyRoi2Image(RyuImage * src, RyuRect roi, RyuImage * dst)
{
	if(NULL == src || NULL == dst) {
		return -1;
	}

	if(roi.width != dst->width || roi.height != dst->height) {
		return 0;
	}

	RyuRect offset = ryuRect(0, 0, roi.width, roi.height);
	if(0 > roi.x) {
		offset.x -= roi.x;
		offset.width += roi.x;
	}
	if(0 > roi.y) {
		offset.y -= roi.y;
		offset.height += roi.y;
	}
	if(src->width < roi.x + roi.width)
		offset.width = src->width - roi.x + offset.x;
	if(src->height < roi.y + roi.height)
		offset.height = src->height - roi.y + offset.y;

	unsigned char * pSrc = src->imageData + (roi.y+offset.y) * src->widthStep 
		+ (roi.x+offset.x) * src->nChannels;
	unsigned char * pDst = dst->imageData + offset.y * dst->widthStep + offset.x * dst->nChannels;

	if(src->nChannels == dst->nChannels) {
		for(int i = 0; i < offset.height; i++) {
			memcpy(pDst, pSrc, offset.width * sizeof(char));
			pSrc += src->widthStep;
			pDst += dst->widthStep;
		}
	} else if(1 == src->nChannels && 3 == dst->nChannels) {
		for(int i = 0; i < offset.height; i++) {
			for(int j = 0; j < offset.width; j++) {
				pDst[j*3] = pDst[j*3+1] = pDst[j*3+2] = pSrc[j];
			}
			pSrc += src->widthStep;
			pDst += dst->widthStep;
		}
	} else if(3 == src->nChannels && 1 == dst->nChannels) {
		for(int i = 0; i < offset.height; i++) {
			for(int j = 0; j < offset.width; j++) {
				pDst[j] = BGR2GRAY(pSrc[j*3], pSrc[j*3+1], pSrc[j*3+2]);
			}
			pSrc += src->widthStep;
			pDst += dst->widthStep;
		}
	} else 
		return 0;
	return 1;
}

int ryuCopyRoi2Roi(RyuImage * src, RyuRect roi_s, RyuImage * dst, RyuRect roi_d)
{
	return 1;
}

int ryuCopyImage2Roi(RyuImage * src, RyuImage * dst, RyuRect roi)
{
	return 1;
}

int ryuCvtColor(RyuImage * src, RyuImage * dst, int cvt_mode)
{
	int nRet = 0;
	int i = 0, j = 0, v = 0;

	unsigned char * plSrc = 0, * pSrc = 0;
	unsigned char * plDst = 0, * pDst = 0;

	if(cvt_mode == RYU_BGR2GRAY) {
		if(src->width != dst->width || src->height != dst->height
				|| src->depth != RYU_DEPTH_8C || src->nChannels != 3
				|| dst->depth != RYU_DEPTH_8C || dst->nChannels != 1) {
			nRet = -1;
			goto nExit;
		}
		plSrc = src->imageData;
		plDst = dst->imageData;
		for(i = 0; i < src->height; i++) {
			pSrc = plSrc;
			pDst = plDst;
			for(j = 0; j < src->width; j++) {
				v = BGR2GRAY(pSrc[0], pSrc[1], pSrc[2]);
				*(pDst++) = ((v < 255) ? v : 255);
				pSrc += 3;
			}
			plSrc += src->widthStep;
			plDst += dst->widthStep;
		}
		nRet = 1;
	} else if(cvt_mode == RYU_GRAY2BGR) {
		if(src->width != dst->width || src->height != dst->height
			|| src->depth != RYU_DEPTH_8C || src->nChannels != 1
			|| dst->depth != RYU_DEPTH_8C || dst->nChannels != 3) {
				nRet = -1;
				goto nExit;
		}
		plSrc = src->imageData;
		plDst = dst->imageData;
		for(i = 0; i < src->height; i++) {
			pSrc = plSrc;
			pDst = plDst;
			for(j = 0; j < src->width; j++) {
				pDst[0] = pDst[1] = pDst[2] = *(pSrc++);
				pDst += 3;
			}
			plSrc += src->widthStep;
			plDst += dst->widthStep;
		}
		nRet = 1;
	} else {
		nRet = 0;
	}

nExit:
	return nRet;
}