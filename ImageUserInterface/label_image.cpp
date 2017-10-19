#include "stdafx.h" 
#include <windows.h>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <opencv2/opencv.hpp>

#include "types.h"

#include "general_utils.h"
#include "ryucv.h"
#include "image_ui.h"



CvPoint mousePos;
CvPoint clickPos[4], orPt[4], shPt[4];
CvSize  rectSz;
float rectAg;
int clickTm = 0;

static void my_mouse_callback(int event, int x, int y, int flags, void* param);
static void draw_rect(IplImage* img);

static CvPoint rsz2orig(CvPoint rsz, double hscl, double vscl);
static CvPoint orig2rsz(CvPoint orig, double hscl, double vscl);

static void init_label();
static int label_point(double hscl, double vscl);
static int label_line(double hscl, double vscl);
static int label_rect(double hscl, double vscl);
static int label_fixed(CvSize im_sz, double hscl, double vscl, int mask_w, int mask_h);
static int label_rotate(double hscl, double vscl);
static int label_crosspoint(CvSize show_sz, double hscl, double vscl);


/*********************************************************************
* 函数名称: CvSize ryuSizeFitScreen(CvSize sz, double hscale, double vscale)
* 说明:获取日志文件路径
* 输入参数:
* 
* 输出参数：
* 
* 返回值:
* 
* 作者: Ryu L.
* 时间 : 2017-7-5
*********************************************************************/ 
CvSize ryuSizeFitScreen(CvSize sz, double &hscale, double &vscale, double s2bratio)
{
	if(0 >= sz.width || 0 >= sz.height) {
#if _LABEL_IM_SAMPLES_PRINT_
		printf("Error! Invalid value. [SizeFitScreen]. sz.width=%d, sz.height=%d\n", 
			sz.width, sz.height);
#endif
		return (cvSize(-1, -1));
	}

	int scW = (int)(s2bratio * GetSystemMetrics(SM_CXFULLSCREEN));
	int scH = (int)(s2bratio * GetSystemMetrics(SM_CYFULLSCREEN));

	if(0 >= scW || 0 >= scH) {
#if _LABEL_IM_SAMPLES_PRINT_
		printf("Error! Bad value. [SizeFitScreen]. screen.width=%d, screen.height=%d\n", 
			scW, scH);
#endif
		return (cvSize(-3, -3));
	}

	int rW = (int)(sz.width * (hscale));
	int rH = (int)(sz.height * (vscale));

	double dfRW = 0.0, dfRH = 0.0;

	// 给入缩放尺度合法且小于屏幕尺寸
	if(0 < rW && 0 < rH && rW <= scW && rH <= scH)
		return (cvSize(rW, rH));
	// 使用小于屏幕尺寸的原始图像尺寸
	else if(sz.width <= scW && sz.height <= scH) {
		hscale = vscale = 1.0;
		return (cvSize(sz.width, sz.height));
	}
	// 将图像调整至屏幕适应尺寸
	else {
		dfRW = sz.width * 1.0 / scW;
		dfRH = sz.height * 1.0 / scH;
		if(dfRW >= dfRH) {
			rW = scW;
			rH = (int)(sz.height * 1.0 / dfRW);
		} else {
			rH = scH;
			rW = (int)(sz.width * 1.0 / dfRH);
		}
		hscale = rW * 1.0 / sz.width;
		vscale = rH * 1.0 / sz.height;
		return (cvSize(rW, rH));
	}

	return (cvSize(0, 0));
}

/*********************************************************************
* 函数名称: int ryuGetPointLum(IplImage* im, CvPoint pt)
* 说明: 获取图像指定坐标像素值(亮度)
* 输入参数:
* 
* 输出参数：
* 
* 返回值:
* 
* 作者: Ryu L.
* 时间 : 2017-7-5
*********************************************************************/ 
int ryuGetPointLum(IplImage* im, CvPoint pt)
{
	if(NULL == im) {
#if _LABEL_IM_SAMPLES_PRINT_
		printf("Error! Null pointer. [GetPointLum]. im=0x%x, lum=0x%x\n", im);
#endif
		return -1;
	}

	if(0 > pt.x || im->width <= pt.x 
		|| 0 > pt.y || im->height <= pt.y) {
		return 0;
	}

	unsigned char * ptrImg = (unsigned char *)im->imageData 
		+ pt.y * im->widthStep + pt.x * im->nChannels;

	if(im->nChannels==1)
	{
		return (0x01000000 | (int)ptrImg[0]);
	}
	else if(im->nChannels==3)
	{
		int r = (int)ptrImg[2];
		int g = (int)ptrImg[1];
		int b = (int)ptrImg[0];

		return (0x03000000 | (r << 16) | (g << 8) | b);
	}

	return 0;
}

/*********************************************************************
* 函数名称: void ryuLabelImageInWindow(IplImage * im, double hscale, double vscale, int mode, int mask_w, int mask_h)
* 说明:获取日志文件路径
* 调用者：Write_Log_Text
* 输入参数:
* int mode -- 标记类型
*    -- RYU_LABELIM_POINT(1): 单点标记
*    -- RYU_LABELIM_RECT(2): 两点标记矩形
*    -- RYU_LABELIM_FIXEDRECT(3): 标记固定大小矩形
*    -- RYU_LABELIM_ROTATERECT(4): 标记旋转矩形
* 输出参数：
* char *pStrPath
* 返回值:
* void -- 空
* 作者: Ryu L.
* 时间 : 2009-10-11
*********************************************************************/
int ryuLabelImageInWindow(IplImage * im, double hscale, double vscale, CvPoint rect[4], 
							 int mode, int mask_w, int mask_h)
{
	int ret = 0, i = 0;
	IplImage * temp, * show = 0, * mask = 0, * mask_c1 = 0;	// 用于显示及标定的图像

	init_label();

	if(NULL == im || NULL == rect) {
#if _LABEL_IM_SAMPLES_PRINT_
		printf("Error! Invalid pointer. [labelImageWindow]. im=0x%x\n", im);
#endif
		ret = -1;
		goto nExit;
	}

	for(i = 0; i < 4; i++) {
		rect[i] = cvPoint(-1, -1);
	}

	if(0 >= im->width || 0 >= im->height) {
#if _LABEL_IM_SAMPLES_PRINT_
		printf("Error! Bad value. [labelImageWindow]. im->width=%d, im->height=%d\n", 
			im->width, im->height);
#endif
		ret = -1;
		goto nExit;
	}

	if(1 != im->nChannels && 3 != im->nChannels) {
#if _LABEL_IM_SAMPLES_PRINT_
		printf("Error! Invalid image nChannels. [labelImageWindow]. im->nChannels=%d\n", 
			im->nChannels);
#endif
		ret = -1;
		goto nExit;
	}

	if(RYU_LABELIM_POINT > mode || RYU_LABELIM_CROSSPOINT < mode) {
#if _LABEL_IM_SAMPLES_PRINT_
		printf("Error! Invalid value. [labelImageWindow]. mode=%d\n", mode);
#endif
		ret = -1;
		goto nExit;
	}

	// 调整显示图像宽高为适应屏幕尺寸
	double hscl = hscale, vscl = vscale;
	CvSize rsz = ryuSizeFitScreen(cvGetSize(im), hscl, vscl);
	if(0 >= rsz.width || 0 >= rsz.height) {
#if _LABEL_IM_SAMPLES_PRINT_
		printf("Warning! Bad value. [labelImageWindow]. rsz.w=%d, rsz.h=%d\n", 
			rsz.width, rsz.height);
#endif
		ret = -1;
		goto nExit;
	}

	// 建立刷新模板
	temp = cvCreateImage(rsz, 8, 3);
	if(3 == im->nChannels) {
		cvResize(im, temp);
	} else {
		IplImage * trsz = cvCreateImage(rsz, 8, 1);
		cvResize(im, trsz);
		cvCvtColor(trsz, temp, CV_GRAY2BGR);
		cvReleaseImage(&trsz);
	}

	// 创建mask图像
	mask = cvCreateImage(cvSize(mask_w, mask_h), 8, 3);
	mask_c1 = cvCreateImage(cvSize(mask_w, mask_h), 8, 1);

#if _LABEL_IM_SAMPLES_PRINT_
	// 初始化打印信息
	printf("<Label image> im_width=%d, im_height=%d, mask_width=%d, mask_height=%d\n",
		im->width, im->height, mask_w, mask_h);
	printf("<Label image> x=0, y=0, lum=0, width=0, height=0, angle=0");
#endif

	int isBrk = 0;
	show = cvCreateImage(rsz, 8, 3);
	cvNamedWindow("MASK", CV_WINDOW_AUTOSIZE | CV_GUI_EXPANDED);
	cvNamedWindow("标定图像", CV_WINDOW_AUTOSIZE | CV_GUI_EXPANDED);
	cvSetMouseCallback("标定图像", my_mouse_callback, (void*) show);
	while(1) {
		cvCopyImage(temp, show);
		switch (mode)
		{
		case RYU_LABELIM_POINT:
			isBrk = label_point(hscl, vscl);
			break;
		case RYU_LABELIM_LINE:
			isBrk = label_line(hscl, vscl);
			break;
		case RYU_LABELIM_RECT:
			isBrk = label_rect(hscl, vscl);
			break;
		case RYU_LABELIM_FIXEDRECT:
			isBrk = label_fixed(cvGetSize(im), hscl, vscl, mask_w, mask_h);
			break;
		case RYU_LABELIM_ROTATERECT:
			isBrk = label_rotate(hscl, vscl);
			break;
		case RYU_LABELIM_CROSSPOINT:
			isBrk = label_crosspoint(rsz, hscl, vscl);
			break;
		default:
			break;
		}
		// 打印坐标信息
		CvPoint realPos = rsz2orig(mousePos, hscl, vscl);
		int lum = ryuGetPointLum(im, realPos);
		if(3 == im->nChannels) {
#if _LABEL_IM_SAMPLES_PRINT_
			printf("\r");
			printf("<Label image> x=%4d, y=%4d, lum=(%3d, %3d, %3d), width=%4d, height=%4d, angle=%3.2f",
				realPos.x, realPos.y, (lum>>16)&0xff, (lum>>8)&0xff, lum&0xff, rectSz.width, rectSz.height, rectAg);
#endif
		} else {
#if _LABEL_IM_SAMPLES_PRINT_
			printf("\r");
			printf("<Label image> x=%4d, y=%4d, lum=%3d, width=%4d, height=%4d, angle=%3.2f",
				realPos.x, realPos.y, lum&0xff, rectSz.width, rectSz.height, rectAg);
#endif
		}
		// 图像显示
		RyuImage * dbgRyu_im = iplImage2ryuImage(im);
		RyuImage * dbgRyu_mask = iplImage2ryuImage(mask);
		ryuCopyRoi2Image(dbgRyu_im, ryuRect(realPos.x-(mask_w>>1), realPos.y-(mask_h>>1), mask_w, mask_h), dbgRyu_mask);
		ryuReleaseImageHeader(&dbgRyu_im);

		cvLine(mask, cvPoint((mask_w>>1)-10, mask_h>>1), cvPoint((mask_w>>1)+10, mask_h>>1), CV_RGB(255,0,0), 1);
		cvLine(mask, cvPoint(mask_w>>1, (mask_h>>1)-10), cvPoint(mask_w>>1, (mask_h>>1)+10), CV_RGB(255,0,0), 1);
		cvShowImage("MASK", mask);

		if(RYU_LABELIM_CROSSPOINT == mode) {
			cvLine(show, shPt[0], shPt[1], CV_RGB(255, 0, 0), 1);
			cvLine(show, shPt[2], shPt[3], CV_RGB(255, 0, 0), 1);
		} 
		else
			draw_rect(show);
		cvShowImage("标定图像",show);

		if(1 == isBrk) {	// 完成标记=>退出
			for(i = 0; i < 4; i++) {
				rect[i] = orPt[i];
			}
			break;
		}

		int kVal = cvWaitKey(15);
		// ESC=>退出
		if(27 == kVal) {
			init_label();
			break;
		}
		// R按键=>重置标记
		else if(114 == kVal) {
			init_label();
			continue;
		}
	}
	ret = isBrk;

#if _LABEL_IM_SAMPLES_PRINT_
	printf("\n");
#endif
	cvWaitKey();

nExit:
	if(temp)
		cvReleaseImage(&temp);
	if(show)
		cvReleaseImage(&show);
	if(mask)
		cvReleaseImage(&mask);
	if(mask_c1)
		cvReleaseImage(&mask_c1);
	cvDestroyWindow("标定图像");
	cvDestroyWindow("MASK");

	return ret;
}

float getLabelRectAngel()
{
	return rectAg;
}

CvSize getLableRectSize()
{
	return rectSz;
}

static void my_mouse_callback(int event, int x, int y, int flags, void* param)
{
	switch(event)
	{
	case CV_EVENT_MOUSEMOVE:
		{
			mousePos.x = x;
			mousePos.y = y;
		}
		break;

	case  CV_EVENT_LBUTTONDOWN:
		{
			if(4 <= clickTm)
				break;
			clickPos[clickTm].x = x;
			clickPos[clickTm].y = y;
			clickTm++;
		}
		break;
	}
}

static void draw_rect(IplImage* img)
{
	int i = 0;
	// 绘制四边
	cvLine(img, shPt[0], shPt[1], CV_RGB(255, 0, 0), 1);
	cvLine(img, shPt[0], shPt[2], CV_RGB(255, 0, 0), 1);
	cvLine(img, shPt[3], shPt[1], CV_RGB(255, 0, 0), 1);
	cvLine(img, shPt[3], shPt[2], CV_RGB(255, 0, 0), 1);
	// 标记四点
	for(i = 0; i < 4; i++)
		cvCircle(img, shPt[i], 2, CV_RGB(255, 0, 0), CV_FILLED);
	// 标示4点
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0.0, 1, CV_AA);
	char txt[32];
	for(i = 0; i < 4; i++) {
		sprintf(txt, "%d", i);
		cvPutText(img, txt, cvPoint(shPt[i].x-5, shPt[i].y-5), &font, CV_RGB(255,0,0));
	}
	return;
}

static CvPoint rsz2orig(CvPoint rsz, double hscl, double vscl)
{
	CvPoint orig;
	orig.x = (int)(rsz.x / hscl + 0.5);
	orig.y = (int)(rsz.y / vscl + 0.5);
	return orig;
}

static CvPoint orig2rsz(CvPoint orig, double hscl, double vscl)
{
	CvPoint rsz;
	rsz.x = (int)(orig.x * hscl + 0.5);
	rsz.y = (int)(orig.y * vscl + 0.5);
	return rsz;
}

static void init_label()
{
	mousePos.x = mousePos.y = -1;
	rectSz.width = rectSz.height = 0;
	clickTm = 0;
	rectAg = 0.0;

	for(int i = 0; i < 4; i++) {
		clickPos[i].x = clickPos[i].y = 0;
		orPt[i].x = orPt[i].y = -1;
		shPt[i].x = shPt[i].y = -1;
	}
}

static int label_point(double hscl, double vscl)
{
	if(clickTm > 0) {
		orPt[0] = rsz2orig(clickPos[0], hscl, vscl);
		return 1;
	}
	else
		return 0;
}

static int label_crosspoint(CvSize show_sz, double hscl, double vscl)
{
	if(0 == clickTm) {
		shPt[0] = cvPoint(0, mousePos.y);
		shPt[1] = cvPoint(show_sz.width, mousePos.y);
		shPt[2] = cvPoint(mousePos.x, 0);
		shPt[3] = cvPoint(mousePos.x, show_sz.height);
		return 0;
	} else if(0 < clickTm) {
		orPt[0] = rsz2orig(clickPos[0], hscl, vscl);
		shPt[0] = cvPoint(0, clickPos[0].y);
		shPt[1] = cvPoint(show_sz.width, clickPos[0].y);
		shPt[2] = cvPoint(clickPos[0].x, 0);
		shPt[3] = cvPoint(clickPos[0].x, show_sz.height);
		return 1;
	}
	else
		return 0;
}

static int label_line(double hscl, double vscl)
{
	if(1 == clickTm) {
		orPt[0] = rsz2orig(clickPos[0], hscl, vscl);
		shPt[0] = clickPos[0];
		shPt[1] = mousePos;
		shPt[2] = shPt[0];
		shPt[3] = shPt[1];
		// 更新宽度
		CvPoint tmPt = rsz2orig(mousePos, hscl, vscl);
		int dx1_0 = tmPt.x - orPt[0].x, dy1_0 = tmPt.y - orPt[0].y;
		rectSz.width = (int)(sqrt(dx1_0 * dx1_0 + dy1_0 * dy1_0) + 0.5);
		rectSz.height = 0;
		float theta = atan2(dy1_0, dx1_0);
		rectAg = theta * 180 / PI;
		return 0;
	} else if(2 == clickTm) {
		orPt[1] = rsz2orig(clickPos[1], hscl, vscl);
		// 更新宽度
		int dx1_0 = orPt[1].x - orPt[0].x, dy1_0 = orPt[1].y - orPt[0].y;
		rectSz.width = (int)(sqrt(dx1_0 * dx1_0 + dy1_0 * dy1_0) + 0.5);
		rectSz.height = 0;
		float theta = atan2(dy1_0, dx1_0);
		rectAg = theta * 180 / PI;
		return 1;
	}
	else
		return 0;
}

static int label_rect(double hscl, double vscl)
{
	if(1 == clickTm) {
		orPt[0] = rsz2orig(clickPos[0], hscl, vscl);
		shPt[0] = clickPos[0];
		shPt[3] = mousePos;
		shPt[1] = cvPoint(shPt[3].x, shPt[0].y);
		shPt[2] = cvPoint(shPt[0].x, shPt[3].y);
		// 更新宽度信息
		CvPoint tmPt = rsz2orig(mousePos, hscl, vscl);
		rectSz.width = abs(tmPt.x - orPt[0].x + 1);
		rectSz.height = abs(tmPt.y - orPt[0].y + 1);
		return 0;
	} else if(2 == clickTm) {
		orPt[3] = rsz2orig(clickPos[1], hscl, vscl);
		// 调正处理
		int wid = orPt[3].x - orPt[0].x;
		int hei = orPt[3].y - orPt[0].y;
		if(0 > wid) {
			orPt[0].x += wid;
			wid *= -1;
		}
		if(0 > hei) {
			orPt[0].y += hei;
			hei *= -1;
		}
		// 推算坐标
		orPt[1].x = orPt[0].x + wid;
		orPt[1].y = orPt[0].y;
		orPt[2].x = orPt[0].x;
		orPt[2].y = orPt[0].y + hei;
		orPt[3].x = orPt[1].x;
		orPt[3].y = orPt[1].y;
		// 更新宽度信息
		rectSz.width = wid + 1;
		rectSz.height = hei + 1;
		return 1;
	} else {
		return 0;
	}
}

static int label_fixed(CvSize im_sz, double hscl, double vscl, int mask_w, int mask_h)
{
	int f_w = mask_w >> 1;
	int f_h = mask_h >> 1;
	orPt[0] = rsz2orig((clickTm ? clickPos[0] : mousePos), hscl, vscl);
	// 越界处理
	if(0 > orPt[0].x - f_w)
		orPt[0].x = 0;
	else if(im_sz.width <= orPt[0].x - f_w + mask_w - 1)
		orPt[0].x = im_sz.width - mask_w;
	else
		orPt[0].x = orPt[0].x - f_w;

	if(0 > orPt[0].y - f_h)
		orPt[0].y = 0;
	else if(im_sz.height <= orPt[0].y - f_h + mask_h - 1)
		orPt[0].y = im_sz.height - mask_h;
	else
		orPt[0].y = orPt[0].y - f_h;

	// 推算坐标
	orPt[1].x = orPt[0].x + mask_w - 1;
	orPt[1].y = orPt[0].y;
	orPt[2].x = orPt[0].x;
	orPt[2].y = orPt[0].y + mask_h - 1;
	orPt[3].x = orPt[1].x;
	orPt[3].y = orPt[2].y;
	// 纠正鼠标位置
	mousePos = orig2rsz(cvPoint(orPt[0].x+f_w, orPt[0].y+f_h), hscl, vscl);
	// 更新宽度信息
	rectSz.width = mask_w;
	rectSz.height = mask_h;
	if(1 == clickTm)
		return 1;
	else {
		for(int i = 0; i < 4; i++)
			shPt[i] = orig2rsz(orPt[i], hscl, vscl);
		return 0;
	}
}


static int label_rotate(double hscl, double vscl)
{
	if(1 == clickTm) {
		orPt[0] = rsz2orig(clickPos[0], hscl, vscl);
		shPt[0] = clickPos[0];
		shPt[1] = mousePos;
		shPt[2] = shPt[0];
		shPt[3] = shPt[1];
		// 更新宽度
		CvPoint tmPt = rsz2orig(mousePos, hscl, vscl);
		int dx1_0 = tmPt.x - orPt[0].x, dy1_0 = tmPt.y - orPt[0].y;
		rectSz.width = (int)(sqrt(dx1_0 * dx1_0 + dy1_0 * dy1_0) + 0.5);
		float theta = atan2(dy1_0, dx1_0);
		rectAg = theta * 180 / PI;
		return 0;
	} else if(2 == clickTm) {
		orPt[1] = rsz2orig(clickPos[1], hscl, vscl);
		shPt[1] = clickPos[1];
		// 计算矩形其他两点及宽、高
		int dx1_0 = orPt[1].x - orPt[0].x, dy1_0 = orPt[1].y - orPt[0].y;
		// 向量模
		rectSz.width = (int)(sqrt(dx1_0 * dx1_0 + dy1_0 * dy1_0) + 0.5);
		// 向量角度及偏移量
		float theta = atan2(dy1_0, dx1_0);
		float A = sin(theta), B = cos(theta);
		float rho =  orPt[0].x * A - orPt[0].y * B;
		rectAg = theta * 180 / PI;
		// 鼠标所在位置换算
		CvPoint tmPt = rsz2orig(mousePos, hscl, vscl);
		float tmRho = tmPt.x * A - tmPt.y * B;
		rectSz.height = (int)(fabs(tmRho - rho) + 0.5);
		// 3、4点位置计算
		int dp = dx1_0 * orPt[0].x + dy1_0 * orPt[0].y;
		float measure = dx1_0 * B + dy1_0 * A;
		orPt[2].x = (int)((dp * B + tmRho * dy1_0) / measure + 0.5);
		orPt[2].y = (int)((dp * A - tmRho * dx1_0) / measure + 0.5);
		orPt[3].x = orPt[2].x + dx1_0;
		orPt[3].y = orPt[2].y + dy1_0;
		// 3、4点显示坐标更新
		shPt[2] = orig2rsz(orPt[2], hscl, vscl);
		shPt[3] = orig2rsz(orPt[3], hscl, vscl);
		return 0;
	} else if(3 == clickTm) {
		int dx1_0 = orPt[1].x - orPt[0].x, dy1_0 = orPt[1].y - orPt[0].y;
		// 向量角度及偏移量
		float theta = atan2(dy1_0, dx1_0);
		float A = sin(theta), B = cos(theta);
		float rho =  orPt[0].x * A - orPt[0].y * B;
		// 鼠标所在位置换算
		CvPoint tmPt = rsz2orig(clickPos[2], hscl, vscl);
		float tmRho = tmPt.x * A - tmPt.y * B;
		rectSz.height = (int)(fabs(tmRho - rho) + 0.5);
		// 3、4点位置计算
		int dp = dx1_0 * orPt[0].x + dy1_0 * orPt[0].y;
		float measure = dx1_0 * B + dy1_0 * A;
		orPt[2].x = (int)((dp * B + tmRho * dy1_0) / measure + 0.5);
		orPt[2].y = (int)((dp * A - tmRho * dx1_0) / measure + 0.5);
		orPt[3].x = orPt[2].x + dx1_0;
		orPt[3].y = orPt[2].y + dy1_0;
		// 3、4点显示坐标更新
		shPt[2] = orig2rsz(orPt[2], hscl, vscl);
		shPt[3] = orig2rsz(orPt[3], hscl, vscl);
		return 1;
	} else
		return 0;
}
