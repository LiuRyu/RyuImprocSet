#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"


static int cast2int(RyuPoint pt)
{
	return ((pt.x << 16) | (pt.y & 0xffff));
}

static RyuPoint cast2point(int n)
{
	return ryuPoint(n >> 16, n & 0xffff);
}

int ryuFloodFill(RyuImage * im_in, RyuPoint seed, int gl_thre, int lo_diff, int up_diff, 
				 RyuImage * im_lable, int label_val, int conn_flag, RyuImage * seeds)
{
	int ret_val = 0;

	int * LabelMat = 0;

	RyuImage * tmp_seeds = 0;
	int * pSeeds = 0;

	RyuPoint CurrentPixel;
	int StackPoint = 1;
	int PixelVal = 0, PixelCnt = 0;
	int LowThresh = 0, UpThresh = 0;

	int offset = 0;
	int * pMat = 0;
	unsigned char * pIm = 0;

// 	int hist[256] = {0};
// 	int midx = 0, mcnt = 0;

	if(NULL == im_in) {
		ret_val = -1;
		goto nExit;
	}

	if(RYU_DEPTH_8C != im_in->depth || 1 != im_in->nChannels 
		|| NULL == im_in->imageData) {
			ret_val = -1;
			goto nExit;
	}

	if(NULL == im_lable) {
		ret_val = -1;
		goto nExit;
	}

	if(RYU_DEPTH_32N != im_lable->depth || 1 != im_lable->nChannels 
		|| NULL == im_lable->imageData) {
		ret_val = -1;
		goto nExit;
	}

	if(im_in->width != im_lable->width || im_in->height != im_lable->height) {
		ret_val = -1;
		goto nExit;
	}

	LabelMat = (int *)im_lable->imageData;

	if(NULL == seeds) {
		tmp_seeds = ryuCreateImage(ryuGetSize(im_in), RYU_DEPTH_32N, 1);
		if(NULL == tmp_seeds) {
			ret_val = -1;
			goto nExit;
		}
		pSeeds = (int *)tmp_seeds->imageData;
	}
	else if(im_in->width != seeds->width || im_in->height != seeds->height
		|| RYU_DEPTH_32N != seeds->depth || 1 != seeds->nChannels
		|| NULL == seeds->imageData) {
		tmp_seeds = ryuCreateImage(ryuGetSize(im_in), RYU_DEPTH_32N, 1);
		if(NULL == tmp_seeds) {
			ret_val = -1;
			goto nExit;
		}
		pSeeds = (int *)tmp_seeds->imageData;
	} else {
		pSeeds = (int *)seeds->imageData;
	}

	// 初始化种子
	pSeeds[1] = cast2int(seed);

	while(StackPoint != 0) {
		// 取出种子
		CurrentPixel = cast2point(pSeeds[StackPoint]);
		StackPoint--;

		offset = CurrentPixel.y * im_in->width + CurrentPixel.x;
		pIm = im_in->imageData + offset;
		pMat = LabelMat + offset;

// 		hist[*pIm]++;
// 		if(hist[*pIm] > mcnt) {
// 			midx = *pIm;
// 			mcnt = hist[*pIm];
// 		}
// 		LowThresh = midx - lo_diff;
// 		UpThresh = midx + up_diff;

		//将当前点（继承种子点）标定，更新阈值
		LowThresh = *pIm - lo_diff;
		UpThresh = *pIm + up_diff;
		*pMat = label_val;
		PixelCnt++;


		//判断左面的点，如果满足漫水条件，则压入堆栈
		//注意防止越界和重复记录
		if(CurrentPixel.x > 1 && *(pMat-1) == 0) {
			//取得当前指针处的像素值，判断漫水条件
			PixelVal = *(pIm - 1);
			if (PixelVal >= gl_thre && PixelVal >= LowThresh && PixelVal <= UpThresh) {
				StackPoint++;
				pSeeds[StackPoint] = cast2int(ryuPoint(CurrentPixel.x-1, CurrentPixel.y));
			}
		}

		//判断上面的点，如果满足漫水条件，则压入堆栈
		//注意防止越界和重复记录
		if(CurrentPixel.y > 1 && *(pMat-im_in->width) == 0) {
			//取得当前指针处的像素值，判断漫水条件
			PixelVal = *(pIm - im_in->width);
			if (PixelVal >= gl_thre && PixelVal >= LowThresh && PixelVal <= UpThresh) {
				StackPoint++;
				pSeeds[StackPoint] = cast2int(ryuPoint(CurrentPixel.x, CurrentPixel.y-1));
			}
		}

		//判断右面的点，如果满足漫水条件，则压入堆栈
		//注意防止越界和重复记录
		if(CurrentPixel.x < im_in->width-1 && *(pMat+1) == 0) {
			//取得当前指针处的像素值，判断漫水条件
			PixelVal = *(pIm + 1);
			if (PixelVal >= gl_thre && PixelVal >= LowThresh && PixelVal <= UpThresh) {
				StackPoint++;
				pSeeds[StackPoint] = cast2int(ryuPoint(CurrentPixel.x+1, CurrentPixel.y));
			}
		}

		//判断下面的点，如果满足漫水条件，则压入堆栈
		//注意防止越界和重复记录
		if(CurrentPixel.y < im_in->height-1 && *(pMat+im_in->width) == 0) {
			//取得当前指针处的像素值，判断漫水条件
			PixelVal = *(pIm + im_in->width);
			if (PixelVal >= gl_thre && PixelVal >= LowThresh && PixelVal <= UpThresh) {
				StackPoint++;
				pSeeds[StackPoint] = cast2int(ryuPoint(CurrentPixel.x, CurrentPixel.y+1));
			}
		}
	}

	ret_val = PixelCnt;

nExit:

	if(tmp_seeds)
		ryuReleaseImage(&tmp_seeds);

	return ret_val;
}


int ryuFloodFillWholeIm(RyuImage * im_in, RyuImage * im_lable, RyuImage * seeds,
						int gl_thre, int lo_diff, int up_diff, int conn_flag)
{
	int ret_val = 0;
	int i = 0, j = 0;

	RyuImage * tmp_seeds = 0;
	RyuImage * pSeeds = 0;

	int count = 0;
	unsigned char * pIm = 0;
	int * pLb = 0;

	int labelCount = 0;
	int status = 0;

	if(NULL == im_in) {
		ret_val = -1;
		goto nExit;
	}

	if(RYU_DEPTH_8C != im_in->depth || 1 != im_in->nChannels 
		|| NULL == im_in->imageData) {
			ret_val = -1;
			goto nExit;
	}

	if(NULL == im_lable) {
		ret_val = -1;
		goto nExit;
	}

	if(RYU_DEPTH_32N != im_lable->depth || 1 != im_lable->nChannels 
		|| NULL == im_lable->imageData) {
			ret_val = -1;
			goto nExit;
	}

	if(im_in->width != im_lable->width || im_in->height != im_lable->height) {
		ret_val = -1;
		goto nExit;
	}

	if(NULL == seeds) {
		tmp_seeds = ryuCreateImage(ryuGetSize(im_in), RYU_DEPTH_32N, 1);
		if(NULL == tmp_seeds) {
			ret_val = -1;
			goto nExit;
		}
		pSeeds = tmp_seeds;
	}
	else if(im_in->width != seeds->width || im_in->height != seeds->height
		|| RYU_DEPTH_32N != seeds->depth || 1 != seeds->nChannels
		|| NULL == seeds->imageData) {
			tmp_seeds = ryuCreateImage(ryuGetSize(im_in), RYU_DEPTH_32N, 1);
			if(NULL == tmp_seeds) {
				ret_val = -1;
				goto nExit;
			}
			pSeeds = tmp_seeds;
	} else {
		pSeeds = seeds;
	}

	ryuZero(im_lable);

	for(j = 0; j < im_in->height; j++) {
		pIm = im_in->imageData + j * im_in->widthStep;
		pLb = (int *)(im_lable->imageData + j * im_lable->widthStep);
		for(i = 0; i < im_in->width; i++) {
			if(gl_thre <= pIm[i] && 0 == pLb[i]) {
				status = ryuFloodFill(im_in, ryuPoint(i, j), gl_thre, lo_diff, up_diff, 
					im_lable, ++labelCount, 4, pSeeds);
				if(0 > status) {
					ret_val = status;
					goto nExit;
				}
			}
		}
	}

	ret_val = labelCount;

nExit:
	if(tmp_seeds)
		ryuReleaseImage(&tmp_seeds);

	return ret_val;
}