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
/* ����ָ��·�������ļ��� */
GENERALUTILITIES_API int creatFlieDir(const char *pStrPath);

/* ����ָ��·�������ļ� */
GENERALUTILITIES_API int creatFile(const char *pStrFile, int iPos);

/* �жϸ���·���ļ����Ƿ���� */
GENERALUTILITIES_API int isDirExist(const char * pStrDirName);

/* �жϸ���·���ļ��Ƿ���� */
GENERALUTILITIES_API int isFileExist(const char * pStrFileName);

/* ��ȡ�ļ����� */
GENERALUTILITIES_API DWORD getFileLenth(const char *pFile);

/* ��ȡ�ļ�����(��׺) */
GENERALUTILITIES_API int getFileTypeR(const char * pFile, char * type);

/* ��ȡ�ļ��� 
 * [type_mode: 0] - �޺�׺�ļ���;  [type_mode: 1] - ����׺�ļ���
 */
GENERALUTILITIES_API int getFileName(const char * pFile, char * name, int type_mode);

/* ��ȡ�ļ�·���ָ������� */
GENERALUTILITIES_API char getPathSeparator(const char * pFilePath);

/* �����ļ�·���ָ������ͣ�Ĭ��Ϊ'/'��������Ϊ'\\' */
GENERALUTILITIES_API char corrPathSeparator(char * pFilePath, char separator);

/* ��ȡ·������һ��·���������򷵻�0 */
GENERALUTILITIES_API int getParentPath(const char * pPath, char * parent);

/* �ж��ļ��Ƿ�Ϊͼ������ */
GENERALUTILITIES_API int isImageFile(const char * pFile);

/* ��ȡ�ļ�·���µ�����ͼ���ļ� */
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

