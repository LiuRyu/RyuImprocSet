#ifndef _EXPRESS_CUTOUT
#define _EXPRESS_CUTOUT

#ifdef EXPRESSCUTOUT_LIBDLL
#define EXPRESSCUTOUT_LIBDLL extern "C" _declspec(dllexport) 
#else
#define EXPRESSCUTOUT_LIBDLL extern "C" _declspec(dllimport) 
#endif

//#define INPUTTYPE_FILEPATH
#ifdef INPUTTYPE_FILEPATH
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#endif

EXPRESSCUTOUT_LIBDLL int Add(int a, int b);

EXPRESSCUTOUT_LIBDLL void PretreatmentImage(char imageData[], int *imageWidth, int *imageHeight, char* code, int codeLength);

// 1--�㷨��ʼ��
#ifndef INPUTTYPE_FILEPATH
EXPRESSCUTOUT_LIBDLL int ExpressCutout_init(int imageWidth, int imageHeight);
#else
EXPRESSCUTOUT_LIBDLL int ExpressCutout_init(int imageWidth, int imageHeight, char * LogPath);
#endif

// 2--��������--���ÿ�ݵ����ߡ�������������Ͻ���Կ�ݵ����Ͻǵ�ƫ����(x,y)�Ȳ���
EXPRESSCUTOUT_LIBDLL int ExpressCutout_set(float ExBillWidth, float ExBillHeight, float codeWidth, float codeOffsetX, float codeOffsetY);

// 3--�㷨����--������ʶ�𼰿�ݵ��и�
#ifndef INPUTTYPE_FILEPATH
EXPRESSCUTOUT_LIBDLL int ExpressCutout_run(unsigned char * inputImage, int * imageWidth, int * imageHeight, char * code, int * codeLength);
#else
EXPRESSCUTOUT_LIBDLL int ExpressCutout_run(char * InputImagePath, char * CutOutPath, char * SuccessPath, 
							char * FailPath, char * ErrorPath, char * code, int * codeLength);
#endif

// 4--��������--ʹ���ò���ʧЧ��ʧЧ���㷨����ֻʶ�����룬���и��ݵ�
EXPRESSCUTOUT_LIBDLL void ExpressCutout_reset();

// 5--�㷨�ͷ�
EXPRESSCUTOUT_LIBDLL void ExpressCutout_release();

EXPRESSCUTOUT_LIBDLL unsigned char * ExpressCutout_getFullResult();

EXPRESSCUTOUT_LIBDLL int ExpressCutout_getSuccessCount();



#endif


