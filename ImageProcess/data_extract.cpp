#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"


// 提取图像中一线上的图像数据存入数组
int ryuExtractImdataLine_8UC1(RyuImage * im, RyuPoint pt0, RyuPoint pt1, int * data)
{
	if(NULL == im || NULL == data)
		return -1;
	if(0 >= im->width || 0 > im->height 
		|| 8 != im->depth || 1 != im->nChannels || NULL == im->imageData)
		return -1;

	int nstep = 0, i = 0;
	double dx, dy;
	double cx, cy;
	int nx = 0, ny = 0;
	unsigned char * pD = 0;

	if(abs(pt0.x - pt1.x) >= abs(pt0.y - pt1.y)) {
		nstep = abs(pt0.x - pt1.x) + 1;
		dx = (0 < pt1.x - pt0.x) ? 1.0 : -1.0;
		dy = 1.0 * abs(pt0.y - pt1.y) / abs(pt0.x - pt1.x);
		dy = (0 <= pt1.y - pt0.y) ? dy : -dy;
	} else {
		nstep = abs(pt0.y - pt1.y) + 1;
		dy = (0 < pt1.y - pt0.y) ? 1.0 : -1.0;
		dx = 1.0 * abs(pt0.x - pt1.x) / abs(pt0.y - pt1.y);
		dx = (0 <= pt1.x - pt0.x) ? dx : -dx;
	}
	if(0 >= nstep)
		return 0;

	cx = 1.0 * pt0.x;
	cy = 1.0 * pt0.y;
	for(i = 0; i < nstep; i++) {
		nx = (int)(cx + 0.5);
		ny = (int)(cy + 0.5);
		if(0 > nx || im->width <= nx || 0 > ny || im->height <= ny) {
			data[i] = -1;
		} else {
			pD = (unsigned char *)im->imageData + ny * im->widthStep + nx;
			data[i] = (int)*pD;
		}
		cx += dx;
		cy += dy;
	}

	return nstep;
}