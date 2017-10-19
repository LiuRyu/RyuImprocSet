#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

#include "BarcodeImageproc.h"
#include "ExtractActiveRegion.h"

#ifdef	_DEBUG_
#ifdef  _DEBUG_EXTRACT_ACTIVEREGION
#include "OpenCv_debugTools.h"
#endif
#endif

const int gnExtrActvRegnConst = 312;

unsigned char * gucExtrActvRegnImageBuff1 = 0;

int gnExtrActvRegnInitFlag = 0;


int ExtractActiveRegion(unsigned char * img, int wid, int hei)
{
	int nErrorRet = -1150200;
	int nRet = 0;

	float fInterval = 0.0;
	int nRszWid = 0, nRszHei = 0;

	int i = 0, j = 0;

	unsigned char * pBuf = 0, * pBufL = 0;
	unsigned char * pIn = 0, * pInL = 0;

	float fTmp1 = 0.0, fTmp2 = 0.0;
	int nTmp1 = 0, nTmp2 = 0, nTmp3 = 0;

	int threshold = 0;

	if(!img || wid <= 0 || hei <= 0) {
		nRet = nErrorRet - 1;
		goto nExit;
	}

	if(wid >= hei) {
		nRszWid = gnExtrActvRegnConst;
		fInterval = (wid - 1) * 1.0 / gnExtrActvRegnConst;
		nRszHei = (hei - 1) * 1.0 / fInterval;
	} else {
		nRszHei = gnExtrActvRegnConst;
		fInterval = (hei - 1) * 1.0 / gnExtrActvRegnConst;
		nRszWid = (wid - 1) * 1.0 / fInterval;
	}

	// 采样图像
	pInL = img;
	pBufL = gucExtrActvRegnImageBuff1;
	for(j = 0; j < nRszHei; j++) {
		pInL = img + ((int)(fTmp2 + 0.5)) * wid;
		pBuf = pBufL;
		fTmp1 = 0.0;
		for(i = 0; i < nRszWid; i++) {
			nTmp1 = (int)(fTmp1 + 0.5);
			*(pBuf++) = pInL[nTmp1];
			fTmp1 += fInterval;
		}
		fTmp2 += fInterval;
		pBufL += nRszWid;
	}

	// 计算阈值
	AutoContrastAnalyze(gucExtrActvRegnImageBuff1, nRszWid, nRszHei, nRszWid, 
			0.03, 0, &nTmp1, 0, &nTmp2, &nTmp3);

	threshold = (nTmp1 + nTmp2 + nTmp3) / 3;

#ifdef	_DEBUG_
#ifdef  _DEBUG_EXTRACT_ACTIVEREGION
	printf("AutoContrastAnalyze high_scale=%d, min=%d, max=%d, threshold=%d\n", 
		nTmp1, nTmp2, nTmp3, threshold);

	IplImage * iplRszImg = cvCreateImage(cvSize(nRszWid, nRszHei), 8, 1);
	uc2IplImageGray(gucExtrActvRegnImageBuff1, iplRszImg);
	cvNamedWindow("Extract Resize");
	cvShowImage("Extract Resize", iplRszImg);
	//cvWaitKey();

	cvReleaseImage(&iplRszImg);
#endif
#endif

	// 二值化
	ryuThreshold(gucExtrActvRegnImageBuff1, gucExtrActvRegnImageBuff1, 
		nRszWid, nRszHei, nRszWid, threshold);
#ifdef	_DEBUG_
#ifdef  _DEBUG_EXTRACT_ACTIVEREGION
	IplImage * iplRszBina = cvCreateImage(cvSize(nRszWid, nRszHei), 8, 1);
	uc2IplImageGray(gucExtrActvRegnImageBuff1, iplRszBina);
	cvNamedWindow("Extract Binarize");
	cvShowImage("Extract Binarize", iplRszBina);
	cvWaitKey();

	cvReleaseImage(&iplRszBina);

	cvDestroyWindow("Extract Resize");
	cvDestroyWindow("Extract Binarize");
#endif
#endif


nExit:
	return nRet;
}


int  ExtractActiveRegion_init()
{
	int nErrorRet = -1150100;

	if( gnExtrActvRegnInitFlag ) {
#ifdef	_PRINT_PROMPT
		printf("ERROR! Unexpected value of gnExtrActvRegnInitFlag in ExtractActiveRegion_init\n");
#endif
		return nErrorRet-1;
	}

	gucExtrActvRegnImageBuff1 = (unsigned char *) malloc(gnExtrActvRegnConst * gnExtrActvRegnConst * sizeof(unsigned char));
	if( !gucExtrActvRegnImageBuff1 ) {
#ifdef	_PRINT_PROMPT
		printf("ERROR! Cannot alloc memory for gucExtrActvRegnImageBuff1 in ExtractActiveRegion_init\n");
#endif
		return nErrorRet-2;
	}

	gnExtrActvRegnInitFlag = 1;
	return 1;
}


void ExtractActiveRegion_release()
{
	gnExtrActvRegnInitFlag = 0;

	if(gucExtrActvRegnImageBuff1) {
		free(gucExtrActvRegnImageBuff1);
		gucExtrActvRegnImageBuff1 = 0;
	}

	return;
}



