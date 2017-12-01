#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <list>
#include <iostream>

using namespace std;

// ��ݵ��и���ʾ����
#define RUN_CUT_PROCESS 1

#include <math.h>
#include <time.h>
#include "OpenCv_debugTools.h"
#include "ExpressBarcodeDetection.h"
#include "ExpressCutout.h"

struct PathElem{
	TCHAR   SrcImgPath[MAX_PATH*2];
	TCHAR   RstImgPath1[MAX_PATH*2];
	TCHAR   RstImgPath2[MAX_PATH*2];
	TCHAR   RstImgPath3[MAX_PATH*2];
	TCHAR   RstImgPath4[MAX_PATH*2];
	TCHAR   RstImgPath5[MAX_PATH*2];
	TCHAR   RstImgPath6[MAX_PATH*2];
	TCHAR   RstImgPath7[MAX_PATH*2];
};

int FindImgs(char * pSrcImgPath, char * pRstImgPath1, char * pRstImgPath2,
	char * pRstImgPath3, char * pRstImgPath4, char * pRstImgPath5, char * pRstImgPath6, 
	std::list<PathElem> &ImgList);

CvPoint cvDivideIntPoint(int point);

int _tmain(int argc, _TCHAR* argv[])
{
	std::list<PathElem> ImgList; 
	std::list<PathElem>::iterator pImgListTemp; 

	// ����ͼ���ļ���ȡ������·��
	int nFlag = FindImgs(
		// ·��1��ͼƬ��ȡ·��
		//"F:\\EN05_Code128Recog\\Materials\\1d\\20150303_kd_nrcg\\",
		//"F:\\EN07_ExpressRecog\\20160628_EMS1200W��ʶ��\\",
		"F:\\EN07_ExpressRecog2\\2017.11.30���\\",	
		// ·��2������ʶ��-ͼƬ����·��
		"1 ʶ��ɹ�\\",
		// ·��3��δʶ������-ͼƬ����·��
		"2 �޷�ʶ��\\",
		// ·��4��δ�ҵ�����-ͼƬ����·��
		"3 δ�ҵ�����\\",
		// ·��5��ʶ���������-ͼƬ����·��
		"4 ʶ�����\\",
		// ·��6��ʶ���쳣����-ͼƬ����·��
		"5 ʶ���쳣\\",
		// ·��7��ʶ���쳣����-ͼƬ����·��
		"6 �и�ʧ��\\",
		ImgList);

	// ���õ���֡��ţ�0Ϊ�رյ���֡��ת
	int debug_frame = 0;
	// �����ֶ��Զ��л����ܿ��أ�0Ϊ�ֶ���ת��>0Ϊ�Զ���ת(ֵΪ��ת���ms)
	int debug_aotu = 0;
	// �����ַ�λ�������˳�����λ��������
	int setCodeDigit = 0;
	// ����ѭ������ģʽ
	int isLoopRun = 0;
	// �������˳���ȡͼƬ
	int isRandomFrame = 0;
	// ����ͼ�񿪹أ�0Ϊ�����࣬1Ϊ�����������࣬2Ϊ����������
	int isSortImage = 0;

	// �㷨��������
	AlgorithmParamSet paramset;
	paramset.nFlag = 0;
	paramset.nCodeCount = 0;
	paramset.nCodeSymbology = 0;
	paramset.nCodeDgtNum = 0;
	paramset.nCodeValidity = 0;

	// ��ʼ���㷨
	unsigned char * results = 0;
	int status = 0, k = 0;
	long loopTime = 1;

	unsigned char * mem_tube_s = (unsigned char *) malloc(sizeof(unsigned char) * 4096 * 3072);

#if RUN_CUT_PROCESS
#ifndef INPUTTYPE_FILEPATH
	status = ExpressCutout_init(4096, 3072);
#else
	status = ExpressCutout_init(4096, 3072, "F://ALGOLOG");
#endif
	if(status < 0) {
		printf("Algorithm load ERROR, exit\n");
		goto nExit;
	} else {
		printf("Algorithm load successfully\n");
	}
#else
	status = algorithm_init(4096, 3072, &results);
	if(1 == status) {
		printf("Algorithm load successfully\n");
	} else {
		printf("Algorithm load ERROR, exit\n");
		goto nExit;
	}
#endif

	algorithm_setparams(&paramset);

	unsigned char * mem_tube_e = (unsigned char *) malloc(sizeof(unsigned char) * 4096 * 3072);
	printf("\n--Ryu--start:0x%x, end:0x%x\n", mem_tube_s, mem_tube_e);
	unsigned int tube_s = (unsigned int) mem_tube_s;
	unsigned int tube_e = (unsigned int) mem_tube_e;
	printf("--Ryu--Algorithm uses %dMB memory heap\n", (tube_e - tube_s) / 1024 / 1024);
	if(mem_tube_s) {
		free(mem_tube_s);
		mem_tube_s = 0;
	}
	if(mem_tube_e) {
		free(mem_tube_e);
		mem_tube_e = 0;
	}
	cvNamedWindow("Ready to Start");
	cvWaitKey();
	cvDestroyWindow("Ready to Start");

	CvFont font, font2;
	char txt[256];
	int nSuccessFrame = 0, nErrorFrame = 0, nFailFrame = 0, nTotalCntFrame = 0;

	time_t t = time(0); 
	char tmp[64]; 

	pImgListTemp = ImgList.begin();
	for(int iik = 1; iik <= ImgList.size(); iik++, pImgListTemp++)
	{
		if(1 == iik)	pImgListTemp = ImgList.begin();
		if(debug_frame) {
			if(iik < debug_frame)	
				continue;	// �ӵ���֡��ʼ����
		}
		else if(isRandomFrame) {
			status = rand() % ImgList.size() + 1;
			pImgListTemp = ImgList.begin();
			for(k = 1; k < status; k++)
				pImgListTemp++;
		}

		IplImage * srcImage = cvLoadImage (pImgListTemp->SrcImgPath, CV_LOAD_IMAGE_GRAYSCALE);
		if(0 == srcImage) {
			if(!debug_frame && isRandomFrame && status == ImgList.size())
				pImgListTemp--;
			continue;
		}
		unsigned char * in_data = (unsigned char *) malloc(srcImage->width * srcImage->height * sizeof(unsigned char));

		// ��ӡͼ���ļ�·��
		printf("\n==========================LOOP%ld FRAME%d==========================\n", loopTime, iik);
		cout << pImgListTemp->SrcImgPath << "\n";

		strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A �����%j�� %z",localtime(&t) ); 
		cout << tmp << "\n";
		if(!debug_frame && isRandomFrame)	printf("���˳���ȡ֡��ʵ��֡���Ϊ%d\n", status);

		Ipl2ucImageGray(srcImage, in_data);

		// ���Ų���ʾԭͼ��
		IplImage * srcImage3C = cvCreateImage (cvGetSize(srcImage), 8, 3);
		cvCvtColor(srcImage, srcImage3C, CV_GRAY2BGR);

		int scW = (int)(0.88 * GetSystemMetrics(SM_CXFULLSCREEN));
		int scH = (int)(0.88 * GetSystemMetrics(SM_CYFULLSCREEN));
		CvSize show_sz;
		float show_zRatio = 0.0;
		if(srcImage->width * scH > srcImage->height * scW) {
			show_zRatio = 1.0 * scW / srcImage->width;
			show_sz = cvSize(scW, (int)(show_zRatio * srcImage->height + 0.5));
		} else {
			show_zRatio = 1.0 * scH / srcImage->height;
			show_sz = cvSize((int)(show_zRatio * srcImage->width + 0.5), scH);
		}
		IplImage * srcShowImage = cvCreateImage(show_sz, 8, 3);
		cvResize(srcImage3C, srcShowImage);
		
		// ���ô�ӡ���塢���
		cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 0.7, 0.7, 0.0, 0.5, CV_AA);
		CvPoint show_offset = cvPoint(5, 20);

		sprintf(txt, "Frame %d", iik);
		cvPutText(srcShowImage, txt, show_offset, &font, CV_RGB(255,255,0));
		cvNamedWindow("Main Window");
		cvShowImage("Main Window", srcShowImage);
		cvWaitKey(debug_aotu);	//++++++++++++++++++++++++++++++++++++++++++++++++++++/

		// ��¼
		nTotalCntFrame++;

		// ��ʱ
		long TimeCost = 0;
		LARGE_INTEGER m_frequency = {0}, m_time1 = {0}, m_time2 = {0};
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_time2);

		// �����㷨
#if RUN_CUT_PROCESS
		int code_length = setCodeDigit;
		char code_rslt[128];
		int image_width = srcImage->width, image_height = srcImage->height;
#ifndef INPUTTYPE_FILEPATH
		status = ExpressCutout_run(in_data, &image_width, &image_height, code_rslt, &code_length);
#else
		status = ExpressCutout_run(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath1, pImgListTemp->RstImgPath2, 
					pImgListTemp->RstImgPath3, pImgListTemp->RstImgPath4, code_rslt, &code_length);
#endif
		results = ExpressCutout_getFullResult();
#else
		algorithm_run(0, in_data, srcImage->width, srcImage->height, &results);
#endif

		// ��ʱ��ӡ
		QueryPerformanceCounter((LARGE_INTEGER*) &m_time1);
		TimeCost = 1000.0*(m_time1.QuadPart-m_time2.QuadPart)/m_frequency.QuadPart;
		printf("\n-----------------------------------�㷨��ʱ: %ldms\n", TimeCost);

		int * first_4char = (int *)results;
		AlgorithmResult result_node;
		int isMatchSetDigit = setCodeDigit ? 0 : 1;

		// ��ӡʶ����������
		if(first_4char[0] > 0) {
			nSuccessFrame++;
			printf("��֡��ʶ��%d������\n", first_4char[0]);
			for(int i = 0; i < first_4char[0]; i++) {
				memcpy(&result_node, results+i*sizeof(AlgorithmResult)+4, sizeof(AlgorithmResult));
				printf("����%d��\n", result_node.nCodeSeqNum);
				printf("---nFlag = %d\n", result_node.nFlag);
				printf("---strCodeData = %s\n", result_node.strCodeData);

				if(result_node.nCodeSymbology == 1)
					printf("---nCodeSymbology=Code128\n");
				else if(result_node.nCodeSymbology == 2)
					printf("---nCodeSymbology=Code39\n");
				printf("---nCodeOrient=%d\n", result_node.nCodeOrient);
				printf("---nCodeCharNum=%d\n", result_node.nCodeCharNum);
				printf("---nCodeModuleWid=%d\n", result_node.nCodeModuleWid);
		
				int num_seq = 0;
				CvPoint ptBound_cv;
				ptBound_cv = cvDivideIntPoint(result_node.ptCodeBound1);
				ptBound_cv = cvPoint((int)(ptBound_cv.x * show_zRatio + 0.5), (int)(ptBound_cv.y * show_zRatio + 0.5));
				cvCircle(srcShowImage, ptBound_cv, 4, CV_RGB(0,255,0), CV_FILLED);
				sprintf(txt, "%d", num_seq++);
				cvPutText(srcShowImage, txt, ptBound_cv, &font, CV_RGB(0,168,0));

				ptBound_cv = cvDivideIntPoint(result_node.ptCodeBound2);
				ptBound_cv = cvPoint((int)(ptBound_cv.x * show_zRatio + 0.5), (int)(ptBound_cv.y * show_zRatio + 0.5));
				cvCircle(srcShowImage, ptBound_cv, 4, CV_RGB(0,255,0), CV_FILLED);
				sprintf(txt, "%d", num_seq++);
				cvPutText(srcShowImage, txt, ptBound_cv, &font, CV_RGB(0,168,0));

				ptBound_cv = cvDivideIntPoint(result_node.ptCodeBound3);
				ptBound_cv = cvPoint((int)(ptBound_cv.x * show_zRatio + 0.5), (int)(ptBound_cv.y * show_zRatio + 0.5));
				cvCircle(srcShowImage, ptBound_cv, 4, CV_RGB(0,255,0), CV_FILLED);
				sprintf(txt, "%d", num_seq++);
				cvPutText(srcShowImage, txt, ptBound_cv, &font, CV_RGB(0,168,0));

				ptBound_cv = cvDivideIntPoint(result_node.ptCodeBound4);
				ptBound_cv = cvPoint((int)(ptBound_cv.x * show_zRatio + 0.5), (int)(ptBound_cv.y * show_zRatio + 0.5));
				cvCircle(srcShowImage, ptBound_cv, 4, CV_RGB(0,255,0), CV_FILLED);
				sprintf(txt, "%d", num_seq++);
				cvPutText(srcShowImage, txt, ptBound_cv, &font, CV_RGB(0,168,0));

				ptBound_cv = cvDivideIntPoint(result_node.ptCodeCenter);
				ptBound_cv = cvPoint((int)(ptBound_cv.x * show_zRatio + 0.5), (int)(ptBound_cv.y * show_zRatio + 0.5));
				sprintf(txt, "%d", result_node.nCodeSeqNum);
				cvPutText(srcShowImage, txt, ptBound_cv, &font, CV_RGB(0,255,0));
				sprintf(txt, "Frame %d: Find %d Code(s)!", iik, first_4char[0]);
				cvPutText(srcShowImage, txt, show_offset, &font, CV_RGB(0,255,0));
				sprintf(txt, "Code %d-", result_node.nCodeSeqNum);
				if( 0x1 == result_node.nCodeSymbology )
					strcat(txt, "(Code128): ");
				else if( 0x2 == result_node.nCodeSymbology)
					strcat(txt, "(Code39): ");
				else if( 0x4 == result_node.nCodeSymbology)
					strcat(txt, "(Code93): ");
				else if( 0x8 == result_node.nCodeSymbology)
					strcat(txt, "(I2of5): ");
				else if( 0x10 == result_node.nCodeSymbology)
					strcat(txt, "(EAN-13): ");
				else
					strcat(txt, "(Unknown): ");
				strcat(txt, result_node.strCodeData);
				cvPutText(srcShowImage, txt, cvPoint(show_offset.x, (i+2) * show_offset.y), &font, CV_RGB(0,255,0));
			}
			if(1 == isSortImage)
				MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath1);
			else if(2 == isSortImage)
				cvSaveImage(pImgListTemp->RstImgPath1, srcImage);
		} else if(first_4char[0] == -2) {
			printf("��֡δ��⵽���� Cannot Find Code!\n");
			nFailFrame++;
			sprintf(txt, "Frame %d: Cannot Find Code!", iik);
			cvPutText(srcShowImage, txt, show_offset, &font, CV_RGB(0,0,255));
			if(1 == isSortImage)
				MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath3);
			else if(2 == isSortImage)
				cvSaveImage(pImgListTemp->RstImgPath3, srcImage);
		} else if(first_4char[0] == 0 || first_4char[0] == -1) {
			printf("��֡�����޷�ʶ�� Cannot Read Code!\n");
			nFailFrame++;
			sprintf(txt, "Frame %d: Cannot Read Code!", iik);
			cvPutText(srcShowImage, txt, show_offset, &font, CV_RGB(255,128,0));
			if(1 == isSortImage)
				MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath2);
			else if(2 == isSortImage)
				cvSaveImage(pImgListTemp->RstImgPath2, srcImage);
		} else if(first_4char[0] < -100) {
			printf("��֡����ֵ�쳣 Error Occurred!\n");
			nErrorFrame++;
			sprintf(txt, "Frame %d: Cannot Read Code!", iik);
			cvPutText(srcShowImage, txt, show_offset, &font, CV_RGB(255,0,0));
			if(1 == isSortImage)
				MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath5);
			else if(2 == isSortImage)
				cvSaveImage(pImgListTemp->RstImgPath5, srcImage);
		}

#if RUN_CUT_PROCESS
		// ��ʾ��ݵ��и���
		if(status > 0) {
			printf("�и���ɽ�� = %s\n", code_rslt);
			IplImage * iplCut = cvCreateImage(cvSize(image_width, image_height), 8, 1);
// 			IplImage * iplCutShow = cvCreateImage(cvSize(512, image_height * 512 / image_width), 8, 1);
// 			IplImage * iplCutShow3C = cvCreateImage(cvSize(512, image_height * 512 / image_width), 8, 3);
// 			uc2IplImageGray(in_data, iplCut);
// 			cvResize(iplCut, iplCutShow);
// 			cvCvtColor(iplCutShow, iplCutShow3C, CV_GRAY2BGR);
			IplImage * iplCutShow3C = cvCreateImage(cvSize(image_width, image_height), 8, 3);
			uc2IplImageGray(in_data, iplCut);
			cvCvtColor(iplCut, iplCutShow3C, CV_GRAY2BGR);

			cvInitFont(&font2, CV_FONT_HERSHEY_COMPLEX_SMALL, 1, 1, 0.0, 1, CV_AA);
			if(status > 1) {
				sprintf(txt, "Frame %d: %s", iik, code_rslt);
				cvPutText(iplCutShow3C, txt, cvPoint(5, 15), &font2, CV_RGB(0,255,0));
			} else {
				sprintf(txt, "Frame %d: No Recog", iik);
				cvPutText(iplCutShow3C, txt, cvPoint(5, 15), &font2, CV_RGB(255,0,0));
			}
			cvNamedWindow("�и�ͼ��");
			cvShowImage("�и�ͼ��", iplCutShow3C);
			cvReleaseImage(&iplCut);
//			cvReleaseImage(&iplCutShow);
			cvReleaseImage(&iplCutShow3C);
		}
#endif

#if		1
		// ��ʾ����λ�ü����
		if(0 <= first_4char[0]) {
			memcpy(&result_node, results+first_4char[0]*sizeof(AlgorithmResult)+4, sizeof(AlgorithmResult));
			if(8 == result_node.nFlag) {
				printf("��⵽����λ�ã�\n", result_node.nCodeModuleWid);
				cvRectangle(srcImage3C, cvPoint(result_node.ptCodeBound1, result_node.ptCodeBound2),
					cvPoint(result_node.ptCodeBound3, result_node.ptCodeBound4), CV_RGB(0, 0, 255), max(srcImage->width/512,3));
				CvPoint pkgCentre = cvPoint(result_node.ptCodeCenter>>16, result_node.ptCodeCenter&0xffff);
				cvLine(srcImage3C, cvPoint(pkgCentre.x-srcImage->width/128, pkgCentre.y),
					cvPoint(pkgCentre.x+srcImage->width/128, pkgCentre.y), CV_RGB(0, 0, 255), max(srcImage->width/512,3));
				cvLine(srcImage3C, cvPoint(pkgCentre.x, pkgCentre.y-srcImage->width/128),
					cvPoint(pkgCentre.x, pkgCentre.y+srcImage->width/128), CV_RGB(0, 0, 255), max(srcImage->width/512,3));
				printf("��������=(%d, %d), ��������=(%d, %d), ��������=(%d, %d)\n", 
					pkgCentre.x, pkgCentre.y, result_node.ptCodeBound1, result_node.ptCodeBound2, 
					result_node.ptCodeBound3, result_node.ptCodeBound4);
			} else {
				printf("δ�ܼ�⵽����λ�ã�\n", result_node.nCodeModuleWid);
			}
		}
#endif

		cvNamedWindow("Main Window");
		cvShowImage("Main Window", srcShowImage);
		cvWaitKey(debug_aotu);	//++++++++++++++++++++++++++++++++++++++++++++++++++++/

		if(srcImage)		cvReleaseImage(&srcImage);
		if(srcImage3C)		cvReleaseImage(&srcImage3C);
		if(srcShowImage)	cvReleaseImage(&srcShowImage);
		if(in_data)			free(in_data);

		if(isRandomFrame && status == ImgList.size())
			pImgListTemp--;

		if(isLoopRun && iik == ImgList.size()) {
			iik = 0;
			loopTime++;
		}
	}

	printf("\n-----------------------------------------\n");
	printf("ʶ������������%d֡��������ȷʶ��%d֡������ʶ��%d֡��δʶ��%d֡��ʶ����%.3f\n",\
		nTotalCntFrame, nSuccessFrame, nErrorFrame, nFailFrame, nSuccessFrame*1.0/nTotalCntFrame);

nExit:
#if RUN_CUT_PROCESS
#ifndef INPUTTYPE_FILEPATH
	ExpressCutout_release();
#else
	ExpressCutout_release();
#endif
#else
	algorithm_release();
#endif
	cvNamedWindow("end");
	cvWaitKey();
	return 0;
}

// ��������wxy
int FindImgs(char * pSrcImgPath, char * pRstImgPath1, char * pRstImgPath2,
	char * pRstImgPath3, char * pRstImgPath4, char * pRstImgPath5, char * pRstImgPath6, 
	std::list<PathElem> &ImgList)
{
	//ԴͼƬ���ڵ�Ŀ¼
	TCHAR   szFileT1[MAX_PATH*2];
	lstrcpy(szFileT1,TEXT(pSrcImgPath));
	lstrcat(szFileT1, TEXT("*.*"));

	//���ͼƬ��ŵ�Ŀ¼1
	TCHAR   RstAddr1[MAX_PATH*2]; 
	lstrcpy(RstAddr1,TEXT(pSrcImgPath));
	lstrcat(RstAddr1,TEXT(pRstImgPath1));
	_mkdir(RstAddr1);   //�����ļ���
	//���ͼƬ��ŵ�Ŀ¼2
	TCHAR   RstAddr2[MAX_PATH*2]; 
	lstrcpy(RstAddr2,TEXT(pSrcImgPath));
	lstrcat(RstAddr2,TEXT(pRstImgPath2));
	_mkdir(RstAddr2);   //�����ļ���
	//���ͼƬ��ŵ�Ŀ¼3
	TCHAR   RstAddr3[MAX_PATH*2]; 
	lstrcpy(RstAddr3,TEXT(pSrcImgPath));
	lstrcat(RstAddr3,TEXT(pRstImgPath3));
	_mkdir(RstAddr3);   //�����ļ���
	//���ͼƬ��ŵ�Ŀ¼4
	TCHAR   RstAddr4[MAX_PATH*2]; 
	lstrcpy(RstAddr4,TEXT(pSrcImgPath));
	lstrcat(RstAddr4,TEXT(pRstImgPath4));
	_mkdir(RstAddr4);   //�����ļ���
	//���ͼƬ��ŵ�Ŀ¼5
	TCHAR   RstAddr5[MAX_PATH*2]; 
	lstrcpy(RstAddr5,TEXT(pSrcImgPath));
	lstrcat(RstAddr5,TEXT(pRstImgPath5));
	_mkdir(RstAddr5);   //�����ļ���
	//���ͼƬ��ŵ�Ŀ¼6
	TCHAR   RstAddr6[MAX_PATH*2]; 
	lstrcpy(RstAddr6,TEXT(pSrcImgPath));
	lstrcat(RstAddr6,TEXT(pRstImgPath6));
	_mkdir(RstAddr6);   //�����ļ���

	WIN32_FIND_DATA   wfd; 	
	HANDLE   hFind   =   FindFirstFile(szFileT1, &wfd); 


	PathElem stPathElemTemp;
	if(hFind != INVALID_HANDLE_VALUE)   
	{
		do 
		{ 
			if(wfd.cFileName[0] == TEXT('.')) 
				continue;
			if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY || strcmp("Thumbs.db", TEXT(wfd.cFileName)) == 0) 
			{ 
				;
			}  
			else 
			{ 

				TCHAR   SrcImgPath[MAX_PATH*2];
				lstrcpy(SrcImgPath, pSrcImgPath); 
				lstrcat(SrcImgPath, TEXT(wfd.cFileName));
				lstrcpy(stPathElemTemp.SrcImgPath, SrcImgPath); 

				//////////////////////////////////////////////////////////////////////////

				TCHAR   AdressTemp[MAX_PATH*2];
				TCHAR   AdressTempC[MAX_PATH*2];
				lstrcpy(AdressTemp,RstAddr1); 
				//lstrcat(AdressTemp, TEXT("/"));  
				lstrcat(AdressTemp, TEXT(wfd.cFileName));  
				lstrcpy(stPathElemTemp.RstImgPath1, AdressTemp); 

				for(int i = lstrlen(AdressTemp)-1; i > 0; i--) {
					if('.' == AdressTemp[i]) {
						lstrcpyn(AdressTempC, AdressTemp, i+1);
						lstrcat(AdressTempC, "_cut");
						lstrcat(AdressTempC, &AdressTemp[i]);
						break;
					}
				}
				lstrcpy(stPathElemTemp.RstImgPath7, AdressTempC); 

				lstrcpy(AdressTemp,RstAddr2); 
				//lstrcat(AdressTemp, TEXT("/"));  
				lstrcat(AdressTemp, TEXT(wfd.cFileName));  
				lstrcpy(stPathElemTemp.RstImgPath2, AdressTemp); 

				lstrcpy(AdressTemp,RstAddr3); 
				//lstrcat(AdressTemp, TEXT("/"));  
				lstrcat(AdressTemp, TEXT(wfd.cFileName));  
				lstrcpy(stPathElemTemp.RstImgPath3, AdressTemp); 

				lstrcpy(AdressTemp,RstAddr4); 
				//lstrcat(AdressTemp, TEXT("/"));  
				lstrcat(AdressTemp, TEXT(wfd.cFileName));  
				lstrcpy(stPathElemTemp.RstImgPath4, AdressTemp); 

				lstrcpy(AdressTemp,RstAddr5); 
				//lstrcat(AdressTemp, TEXT("/"));  
				lstrcat(AdressTemp, TEXT(wfd.cFileName));  
				lstrcpy(stPathElemTemp.RstImgPath5, AdressTemp); 

				lstrcpy(AdressTemp,RstAddr6); 
				//lstrcat(AdressTemp, TEXT("/"));  
				lstrcat(AdressTemp, TEXT(wfd.cFileName));  
				lstrcpy(stPathElemTemp.RstImgPath6, AdressTemp); 

				ImgList.push_back(stPathElemTemp);

			}
		}while(FindNextFile(hFind, &wfd));
	}
	else
	{
		return -1;
	}
	return 0;
}

CvPoint cvDivideIntPoint(int point)
{
	int dx = 0, dy = 0;
	int s = 0;

	CvPoint pt;

	dx = point >> 16;
	s = dx & 0x8000;
	pt.x = (0 == s) ? (dx & 0x0000ffff) : (dx | 0xffff0000);

	dy = point & 0x0000ffff;
	s = dy & 0x8000;
	pt.y = (0 == s) ? (dy) : (dy | 0xffff0000);

	return pt;
}
