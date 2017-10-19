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
	int nCodeSymbology;	// ��������
	int nCodeWidth;		// ������(����)
	int nCodeHeight;	// ����߶�(����)
	int nCodeDgtNum;	// ����λ��
	int nCodeBlkNum;	// �����к�ɫ����
	int nGradThre;		// �ݶ���ֵ
	int nClusMinNum;	// ����ʱ��С����������ֵ
	//int reserve1;		// Ԥ��1
	int nCodeCount;  	// �滻Ԥ��1����ʶ����������0-�����ƣ�>0-�涨����
	//int reserve2;	 	// Ԥ��2
	int nMultiPkg;		// �滻Ԥ��2����������ģ�鿪��
	int reserve3;  		// Ԥ��3
	int reserve4;  		// Ԥ��4
	int reserve5;  		// Ԥ��5
	int reserve6;	 	// Ԥ��6
	int reserve7;  		// Ԥ��7
	int reserve8;  		// Ԥ��8
} AlgorithmParamSet;

typedef struct tagAlgorithmResult
{
	int  nFlag;				// ʶ���־
	int  nCodeSymbology;	// ��������
	int  nCodeCharNum;		// �ַ�λ��
	char strCodeData[128];	// ������
	int  ptCodeCenter;		// ������������(��16:X,��16:Y)
	int  ptCodeBound1;		// ���붥������1
	int  ptCodeBound2;		// ���붥������2
	int  ptCodeBound3;		// ���붥������3
	int  ptCodeBound4;		// ���붥������4
	int  nCodeOrient;		// ������ת�Ƕ�
	//int  reserve1;  		// Ԥ��1
	int  nCodeWidth;		// �滻Ԥ��1��������
	//int  reserve2;		// Ԥ��2
	int  nCodeHeight;		// �滻Ԥ��2������߶�
	//int  reserve3;  		// Ԥ��3
	int  nCodeModuleWid;	// �滻Ԥ��3�����뵥λ���*1024
	//int  reserve4;  		// Ԥ��4
	int  nCodeSeqNum;		// �滻Ԥ��4���������
	int  reserve5;  		// Ԥ��5
	int  reserve6;	 		// Ԥ��6
	int  reserve7;  		// Ԥ��7
	int  reserve8;  		// Ԥ��8
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