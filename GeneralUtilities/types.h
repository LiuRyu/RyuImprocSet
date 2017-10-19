#ifndef __GENERAL_UTILITIES_TYPES_C_H__
#define __GENERAL_UTILITIES_TYPES_C_H__

#include <stdlib.h>

// ģ�����̨��ӡ����
#define _PRINT_GENERAL_UTILITIES_	(1)


#ifndef RYU_EXTERN_C
#  ifdef __cplusplus
#    define RYU_EXTERN_C extern "C"
#    define RYU_DEFAULT(val) = val
#  else
#    define RYU_EXTERN_C
#    define RYU_DEFAULT(val)
#  endif
#endif


/****************************************************************************************\
*									File operations								         *
\****************************************************************************************/
#define DWORD_NULL			(0xFFFFFFFF)

// �ļ�����
enum {
	FILE_TYPE_SUCCESS = 1,
	FILE_TYPE_NOTYPE = 0,
	FILE_TYPE_FAILED = -1
};

// ͼ���ļ�����
enum {
	IMFILE_TYPE_ERROR		= -1,
	IMFILE_TYPE_UNKNOWN		= 0,
	IMFILE_TYPE_BMP			= 1,
	IMFILE_TYPE_JPG			= 2,
	IMFILE_TYPE_JPEG		= 3,
};

struct PathElem{
	TCHAR   SrcImgPath[MAX_PATH*2];
	TCHAR   RstImgPath[MAX_PATH*2];
};

/****************************************************************************************\
*									Log operations								         *
\****************************************************************************************/
#define _LOG_WRITE_STATE_		(1)		/* �������뿪�أ�1��д��־��0����д��־ */
#define _WRITE_LOG_WITH_PRINT_	(1)		/* �������뿪�أ�1������̨��ӡ��־��0������ӡ��־ */

#define LOG_SUCCESS (0)  
#define LOG_FAILED  (-1)  
#define LOG_BOOL_TRUE (1)  
#define LOG_BOOL_FALSE (0)  
#define DWORD_NULL  (0xFFFFFFFF)  
#define MAX_LOGTEXT_LEN (2048)			/* ÿ����־����󳤶� */
#define MAX_FILE_PATH (255)				/* ��־�ļ�·������󳤶� */
#define MAX_LOG_FILE_SIZE (1024 * 1024)	/* ��־�ļ����ݵ���󳤶� */
#define MAX_LOG_FILE_NAME_LEN (256)		/* ��־�ļ�������󳤶� */

// ��־����
enum {
	LOG_TYPE_INFO = 0,
	LOG_TYPE_ERROR = 1,
	LOG_TYPE_SYSTEM = 2
};

#define TEST_CASE_MAX_FILE_LEN (1024)	/* ���Ժ������ļ�������󳤶� */


/****************************************************************************************\
*									time operations								         *
\****************************************************************************************/
// ��ʱ�����
enum {
	TIMER_SEQ_0 = 0,
	TIMER_SEQ_1 = 1,
	TIMER_SEQ_2 = 2,
	TIMER_SEQ_3 = 3,
	TIMER_SEQ_4 = 4,
	TIMER_SEQ_5 = 5,
	TIMER_SEQ_6 = 6,
	TIMER_SEQ_7 = 7,
	TIMER_SEQ_8 = 8,
	TIMER_SEQ_9 = 9
};

/****************************************************************************************\
*									math utilities								         *
\****************************************************************************************/
#define PI	(3.14159265359f) 

typedef struct RyuVec2dim
{
	double x;
	double y;
} RyuVec2dim;


#endif  __GENERAL_UTILITIES_TYPES_C_H__

