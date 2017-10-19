#ifndef __GENERAL_UTILITIES_TYPES_C_H__
#define __GENERAL_UTILITIES_TYPES_C_H__

#include <stdlib.h>

// 模块控制台打印开关
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

// 文件类型
enum {
	FILE_TYPE_SUCCESS = 1,
	FILE_TYPE_NOTYPE = 0,
	FILE_TYPE_FAILED = -1
};

// 图像文件类型
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
#define _LOG_WRITE_STATE_		(1)		/* 条件编译开关，1：写日志，0：不写日志 */
#define _WRITE_LOG_WITH_PRINT_	(1)		/* 条件编译开关，1：控制台打印日志，0：不打印日志 */

#define LOG_SUCCESS (0)  
#define LOG_FAILED  (-1)  
#define LOG_BOOL_TRUE (1)  
#define LOG_BOOL_FALSE (0)  
#define DWORD_NULL  (0xFFFFFFFF)  
#define MAX_LOGTEXT_LEN (2048)			/* 每行日志的最大长度 */
#define MAX_FILE_PATH (255)				/* 日志文件路径的最大长度 */
#define MAX_LOG_FILE_SIZE (1024 * 1024)	/* 日志文件内容的最大长度 */
#define MAX_LOG_FILE_NAME_LEN (256)		/* 日志文件名的最大长度 */

// 日志类型
enum {
	LOG_TYPE_INFO = 0,
	LOG_TYPE_ERROR = 1,
	LOG_TYPE_SYSTEM = 2
};

#define TEST_CASE_MAX_FILE_LEN (1024)	/* 测试函数中文件内容最大长度 */


/****************************************************************************************\
*									time operations								         *
\****************************************************************************************/
// 计时器序号
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

