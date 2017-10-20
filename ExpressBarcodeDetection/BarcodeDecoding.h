#ifndef BARCODE_DECODING_H
#define BARCODE_DECODING_H

#include "RyuCore.h"


typedef struct DecodeDemarcateNode
{
	int type;
	int idx_b;
	int idx_e;
} DecodeDemarcateNode;

int BarcodeDecoding_init( int max_width, int max_height );

int DecodeBarcode( unsigned char * bina, int width, int height, int sliceH, 
	char * code_result, int * code_type, int * char_num, int * module_num, 
	int * code_direct, int * leftOffset, int * rightOffset);

int BarcodeDecoding_Integrogram( unsigned char * im, int * integr, int width, int height,
								char * code_result, int * code_type, int * char_num, int * module_num, 
								int * code_direct, int * leftOffset, int * rightOffset);

void BarcodeDecoding_release();

#endif


