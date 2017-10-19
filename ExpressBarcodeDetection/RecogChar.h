#ifndef _ALGORITHM_RECOG_CHAR_H
#define _ALGORITHM_RECOG_CHAR_H

#ifdef  ALGORITHM_RECOG_CHAR_LIBDLL
#define ALGORITHM_RECOG_CHAR_LIBDLL extern "C" _declspec(dllexport) 
#else
#define ALGORITHM_RECOG_CHAR_LIBDLL extern "C" _declspec(dllimport) 
#endif

#define CODE_RESULT_ARR_LENGTH	(128)

ALGORITHM_RECOG_CHAR_LIBDLL int Init_RecogChar();

ALGORITHM_RECOG_CHAR_LIBDLL int Algorithm_RecogChar(unsigned char * im_data, const int im_width, const int im_height, 
	const int ex_param, char * code_result);

ALGORITHM_RECOG_CHAR_LIBDLL void Release_RecogChar();

#endif