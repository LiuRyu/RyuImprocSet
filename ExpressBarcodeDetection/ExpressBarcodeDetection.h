#ifndef _EXPRESS_BARCODE_DETECTION_H
#define _EXPRESS_BARCODE_DETECTION_H

//#define EXPRESS_BARCODE_DETECT_LIBDLL
#ifdef  EXPRESS_BARCODE_DETECT_LIBDLL
#define EXPRESS_BARCODE_DETECT_LIBDLL extern "C" _declspec(dllexport) 
#else
#define EXPRESS_BARCODE_DETECT_LIBDLL extern "C" _declspec(dllimport) 
#endif

typedef struct tagAlgorithmParamSet
{
	int nFlag;
	int nCodeSymbology;	// 条码类型
	int nCodeWidth;		// 条码宽度(像素)
	int nCodeHeight;	// 条码高度(像素)
	int nCodeDgtNum;	// 解码位数
	int nCodeBlkNum;	// 条码中黑色条数
	int nGradThre;		// 梯度阈值
	int nClusMinNum;	// 聚类时最小特征个数阈值
	//int reserve1;		// 预留1
	int nCodeCount;  	// 替换预留1，待识别条码数，0-无限制，>0-规定个数
	//int reserve2;	 	// 预留2
	int nMultiPkg;		// 替换预留2，多包裹检测模块开关
	int reserve3;  		// 预留3
	int reserve4;  		// 预留4
	int reserve5;  		// 预留5
	int reserve6;	 	// 预留6
	int reserve7;  		// 预留7
	int reserve8;  		// 预留8
} AlgorithmParamSet;

typedef struct tagAlgorithmResult
{
	int  nFlag;				// 识别标志
	int  nCodeSymbology;	// 条码类型
	int  nCodeCharNum;		// 字符位数
	char strCodeData[128];	// 解码结果
	int  ptCodeCenter;		// 条码中心坐标(高16:X,低16:Y)
	int  ptCodeBound1;		// 条码顶点坐标1
	int  ptCodeBound2;		// 条码顶点坐标2
	int  ptCodeBound3;		// 条码顶点坐标3
	int  ptCodeBound4;		// 条码顶点坐标4
	int  nCodeOrient;		// 条码旋转角度
	//int  reserve1;  		// 预留1
	int  nCodeWidth;		// 替换预留1，条码宽度
	//int  reserve2;		// 预留2
	int  nCodeHeight;		// 替换预留2，条码高度
	//int  reserve3;  		// 预留3
	int  nCodeModuleWid;	// 替换预留3，条码单位宽度*1024
	//int  reserve4;  		// 预留4
	int  nCodeSeqNum;		// 替换预留4，条码序号
	int  reserve5;  		// 预留5
	int  reserve6;	 		// 预留6
	int  reserve7;  		// 预留7
	int  reserve8;  		// 预留8
} AlgorithmResult;


EXPRESS_BARCODE_DETECT_LIBDLL int algorithm_init(int max_width, int max_height, 
										unsigned char ** results);

EXPRESS_BARCODE_DETECT_LIBDLL int algorithm_run(int lrning_flag, unsigned char * in_data, 
										int width, int height, unsigned char ** results);

EXPRESS_BARCODE_DETECT_LIBDLL void algorithm_release();

void CoorRelative2Absolute(int centAbs_x, int centAbs_y, int coorRltv_x, int coorRltv_y, 
	unsigned char rttAngle, int * coorAbs_x, int * coorAbs_y);

void CoorAbsolute2Relative(int centAbs_x, int centAbs_y, int coorAbs_x, int coorAbs_y, 
	unsigned char rttAngle, int * coorRltv_x, int * coorRltv_y);

#endif