#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "core.h"
#include "improc.h"


int ryuImageContrastAnalyze(unsigned char * img, int width, int height, int widthstep, int * hist, 
						float low_ratio, int * low_scale, float high_ratio, int * high_scale, 
						int * min_scale, int * max_scale, 
						int * avg_scale, int * mid_scale, int * grav_scale)
{
	int gHistNorm_hist[256] = {0};
	//int * hist = gHistNorm_hist;

	int i = 0, j = 0;
	int index = 0;
	int temp = 0, temp2 = 0;
	int size = height * width;

	int lowThresh = (int)(size * low_ratio);
	int highThresh = (int)(size * high_ratio);

	long sum = 0;

	unsigned char * pImg = 0, * pImgL = img;
	int * pHist = 0;

	if(NULL == hist)
		hist = gHistNorm_hist;
	else
		memset(hist, 0, 256 * sizeof(int));

	// 统计图像亮度直方图
	for(j = 0; j < height; j++) {
		pImg = pImgL;
		for(i = 0; i < width; i++) {
			hist[*pImg]++;
			sum += *pImg;
			pImg++;
		}
		pImgL += widthstep;
	}

	if(avg_scale) {
		*avg_scale = (int)(1.0 * sum / size + 0.5);
	}

	if(min_scale) {
		pHist = hist;
		for(i = 0; i < 256; i++) {
			if(*pHist > 0) {
				index = i;
				break;
			}
			pHist++;
		}
		*min_scale = index;
	}

	if(max_scale) {
		pHist = hist + 255;
		for(i = 255; i >= 0; i--) {
			if(*pHist > 0) {
				index = i;
				break;
			}
			pHist--;
		}
		*max_scale = index;
	}

	if(low_scale && lowThresh > 0) {
		index = temp = 0;
		pHist = hist;
		for(i = 0; i < 256; i++) {
			temp += (*pHist);
			pHist++;
			if(temp >= lowThresh) {
				index = i;
				break;
			}
		}
		*low_scale = index;
	}

	if(high_scale && highThresh > 0) {
		index = temp = 0;
		pHist = hist + 255;
		for(i = 255; i >= 0; i--) {
			temp += (*pHist);
			pHist--;
			if(temp >= highThresh) {
				index = i;
				break;
			}
		}
		*high_scale = index;
	}

	if(mid_scale) {
		index = temp = 0;
		pHist = hist;
		for(i = 0; i < 256; i++) {
			temp += (*pHist);
			pHist++;
			if(temp >= (size>>1)) {
				index = i;
				break;
			}
		}
		*mid_scale = index;
	}

	if(grav_scale) {
		temp = temp2 = 0;
		pHist = hist + 255;
		for(i = 255; i >= 0; i--) {
			temp += (*pHist);
			temp2 += (*pHist) * i;
			pHist--;
		}

		if(temp > 0) {
			*grav_scale = temp2 / temp;
		} else {
			*grav_scale = 0;
		}
	}

	return 1;
}


int ryuAutoContrast(RyuImage * in, RyuImage * out, float thre_ratio)
{
	int hist[256] = {0};
	int eqHist[256] = {0};

	int ret_val = 0;
	int i = 0, j = 0;
	int l_index = 0, r_index = 255;
	int temp = 0, temp2 = 0, temp3 = 0, m = 0, n = 0;

	int threshold = in->width * in->height * thre_ratio;

	float avg = 0, acc = 0;

	unsigned char * pImg = 0, * pImgL = 0;
	unsigned char * pNorm = 0, * pNormL = 0;
	int * pHist = 0, * pEqHist = 0;

	pImgL = in->imageData;
	for(i = 0; i < in->height; i++) {
		pImg = pImgL;
		for(j = 0; j < in->width; j++) {
			hist[*pImg]++;
			pImg++;
		}
		pImgL += in->widthStep;
	}

	// 由于0一般都是旋转填充引起的，所以去掉0的累积
	temp = 0;
	pHist = hist;
	for(i = 0; i < 256; i++) {
		temp += (*pHist);
		pHist++;
		if(temp >= threshold) {
			l_index = i;
			break;
		}
	}

	// 由于255一般都是反光引起的，所以去掉255的累积
	temp = 0;
	pHist = hist + 254;
	for(i = 255; i > 0; i--) {
		temp += (*pHist);
		pHist--;
		if(temp >= threshold) {
			r_index = i - 1;
			break;
		}
	}

	m = r_index - l_index;
	n = 255 - m;

	if(m < 0 || n < 0) {
		ret_val = -1;
		goto nExit;
	}

	if(m <= 0 || n <= 0) {
		if(in == out) {
			ret_val = 1;
			goto nExit;
		}
		else {
			memcpy(out, in, in->widthStep * in->height * sizeof(unsigned char));
			ret_val = 1;
			goto nExit;
		}
	}

	pEqHist = eqHist;
	for(i = l_index + 1; i > 0; i--) {
		*(pEqHist++) = 0;
	}

	pEqHist = eqHist + 255;
	for(i = 255 - r_index + 1; i > 0; i--) {
		*(pEqHist--) = 255;
	}

	if(m < n) {
		avg = n * 1.0 / m;
		acc = 1;
		pEqHist = eqHist + l_index + 1;
		for(i = m - 1; i > 0; i--) {
			acc = acc + avg;
			*(pEqHist++) = (int)(acc + 0.5);
			acc = acc + 1;
		}
	} else {
		avg = m * 1.0 / n;
		acc = avg / 2;
		temp = temp2 = 0;
		temp3 = 1;
		pEqHist = eqHist + l_index + 1;
		for(i = n; i > 0; i--) {
			temp = (int)(acc + 0.5);
			temp -= temp2;
			for(j = temp; j > 0; j--) {
				*(pEqHist++) = (temp3++);
			}
			temp2 += temp;
			temp3++;
			acc = acc + avg;
		}

		for(i = m - temp2; i > 0; i--) {
			*(pEqHist++) = (temp3++);
		}
	}

	pImgL = in->imageData;
	pNormL = out->imageData;
	//进行灰度映射均衡化
	for(i = 0; i < in->height; i++) {
		pImg = pImgL;
		pNorm = pNormL;
		for(j = 0; j < in->width; j++) {
			*pNorm = eqHist[*pImg];
			pNorm++;
			pImg++;
		}
		pImgL += in->widthStep;
		pNormL += out->widthStep;
	}

	ret_val = 1;

nExit:
	return ret_val;
}