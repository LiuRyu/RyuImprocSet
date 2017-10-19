// BarcodeQualityDetect.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "image_ui.h"
#include "im_proc.h"

#include "code_qal.h"

#define _DEBUG_QUALITY_DETECT_

unsigned char gcCalibrationRegister[32];

const int nConstNormalization = 256;
const int nConstDetectDia = 200;
const int nConstDetectThk = 50;

// 固定距离内检测条码质量
int BarcodeQualityDetect(IplImage * im, CvPoint code_cent, 
						 int * contrast, int * luminance, int * definition)
{
	int ret_val = 0;
	int i = 0, sum = 0, nTmp = 0;
	int lineArr[nConstDetectDia] = {0}, lineMax = 0, lineMin = 255;

	unsigned char * pIm = 0;

	const float low_ratio = 0.15, high_ratio = 0.15;
	int low_scale, high_scale, min_scale, max_scale, avg_scale, mid_scale, grav_scale;

	const int bwDiffThresh = 36;
	int bwThresh = 0, bCount = 0, wCount = 0;
	int bwPrevious = 0, bwCurrent = 0, bwSwicth = -1;

	int gradState = 0, currState = 0, gradMax = 0, gradIdx = 0, gradThresh = 0, gradEffc = 0;
	int gradArr[nConstDetectDia] = {0};

	if(NULL == im) {
		ret_val = -1;
		goto nExit;
	}

	if(0 > code_cent.x - nConstDetectDia || im->width < code_cent.x + nConstDetectDia
		|| 0 > code_cent.y - nConstDetectThk || im->height < code_cent.y + nConstDetectThk) {
		ret_val = -1;
		goto nExit;
	}

	pIm = (unsigned char *)im->imageData 
		+ (code_cent.y-(nConstDetectThk>>1)) * im->widthStep + code_cent.x - (nConstDetectDia>>1);
	ryuImageContrastAnalyze(pIm, nConstDetectDia, nConstDetectThk, im->widthStep, 0,
		low_ratio, &low_scale, high_ratio, &high_scale, 
		&min_scale, &max_scale, &avg_scale, &mid_scale, &grav_scale);

	*contrast = high_scale - low_scale;
	*luminance = (high_scale + low_scale) >> 1;

#ifdef _DEBUG_QUALITY_DETECT_
	printf("条码图像分析：low_scale = %d, high_scale = %d, min_scale = %d, max_scale = %d\n", 
		low_scale, high_scale, min_scale, max_scale);
	printf("             avg_scale = %d, mid_scale = %d, grav_scale = %d\n",
		avg_scale, mid_scale, grav_scale);
	printf("条码亮度：%d, 条码对比度：%d\n", *luminance, *contrast);

	IplImage * im_area = cvCreateImage(cvSize(nConstDetectDia, nConstDetectThk), 8, 1);
	char * pIm_dbg = im->imageData + (code_cent.y-(nConstDetectThk>>1)) * im->widthStep 
		+ code_cent.x - (nConstDetectDia>>1);
	for(int iid = 0; iid < nConstDetectThk; iid++) {
		memcpy(im_area->imageData+iid*im_area->widthStep, pIm_dbg, nConstDetectDia * sizeof(char));
		pIm_dbg += im->widthStep;
	}
	cvNamedWindow("im_area");
	cvShowImage("im_area", im_area);
	cvReleaseImage(&im_area);
#endif

	bwThresh = (high_scale + low_scale) >> 1;
	sum = bCount = wCount = 0;
	pIm = (unsigned char *)im->imageData 
		+ code_cent.y * im->widthStep + code_cent.x - (nConstDetectDia>>1);
	for(i = 0; i < nConstDetectDia; i++) {
 		lineArr[i] = (int)pIm[i];
		lineMax = RYUMAX(lineMax, lineArr[i]);
		lineMin = RYUMIN(lineMin, lineArr[i]);

		sum += (lineArr[i]-bwThresh) * (lineArr[i]-bwThresh);

		gradArr[i] = (int)pIm[i] - (int)pIm[i+1];
	}

	sum = (int)(sqrt(1.0 * sum / nConstDetectDia) + 0.5);

	gradThresh = (high_scale - low_scale) >> 2;
	nTmp = RYUMAX(10, gradThresh);

#ifdef _DEBUG_QUALITY_DETECT_
	CvPoint ptOrigin;
	IplImage * diagram = ryuDrawDiagram1D(lineArr, nConstDetectDia, 1.0, 1.0, 
		RYU_DRAWDIAG_VALUE_ORIN, RYU_DRAWDIAG_TYPE_COLUMN, &ptOrigin);
	cvLine(diagram, cvPoint(0, ptOrigin.y - bwThresh), 
		cvPoint(diagram->width, ptOrigin.y - bwThresh), CV_RGB(255, 0, 0));
	cvNamedWindow("diagram");
	cvShowImage("diagram", diagram);
	cvReleaseImage(&diagram);

	diagram = ryuDrawDiagram1D(gradArr, nConstDetectDia, 1.0, 1.0, 
		RYU_DRAWDIAG_VALUE_ORIN, RYU_DRAWDIAG_TYPE_COLUMN, &ptOrigin);

	cvLine(diagram, cvPoint(0, ptOrigin.y - nTmp), 
		cvPoint(diagram->width, ptOrigin.y - nTmp), CV_RGB(255, 0, 0));
	cvLine(diagram, cvPoint(0, ptOrigin.y + nTmp), 
		cvPoint(diagram->width, ptOrigin.y + nTmp), CV_RGB(255, 0, 0));
	cvNamedWindow("diagram2");
	cvShowImage("diagram2", diagram);
	cvReleaseImage(&diagram);
#endif

	// 寻找梯度数组每一个正负区间的最大值
	gradState = (gradArr[0] == 0) ? 0 : (gradArr[0] / abs(gradArr[0]));
	gradMax = abs(gradArr[0]);
	gradIdx = 0;
	gradArr[0] = (gradArr[0] > 0) ? (0 - gradArr[0]) : gradArr[0];
	for(i = 1; i < nConstDetectDia-1; i++) {
		currState = (gradArr[i] == 0) ? 0 : (gradArr[i] / abs(gradArr[i]));
		if(currState != gradState) {
			if(gradMax >= nTmp) {
				gradArr[gradIdx] = 0 - gradArr[gradIdx];
				gradEffc++;
			}
			gradState = currState;
			gradMax = abs(gradArr[i]);
			gradIdx = i;
		} else {
			if(gradMax < abs(gradArr[i])) {
				gradMax = abs(gradArr[i]);
				gradIdx = i;
			}
		}
		gradArr[i] = (gradArr[i] > 0) ? (0 - gradArr[i]) : gradArr[i];
	}
	// 最后一个区间处理
	if(gradMax >= nTmp) {
		gradArr[gradIdx] = 0 - gradArr[gradIdx];
		gradEffc++;
	}

	int sum1 = 0, sum2 = 0;
	for(i = 0; i < nConstDetectDia-1; i++) {
		if(gradArr[i] > 0) {
			sum1 += gradArr[i];
		} else {
			sum2 -= gradArr[i];
		}
	}
	
	if(sum1 > 0 && gradEffc > 0)	
		sum1 /= gradEffc;
	if(sum2 > 0 && gradEffc < nConstDetectDia - 1)
		sum2 /= (nConstDetectDia - 1 - gradEffc);

	nTmp = *definition = (sum1 - sum2 + sum + gradThresh) * 255 / 446;

#ifdef _DEBUG_QUALITY_DETECT_
	printf("条码清晰度分析: 均差 = %d, gradThresh = %d, gradEffc = %d, sum1 = %d, sum2 = %d\n", 
		sum, gradThresh, gradEffc, sum1, sum2);
	diagram = ryuDrawDiagram1D(gradArr, nConstDetectDia, 1.0, 1.0, 
		RYU_DRAWDIAG_VALUE_ORIN, RYU_DRAWDIAG_TYPE_COLUMN, &ptOrigin);
	cvNamedWindow("diagram3");
	cvShowImage("diagram3", diagram);
	//cvWaitKey();
	cvReleaseImage(&diagram);
#endif

	// 检查计算最佳质量
	gcCalibrationRegister[0] = RYUMAX(gcCalibrationRegister[0], nTmp);

	// 将当前值加入"队列"，更新"队列"
	for(i = 30; i > 0; i--) {
		gcCalibrationRegister[i] = gcCalibrationRegister[i-1];
	}
	gcCalibrationRegister[1] = nTmp;

	// 计算条码质量变化趋势
	sum = nTmp = 0;
	for(i = 1; i < 11; i++) {
		sum += (gcCalibrationRegister[i] - gcCalibrationRegister[i+1]);
		if(5 < abs(sum)) {
			nTmp = 1;
			break;
		}
	}
	if(nTmp) {
		gcCalibrationRegister[31] = (sum > 0) ? 0x1 : 0xff;
	}
	nTmp = (0xff == gcCalibrationRegister[31]) ? -1 : 1;

#ifdef _DEBUG_QUALITY_DETECT_
	printf("条码质量：%d, 当前标定最佳质量：%d, 当前质量改变方向：%d\n", 
		*definition, gcCalibrationRegister[0], nTmp);
	cvWaitKey();
#endif

nExit:
	return ret_val;
}

void BarcodeDetect_calibration_reset()
{
	memset(gcCalibrationRegister, 0, 32 * sizeof(char));
	gcCalibrationRegister[31] = 0x1;
}

