#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "wininet.h"
#include <direct.h>
#include <list>
#pragma comment(lib,"Wininet.lib")
#include <iostream>

using namespace std;

// 快递单切割显示开关
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

int _tmain(int argc, _TCHAR* argv[])
{
	std::list<PathElem> ImgList; 
	std::list<PathElem>::iterator pImgListTemp; 

	// 设置图像文件读取及保存路径
	int nFlag = FindImgs(
		// 路径1：图片读取路径
		"F:\\EN07_ExpressRecog2\\2017.10.16 正常不识别 重点调试\\",
		//"F:\\EN05_Code128Recog\\Materials\\2d\\fpc二维码\\",
		// 路径2：条码识别-图片保存路径
		"1 识别成功\\",
		// 路径3：未识别条码-图片保存路径
		"2 无法识别\\",
		// 路径4：未找到条码-图片保存路径
		"3 未找到条码\\",
		// 路径5：识别错误条码-图片保存路径
		"4 识别错误\\",
		// 路径6：识别异常条码-图片保存路径
		"5 识别异常\\",
		// 路径7：识别异常条码-图片保存路径
		"6 切割失败\\",
		ImgList);

	// 保存图像开关，0为不保存，1为保存不识别，2为分别保存识别，3位保存多分类
	int isSaveImage = 0;
	int isSaveLabel = 0;	// 是否在图像上标记识别结果
	// 调试手动自动切换功能开关，0为手动跳转，>0为自动跳转(值为跳转间隔ms)
	int debug_aotu = 0;
	// 设置调试帧序号，0为关闭调试帧跳转
	int debug_frame = 0;
	// 设置字符位数，过滤出错误位数的条码
	int setCodeDigit = 0;
	// 设置循环运行模式
	int isLoopRun = 0;
	// 设置随机顺序读取图片
	int isRandomFrame = 0;

	// 初始化算法
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
	char txt[50];
	int nSuccessFrame = 0, nErrorFrame = 0, nFailFrame = 0, nTotalCntFrame = 0;

	time_t t = time(0); 
	char tmp[64]; 

	pImgListTemp = ImgList.begin();
	for(int iik = 1; iik <= ImgList.size(); iik++, pImgListTemp++)
	{
		if(1 == iik)	pImgListTemp = ImgList.begin();
		if(debug_frame) {
			if(iik < debug_frame)	
				continue;	// 从调试帧开始运行
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

		// 打印图像文件路径
		printf("\n==========================LOOP%ld FRAME%d==========================\n", loopTime, iik);
		cout << pImgListTemp->SrcImgPath << "\n";

		strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A 本年第%j天 %z",localtime(&t) ); 
		cout << tmp << "\n";
		if(!debug_frame && isRandomFrame)	printf("随机顺序读取帧，实际帧序号为%d\n", status);

		Ipl2ucImageGray(srcImage, in_data);

		// 缩放并显示原图像
		IplImage * srcImage3C = cvCreateImage (cvGetSize(srcImage), 8, 3);
		cvCvtColor(srcImage, srcImage3C, CV_GRAY2BGR);
		IplImage * srcShowImage = cvCreateImage(cvSize(800, 800 * srcImage->height / srcImage->width), 8, 3);
		cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, max(srcImage->width*1.0/2048,1.0), max(srcImage->width*1.0/2048,1.0), 
			0.0, max(srcImage->width*1.0/2048,1.0), CV_AA);
		sprintf(txt, "Frame %d", iik);
		sprintf(txt, "Frame %d", iik);
		sprintf(txt, "Frame %d", iik);
		cvPutText(srcImage3C, txt, cvPoint(5, srcImage->width/64), &font, CV_RGB(255,255,0));
		cvResize(srcImage3C, srcShowImage);
		cvNamedWindow("Main Window");
		cvShowImage("Main Window", srcShowImage);
		cvWaitKey(debug_aotu);	//++++++++++++++++++++++++++++++++++++++++++++++++++++/

		// 记录
		nTotalCntFrame++;

		// 测时
		long TimeCost = 0;
		LARGE_INTEGER m_frequency = {0}, m_time1 = {0}, m_time2 = {0};
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_time2);

		// 调用算法
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

		// 测时打印
		QueryPerformanceCounter((LARGE_INTEGER*) &m_time1);
		TimeCost = 1000.0*(m_time1.QuadPart-m_time2.QuadPart)/m_frequency.QuadPart;
		printf("\n-----------------------------------算法耗时: %ldms\n", TimeCost);

		int * first_4char = (int *)results;
		AlgorithmResult result_node;
		int isMatchSetDigit = setCodeDigit ? 0 : 1;

		// 打印识别结果并绘制
		if(first_4char[0] > 0) {
			nSuccessFrame++;
			printf("本帧共识别到%d个条码\n", first_4char[0]);
			for(int i = 0; i < first_4char[0]; i++) {
				memcpy(&result_node, results+i*sizeof(AlgorithmResult)+4, sizeof(AlgorithmResult));
				printf("条码%d：\n", result_node.nCodeSeqNum);
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
				cvCircle(srcImage3C, cvPoint(result_node.ptCodeBound1>>16, result_node.ptCodeBound1&0xffff), max(srcImage->width/256,4), CV_RGB(0,255,0), CV_FILLED);
				sprintf(txt, "%d", num_seq++);
				cvPutText(srcImage3C, txt, cvPoint(result_node.ptCodeBound1>>16, result_node.ptCodeBound1&0xffff), &font, CV_RGB(0,168,0));
				cvCircle(srcImage3C, cvPoint(result_node.ptCodeBound2>>16, result_node.ptCodeBound2&0xffff), max(srcImage->width/256,4), CV_RGB(0,255,0), CV_FILLED);
				sprintf(txt, "%d", num_seq++);
				cvPutText(srcImage3C, txt, cvPoint(result_node.ptCodeBound2>>16, result_node.ptCodeBound2&0xffff), &font, CV_RGB(0,168,0));
				cvCircle(srcImage3C, cvPoint(result_node.ptCodeBound3>>16, result_node.ptCodeBound3&0xffff), max(srcImage->width/256,4), CV_RGB(0,255,0), CV_FILLED);
				sprintf(txt, "%d", num_seq++);
				cvPutText(srcImage3C, txt, cvPoint(result_node.ptCodeBound3>>16, result_node.ptCodeBound3&0xffff), &font, CV_RGB(0,168,0));
				cvCircle(srcImage3C, cvPoint(result_node.ptCodeBound4>>16, result_node.ptCodeBound4&0xffff), max(srcImage->width/256,4), CV_RGB(0,255,0), CV_FILLED);
				sprintf(txt, "%d", num_seq++);
				cvPutText(srcImage3C, txt, cvPoint(result_node.ptCodeBound4>>16, result_node.ptCodeBound4&0xffff), &font, CV_RGB(0,168,0));
				
				sprintf(txt, "%d", result_node.nCodeSeqNum);
				cvPutText(srcImage3C, txt, cvPoint(result_node.ptCodeCenter>>16, result_node.ptCodeCenter&0xffff), &font, CV_RGB(0,255,0));
				sprintf(txt, "Find %d Code(s)!", first_4char[0]);
				cvPutText(srcImage3C, txt, cvPoint(5, srcImage->width/32), &font, CV_RGB(0,255,0));
				sprintf(txt, "Code %d Result: ", result_node.nCodeSeqNum);
				strcat(txt, result_node.strCodeData);
				cvPutText(srcImage3C, txt, cvPoint(5, srcImage->width/32+(i+1)*srcImage->width/80), &font, CV_RGB(0,255,0));

				if(setCodeDigit == strlen(result_node.strCodeData) && setCodeDigit) {
					isMatchSetDigit = 1;
				}
			}
			// 保存图像
			if(3 == isSaveImage || 2 == isSaveImage)  {
				if(1 == isMatchSetDigit)	
					// cvSaveImage(pImgListTemp->RstImgPath1, isSaveLabel ? srcImage3C : srcImage);
					if(isSaveLabel)
						cvSaveImage(pImgListTemp->RstImgPath1, srcImage3C);
					else
						int mfret = MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath1);
				else {
					printf("本帧未检测到正确条码\n");
					nSuccessFrame--;
					nErrorFrame++;
					sprintf(txt, "Cannot Find Good Code!");
					cvPutText(srcImage3C, txt, cvPoint(5, srcImage->width/32+(first_4char[0]+1)*srcImage->width/64), &font, CV_RGB(255,0,0));
					//cvSaveImage(pImgListTemp->RstImgPath4, isSaveLabel ? srcImage3C : srcImage);
					if(isSaveLabel)
						cvSaveImage(pImgListTemp->RstImgPath4, srcImage3C);
					else
						MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath4);
				}
			}
		} else if(first_4char[0] == -2){
			printf("本帧未检测到条码\n");
			nFailFrame++;
			sprintf(txt, "Cannot Find Code!");
			cvPutText(srcImage3C, txt, cvPoint(5, srcImage->width/32), &font, CV_RGB(255,0,0));
			if(3 == isSaveImage || 1 == isSaveImage)	
				// cvSaveImage(pImgListTemp->RstImgPath3, isSaveLabel ? srcImage3C : srcImage);
				MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath3);
		} else if(first_4char[0] == 0 || first_4char[0] == -1){
			printf("本帧条码无法识别\n");
			nFailFrame++;
			sprintf(txt, "Cannot Read Code!");
			cvPutText(srcImage3C, txt, cvPoint(5, srcImage->width/32), &font, CV_RGB(255,0,0));
			if(3 == isSaveImage || 1 == isSaveImage)	
				//cvSaveImage(pImgListTemp->RstImgPath2, isSaveLabel ? srcImage3C : srcImage);
				MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath2);
		} else if(first_4char[0] < -100){
			printf("本帧返回值异常\n");
			nFailFrame++;
			sprintf(txt, "Error Occurred!");
			cvPutText(srcImage3C, txt, cvPoint(5, srcImage->width/32), &font, CV_RGB(255,0,0));
			if(1 <= isSaveImage)	
				//cvSaveImage(pImgListTemp->RstImgPath5, isSaveLabel ? srcImage3C : srcImage);
				MoveFile(pImgListTemp->SrcImgPath, pImgListTemp->RstImgPath5);
		}

#if RUN_CUT_PROCESS
		// 显示快递单切割结果
		if(status > 0) {
			printf("切割采纳结果 = %s\n", code_rslt);
			IplImage * iplCut = cvCreateImage(cvSize(image_width, image_height), 8, 1);
			IplImage * iplCutShow = cvCreateImage(cvSize(512, image_height * 512 / image_width), 8, 1);
			IplImage * iplCutShow3C = cvCreateImage(cvSize(512, image_height * 512 / image_width), 8, 3);
			uc2IplImageGray(in_data, iplCut);
			cvResize(iplCut, iplCutShow);
			cvCvtColor(iplCutShow, iplCutShow3C, CV_GRAY2BGR);
			cvInitFont(&font2, CV_FONT_HERSHEY_COMPLEX_SMALL, 1, 1, 0.0, 1, CV_AA);
			cvPutText(iplCutShow3C, code_rslt, cvPoint(5, 15), &font2, CV_RGB(0,255,0));
			cvNamedWindow("切割图像");
			cvShowImage("切割图像", iplCutShow3C);
			cvReleaseImage(&iplCut);
			cvReleaseImage(&iplCutShow);
			cvReleaseImage(&iplCutShow3C);
		}
#endif
#if		1
		// 显示包裹位置检测结果
		if(0 <= first_4char[0]) {
			memcpy(&result_node, results+first_4char[0]*sizeof(AlgorithmResult)+4, sizeof(AlgorithmResult));
			if(8 == result_node.nFlag) {
				printf("检测到包裹位置！\n", result_node.nCodeModuleWid);
				cvRectangle(srcImage3C, cvPoint(result_node.ptCodeBound1, result_node.ptCodeBound2),
					cvPoint(result_node.ptCodeBound3, result_node.ptCodeBound4), CV_RGB(0, 0, 255), max(srcImage->width/512,3));
				CvPoint pkgCentre = cvPoint(result_node.ptCodeCenter>>16, result_node.ptCodeCenter&0xffff);
				cvLine(srcImage3C, cvPoint(pkgCentre.x-srcImage->width/128, pkgCentre.y),
					cvPoint(pkgCentre.x+srcImage->width/128, pkgCentre.y), CV_RGB(0, 0, 255), max(srcImage->width/512,3));
				cvLine(srcImage3C, cvPoint(pkgCentre.x, pkgCentre.y-srcImage->width/128),
					cvPoint(pkgCentre.x, pkgCentre.y+srcImage->width/128), CV_RGB(0, 0, 255), max(srcImage->width/512,3));
				printf("包裹中心=(%d, %d), 包裹左上=(%d, %d), 包裹右下=(%d, %d)\n", 
					pkgCentre.x, pkgCentre.y, result_node.ptCodeBound1, result_node.ptCodeBound2, 
					result_node.ptCodeBound3, result_node.ptCodeBound4);
			} else {
				printf("未能检测到包裹位置！\n", result_node.nCodeModuleWid);
			}
		}
#endif

		cvResize(srcImage3C, srcShowImage);
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
	printf("识别结束！共完成%d帧，其中正确识别%d帧，错误识别%d帧，未识别%d帧，识别率%.3f\n",\
		nTotalCntFrame, nSuccessFrame, nErrorFrame, nFailFrame, nSuccessFrame*1.0/nTotalCntFrame);

	algorithm_release();

nExit:
	cvNamedWindow("end");
	cvWaitKey();
	return 0;
}

// 代码来自wxy
int FindImgs(char * pSrcImgPath, char * pRstImgPath1, char * pRstImgPath2,
	char * pRstImgPath3, char * pRstImgPath4, char * pRstImgPath5, char * pRstImgPath6, 
	std::list<PathElem> &ImgList)
{
	//源图片存在的目录
	TCHAR   szFileT1[MAX_PATH*2];
	lstrcpy(szFileT1,TEXT(pSrcImgPath));
	lstrcat(szFileT1, TEXT("*.*"));

	//结果图片存放的目录1
	TCHAR   RstAddr1[MAX_PATH*2]; 
	lstrcpy(RstAddr1,TEXT(pSrcImgPath));
	lstrcat(RstAddr1,TEXT(pRstImgPath1));
	_mkdir(RstAddr1);   //创建文件夹
	//结果图片存放的目录2
	TCHAR   RstAddr2[MAX_PATH*2]; 
	lstrcpy(RstAddr2,TEXT(pSrcImgPath));
	lstrcat(RstAddr2,TEXT(pRstImgPath2));
	_mkdir(RstAddr2);   //创建文件夹
	//结果图片存放的目录3
	TCHAR   RstAddr3[MAX_PATH*2]; 
	lstrcpy(RstAddr3,TEXT(pSrcImgPath));
	lstrcat(RstAddr3,TEXT(pRstImgPath3));
	_mkdir(RstAddr3);   //创建文件夹
	//结果图片存放的目录4
	TCHAR   RstAddr4[MAX_PATH*2]; 
	lstrcpy(RstAddr4,TEXT(pSrcImgPath));
	lstrcat(RstAddr4,TEXT(pRstImgPath4));
	_mkdir(RstAddr4);   //创建文件夹
	//结果图片存放的目录5
	TCHAR   RstAddr5[MAX_PATH*2]; 
	lstrcpy(RstAddr5,TEXT(pSrcImgPath));
	lstrcat(RstAddr5,TEXT(pRstImgPath5));
	_mkdir(RstAddr5);   //创建文件夹
	//结果图片存放的目录6
	TCHAR   RstAddr6[MAX_PATH*2]; 
	lstrcpy(RstAddr6,TEXT(pSrcImgPath));
	lstrcat(RstAddr6,TEXT(pRstImgPath6));
	_mkdir(RstAddr6);   //创建文件夹

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


