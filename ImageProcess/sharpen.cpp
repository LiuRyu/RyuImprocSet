#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"


int ryuUsmSharpening(RyuImage * src, RyuImage * dst, RyuImage * buff, 
					 int amount, int thresh, int radius)
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
	if(5 > src->width || 5 > src->height) {
		return 0;
	}

	int inbuff = 0;
	if(buff) {
		if(8 != buff->depth || 1 != buff->nChannels 
			|| src->width != buff->width
			|| src->height != buff->height) {
				return 0;
		}
	} else {
		buff = ryuCreateImage(ryuGetSize(src), 8, 1);
		if(NULL == buff) {
			return -1;
		}
		inbuff = 1;
	}

	int i = 0;
	int sub = 0;
	int nsize = src->widthStep * src->height;

	unsigned char * pIn = (unsigned char *)src->imageData;
	unsigned char * pBlur = (unsigned char *)buff->imageData;
	unsigned char * pOut = (unsigned char *)dst->imageData;

	amount = RYUMAX(0, RYUMIN(320, amount));
	thresh = RYUMAX(0, RYUMIN(255, thresh));

	switch (radius)
	{
	case (1):
		ryuGaussianBlur3x3(src, buff);

	case (2):
		ryuGaussianBlur5x5_Fast(src, buff);

	default:
		ryuGaussianBlur5x5_Fast(src, buff);
	}

	for(i = nsize; i > 0; i--) {
		sub = *pIn - *pBlur;
		if(abs(sub) > thresh) {
			sub = *pIn + ((amount * sub) >> 5);
		} else {
			sub = *pIn;
		}

		*pOut = RYUMAX(0, RYUMIN(255, sub));

		pIn++;
		pBlur++;
		pOut++;
	}

	if(inbuff)
		ryuReleaseImage(&buff);

	return 1;
}