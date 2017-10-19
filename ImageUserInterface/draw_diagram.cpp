#include "stdafx.h" 
#include <windows.h>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "ryucv.h"
#include "types.h"
#include "image_ui.h"


const int xoffset = 40;
const int yoffset = 40;
const int ruling  = 8;

IplImage * ryuDrawDiagram1D(int * arr, int arr_len, double hscl, double vscl, 
							int val_form, int diag_type, CvPoint * origin_pt)
{
	if(NULL == arr || 0 >= arr_len) {
		return 0;
	}

	int i = 0;
	int maximum = arr[0], minimum = arr[0];

	int diagW = 0, diagH_pos = 0, diagH_neg = 0;

	// 遍历数组确定最大、最小值
	for(i = 0; i < arr_len; i++) {
		maximum = RYUMAX(maximum, arr[i]);
		minimum = RYUMIN(minimum, arr[i]);
	}

	// 确定图表上下界
	if(RYU_DRAWDIAG_VALUE_ABS == val_form) {
		diagH_pos = RYUMAX(abs(maximum), abs(minimum));
		diagH_neg = 0;
	} else {
		diagH_pos = RYUMAX(0, maximum);
		diagH_neg = abs(RYUMIN(0, minimum));
	}

	// 打印信息
	printf("<Draw Diagram> maximum=%d, minimum=%d; diagram-up=%d, diagram-btm=%d\n", 
		maximum, minimum, diagH_pos, 0-diagH_neg);

	// 绘制坐标轴
	IplImage * diagram = ryuDrawDiagramFrame(arr_len, diagH_pos, diagH_neg, 
		hscl, vscl, CV_RGB(0,255,0));
	if(NULL == diagram) {
		return 0;
	}
	// Val=0即X轴位置
	int yzero = (int)(diagH_pos * vscl + 0.5) + yoffset;	// 横坐标轴位置

	// 绘制数组值
	if(RYU_DRAWDIAG_TYPE_COLUMN == diag_type) {
		for(i = 0; i < arr_len; i++) {
			int val = (RYU_DRAWDIAG_VALUE_ABS == val_form) ? abs(arr[i]) : arr[i];
			cvLine(diagram, cvPoint(xoffset+i*hscl, yzero), 
				cvPoint(xoffset+i*hscl, yzero-val*vscl), CV_RGB(255,255,255));
		}
	} else if(RYU_DRAWDIAG_TYPE_CURVE == diag_type) {
		for(i = 0; i < arr_len - 1; i++) {
			int val = (RYU_DRAWDIAG_VALUE_ABS == val_form) ? abs(arr[i]) : arr[i];
			int val1 = (RYU_DRAWDIAG_VALUE_ABS == val_form) ? abs(arr[i+1]) : arr[i+1];
			cvLine(diagram, cvPoint(xoffset+i*hscl, yzero-val*vscl), 
				cvPoint(xoffset+(i+1)*hscl, yzero-val1*vscl), CV_RGB(255,255,255));
		}
	}

	if(origin_pt) {
		origin_pt->x = xoffset;
		origin_pt->y = yzero;
	}

	return diagram;
}


IplImage * ryuDrawDiagramFrame(int xrange, int yrange_pos, int yrange_neg, 
							   double hscl, double vscl, CvScalar color)
{
	int x = (int)(xrange * hscl + 0.5);
	int yp = (int)(yrange_pos * vscl + 0.5);
	int yn = (int)(yrange_neg * vscl + 0.5);

	IplImage * diagFrame = cvCreateImage(cvSize(x+xoffset*2, yp+yn+yoffset*2), 8, 3);
	if(NULL == diagFrame)
		return 0;
	cvZero(diagFrame);

	int yzero = yp + yoffset;	// 横坐标轴位置
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8, 0.8);
	char txt[32];

	// 绘制坐标轴
	cvLine(diagFrame, cvPoint(xoffset, 0), cvPoint(xoffset, diagFrame->height-1), color);
	cvLine(diagFrame, cvPoint(xoffset, yzero), cvPoint(diagFrame->width-1, yzero), color);
	// 绘制刻度Y=0
	cvLine(diagFrame, cvPoint(xoffset-ruling, yzero), cvPoint(xoffset, yzero), color);
	txt[0] = '0';
	txt[1] = '\0';
	cvPutText(diagFrame, txt, cvPoint(0, yzero), &font, color);
	// 绘制刻度Y=yrange_pos
	if(yrange_pos) {
		cvLine(diagFrame, cvPoint(xoffset-ruling, yoffset), 
			cvPoint(xoffset+ruling, yoffset), color);
		sprintf_s(txt, "%d", yrange_pos);
		cvPutText(diagFrame, txt, cvPoint(0, yoffset), &font, color);
	}
	// 绘制刻度Y=yrange_neg
	if(yrange_neg) {
		cvLine(diagFrame, cvPoint(xoffset-ruling, diagFrame->height-yoffset), 
			cvPoint(xoffset+ruling, diagFrame->height-yoffset), color);
		sprintf_s(txt, "%d", 0-yrange_neg);
		cvPutText(diagFrame, txt, cvPoint(0, diagFrame->height-yoffset), &font, color);
	}
	// 绘制刻度X=xrange
	cvLine(diagFrame, cvPoint(diagFrame->width-xoffset, yzero-ruling), 
		cvPoint(diagFrame->width-xoffset, yzero+ruling), color);
	sprintf_s(txt, "%d", xrange);
	cvPutText(diagFrame, txt, cvPoint(diagFrame->width-xoffset, yzero+ruling*2), &font, color);

	return diagFrame;
}

