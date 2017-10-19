#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>  
#include <stdarg.h>  
#include <direct.h>
#include <windows.h>
#include <list>

#include "types.h"
#include "general_utils.h"

/*********************************************************************
* 函数名称:int CreatFlieDir(const char *pStrPath)
* 说明:创建日志存放路径
* 调用者：
* 输入参数:
* const char *pStrPath --用户指定的根路径
* 输出参数：
* 无
* 返回值:
* int  -- CREAT_FILEDIR_FAILED:  失败
*      -- CREAT_FILEDIR_SUCCESS: 成功
* 作者: liuryu
* 时间: 2017-3-31
*********************************************************************/ 
int creatFlieDir(const char *pStrPath) 
{ 
	if(NULL == pStrPath) 
	{ 
		return 0; 
	} 
	int iRet = 0; 
	char szSub[MAX_PATH]; 
	char *pSub = NULL; 
	int iIndex = 0; 
	int iLen = 0; 
	int bFind = 0; 
	memset(szSub, 0, sizeof(MAX_PATH)); 

	// 逐层创建目录
	while(1) 
	{ 
		pSub = (char *)strchr(pStrPath + iLen, '//'); 
		if(NULL == pSub) 
		{ 
			if(iLen == 0) 
			{ 
				return 0; 
			} 
			iRet = CreateDirectory(pStrPath, NULL); 
			if(0 == iRet) 
			{ 
				iRet = GetLastError(); 
				if(ERROR_ALREADY_EXISTS == iRet) 
				{ 
					return 1; 
				} 
				return 0; 
			} 
			return 1; 
		} 
		else 
		{ 
			if (!bFind) 
			{ 
				bFind = 1; 
			} 
			else 
			{ 
				memset(szSub, 0, sizeof(szSub)); 
				strncpy_s(szSub, pStrPath, pSub - pStrPath); 
				CreateDirectory(szSub, NULL); 
			} 
			iLen = pSub - pStrPath + 1; 
		} 
	} 
	return 1; 
} 

/*********************************************************************
* 函数名称:int CreatFile(const char *pStrFile, int iPos)
* 说明:创建日志文件
* 调用者：
* 输入参数:
* const char *pStrFile --文件名
* int iPos             --文件指针位置
* 输出参数：
* 无
* 返回值:
* int  -- CREAT_FILE_FAILED:  失败
*      -- CREAT_FILE_SUCCESS: 成功
* 作者: liuryu
* 时间: 2017-3-31
*********************************************************************/ 
int creatFile(const char *pStrFile, int iPos) 
{ 
	HANDLE hd = 0; 
	int iRet = 0; 
	if(NULL == pStrFile) 
	{ 
		return 0; 
	} 
	hd = CreateFile(pStrFile,  
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL); 
	if(INVALID_HANDLE_VALUE == hd) 
	{ 
		return 0; 
	} 
	if(DWORD_NULL == SetFilePointer(hd, iPos, NULL, FILE_BEGIN)) 
	{ 
		return 0; 
	} 
	iRet = SetEndOfFile(hd); 
	CloseHandle(hd); 
	return 1; 
} 

/*********************************************************************
* 函数名称:int IsFileExist(const char *pStrFile)
* 说明:判断指定的文件是否存在
* 调用者：
* 输入参数:
* const char *pStrFile --文件名
* 输出参数：
* 无
* 返回值:
* int  -- FILE_OPERATE_FALSE:不存在
*      -- FILE_OPERATE_TRUE: 存在
* 作者: liuryu
* 时间: 2017-3-31
*********************************************************************/ 
/*
int isFileExist(const char *pStrFile) 
{ 
	int iLen = 0; 
	WIN32_FIND_DATA finddata; 
	memset(&finddata, 0, sizeof(WIN32_FIND_DATA)); 
	HANDLE hd = FindFirstFile(pStrFile, &finddata); 
	if(INVALID_HANDLE_VALUE == hd) 
	{ 
		DWORD dwRet = GetLastError(); 
		if(ERROR_FILE_NOT_FOUND == dwRet || ERROR_PATH_NOT_FOUND == dwRet) 
		{ 
			return 0; 
		} 
	} 
	FindClose(hd); 
	return 1; 
}
*/

//检查目录是否存在
int isDirExist(const char * pStrDirName)
{
	DWORD attr; 
	attr = GetFileAttributes(pStrDirName); 
	if( (attr != (DWORD)(-1)) && ( attr & FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	else
		return 0;
}

//文件是否存在
int isFileExist(const char * pStrFileName)
{
	DWORD attr; 
	attr = GetFileAttributes(pStrFileName); 
	if( (attr != (DWORD)(-1)) && !( attr & FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	else
		return 0;
}

/*********************************************************************
* 函数名称:DWORD GetFileLenth(const char *pFile)
* 说明:判断指定的文件大小
* 调用者：
* 输入参数:
* const char *pFile --文件名
* 输出参数：
* 无
* 返回值:
* DWORD -- 文件大小
* 作者: liuryu
* 时间: 2017-3-31
*********************************************************************/ 
DWORD getFileLenth(const char *pFile) 
{ 
	WIN32_FIND_DATA buff; 
	HANDLE hd = NULL; 
	memset(&buff, 0, sizeof(WIN32_FIND_DATA)); 
	hd = FindFirstFile(pFile, &buff); 
	FindClose(hd); 
	return (buff.nFileSizeHigh * MAXDWORD) + buff.nFileSizeLow; 
} 

/*********************************************************************
* 函数名称:int GetFileTypeR(const char * pFile, char * type)
* 说明:判断指定的文件的类型(提取后缀名)
* 调用者：
* 输入参数:
* const char * pFile --文件(夹)名
* 输出参数：
* char * type --文件类型(后缀名)
* 返回值:
* int	-- FILE_TYPE_SUCCESS:成功获取文件类型
		-- FILE_TYPE_NOTYPE: 无后缀名
		-- FILE_TYPE_FAILED: 获取失败，发生错误
* 作者: liuryu
* 时间: 2017-4-6
*********************************************************************/ 
int getFileTypeR(const char * pFile, char * type)
{
	int i = 0, j = 0;
	char ch = 0;

	if(NULL == pFile || NULL == type) {
		printf("Invalid input. [GetFileTypeR]\n");
		return FILE_TYPE_FAILED; 
	} 

	i = strlen(pFile) - 1;

	for(type[0] = '\0'; i >= 0; i--) {
		if('\\' == pFile[i] || '/' == pFile[i]) {	// 文件夹分割符
			return FILE_TYPE_NOTYPE;
		}
		if('.' == pFile[i]) {	// 文件类型分割符
			for(j = i; pFile[j] != '\0'; j++) {                
				ch = pFile[j];
				type[j-i] = ('A'<=ch && ch<='Z') ? (ch+'a'-'A'): ch;
			}
			type[j-i] = '\0';
			return FILE_TYPE_SUCCESS;
		}
	}
	return FILE_TYPE_NOTYPE;
}

int getFileName(const char * pFile, char * name, int type_mode)
{
	int i = 0, j = 0, len = 0;
	int dot = 0, is_dot = 0;
	char ch = 0;

	if(NULL == pFile || NULL == name) {
		printf("Error. Invalid input. [getFileName]\n");
		return -1; 
	} 

	i = strlen(pFile) - 1;
	dot = i + 1;

	for(name[0] = '\0'; i >= 0; i--) {
		if('.' == pFile[i] && !type_mode && !is_dot) {	// 文件类型分割符
			dot = i;
			is_dot = 1;
		}
		if('\\' == pFile[i] || '/' == pFile[i]) {	// 遇到文件夹分割符
			j = i + 1;
			for( ; j < dot; j++) {                
				name[j-i-1] = pFile[j];
				len++;
			}
			name[len] = '\0';
			break;
		}
	}

	return len;
}

char getPathSeparator(const char * pFilePath)
{
	int i = 0;
	char ch = '/';		// 默认返回windows/linux下通用分隔符

	if(NULL == pFilePath) {
		printf("Invalid input. [getPathSeparator]\n");
		return -1; 
	}
	// 返回首个分隔符类型
	for(i = 0; i < strlen(pFilePath); i++) {
		if('\\' == pFilePath[i] || '/' == pFilePath[i]) {
			ch = pFilePath[i];
			break;
		}
	}
	return ch;
}

char corrPathSeparator(char * pFilePath, char separator)
{
	int i = 0;
	char ch = '/';		// 默认返回windows/linux下通用分隔符

	if(NULL == pFilePath) {
		printf("Invalid input. [getPathSeparator]\n");
		return -1; 
	}
	if('\\' == separator) {
		ch = '\\';
	}
	// 修正分隔符
	for(i = 0; i < strlen(pFilePath); i++) {
		if('\\' == pFilePath[i] || '/' == pFilePath[i]) {
			pFilePath[i] = ch;
		}
	}
	return ch;
}

int getParentPath(const char * pPath, char * parent)
{
	int i = 0, j = 0, echo = 0;

	if(NULL == pPath || NULL == parent) {
		printf("Invalid input. [GetFileTypeR]\n");
		return FILE_TYPE_FAILED; 
	} 

	i = strlen(pPath) - 1;
	if('\\' == pPath[i] || '/' == pPath[i]) {  // 处理结尾为分隔符的情形
		for( ; i >= 0; i--) {
			if('\\' != pPath[i] && '/' != pPath[i])
				break;
		}
	}

	for(parent[0] = '\0'; i >= 0; i--) {
		if('\\' == pPath[i] || '/' == pPath[i]) {
			for(j = i; j >= 0; j--) {
				if('\\' != pPath[j] && '/' != pPath[j]) {
					echo = j + 1;
					break;
				}
			}
			break;
		}
	}

	for(i = 0; i < echo; i++) {
		parent[i] = pPath[i];
	}
	parent[echo] = '\0';

	return ((echo > 0) ? 1 : 0);
}

/*********************************************************************
* 函数名称:int IsImageFile(const char * pFile)
* 说明:判断指定的文件是否为图像类型
* 调用者：
* 输入参数:
* const char * pFile --文件名
* 输出参数：
* 无
* 返回值:
* int  -- IMFILE_TYPE_ERROR:		错误
*      -- IMFILE_TYPE_UNKNOWN:	文件不是预设图像类型
*      -- (> 0):					返回预设图像类型值
* 作者: liuryu
* 时间: 2017-4-6
*********************************************************************/ 
int isImageFile(const char * pFile)
{
	char ext[32];
	int status = 0;

	if(NULL == pFile) {
		printf("Invalid input. [IsImageFile]\n");
		return IMFILE_TYPE_ERROR;
	}

	status = getFileTypeR(pFile, ext);
	if(FILE_TYPE_SUCCESS != status) {
		printf("Error in [GetFileTypeR], return = %d. [IsImageFile]\n", status);
		return IMFILE_TYPE_ERROR;
	}

	if(0 == strcmp(ext, ".bmp")) {
		return IMFILE_TYPE_BMP;
	} else if(0 == strcmp(ext, ".jpg")) {
		return IMFILE_TYPE_JPG;
	} else if(0 == strcmp(ext, ".jpeg")) {
		return IMFILE_TYPE_JPEG;
	} else {
		return IMFILE_TYPE_UNKNOWN;
	}

	return IMFILE_TYPE_UNKNOWN;
}

/*********************************************************************
* 函数名称:int FindImageFiles(char * pSrcImgPath, char * pRstImgPath, 
*				std::list<PathElem> &ImgList)
* 说明:判断指定的文件是否为图像类型
* 调用者：
* 输入参数:
* const char * pFile --文件名
* 输出参数：
* 无
* 返回值:
* int  -- IMFILE_TYPE_ERROR:		错误
*      -- IMFILE_TYPE_UNKNOWN:	文件不是预设图像类型
*      -- (> 0):					返回预设图像类型值
* 作者: liuryu
* 时间: 2017-4-6
*********************************************************************/ 
int findImageFiles(const char * pFilePath, std::list<PathElem> &ImgList)
{
	int nRet = 0, status = 0;
	
	PathElem stPathElemTemp;

	if(isFileExist(pFilePath)) {
		if(isImageFile(pFilePath)) {
			lstrcpy(stPathElemTemp.SrcImgPath, pFilePath);
			ImgList.push_back(stPathElemTemp);
			nRet = 1;
			goto nExit;
		} else {
			printf("路径无效：不是已知的图像文件格式\n");
			return 0;
		}
	} else if(!isDirExist(pFilePath)) {
		printf("路径无效：路径需为文件夹或图像文件，请检查当前路径是否有效\n");
		return 0;
	}

	char srcPath[MAX_PATH*2];
	strcpy_s(srcPath, pFilePath);

	int nLen = strlen(srcPath);
	if('\\' != srcPath[nLen-1]) {
		srcPath[nLen] = '\\';
		srcPath[nLen+1] = '\0';
	}

	//源图片存在的目录
	TCHAR   szFileT1[MAX_PATH*2];
	lstrcpy(szFileT1,TEXT(srcPath));
	lstrcat(szFileT1, TEXT("\\*.*"));

	WIN32_FIND_DATA   wfd; 	
	HANDLE   hFind   =   FindFirstFile(szFileT1, &wfd); 

	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			if(wfd.cFileName[0] == TEXT('.')) 
				continue;

			if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY 
				|| strcmp("Thumbs.db", TEXT(wfd.cFileName)) == 0) 
				continue;

			int status = isImageFile(TEXT(wfd.cFileName));
			if(0 >= status)	// 滤掉非预设图像格式文件
				continue;

			TCHAR   SrcImgPath[MAX_PATH*2];
			lstrcpy(SrcImgPath, srcPath); 
			lstrcat(SrcImgPath, TEXT(wfd.cFileName));
			lstrcpy(stPathElemTemp.SrcImgPath, SrcImgPath); 

			ImgList.push_back(stPathElemTemp);
			nRet++;
		} while(FindNextFile(hFind, &wfd));
	} else {
		printf("错误：未预料的错误INVALID_HANDLE_VALUE，发生于[findImageFiles]\n");
		return -1;
	}

nExit:
	if(0 < nRet) {
		printf("输入的路径下包含图像文件%d个\n", nRet);
	} else {
		printf("输入的路径下未找到图像文件\n");
	}
	return nRet;
}
/*
int findImageFiles(const char * pSrcImgPath, const char * pRstImgPath, 
		std::list<PathElem> &ImgList)
{
	int nRet = 0, status = 0;
	char ext[32];

	PathElem stPathElemTemp;

	// 
	if(isImageFile(pSrcImgPath) && isFileExist(pSrcImgPath)) {
		lstrcpy(stPathElemTemp.SrcImgPath, pSrcImgPath);
		ImgList.push_back(stPathElemTemp);
		nRet = 1;
		goto nExit;
	}

	if(0 < isImageFile(pSrcImgPath)) {	// 输入路径为图像文件
		lstrcpy(stPathElemTemp.SrcImgPath, pSrcImgPath);
		ImgList.push_back(stPathElemTemp);
		nRet = 0;
		goto nExit;
	} else if(FILE_TYPE_NOTYPE != getFileTypeR(pSrcImgPath, ext)) {	// 输入路径为非预设图像类型
		printf("路径无效：路径需为文件夹或图像文件，请检查当前路径是否有效\n");
		return 0;
	}

	//源图片存在的目录
	TCHAR   szFileT1[MAX_PATH*2];
	lstrcpy(szFileT1,TEXT(pSrcImgPath));
	lstrcat(szFileT1, TEXT("\\*.*"));

	//结果图片存放的目录
	TCHAR   RstAddr[MAX_PATH*2]; 
	lstrcpy(RstAddr,TEXT(pSrcImgPath));
	lstrcat(RstAddr,TEXT(pRstImgPath));
	_mkdir(RstAddr);   //创建文件夹

	WIN32_FIND_DATA   wfd; 	
	HANDLE   hFind   =   FindFirstFile(szFileT1, &wfd); 

	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			if(wfd.cFileName[0] == TEXT('.')) 
				continue;

			if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY 
				|| strcmp("Thumbs.db", TEXT(wfd.cFileName)) == 0) 
				continue;

			int status = isImageFile(TEXT(wfd.cFileName));
			if(0 >= status)	// 滤掉非预设图像格式文件
				continue;

			TCHAR   SrcImgPath[MAX_PATH*2];
			lstrcpy(SrcImgPath, pSrcImgPath); 
			lstrcat(SrcImgPath, TEXT(wfd.cFileName));
			lstrcpy(stPathElemTemp.SrcImgPath, SrcImgPath); 

			TCHAR   AdressTemp[MAX_PATH*2];
			lstrcpy(AdressTemp,RstAddr); 
			//lstrcat(AdressTemp, TEXT("/"));  
			lstrcat(AdressTemp, TEXT(wfd.cFileName));  
			lstrcpy(stPathElemTemp.RstImgPath, AdressTemp); 

			ImgList.push_back(stPathElemTemp);
			nRet++;
		} while(FindNextFile(hFind, &wfd));
	} else {
		return -1;
	}

nExit:
	if(0 < nRet) {
		printf("您输入的路径下，包含图像文件%d个\n", nRet);
	} else {
		printf("您输入的路径下，未找到图像文件\n", nRet);
	}

	return nRet;
}
*/
