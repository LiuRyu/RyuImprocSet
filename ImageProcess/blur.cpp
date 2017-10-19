#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"

int ryuGaussianBlur3x3(RyuImage * src, RyuImage * dst)
{
	if(NULL == src || NULL == dst) {
		return -1;
	}
	if(8 != src->depth || 8 != dst->depth) {
		return 0;
	}
	if(1 != src->nChannels || 1 != dst->nChannels) {
		return 0;
	}
	if(src->width != dst->width || src->height != dst->height) {
		return 0;
	}

	int nh = src->height - 2; 
	int nw = src->width - 2;
	if(0 >= nh || 0 >= nw) {
		return 0;
	}

	int i = 0, j = 0;
	int t = 0;

	unsigned char * pIn = (unsigned char *)src->imageData;
	unsigned char * pOut = (unsigned char *)dst->imageData;
	unsigned char * lOut = 0;

	unsigned char * loffset, * loffset_t, * loffset_b;
	unsigned char * poffset, * poffset_t, * poffset_b;

	loffset = loffset_t = loffset_b = 0;
	poffset = poffset_t = poffset_b = 0;

	// 首行处理
	memcpy(pOut, pIn, sizeof(unsigned char) * src->widthStep);

	loffset		= pIn + 1;
	loffset_t	= pIn - src->widthStep + 1;
	loffset_b	= pIn + src->widthStep + 1;
	lOut		= pOut + 1;

	for(i = nh; i > 0; i--)	
	{
		loffset		+= src->widthStep;
		loffset_t	+= src->widthStep;
		loffset_b	+= src->widthStep;
		poffset		= loffset;
		poffset_t	= loffset_t;
		poffset_b	= loffset_b;

		lOut += dst->widthStep;
		pOut = lOut;

		for(j = nw; j > 0; j--)
		{
			t = (*(poffset_t-1))	+ ((*poffset_t)<<1)	+ (*(poffset_t+1))
				+ ((*(poffset-1))<<1)	+ ((*poffset)<<2)	+ ((*(poffset+1))<<1)
				+ (*(poffset_b-1))	+ ((*poffset_b)<<1)	+ (*(poffset_b+1));

			*pOut = (t>>4);

			poffset++;
			poffset_t++;
			poffset_b++;
			pOut++;
		}

		*(lOut-1) = *lOut;	// 首元素处理
		*pOut = *(pOut-1);	// 尾元素处理
	}

	// 末行处理
	memcpy(pOut+1, poffset+1, sizeof(unsigned char) * src->width);

	return 1;
}


int ryuGaussianBlur5x5(RyuImage * src, RyuImage * dst)
{
	if(NULL == src || NULL == dst) {
		return -1;
	}
	if(8 != src->depth || 8 != dst->depth) {
		return 0;
	}
	if(1 != src->nChannels || 1 != dst->nChannels) {
		return 0;
	}
	if(src->width != dst->width || src->height != dst->height) {
		return 0;
	}

	int nh = src->height - 4; 
	int nw = src->width - 4;
	if(0 >= nh || 0 >= nw) {
		return 0;
	}

	int i = 0, j = 0;
	int t = 0;

	unsigned char * pIn = (unsigned char *)src->imageData;
	unsigned char * pOut = (unsigned char *)dst->imageData;
	unsigned char * lOut = 0;

	unsigned char * loffset, * loffset_t, * loffset_b, * loffset_t2, * loffset_b2;
	unsigned char * poffset, * poffset_t, * poffset_b, * poffset_t2, * poffset_b2;

	loffset = loffset_t = loffset_b = loffset_t2 = loffset_b2 = 0;
	poffset = poffset_t = poffset_b = poffset_t2 = poffset_b2 = 0;

	// 首行处理
	memcpy(pOut, pIn, sizeof(unsigned char) * (src->widthStep<<1));

	loffset_t2	= pIn + 2;
	loffset_t	= loffset_t2 + src->widthStep;
	loffset		= loffset_t + src->widthStep;
	loffset_b	= loffset + src->widthStep;
	loffset_b2	= loffset_b + src->widthStep;
	lOut		= pOut + (dst->widthStep<<1) + 2;

	for(i = nh; i > 0; i--)	
	{
		poffset_t2  = loffset_t2;
		poffset_t	= loffset_t;
		poffset		= loffset;
		poffset_b	= loffset_b;
		poffset_b2	= loffset_b2;

		loffset_t2	+= src->widthStep;
		loffset_t	+= src->widthStep;
		loffset		+= src->widthStep;
		loffset_b	+= src->widthStep;
		loffset_b2	+= src->widthStep;

		pOut = lOut;
		lOut += dst->widthStep;

		for(j = nw; j > 0; j--)
		{
			t = (*(poffset_t2-2))		+ ((*(poffset_t2-1))<<2) + ((*(poffset_t2))*7) + ((*(poffset_t2+1))<<2) + (*(poffset_t2+2))
				+ ((*(poffset_t-2))<<2)	+ ((*(poffset_t-1))<<4)	 + ((*(poffset_t))*26) + ((*(poffset_t+1))<<4)	+ ((*(poffset_t+2))<<2)
				+ ((*(poffset-2))*7)	+ ((*(poffset-1))*26)	 + ((*(poffset))*41)   + ((*(poffset+1))*26)	+ ((*(poffset+2))*7)
				+ ((*(poffset_b-2))<<2)	+ ((*(poffset_b-1))<<4)	 + ((*(poffset_b))*26) + ((*(poffset_b+1))<<4)	+ ((*(poffset_b+2))<<2)
				+ (*(poffset_b2-2))		+ ((*(poffset_b2-1))<<2) + ((*(poffset_b2))*7) + ((*(poffset_b2+1))<<2) + (*(poffset_b2+2));

			*pOut = t / 273;

			poffset_t2++;
			poffset_t++;
			poffset++;
			poffset_b++;
			poffset_b2++;
			pOut++;
		}

		*(lOut-1) = *lOut;		// 首元素处理
		*(lOut-2) = *lOut;		// 首元素处理
		*pOut = *(pOut-1);		// 尾元素处理
		*(pOut+1) = *(pOut-1);	// 尾元素处理
	}

	// 末行处理
	memcpy(pOut+2, poffset+2, sizeof(unsigned char) * (src->widthStep<<1));

	return 0;
}


int ryuGaussianBlur5x5_Fast(RyuImage * src, RyuImage * dst)
{
	if(NULL == src || NULL == dst) {
		return -1;
	}
	if(8 != src->depth || 8 != dst->depth) {
		return 0;
	}
	if(1 != src->nChannels || 1 != dst->nChannels) {
		return 0;
	}
	if(src->width != dst->width || src->height != dst->height) {
		return 0;
	}

	int nh = src->height - 4; 
	int nw = src->width - 4;
	if(0 >= nh || 0 >= nw) {
		return 0;
	}

	int i = 0, j = 0;
	int t = 0;

	unsigned char * pIn = (unsigned char *)src->imageData;
	unsigned char * pOut = (unsigned char *)dst->imageData;
	unsigned char * lOut = 0;

	unsigned char * loffset, * loffset_t, * loffset_b, * loffset_t2, * loffset_b2;
	unsigned char * poffset, * poffset_t, * poffset_b, * poffset_t2, * poffset_b2;

	loffset = loffset_t = loffset_b = loffset_t2 = loffset_b2 = 0;
	poffset = poffset_t = poffset_b = poffset_t2 = poffset_b2 = 0;

	// 首行处理
	memcpy(pOut, pIn, sizeof(unsigned char) * (src->widthStep<<1));

	loffset_t2	= pIn + 2;
	loffset_t	= loffset_t2 + src->widthStep;
	loffset		= loffset_t + src->widthStep;
	loffset_b	= loffset + src->widthStep;
	loffset_b2	= loffset_b + src->widthStep;
	lOut		= pOut + (dst->widthStep<<1) + 2;

	for(i = nh; i > 0; i--)	
	{
		poffset_t2  = loffset_t2;
		poffset_t	= loffset_t;
		poffset		= loffset;
		poffset_b	= loffset_b;
		poffset_b2	= loffset_b2;

		loffset_t2	+= src->widthStep;
		loffset_t	+= src->widthStep;
		loffset		+= src->widthStep;
		loffset_b	+= src->widthStep;
		loffset_b2	+= src->widthStep;

		pOut = lOut;
		lOut += src->widthStep;

		for(j = nw; j > 0; j--)
		{
			t = (*(poffset_t2-2))		+ ((*(poffset_t2-1))<<2) + ((*(poffset_t2))<<3) + ((*(poffset_t2+1))<<2) + (*(poffset_t2+2))
				+ ((*(poffset_t-2))<<2)	+ ((*(poffset_t-1))<<4)	 + ((*(poffset_t))<<5) + ((*(poffset_t+1))<<4)	+ ((*(poffset_t+2))<<2)
				+ ((*(poffset-2))<<3)	+ ((*(poffset-1))<<5)	 + ((*(poffset))<<6)   + ((*(poffset+1))<<5)	+ ((*(poffset+2))<<3)
				+ ((*(poffset_b-2))<<2)	+ ((*(poffset_b-1))<<4)	 + ((*(poffset_b))<<5) + ((*(poffset_b+1))<<4)	+ ((*(poffset_b+2))<<2)
				+ (*(poffset_b2-2))		+ ((*(poffset_b2-1))<<2) + ((*(poffset_b2))<<3) + ((*(poffset_b2+1))<<2) + (*(poffset_b2+2));

			*pOut = t / 324;

			poffset_t2++;
			poffset_t++;
			poffset++;
			poffset_b++;
			poffset_b2++;
			pOut++;
		}

		*(lOut-1) = *lOut;		// 首元素处理
		*(lOut-2) = *lOut;		// 首元素处理
		*pOut = *(pOut-1);		// 尾元素处理
		*(pOut+1) = *(pOut-1);	// 尾元素处理
	}

	// 末行处理
	memcpy(pOut+2, poffset+2, sizeof(unsigned char) * (src->widthStep<<1));

	return 0;
}

