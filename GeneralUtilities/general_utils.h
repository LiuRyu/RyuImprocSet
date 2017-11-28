#ifndef __RYU_GENERAL_UTILITIES_H__
#define __RYU_GENERAL_UTILITIES_H__

#ifdef GENERALUTILITIES_EXPORTS
#define GENERALUTILITIES_API __declspec(dllexport)
#else
#define GENERALUTILITIES_API __declspec(dllimport)
#endif

#include <list>
#include <time.h>  
#include <windows.h>
#include <math.h>

#include "types.h"


/****************************************************************************************\
*									File operations								         *
\****************************************************************************************/
/* 根据指定路径创建文件夹 */
GENERALUTILITIES_API int creatFlieDir(const char *pStrPath);

/* 根据指定路径创建文件 */
GENERALUTILITIES_API int creatFile(const char *pStrFile, int iPos);

/* 判断给定路径文件夹是否存在 */
GENERALUTILITIES_API int isDirExist(const char * pStrDirName);

/* 判断给定路径文件是否存在 */
GENERALUTILITIES_API int isFileExist(const char * pStrFileName);

/* 获取文件长度 */
GENERALUTILITIES_API DWORD getFileLenth(const char *pFile);

/* 获取文件类型(后缀) */
GENERALUTILITIES_API int getFileTypeR(const char * pFile, char * type);

/* 获取文件名 
 * [type_mode: 0] - 无后缀文件名;  [type_mode: 1] - 带后缀文件名
 */
GENERALUTILITIES_API int getFileName(const char * pFile, char * name, int type_mode);

/* 获取文件路径分隔符类型 */
GENERALUTILITIES_API char getPathSeparator(const char * pFilePath);

/* 修正文件路径分隔符类型，默认为'/'，可设置为'\\' */
GENERALUTILITIES_API char corrPathSeparator(char * pFilePath, char separator);

/* 获取路径的上一级路径，若无则返回0 */
GENERALUTILITIES_API int getParentPath(const char * pPath, char * parent);

/* 判断文件是否为图像类型 */
GENERALUTILITIES_API int isImageFile(const char * pFile);

/* 获取文件路径下的所有图像文件 */
GENERALUTILITIES_API int findImageFiles(const char * pFilePath, std::list<PathElem> &ImgList);
// GENERALUTILITIES_API int findImageFiles(const char * pSrcImgPath, const char * pRstImgPath, 
// 				   std::list<PathElem> &ImgList);

/****************************************************************************************\
*									Log operations								         *
\****************************************************************************************/
GENERALUTILITIES_API int generateLogIdCode();

GENERALUTILITIES_API int setLogPath(char *pStrPath);

GENERALUTILITIES_API void writeLog(unsigned int uiLogType, char *pstrFmt, ...);


/****************************************************************************************\
*									time operations								         *
\****************************************************************************************/
GENERALUTILITIES_API void ryuTimerStart(int seq RYU_DEFAULT(TIMER_SEQ_0));

GENERALUTILITIES_API long ryuTimerStop(int seq RYU_DEFAULT(TIMER_SEQ_0));

GENERALUTILITIES_API void ryuTimerPrint(int seq RYU_DEFAULT(TIMER_SEQ_0));

/****************************************************************************************\
*									print operations								     *
\****************************************************************************************/
/* 
 * convert a long val to thousand-type val string. 
 * eg. 1589746 => 1,589,476
 */
GENERALUTILITIES_API void ryuThousandType(long val, char (&Dest)[32U]);


/****************************************************************************************\
*									XML operations								         *
\****************************************************************************************/
#define XML_PARSE_SUCCESS	(1)
#define XML_PARSE_FAILED	(-1)

GENERALUTILITIES_API int parseXmlNodeValue(const char * docname, const char * elename, char * content);

GENERALUTILITIES_API int reviseXmlNodeValue(const char * docname, const char * elename, const char * content);

#endif  __RYU_GENERAL_UTILITIES_H__

