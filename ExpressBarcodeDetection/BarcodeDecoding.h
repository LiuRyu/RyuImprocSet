#ifndef BARCODE_DECODING_H
#define BARCODE_DECODING_H

#include "RyuCore.h"

/************************************************************************/
/* ʶ����󷵻ر�־                                                      */
/************************************************************************/
#define RYU_DECODERR_NULLPTR		(-1)
#define RYU_DECODERR_SHORTLEN		(-2)
#define RYU_DECODERR_NOST			(-3)
#define RYU_DECODERR_SHORTLEN_ST	(-4)
#define RYU_DECODERR_MATCHFAILED	(-5)
#define RYU_DECODERR_VERIFYFAILED	(-6)
#define RYU_DECODERR_TRANSCFAILED	(-7)

/************************************************************************/
/* ������ģ��	                                                        */
/************************************************************************/
typedef struct DecodeDemarcateNode
{
	int type;
	int idx_b;
	int idx_e;
	int idxex_b;
	int idxex_e;
	int acc;
	int max_v;
	int count;
	float gravity;
} DecodeDemarcateNode;

int BarcodeDecoding_init( int max_width, int max_height );

int DecodeBarcode( unsigned char * bina, int width, int height, int sliceH, 
	char * code_result, int * code_type, int * char_num, int * module_num, 
	int * code_direct, int * leftOffset, int * rightOffset, float * minModule);

// v2.6�汾��ʼʹ�õĽ����㷨
int BarcodeDecoding_run( unsigned char * im, int * integr, int width, int height, int slice_height, 
						char * code_result, int * code_type, int * char_num, int * module_num, 
						int * code_direct, int * leftOffset, int * rightOffset, float * minModule);

// int BarcodeDecoding_Integrogram( unsigned char * im, int * integr, int width, int height, int slice_height, 
// 								char * code_result, int * code_type, int * char_num, int * module_num, 
// 								int * code_direct, int * leftOffset, int * rightOffset);

void BarcodeDecoding_release();


/************************************************************************/
/* Code128����ģ��                                                      */
/************************************************************************/
int allocVariableMemStorage_code128(unsigned char * heapPtr, int heapSize);

int RecgCode128(int * decode_arr, int arr_count, char * code_result, int * code_digit, 
				int * code_module, int * code_direct, int * code_idxL, int * code_idxR);

int Decoder_code128(int * decode_arr, int arr_count, 
					char * code_result, int * code_digit, int * code_module, 
					int * code_direct, int * code_idxL, int * code_idxR);


#endif


