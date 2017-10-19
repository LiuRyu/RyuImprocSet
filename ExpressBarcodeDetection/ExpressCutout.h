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

// 1--算法初始化
#ifndef INPUTTYPE_FILEPATH
EXPRESSCUTOUT_LIBDLL int ExpressCutout_init(int imageWidth, int imageHeight);
#else
EXPRESSCUTOUT_LIBDLL int ExpressCutout_init(int imageWidth, int imageHeight, char * LogPath);
#endif

// 2--参数设置--设置快递单宽、高、条码宽、条码左上角相对快递单左上角的偏移量(x,y)等参数
EXPRESSCUTOUT_LIBDLL int ExpressCutout_set(float ExBillWidth, float ExBillHeight, float codeWidth, float codeOffsetX, float codeOffsetY);

// 3--算法运行--条形码识别及快递单切割
#ifndef INPUTTYPE_FILEPATH
EXPRESSCUTOUT_LIBDLL int ExpressCutout_run(unsigned char * inputImage, int * imageWidth, int * imageHeight, char * code, int * codeLength);
#else
EXPRESSCUTOUT_LIBDLL int ExpressCutout_run(char * InputImagePath, char * CutOutPath, char * SuccessPath, 
							char * FailPath, char * ErrorPath, char * code, int * codeLength);
#endif

// 4--参数重置--使设置参数失效，失效后算法运行只识别条码，不切割快递单
EXPRESSCUTOUT_LIBDLL void ExpressCutout_reset();

// 5--算法释放
EXPRESSCUTOUT_LIBDLL void ExpressCutout_release();

EXPRESSCUTOUT_LIBDLL unsigned char * ExpressCutout_getFullResult();

EXPRESSCUTOUT_LIBDLL int ExpressCutout_getSuccessCount();



#endif


