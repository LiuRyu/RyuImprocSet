#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "RyuCore.h"

#include "BarcodeDecoding.h"

#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
#include "OpenCv_debugTools.h"
#endif
#endif

#define I2OF5_MIN_DECODEARR_COUNT	(36)
#define I2OF5_MIN_SEQARR_COUNT		(5)

const short gnDecoderModuleI2of5[13] = {
	0x6,   0x11,  0x9,   0x18,  0x5, 
//  00110, 10001, 01001, 11000, 00101, 
	0x14,  0xC,   0x3,   0x12,  0xA, 
//	10100, 01100, 00011, 10010, 01010, 
	0x0,  0x4, 0x1
//	0000, 100, 001(�����ʶλ)
};

// static int gnDecoderSeqArrI2of5[128] = {0};
// static int gnDecoderFaithArrI2of5[128] = {0};

const int maxArrLength_I2of5= 128;

int * gnDecoderProcArr_I2of5 = 0;

int * gnDecoderSeqArrI2of5 = 0;
int * gnDecoderFaithArrI2of5 = 0;

int * gnDecoderStartArr_I2of5 = 0;
int * gnDecoderStopArr_I2of5 = 0;
RyuPoint * gptDecoderStArr_I2of5 = 0;

int CodeStartStopMatch_I2of5( int * decode_arr, int arr_count,  int * start_idx, int * stop_idx );

int CodeModelMatch_I2of5(int * decode_arr, int arr_count, int * seq_arr, int * faith_arr);

int CheckDigitVerify_I2of5(int * seq_arr, int * faith_arr, int seq_cnt, int isCheckDigit);

int Transcode_I2of5( int * seq_arr, int seq_cnt, char * strResult );


int DcdrFindCodeSt_I2of5(int * decode_arr, int arr_count);

int DcdrCheckStValid_I2of5(int start_idx, int stop_idx);

int DcdrModelMatch_I2of5(int * decode_arr, int arr_count, int * seq_arr, int * faith_arr, int * seq_cnt);

int DcdrCheckVerify_I2of5(int * seq_arr, int seq_cnt, int isCheckDigit);

int DcdrTranscode_I2of5( int * seq_arr, int seq_cnt, int isCheckDigit, 
						int isKeep1stZero, char * strResult );


int allocVariableMemStorage_I2of5(unsigned char * heapPtr, int heapSize)
{
	int offset = 0;

	gnDecoderProcArr_I2of5 = (int *)(heapPtr + offset);
	offset += 6 * maxArrLength_I2of5 * sizeof(int);
	if(offset > heapSize)
		return 0;

	gnDecoderSeqArrI2of5 = (int *)(heapPtr + offset);
	offset += maxArrLength_I2of5 * sizeof(int);
	if(offset > heapSize)
		return 0;

	gnDecoderFaithArrI2of5 = (int *)(heapPtr + offset);
	offset += maxArrLength_I2of5 * sizeof(int);
	if(offset > heapSize)
		return 0;

	gnDecoderStartArr_I2of5 = (int *)(heapPtr + offset);
	offset += maxArrLength_I2of5 * sizeof(int);
	if(offset > heapSize)
		return 0;

	gnDecoderStopArr_I2of5 = (int *)(heapPtr + offset);
	offset += maxArrLength_I2of5 * sizeof(int);
	if(offset > heapSize)
		return 0;

	gptDecoderStArr_I2of5 = (RyuPoint *)(heapPtr + offset);
	offset += maxArrLength_I2of5 * sizeof(RyuPoint);
	if(offset > heapSize)
		return 0;

	return 1;
}

void resetVariableMemStorage_I2of5()
{
	gnDecoderProcArr_I2of5 = 0;

	gnDecoderSeqArrI2of5 = 0;
	gnDecoderFaithArrI2of5 = 0;

	gnDecoderStartArr_I2of5 = 0;
	gnDecoderStopArr_I2of5 = 0;
	gptDecoderStArr_I2of5 = 0;

	return;
}



int RecgCodeI2of5(int * decode_arr, int arr_count, char * code_result, int * code_digit, 
	int * code_module, int * code_direct, int * code_idxL, int * code_idxR)
{
	int nRet = 0, status = 0;

	int nDigitCnt = 0, nModuleCnt = 0, nDirection = 0;
	int nStartIdx = 0, nStopIdx = 0;
	int nArrCount = 0, nSeqCount = 0;

	if(!decode_arr || !code_result || !code_digit || !code_module
		|| !code_direct || !code_idxL || !code_idxR) {
#ifdef	_PRINT_PROMPT
			printf( "ERROR! Invalid input of RecgI2of5, decode_arr=0x%x, code_result=0x%x\
					, code_digit=0x%x, code_module=0x%x, code_direct=0x%x, code_idxL=0x%x\
					, code_idxR=0x%x\n", decode_arr, code_result, code_digit, code_module, \
					code_direct, code_idxL, code_idxR );
#endif
			nRet = -1;
			goto nExit;
	}

	// ���ó�ʼ״̬
	code_result[0] = 0;
	*code_digit = *code_module = *code_direct = *code_idxL = 0;
	*code_idxR = arr_count - 1;

	if( arr_count < I2OF5_MIN_DECODEARR_COUNT ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, too small arr_count: %d\n", arr_count);
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	status = CodeStartStopMatch_I2of5( decode_arr, arr_count, &nStartIdx, &nStopIdx );
	if(1 != status && 2 != status) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf( "Unexpected return of CodeStartStopMatch_I2of5, cannot find code start/stop,\
				return=%d\n", status );
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	nDirection = (1 == status) ? 1 : -1;		// ���뷽��

	nArrCount = nStopIdx - nStartIdx + 1;	// �ڰ���
	if( nArrCount < I2OF5_MIN_DECODEARR_COUNT || 0 != (nArrCount-7) % 10) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, bad nColCount: %d\n", nArrCount);
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	status = CodeModelMatch_I2of5( decode_arr+nStartIdx, nArrCount, 
		gnDecoderSeqArrI2of5, gnDecoderFaithArrI2of5 );
	if( status < I2OF5_MIN_SEQARR_COUNT ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, bad nSeqCount: %d\n", status);
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	nSeqCount = status;
	nModuleCnt = nSeqCount * 8 + 9;

	/*
	// У��λ���飬��������У��λ��I2of5����
	status = CheckDigitVerify_I2of5( gnDecoderSeqArrI2of5, gnDecoderFaithArrI2of5, nSeqCount, 0 );
	if(1 != status ) {
#ifdef	_DEBUG_

#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, check digit verification failed, return=%d\n", status);
#endif
#endif
		nRet = 0;
		goto nExit;
	}
	*/

	status = Transcode_I2of5( gnDecoderSeqArrI2of5, nSeqCount, code_result );
	if( status <= 0 ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, bad return of Transcode_I2of5, return=%d\n", status);
#endif
#endif
		code_result[0] = 0;
		nRet = 0;
		goto nExit;
	}

	nDigitCnt = status;

	*code_digit = nDigitCnt;
	*code_module = nModuleCnt;
	*code_direct = nDirection;
	*code_idxL = (nDirection > 0) ? nStartIdx : (arr_count-1-nStopIdx);
	*code_idxR = (nDirection > 0) ? nStopIdx : (arr_count-1-nStartIdx);
	nRet = 1;

nExit:
	return nRet;
}


int Decoder_I2of5(int * decode_arr, int arr_count, DecodeResultNode * result)
{
	int nRet = 0, status = 0;
	int i = 0, j = 0;

	char strResult[CODE_RESULT_ARR_LENGTH] = {0};

	int nDigitCnt = 0, nModuleCnt = 0, nDirection = 0, nDirection0 = 0;
	int nStartIdx = 0, nStopIdx = 0;
	int nArrCount = 0, nSeqCount = 0, nEffSeqCount = 0;
	int nIsRcg = 0;

	int nStCnt = 0;
	RyuPoint * pStpArr = gptDecoderStArr_I2of5;

	int * pProcArr = gnDecoderProcArr_I2of5;
	int * pSeqArr = gnDecoderSeqArrI2of5;

	if(!decode_arr || !result) {
#ifdef	_PRINT_PROMPT
		printf( "ERROR! Invalid input of Decoder_I2of5, decode_arr = 0x%x, result=0x%x\n", 
			decode_arr, result);
#endif
		nRet = RYU_DECODERR_NULLPTR;
		goto nExit;
	}

	// ���ó�ʼ״̬
	memset(result, 0, sizeof(DecodeResultNode));

	if( arr_count < I2OF5_MIN_DECODEARR_COUNT ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, too small arr_count: %d\n", arr_count);
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	nStCnt = DcdrFindCodeSt_I2of5(decode_arr, arr_count);
	if(0 >= nStCnt) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("[Decoder_I2of5]- Cannot find code start/stop, bad nStCnt: %d\n", nStCnt);
#endif
#endif
		nRet = RYU_DECODERR_NOST;
		goto nExit;
	}

	for(i = 0; i < nStCnt; i++) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("[Decoder_I2of5]- ******** St loop %d(/%d) ********\n", i, nStCnt);
		printf("[Decoder_I2of5]- start_idx = %d, stop_idx = %d\n", pStpArr[i].x, pStpArr[i].y);
#endif
#endif
		if(pStpArr[i].x < pStpArr[i].y) {
			nDirection = 1;
			nArrCount = pStpArr[i].y - pStpArr[i].x + 1;
			for(j = 0; j < nArrCount; j++) {
				pProcArr[j] = decode_arr[pStpArr[i].x+j];
			}
		} else {
			nDirection = -1;
			nArrCount = pStpArr[i].x - pStpArr[i].y + 1;
			for(j = 0; j < nArrCount; j++) {
				pProcArr[j] = decode_arr[pStpArr[i].x-j];
			}
		}

		nDirection0 = i ? nDirection0 : nDirection;

		status = DcdrModelMatch_I2of5(pProcArr, nArrCount, 
			pSeqArr, &nSeqCount, &nEffSeqCount);
		if(1 != status) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("[Decoder_I2of5]- Cannot match code model, bad return: %d\n", status);
#endif
#endif
			continue;
		}

		status = DcdrCheckVerify_I2of5(pSeqArr, nSeqCount, 0);
		if(1 != status ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("[Decoder_I2of5]- Cannot go-through digit verification, bad return: %d\n", status);
#endif
#endif
			continue;
		}

		status = DcdrTranscode_I2of5(pSeqArr, nSeqCount, 0, 1, strResult);
		if( status <= 0 ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("[Decoder_I2of5]- Cannot transcode, bad return: %d\n", status);
#endif
#endif
			strResult[0] = 0;
			continue;
		}

		nStartIdx = pStpArr[i].x;
		nStopIdx = pStpArr[i].y;
		nDigitCnt = status;
		nModuleCnt = nSeqCount * 8 + 9;
		nIsRcg = 1;
		break;
	}

	if(1 == nIsRcg) {
		result->flag = 1;
		result->code_type = 0x8;
		result->rslt_digit = nDigitCnt;
		result->code_module = nModuleCnt;
		result->code_unit = nSeqCount;
		result->code_unit_r = nEffSeqCount;
		result->code_direct = nDirection;
		result->left_idx = (nDirection > 0) ? nStartIdx : nStopIdx;
		result->right_idx = (nDirection > 0) ? nStopIdx : nStartIdx;
		memcpy(result->strCodeData, strResult, CODE_RESULT_ARR_LENGTH * sizeof(char));
		nRet = 1;
	} else {
		result->code_direct = nDirection0;
		nRet = 0;
	}

nExit:
	return nRet;
}


int CodeStartStopMatch_I2of5( int * decode_arr, int arr_count,  int * start_idx, int * stop_idx )
{
	int nRet = 0;
	int i = 0, j = 0;
	int * pCodeCol = 0;

	int nStartIdx = 0, nStopIdx = 0;
	int nIsFindL = 0, nIsFindR = 0;
	int s = 0, t = 0, f[14] = {0}, r[14] = {0}; 
	int mask = 0, loop = 0;

	int k = 0, tmp = 0, sort[14] = {0};

	const int half_fixed = 1 << (FLOAT2FIXED_SHIFT_DIGIT - 1);

	*start_idx = *stop_idx = 0;

	//////////////////////////////////////////////////////////////////////////
	// Ѱ��������ʼλ������ֹ����λ
	pCodeCol = decode_arr;
	//////////////////////////////////////////////////////////////////////////
	// 2.0.5.2�汾�Ż����������Ҽ����߽磬��������
	//for( i = 0; i < arr_count - 9; i++ ) {
	for( i = 0; i < 6; i++ ) {
		if( pCodeCol[0] > 0 ) {	// �涨�Ӻ�ɫ��ʼ
			pCodeCol++;
			continue;
		}

		// ȡ����һ���ַ�������ֵ
		s = -pCodeCol[4] + pCodeCol[5] - pCodeCol[6] + pCodeCol[7]
			-pCodeCol[8] + pCodeCol[9] - pCodeCol[10] + pCodeCol[11] - pCodeCol[12] + pCodeCol[13];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf( "Unexpected format of decode_arr in CodeStartStopMatch_I2of5, s=%d\n", s );
#endif
#endif
			nRet = -1;
			goto nExit;
		}

		//////////////////////////////////////////////////////////////////////////
		// 2.0.4.2�汾�Ľ�
		// �ɰ���룬ע��
		// �Աȿ�ʼλ�ͷ������λ
		// 		mask = 0;
		// 		t = (s << FLOAT2FIXED_SHIFT_DIGIT) >> 3;
		// 		for( j = 0; j < 9; j++ ) {
		// 			f[j] = abs(pCodeCol[j]) << FLOAT2FIXED_SHIFT_DIGIT;
		// 			r[j] = (f[j] > t) ? 1 : 0;
		// 			mask  |= (r[j] << (8-j));
		// 		}

		//////////////////////////////////////////////////////////////////////////
		// �°���룬���EMS������������иĽ����Գ��Ƚ�������õ���ֵ
		// ����ͨ����ֵ
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;

		// ������Ԫ�ؿ�Ƚ�������
		for( j = 0; j < 14; j++ ) {
			sort[j] = abs(pCodeCol[j]);
		}
		for( j = 0; j < 14; j++ ) {
			for( k = j+1; k < 14; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[3]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[13] * 6 ) {
				pCodeCol++;
				continue;
		}
		// ������ֵ
		tmp = (sort[3] + sort[4]) >> 1;
		// ��ȡ����
		mask = 0;
		for( j = 0; j < 4; j++ ) {
			r[j] = (abs(pCodeCol[j]) > tmp) ? 1 : 0;
			mask  |= (r[j] << (3-j));
		}
		// 2.0.4.2�汾�Ľ�����
		//////////////////////////////////////////////////////////////////////////

		// �Աȿ�ʼλ
		if( mask == gnDecoderModuleI2of5[10] ) {
			nStartIdx = i;
			nIsFindL = 1;
			break;
		} 

		//////////////////////////////////////////////////////////////////////////
		// ��֤�������λ
		s = s + pCodeCol[3] - pCodeCol[13];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf( "Unexpected format of decode_arr in CodeStartStopMatch_I2of5, s=%d\n", s );
#endif
#endif
			nRet = -1;
			goto nExit;
		}
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;
		for( j = 0; j < 13; j++ ) {
			sort[j] = abs(pCodeCol[j]);
		}
		for( j = 0; j < 13; j++ ) {
			for( k = j+1; k < 13; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[5]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[12] * 6 ) {
				pCodeCol++;
				continue;
		}
		// ������ֵ
		tmp = (sort[4] + sort[5]) >> 1;
		// ��ȡ����
		mask = 0;
		for( j = 0; j < 3; j++ ) {
			r[j] = (abs(pCodeCol[j]) > tmp) ? 1 : 0;
			mask  |= (r[j] << (2-j));
		}
		// �Աȿ�ʼ����λ
		if( mask == gnDecoderModuleI2of5[12] ) {
			nStopIdx = i;
			nIsFindL = 2;
			break;
		} 
		//////////////////////////////////////////////////////////////////////////

		pCodeCol++;
	}

	// �Ҳ�����ʼ����ֹλ
	if( nIsFindL != 1 && nIsFindL != 2 ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, cannot find start/~end in CodeStartStopMatch_I2of5\n");
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	// ��������������ֹλ����ʼ����λ
	pCodeCol = decode_arr + arr_count - 1;
	//////////////////////////////////////////////////////////////////////////
	// 2.0.4.2�汾�Ż����������Ҽ����߽磬��������
	//for(i = 0; i < arr_count - 9; i++) {
	for(i = 0; i < 6; i++) {
		if( pCodeCol[0] > 0 ) {	// �涨�Ӻ�ɫ��ʼ
			pCodeCol--;
			continue;
		}

		// ȡ����һ���ַ�������ֵ
		s = -pCodeCol[-4] + pCodeCol[-5] - pCodeCol[-6] + pCodeCol[-7]
		-pCodeCol[-8] + pCodeCol[-9] - pCodeCol[-10] + pCodeCol[-11] - pCodeCol[-12] + pCodeCol[-13];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf( "Unexpected format of decode_arr in CodeStartStopMatch_I2of5, s=%d\n", s );
#endif
#endif
			nRet = -1;
			goto nExit;
		}

		//////////////////////////////////////////////////////////////////////////
		// 2.0.4.2�汾�Ľ�
		// �ɰ���룬ע��
		// �Աȷ���ʼλ����ֹλ
		// 		mask = 0;
		// 		t = (s << FLOAT2FIXED_SHIFT_DIGIT) >> 3;
		// 		for( j = 0; j < 9; j++ ) {
		// 			f[j] = abs(pCodeCol[-j]) << FLOAT2FIXED_SHIFT_DIGIT;
		// 			r[j] = (f[j] > t) ? 1 : 0;
		// 			mask |= (r[j] << (8-j));
		// 		}

		//////////////////////////////////////////////////////////////////////////
		// �°���룬���EMS������������иĽ����Գ��Ƚ�������õ���ֵ
		// ����ͨ����ֵ
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;

		// ������Ԫ�ؿ�Ƚ�������
		for( j = 0; j < 14; j++ ) {
			sort[j] = abs(pCodeCol[-j]);
		}
		for( j = 0; j < 14; j++ ) {
			for( k = j+1; k < 14; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[3]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[13] * 6 ) {
				pCodeCol--;
				continue;
		}
		// ������ֵ
		tmp = (sort[3] + sort[4]) >> 1;
		// ��ȡ����
		mask = 0;
		for( j = 0; j < 4; j++ ) {
			r[j] = (abs(pCodeCol[-j]) > tmp) ? 1 : 0;
			mask  |= (r[j] << (3-j));
		}
		// 2.0.4.2�汾�Ľ�����
		//////////////////////////////////////////////////////////////////////////

		if( mask == gnDecoderModuleI2of5[10] ) {	// ���ַ���ʼ����
			nStartIdx = arr_count - i - 1;
			nIsFindR = 1;
			break;
		}

		s = s + pCodeCol[-3] - pCodeCol[-13];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf( "Unexpected format of decode_arr in CodeStartStopMatch_I2of5, s=%d\n", s );
#endif
#endif
			nRet = -1;
			goto nExit;
		}
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;
		for( j = 0; j < 13; j++ ) {
			sort[j] = abs(pCodeCol[-j]);
		}
		for( j = 0; j < 13; j++ ) {
			for( k = j+1; k < 13; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[5]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[12] * 6 ) {
				pCodeCol--;
				continue;
		}
		// ������ֵ
		tmp = (sort[4] + sort[5]) >> 1;
		// ��ȡ����
		mask = 0;
		for( j = 0; j < 3; j++ ) {
			r[j] = (abs(pCodeCol[-j]) > tmp) ? 1 : 0;
			mask  |= (r[j] << (2-j));
		}
		// �Աȿ�ʼ����λ
		if( mask == gnDecoderModuleI2of5[12] ) {
			nStopIdx = arr_count - i - 1;
			nIsFindR = 2;
			break;
		} 

		pCodeCol--;
	}

	// �Ҳ�����ʼ����ֹλ
	if(nIsFindR != 1 && nIsFindR != 2) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, cannot find end/~start in CodeStartStopMatch_I2of5\n");
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	// �����ҵ���ͬ�Ŀ�ʼ��ֹλ
	if(3 != nIsFindL + nIsFindR) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, start&end do not match in CodeStartStopMatch_I2of5\n");
#endif
#endif
		nRet = -3;
		goto nExit;
	}

	// ˳����ȷ
	if(1 == nIsFindL) {
		*start_idx = nStartIdx;
		*stop_idx = nStopIdx;
		nRet = 1;
	} else {	// ˳��ߵ�
		loop = arr_count >> 1;
		for (i = 0; i < loop; i++) {
			mask = decode_arr[i];
			decode_arr[i] = decode_arr[arr_count-i-1];
			decode_arr[arr_count-i-1] = mask; 
		}
		*start_idx = arr_count - 1 - nStartIdx;
		*stop_idx = arr_count - 1 - nStopIdx;
		nRet = 2;
	}

nExit:
	return nRet;
}


int DcdrCheckStValid_I2of5(int start_idx, int stop_idx)
{
	int nCnt = stop_idx - start_idx + 1;
	if( nCnt > 6 * maxArrLength_I2of5
		|| nCnt < I2OF5_MIN_DECODEARR_COUNT 
		||0 != (nCnt-7) % 10)
		return 0;
	else
		return 1;
}

int DcdrFindCodeSt_I2of5( int * decode_arr, int arr_count )
{
	int nRet = 0;
	int i = 0, j = 0;
	
	int * pCodeCol = 0;
	int * pStartArr = gnDecoderStartArr_I2of5;
	int nStartCnt = 0;
	int * pStopArr = gnDecoderStopArr_I2of5;
	int nStopCnt = 0;
	RyuPoint * pStartStopArr = gptDecoderStArr_I2of5;
	int nStartStopCnt = 0;

	RyuPoint ptTmp;

	int s = 0, t = 0, f[16] = {0}, r[16] = {0}; 
	int mask = 0, nVar = 0;

	int k = 0, tmp = 0, sort[16] = {0};

	const int half_fixed = 1 << (FLOAT2FIXED_SHIFT_DIGIT - 1);


	//////////////////////////////////////////////////////////////////////////
	// Ѱ��������ʼλ������ֹ����λ
	pCodeCol = decode_arr;
	//////////////////////////////////////////////////////////////////////////
	// 2.0.5.2�汾�Ż����������Ҽ����߽磬��������
	//for( i = 0; i < arr_count - 9; i++ ) {
	for( i = 0; i < 6; i++, pCodeCol++ ) {
		if( pCodeCol[0] > 0 ) {	// �涨�Ӻ�ɫ��ʼ
			continue;
		}

		// ȡ����һ���ַ�������ֵ
		s = -pCodeCol[4] + pCodeCol[5] - pCodeCol[6] + pCodeCol[7]
		-pCodeCol[8] + pCodeCol[9] - pCodeCol[10] + pCodeCol[11] - pCodeCol[12] + pCodeCol[13];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf( "Unexpected format of decode_arr in CodeStartStopMatch_I2of5, s=%d\n", s );
#endif
#endif
			nRet = -1;
			goto nExit;
		}

		//////////////////////////////////////////////////////////////////////////
		// 2.0.4.2�汾�Ľ�
		// �ɰ���룬ע��
		// �Աȿ�ʼλ�ͷ������λ
		// 		mask = 0;
		// 		t = (s << FLOAT2FIXED_SHIFT_DIGIT) >> 3;
		// 		for( j = 0; j < 9; j++ ) {
		// 			f[j] = abs(pCodeCol[j]) << FLOAT2FIXED_SHIFT_DIGIT;
		// 			r[j] = (f[j] > t) ? 1 : 0;
		// 			mask  |= (r[j] << (8-j));
		// 		}

		//////////////////////////////////////////////////////////////////////////
		// �°���룬���EMS������������иĽ����Գ��Ƚ�������õ���ֵ
		// ����ͨ����ֵ
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;

		// ������Ԫ�ؿ�Ƚ�������
		for( j = 0; j < 14; j++ ) {
			sort[j] = abs(pCodeCol[j]);
		}
		for( j = 0; j < 14; j++ ) {
			for( k = j+1; k < 14; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[3]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[13] * 6 ) {
				continue;
		}
		// ������ֵ
		tmp = (sort[3] + sort[4]) >> 1;
		// ��ȡ����
		mask = 0;
		for( j = 0; j < 4; j++ ) {
			r[j] = (abs(pCodeCol[j]) > tmp) ? 1 : 0;
			mask  |= (r[j] << (3-j));
		}
		// 2.0.4.2�汾�Ľ�����
		//////////////////////////////////////////////////////////////////////////

		// �Աȿ�ʼλ
		if( mask == gnDecoderModuleI2of5[10] ) {
			pStartArr[nStartCnt++] = i;
			continue;
		} 

		//////////////////////////////////////////////////////////////////////////
		// ��֤�������λ
		s = s + pCodeCol[3] - pCodeCol[13];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf( "Unexpected format of decode_arr in CodeStartStopMatch_I2of5, s=%d\n", s );
#endif
#endif
			nRet = -1;
			goto nExit;
		}
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;
		for( j = 0; j < 13; j++ ) {
			sort[j] = abs(pCodeCol[j]);
		}
		for( j = 0; j < 13; j++ ) {
			for( k = j+1; k < 13; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[5]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[12] * 6 ) {
				continue;
		}
		// ������ֵ
		tmp = (sort[4] + sort[5]) >> 1;
		// ��ȡ����
		mask = 0;
		for( j = 0; j < 3; j++ ) {
			r[j] = (abs(pCodeCol[j]) > tmp) ? 1 : 0;
			mask  |= (r[j] << (2-j));
		}
		// �Աȿ�ʼ����λ
		if( mask == gnDecoderModuleI2of5[12] ) {
			pStopArr[nStopCnt++] = 0 - i;
			continue;
		} 
		//////////////////////////////////////////////////////////////////////////
	}

	// �Ҳ�����ʼ����ֹλ
	if(0 >= nStartCnt && 0 >= nStopCnt) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("Cannot find I2of5, cannot find start/~end in CodeStartStopMatch_I2of5\n");
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	// ��������������ֹλ����ʼ����λ
	pCodeCol = decode_arr + arr_count - 1;
	//////////////////////////////////////////////////////////////////////////
	// 2.0.4.2�汾�Ż����������Ҽ����߽磬��������
	//for(i = 0; i < arr_count - 9; i++) {
	for(i = 0; i < 6; i++, pCodeCol--) {
		if( pCodeCol[0] > 0 ) {	// �涨�Ӻ�ɫ��ʼ
			continue;
		}

		// ȡ����һ���ַ�������ֵ
		s = -pCodeCol[-4] + pCodeCol[-5] - pCodeCol[-6] + pCodeCol[-7]
		-pCodeCol[-8] + pCodeCol[-9] - pCodeCol[-10] + pCodeCol[-11] - pCodeCol[-12] + pCodeCol[-13];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf( "Unexpected format of decode_arr in CodeStartStopMatch_I2of5, s=%d\n", s );
#endif
#endif
			nRet = -1;
			goto nExit;
		}

		//////////////////////////////////////////////////////////////////////////
		// 2.0.4.2�汾�Ľ�
		// �ɰ���룬ע��
		// �Աȷ���ʼλ����ֹλ
		// 		mask = 0;
		// 		t = (s << FLOAT2FIXED_SHIFT_DIGIT) >> 3;
		// 		for( j = 0; j < 9; j++ ) {
		// 			f[j] = abs(pCodeCol[-j]) << FLOAT2FIXED_SHIFT_DIGIT;
		// 			r[j] = (f[j] > t) ? 1 : 0;
		// 			mask |= (r[j] << (8-j));
		// 		}

		//////////////////////////////////////////////////////////////////////////
		// �°���룬���EMS������������иĽ����Գ��Ƚ�������õ���ֵ
		// ����ͨ����ֵ
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;

		// ������Ԫ�ؿ�Ƚ�������
		for( j = 0; j < 14; j++ ) {
			sort[j] = abs(pCodeCol[-j]);
		}
		for( j = 0; j < 14; j++ ) {
			for( k = j+1; k < 14; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[3]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[13] * 6 ) {
				continue;
		}
		// ������ֵ
		tmp = (sort[3] + sort[4]) >> 1;
		// ��ȡ����
		mask = 0;
		for( j = 0; j < 4; j++ ) {
			r[j] = (abs(pCodeCol[-j]) > tmp) ? 1 : 0;
			mask  |= (r[j] << (3-j));
		}
		// 2.0.4.2�汾�Ľ�����
		//////////////////////////////////////////////////////////////////////////

		if( mask == gnDecoderModuleI2of5[10] ) {	// ���ַ���ʼ����
			pStartArr[nStartCnt++] = i - (arr_count - 1);
			continue;
		}

		s = s + pCodeCol[-3] - pCodeCol[-13];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf( "Unexpected format of decode_arr in CodeStartStopMatch_I2of5, s=%d\n", s );
#endif
#endif
			nRet = -1;
			goto nExit;
		}
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;
		for( j = 0; j < 13; j++ ) {
			sort[j] = abs(pCodeCol[-j]);
		}
		for( j = 0; j < 13; j++ ) {
			for( k = j+1; k < 13; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[5]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[12] * 6 ) {
				continue;
		}
		// ������ֵ
		tmp = (sort[4] + sort[5]) >> 1;
		// ��ȡ����
		mask = 0;
		for( j = 0; j < 3; j++ ) {
			r[j] = (abs(pCodeCol[-j]) > tmp) ? 1 : 0;
			mask  |= (r[j] << (2-j));
		}
		// �Աȿ�ʼ����λ
		if( mask == gnDecoderModuleI2of5[12] ) {
			pStopArr[nStopCnt++] = arr_count - 1 - i;
			continue;
		} 
	}

	//////////////////////////////////////////////////////////////////////////
	// ��ʼ����λ��ԣ����ݿ������
	for(i = 0; i < nStartCnt; i++) {
		for(j = 0; j < nStopCnt; j++) {
			if(pStartArr[i] < pStopArr[j] && 
				((pStartArr[i] >= 0 && pStopArr[j] > 0) || (pStartArr[i] < 0 && pStopArr[j] <= 0))) {
					nVar = DcdrCheckStValid_I2of5(pStartArr[i], pStopArr[j]);
					if(1 == nVar) {
						pStartStopArr[nStartStopCnt++] = ryuPoint(abs(pStartArr[i]), abs(pStopArr[j]));
					}
			}
		}
	}

	for(i = 0; i < nStartStopCnt; i++) {
		for(j = i+1; j < nStartStopCnt; j++) {
			if(abs(pStartStopArr[j].x-pStartStopArr[j].y) 
		> abs(pStartStopArr[i].x-pStartStopArr[i].y)) {
			ptTmp = pStartStopArr[i];
			pStartStopArr[i] = pStartStopArr[j];
			pStartStopArr[j] = ptTmp;
			}
		}
	}

	nRet = nStartStopCnt;

nExit:
	return nRet;
}

int CodeModelMatch_I2of5(int * decode_arr, int arr_count, int * seq_arr, int * faith_arr)
{
	int nRet = 0;
	int i = 0, j = 0, n = 0;
	int * pCodeCol = decode_arr + 4;

	int s = 0, t = 0, f[14] = {0}, r[14] = {0};
	int mask = 0, mask1 = 0, loop = (arr_count-7) / 10;

	int k = 0, tmp = 0, sort[14] = {0};

	const int half_fixed = 1 << (FLOAT2FIXED_SHIFT_DIGIT - 1);

	for( i = 0; i < loop; i++ ) {
		if( pCodeCol[0] > 0 ) {	// �涨�Ӻ�ɫ��ʼ
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("Cannot match I2of5, unexpected value of pCodeCol[0] in CodeModelMatch_I2of5, \
				   pCodeCol[0]=%d (should be <0)\n", pCodeCol[0]);
#endif
#endif
			nRet = -1;
			goto nExit;
		}
		s = -pCodeCol[0] + pCodeCol[1] - pCodeCol[2] + pCodeCol[3] - pCodeCol[4] + pCodeCol[5]
		-pCodeCol[6] + pCodeCol[7] - pCodeCol[8] + pCodeCol[9];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("Cannot match I2of5, unexpected value of s in CodeModelMatch_I2of5, \
				   s=%d (should be >0)\n", s);
#endif
#endif
			nRet = -1;
			goto nExit;
		}

		//////////////////////////////////////////////////////////////////////////
		// 2.0.4.2�汾�Ľ�
		// �ɰ���룬ע��
		// �Աȿ�ʼλ�ͷ������λ
		// 		mask = 0;
		// 		t = (s << FLOAT2FIXED_SHIFT_DIGIT) >> 3;
		// 		for( j = 0; j < 9; j++ ) {
		// 			f[j] = abs(pCodeCol[j]) << FLOAT2FIXED_SHIFT_DIGIT;
		// 			r[j] = (f[j] > t) ? 1 : 0;
		// 			mask  |= (r[j] << (8-j));
		// 		}

		//////////////////////////////////////////////////////////////////////////
		// �°���룬���EMS������������иĽ����Գ��Ƚ�������õ���ֵ
		// ����ͨ����ֵ
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;

		// ������Ԫ�ؿ�Ƚ�������
		for( j = 0; j < 10; j++ ) {
			sort[j] = abs(pCodeCol[j]);
		}
		for( j = 0; j < 10; j++ ) {
			for( k = j+1; k < 10; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[3]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[9] * 6 ) {
				pCodeCol++;
				continue;
		}
		// ������ֵ
		tmp = (sort[3] + sort[4]) >> 1;
		// ��ȡ����
		mask = mask1 = 0;
		for( j = 0; j < 5; j++ ) {
			r[j] = (abs(pCodeCol[j*2]) > tmp) ? 1 : 0;
			f[j] = (abs(pCodeCol[j*2+1]) > tmp) ? 1 : 0;
			mask   |= (r[j] << (4-j));
			mask1  |= (f[j] << (4-j));
		}
		// 2.0.4.2�汾�Ľ�����
		//////////////////////////////////////////////////////////////////////////

		// ֱ�ӶԱ�
 		for( n = 0; n < 10; n++ ) {
			if( mask == gnDecoderModuleI2of5[n] )
				break;
		}
		if( n < 10) {
			seq_arr[i*2] = n;
			faith_arr[i*2] = 1;	// �ɿ�ϵ��1��ֱ��ƥ��ģ��
		} else {
			seq_arr[i*2] = -1;
			faith_arr[i*2] = 0;
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("Cannot match I2of5 directly in CodeModelMatch_I2of5, seq=%d\n", i);
			printf("s:%5d  %5d  %5d  %5d  %5d\n",\
				pCodeCol[0], pCodeCol[2], pCodeCol[4], pCodeCol[6], pCodeCol[8]);
			printf("r:%5d  %5d  %5d  %5d  %5d\n",\
				r[0], r[1], r[2], r[3], r[4]);
			printf("s:%5d  %5d  %5d  %5d  %5d\n",\
				pCodeCol[1], pCodeCol[3], pCodeCol[5], pCodeCol[7], pCodeCol[9]);
			printf("f:%5d  %5d  %5d  %5d  %5d\n",\
				f[0], f[1], f[2], f[3], f[4]);
#endif
#endif
		}
		for( n = 0; n < 10; n++ ) {
			if( mask1 == gnDecoderModuleI2of5[n] )
				break;
		}
		if( n < 10) {
			seq_arr[i*2+1] = n;
			faith_arr[i*2+1] = 1;	// �ɿ�ϵ��1��ֱ��ƥ��ģ��
		} else {
			seq_arr[i*2+1] = -1;
			faith_arr[i*2+1] = 0;
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("Cannot match I2of5 directly in CodeModelMatch_I2of5, seq=%d\n", i);
			printf("s:%5d  %5d  %5d  %5d  %5d\n",\
				pCodeCol[0], pCodeCol[2], pCodeCol[4], pCodeCol[6], pCodeCol[8]);
			printf("r:%5d  %5d  %5d  %5d  %5d\n",\
				r[0], r[1], r[2], r[3], r[4]);
			printf("s:%5d  %5d  %5d  %5d  %5d\n",\
				pCodeCol[1], pCodeCol[3], pCodeCol[5], pCodeCol[7], pCodeCol[9]);
			printf("f:%5d  %5d  %5d  %5d  %5d\n",\
				f[0], f[1], f[2], f[3], f[4]);
#endif
#endif
		}
		pCodeCol += 10;
	}

	nRet = loop * 2;

#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
	printf("Decode seq_arr: ");
	for(i = 0; i < loop*2; i++)
		printf("%5d",seq_arr[i]);
	printf("\n");
#endif
#endif

nExit:
	return nRet;
}

int DcdrModelMatch_I2of5(int * decode_arr, int arr_count, int * seq_arr, int * seq_cnt, int * eff_cnt)
{
	int nRet = 0;
	int i = 0, j = 0, n = 0;

	int nSeqCnt = 0, nEffCnt = 0;

	int * pCodeCol = decode_arr + 4;

	int s = 0, t = 0, f[16] = {0}, r[16] = {0};
	int mask = 0, mask1 = 0, loop = (arr_count-7) / 10;

	int k = 0, tmp = 0, sort[16] = {0};

	const int half_fixed = 1 << (FLOAT2FIXED_SHIFT_DIGIT - 1);

	for( i = 0; i < loop; i++ ) {
		if( pCodeCol[0] > 0 ) {	// �涨�Ӻ�ɫ��ʼ
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("Cannot match I2of5, unexpected value of pCodeCol[0] in CodeModelMatch_I2of5, \
				   pCodeCol[0]=%d (should be <0)\n", pCodeCol[0]);
#endif
#endif
			nRet = -1;
			goto nExit;
		}
		s = -pCodeCol[0] + pCodeCol[1] - pCodeCol[2] + pCodeCol[3] - pCodeCol[4] + pCodeCol[5]
		-pCodeCol[6] + pCodeCol[7] - pCodeCol[8] + pCodeCol[9];
		if( 0 >= s ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("Cannot match I2of5, unexpected value of s in CodeModelMatch_I2of5, \
				   s=%d (should be >0)\n", s);
#endif
#endif
			nRet = -1;
			goto nExit;
		}

		//////////////////////////////////////////////////////////////////////////
		// 2.0.4.2�汾�Ľ�
		// �ɰ���룬ע��
		// �Աȿ�ʼλ�ͷ������λ
		// 		mask = 0;
		// 		t = (s << FLOAT2FIXED_SHIFT_DIGIT) >> 3;
		// 		for( j = 0; j < 9; j++ ) {
		// 			f[j] = abs(pCodeCol[j]) << FLOAT2FIXED_SHIFT_DIGIT;
		// 			r[j] = (f[j] > t) ? 1 : 0;
		// 			mask  |= (r[j] << (8-j));
		// 		}

		//////////////////////////////////////////////////////////////////////////
		// �°���룬���EMS������������иĽ����Գ��Ƚ�������õ���ֵ
		// ����ͨ����ֵ
		t = ((s << FLOAT2FIXED_SHIFT_DIGIT) * 7) >> 6;

		// ������Ԫ�ؿ�Ƚ�������
		for( j = 0; j < 10; j++ ) {
			sort[j] = abs(pCodeCol[j]);
		}
		for( j = 0; j < 10; j++ ) {
			for( k = j+1; k < 10; k++ ) {
				if( sort[j] < sort[k] ) {
					tmp = sort[j];
					sort[j] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		// �Ա߽��Ƚ�����ֵ����
		if( (sort[3]<<FLOAT2FIXED_SHIFT_DIGIT) < t * 2/3 ||
			(sort[4]<<FLOAT2FIXED_SHIFT_DIGIT) > t * 4/3 ||
			sort[0] > sort[9] * 6 ) {
				nRet = 0;
				goto nExit;
		}
		// ������ֵ
		tmp = (sort[3] + sort[4]) >> 1;
		// ��ȡ����
		mask = mask1 = 0;
		for( j = 0; j < 5; j++ ) {
			r[j] = (abs(pCodeCol[j*2]) > tmp) ? 1 : 0;
			f[j] = (abs(pCodeCol[j*2+1]) > tmp) ? 1 : 0;
			mask   |= (r[j] << (4-j));
			mask1  |= (f[j] << (4-j));
		}
		// 2.0.4.2�汾�Ľ�����
		//////////////////////////////////////////////////////////////////////////

		// ֱ�ӶԱ�
		for( n = 0; n < 10; n++ ) {
			if( mask == gnDecoderModuleI2of5[n] )
				break;
		}
		if( n < 10) {
			seq_arr[i*2] = n;
			nSeqCnt++;
			nEffCnt++;
		} else {
			seq_arr[i*2] = -1;
			nSeqCnt++;
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("Cannot match I2of5 directly in CodeModelMatch_I2of5, seq=%d\n", i);
			printf("s:%5d  %5d  %5d  %5d  %5d\n",\
				pCodeCol[0], pCodeCol[2], pCodeCol[4], pCodeCol[6], pCodeCol[8]);
			printf("r:%5d  %5d  %5d  %5d  %5d\n",\
				r[0], r[1], r[2], r[3], r[4]);
			printf("s:%5d  %5d  %5d  %5d  %5d\n",\
				pCodeCol[1], pCodeCol[3], pCodeCol[5], pCodeCol[7], pCodeCol[9]);
			printf("f:%5d  %5d  %5d  %5d  %5d\n",\
				f[0], f[1], f[2], f[3], f[4]);
#endif
#endif
		}
		for( n = 0; n < 10; n++ ) {
			if( mask1 == gnDecoderModuleI2of5[n] )
				break;
		}
		if( n < 10) {
			seq_arr[i*2+1] = n;
			nSeqCnt++;
			nEffCnt++;
		} else {
			seq_arr[i*2+1] = -1;
			nSeqCnt++;
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("Cannot match I2of5 directly in CodeModelMatch_I2of5, seq=%d\n", i);
			printf("s:%5d  %5d  %5d  %5d  %5d\n",\
				pCodeCol[0], pCodeCol[2], pCodeCol[4], pCodeCol[6], pCodeCol[8]);
			printf("r:%5d  %5d  %5d  %5d  %5d\n",\
				r[0], r[1], r[2], r[3], r[4]);
			printf("s:%5d  %5d  %5d  %5d  %5d\n",\
				pCodeCol[1], pCodeCol[3], pCodeCol[5], pCodeCol[7], pCodeCol[9]);
			printf("f:%5d  %5d  %5d  %5d  %5d\n",\
				f[0], f[1], f[2], f[3], f[4]);
#endif
#endif
		}
		pCodeCol += 10;
	}

	if(nSeqCnt == loop * 2) {
		*seq_cnt = nSeqCnt;
		*eff_cnt = nEffCnt;
		nRet = 1;
	} else {
		*seq_cnt = 0;
		*eff_cnt = 0;
		nRet = 0;
	}

#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
	printf("Decode seq_arr: ");
	for(i = 0; i < loop*2; i++)
		printf("%5d",seq_arr[i]);
	printf("\n");
#endif
#endif

nExit:
	return nRet;
}

// У��λ����
int CheckDigitVerify_I2of5(int * seq_arr, int * faith_arr, int seq_cnt, int isCheckDigit)
{
	int nRet = 0;
	int i = 0, sum = 0, tmp = 0;
	int failIdx = 0, failCnt = 0;

	// ��ʼ�������Ŵ���
	if(seq_arr[0] != 43 || seq_arr[seq_cnt-1] != 43) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("#CheckDigitVerify39# Unexpected Start-stop, seq_arr[0]=%d, seq_arr[seq_cnt-1]=%d",\
			seq_arr[0], seq_arr[seq_cnt-1]);
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	// ��У��λУ��
	if( !isCheckDigit ) {
		tmp = 0;
		for( i = 1; i < seq_cnt - 1; i++ ) {
			if( 1 != faith_arr[i] ) {
				tmp = i;
				break;
			}
		}
		if( 0 == tmp ) {
			nRet = 1;
			goto nExit;
		} 
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("#CheckDigitVerify39# Cannot pass CheckDigit directly, CheckDigit off and seq_arr[%d]=%d, faith_arr[%d]=%d\n",\
			tmp, seq_arr[tmp], tmp, faith_arr[tmp]);
#endif
#endif
		nRet = 0;
		goto nExit;
	}

	// ��У��λУ��
	for( i = 1; i < seq_cnt - 2; i++ ) {
		if( 1 == faith_arr[i] ) {
			sum += seq_arr[i];
		} else {
			failCnt++;
			failIdx = i;
		}
	}

	if( 0 == failCnt) {
		i = sum % 43;
		if( i == seq_arr[seq_cnt-2] ) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("#CheckDigitVerify39# CheckDigitVerify succeed\n");
#endif
#endif
			nRet = 1;
			goto nExit;
		} else if( 1 != seq_arr[seq_cnt-2] ) {	// У��λ���ɿ�
			seq_arr[seq_cnt-2] = i;
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
			printf("#CheckDigitVerify39# Cannot pass CheckDigit directly, change seq_arr[%d](vertify)=%d\n",\
				seq_cnt-2, seq_arr[seq_cnt-2]);
#endif
#endif
			nRet = 1;
			goto nExit;
		}
	} else if( 1 == failCnt && 1 == faith_arr[seq_cnt-2]) {
		for( i = 0; i < 43; i++ ) {
			tmp = sum + i;
			if( seq_arr[seq_cnt-2] == tmp % 43 ) {
				seq_arr[failIdx] = i;
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
				printf("#CheckDigitVerify39# Cannot pass CheckDigit directly, change seq_arr[%d]=%d\n",\
					failIdx, seq_arr[failIdx]);
#endif
#endif
				nRet = 1;
				goto nExit;
			}
		}
	} else {
		nRet = 0;
		goto nExit;
	}

nExit:
	return nRet;
}

// У��λ����
int DcdrCheckVerify_I2of5(int * seq_arr, int seq_cnt, int isCheckDigit)
{
	int nRet = 0;
	int i = 0, sum = 0, tmp = 0;
	int failIdx = 0, failCnt = 0;

	// ��У��λУ��
	if( !isCheckDigit ) {
		for( i = 0; i < seq_cnt; i++ ) {
			if(0 > seq_arr[i] || 10 <= seq_arr[i]) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
				printf("#CheckDigitVerify39# Cannot pass CheckDigit directly, CheckDigit off and seq_arr[%d]=%d, faith_arr[%d]=%d\n",\
					tmp, seq_arr[tmp], tmp, faith_arr[tmp]);
#endif
#endif
				nRet = 0;
				goto nExit;
			}
		}

		nRet = 1;
		goto nExit;
	}

	// ��У��λУ��
	for( i = 0; i < seq_cnt - 1; i++ ) {
		if(0 <= seq_arr[i] && 10 > seq_arr[i]) {
			sum += seq_arr[i];
		} else {
			failCnt++;
			failIdx = i;
		}
	}

	for(i = seq_cnt - 2; i >= 0; i -= 2) {
		if(0 <= seq_arr[i] && 10 > seq_arr[i]) {
			sum += seq_arr[i];
		} else {
			nRet = 0;
			goto nExit;
		}
	}

	sum *= 3;

	for(i = seq_cnt - 3; i >= 0; i -= 2) {
		if(0 <= seq_arr[i] && 10 > seq_arr[i]) {
			tmp += seq_arr[i];
		} else {
			nRet = 0;
			goto nExit;
		}
	}

	sum += tmp;
	tmp = 10 - (sum % 10);
	tmp = tmp < 10 ? tmp : 0;

	if(tmp == seq_arr[seq_cnt-1]) {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("#CheckDigitVerifyI2of5# CheckDigitVerify succeed\n");
#endif
#endif
		nRet = 1;
		goto nExit;
	} else {
#ifdef	_DEBUG_
#ifdef  _DEBUG_DECODER_I2OF5
		printf("#CheckDigitVerifyI2of5# Cannot pass CheckDigit directly, change seq_arr[%d]=%d\n",\
			failIdx, seq_arr[failIdx]);
#endif
#endif
		nRet = 0;
		goto nExit;
	}

nExit:
	return nRet;
}


int Transcode_I2of5( int * seq_arr, int seq_cnt, char * strResult )
{
	int i = 0, nRet = 0, nStrNum = 0;

	if(seq_arr[0] > 0) {
		strResult[0] = seq_arr[0] + 48;
		nStrNum++;
	} else if(seq_arr[0] < 0) {
		strResult[0] = 0;
		nRet = -1;
		goto nExit;
	}

	for (i = 1; i < seq_cnt; i++)
	{
		if(seq_arr[i] < 10 && seq_arr[i] >= 0) {
			strResult[nStrNum++] = seq_arr[i] + 48;
		} else {
			strResult[0] = 0;
			nRet = -1;
			goto nExit;
		}
	}

	strResult[nStrNum] = 0;
	nRet = nStrNum;

nExit:
	return nRet;
}


int DcdrTranscode_I2of5( int * seq_arr, int seq_cnt, int isCheckDigit, 
						int isKeep1stZero, char * strResult )
{
	int i = 0, nRet = 0, nStrNum = 0;
	int startIdx = 0, endIdx = 0;

	if(isKeep1stZero && 0 == seq_arr[0]) {
		strResult[nStrNum++] = seq_arr[0] + 48;
		startIdx = 1;
	} else if(!isKeep1stZero && 0 == seq_arr[0]) {
		startIdx = 1;
	} else if(seq_arr[i] >= 10 || seq_arr[i] < 0) {
		strResult[0] = 0;
		nRet = -1;
		goto nExit;
	}

	if(isCheckDigit)
		endIdx = seq_cnt - 1;
	else
		endIdx = seq_cnt;

	for (i = startIdx; i < endIdx; i++)
	{
		if(seq_arr[i] < 10 && seq_arr[i] >= 0) {
			strResult[nStrNum++] = seq_arr[i] + 48;
		} else {
			strResult[0] = 0;
			nRet = -1;
			goto nExit;
		}
	}

	strResult[nStrNum] = 0;
	nRet = nStrNum;

nExit:
	return nRet;
}


