// LableImSamples.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <time.h>

#include "general_utils.h"
#include "image_ui.h"
#include "im_proc.h"

int _tmain(int argc, _TCHAR* argv[])
{
	// ͼ���ȡ·�����������ļ��л򵥸�ͼ���ļ�
	char srcFilePath[] = "F:/EN07_ExpressRecog/20170825������������δʶ��ͼƬ";
	char dstPath[] = "F:/EN08_OCRNumRecog/20170825������������δʶ��ͼƬ";

	std::list<PathElem> ImgList; 
	std::list<PathElem>::iterator pImgListTemp; 

	// ��ȡͼ���ļ�/�ļ����µ�ͼ���ļ�
	int status = findImageFiles(srcFilePath, ImgList);
	if(0 >= status) {
		return 0;
	}

	// ��֤�洢·����Ч��
	if(1 != isDirExist(dstPath)) {
		status = creatFlieDir(dstPath);
		if(1 != status) {
			printf("[_tmain] Error. Cannot create file dir. ret=%d\n", status);
			return 0;
		}
	}

	int start_frame = 101;

	// ���ζ�ȡͼ���ļ�
	IplImage * im = 0, * ocr_im = 0;
	pImgListTemp = ImgList.begin();
	for(int iik = 1; iik <= ImgList.size(); iik++, pImgListTemp++)
	{
		// ��ָ��֡�ſ�ʼѭ��
		if(start_frame > iik) 
			continue;

		printf("\n==========================[_tmain] Frame:%d==========================\n", iik);
		printf("[_tmain] %s\n", pImgListTemp->SrcImgPath);

		im = cvLoadImage(pImgListTemp->SrcImgPath, CV_LOAD_IMAGE_GRAYSCALE);
		if(NULL == im) {
			printf("[_tmain] Warn. Load image failed. im=0x%x\n", im);
			continue;
		}

		// ͼ������궨
		CvPoint rect[4];
		status = ryuLabelImageInWindow(im, 1.0, 1.0, rect, RYU_LABELIM_ROTATERECT);
		if(1 != status) {
			printf("[_tmain] Error. Unexpected return of ryuLabelImageInWindow. ret=%d\n", status);
			printf("[_tmain] Press <esc> to exit, press other key to continue...\n");
			int wait_key = cvWaitKey();
			if(27 == wait_key)
				goto nExit;
			else
				continue;
		}

		// ͼ����ת
		CvSize rcsz = getLableRectSize();
		ocr_im = cvCreateImage(rcsz, 8, 1);

		int srcX[8] = {0}, dstX[8] = {0};
		double matrix[9];
		srcX[0] = 0;			srcX[1] = 0; 
		srcX[2] = rcsz.width-1;	srcX[3] = 0;
		srcX[4] = 0;			srcX[5] = rcsz.height-1; 
		srcX[6] = rcsz.width-1;	srcX[7] = rcsz.height-1;

		dstX[0] = rect[0].x;	dstX[1] = rect[0].y; 
		dstX[2] = rect[1].x;	dstX[3] = rect[1].y;
		dstX[4] = rect[2].x;	dstX[5] = rect[2].y; 
		dstX[6] = rect[3].x;	dstX[7] = rect[3].y;

		getPerspectiveTransformMat(srcX, dstX, matrix);
		status = warpPerspectiveTransformFixed(im, ocr_im, matrix);
		if(1 != status) {
			printf("[_tmain] Error. Unexpected return of warpPerspectiveTransformFixed. ret=%d\n", status);
			printf("[_tmain] Press <esc> to exit, press other key to continue...\n");
			int wait_key = cvWaitKey();
			if(27 == wait_key)
				goto nExit;
			else
				continue;
		}

		cvNamedWindow("ocr_im");
		cvShowImage("ocr_im", ocr_im);
		cvWaitKey(50);

		// ����ͼ����Ϣ
		char code_str[32] = {0};
		scanf("%s", code_str);

		if(0 == strcmp(code_str, "-skip")) {
			goto skipLabel;
		}

		// ��ȡ��ǰʱ��
		time_t rawtime;
		struct tm * timeinfo;
		time (&rawtime);
		timeinfo = localtime(&rawtime);
		char strTimeInfo[64] = {0};
		sprintf_s(strTimeInfo, "_%d%02d%02d_%02d%02d%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, 
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

		// �γɴ洢�ļ�������+ʱ��
		char dstFilePath[MAX_PATH] = {0};
		strcpy(dstFilePath, dstPath);

		char sp = getPathSeparator(dstFilePath);
		corrPathSeparator(dstFilePath, sp);
		int len = strlen(dstFilePath);
		if(sp != dstFilePath[len-1]) {
			dstFilePath[len++] = sp;
			dstFilePath[len] = 0;
		}
		strcat_s(dstFilePath, code_str);
		strcat_s(dstFilePath, strTimeInfo);
		strcat_s(dstFilePath, ".jpg");

		// ͼ��洢
		status = cvSaveImage(dstFilePath, ocr_im);
		if(1 != status) {
			printf("[_tmain] Error. Unexpected return of cvSaveImage. ret=%d\n", status);
			continue;
		}
		printf("[_tmain] Label image saved as %s\n", dstFilePath);
		printf("[_tmain] Press <esc> to exit, press other key to next frame...\n");
		
		if(27 == cvWaitKey())
			goto nExit;

skipLabel:
		if(im)	
			cvReleaseImage(&im);
		if(ocr_im)
			cvReleaseImage(&ocr_im);
	}

nExit:
	if(im)	
		cvReleaseImage(&im);
	if(ocr_im)
		cvReleaseImage(&ocr_im);
	cvDestroyAllWindows();

	return 0;
}

