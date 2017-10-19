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
* ��������:int CreatFlieDir(const char *pStrPath)
* ˵��:������־���·��
* �����ߣ�
* �������:
* const char *pStrPath --�û�ָ���ĸ�·��
* ���������
* ��
* ����ֵ:
* int  -- CREAT_FILEDIR_FAILED:  ʧ��
*      -- CREAT_FILEDIR_SUCCESS: �ɹ�
* ����: liuryu
* ʱ��: 2017-3-31
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

	// ��㴴��Ŀ¼
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
* ��������:int CreatFile(const char *pStrFile, int iPos)
* ˵��:������־�ļ�
* �����ߣ�
* �������:
* const char *pStrFile --�ļ���
* int iPos             --�ļ�ָ��λ��
* ���������
* ��
* ����ֵ:
* int  -- CREAT_FILE_FAILED:  ʧ��
*      -- CREAT_FILE_SUCCESS: �ɹ�
* ����: liuryu
* ʱ��: 2017-3-31
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
* ��������:int IsFileExist(const char *pStrFile)
* ˵��:�ж�ָ�����ļ��Ƿ����
* �����ߣ�
* �������:
* const char *pStrFile --�ļ���
* ���������
* ��
* ����ֵ:
* int  -- FILE_OPERATE_FALSE:������
*      -- FILE_OPERATE_TRUE: ����
* ����: liuryu
* ʱ��: 2017-3-31
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

//���Ŀ¼�Ƿ����
int isDirExist(const char * pStrDirName)
{
	DWORD attr; 
	attr = GetFileAttributes(pStrDirName); 
	if( (attr != (DWORD)(-1)) && ( attr & FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	else
		return 0;
}

//�ļ��Ƿ����
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
* ��������:DWORD GetFileLenth(const char *pFile)
* ˵��:�ж�ָ�����ļ���С
* �����ߣ�
* �������:
* const char *pFile --�ļ���
* ���������
* ��
* ����ֵ:
* DWORD -- �ļ���С
* ����: liuryu
* ʱ��: 2017-3-31
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
* ��������:int GetFileTypeR(const char * pFile, char * type)
* ˵��:�ж�ָ�����ļ�������(��ȡ��׺��)
* �����ߣ�
* �������:
* const char * pFile --�ļ�(��)��
* ���������
* char * type --�ļ�����(��׺��)
* ����ֵ:
* int	-- FILE_TYPE_SUCCESS:�ɹ���ȡ�ļ�����
		-- FILE_TYPE_NOTYPE: �޺�׺��
		-- FILE_TYPE_FAILED: ��ȡʧ�ܣ���������
* ����: liuryu
* ʱ��: 2017-4-6
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
		if('\\' == pFile[i] || '/' == pFile[i]) {	// �ļ��зָ��
			return FILE_TYPE_NOTYPE;
		}
		if('.' == pFile[i]) {	// �ļ����ͷָ��
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
		if('.' == pFile[i] && !type_mode && !is_dot) {	// �ļ����ͷָ��
			dot = i;
			is_dot = 1;
		}
		if('\\' == pFile[i] || '/' == pFile[i]) {	// �����ļ��зָ��
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
	char ch = '/';		// Ĭ�Ϸ���windows/linux��ͨ�÷ָ���

	if(NULL == pFilePath) {
		printf("Invalid input. [getPathSeparator]\n");
		return -1; 
	}
	// �����׸��ָ�������
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
	char ch = '/';		// Ĭ�Ϸ���windows/linux��ͨ�÷ָ���

	if(NULL == pFilePath) {
		printf("Invalid input. [getPathSeparator]\n");
		return -1; 
	}
	if('\\' == separator) {
		ch = '\\';
	}
	// �����ָ���
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
	if('\\' == pPath[i] || '/' == pPath[i]) {  // �����βΪ�ָ���������
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
* ��������:int IsImageFile(const char * pFile)
* ˵��:�ж�ָ�����ļ��Ƿ�Ϊͼ������
* �����ߣ�
* �������:
* const char * pFile --�ļ���
* ���������
* ��
* ����ֵ:
* int  -- IMFILE_TYPE_ERROR:		����
*      -- IMFILE_TYPE_UNKNOWN:	�ļ�����Ԥ��ͼ������
*      -- (> 0):					����Ԥ��ͼ������ֵ
* ����: liuryu
* ʱ��: 2017-4-6
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
* ��������:int FindImageFiles(char * pSrcImgPath, char * pRstImgPath, 
*				std::list<PathElem> &ImgList)
* ˵��:�ж�ָ�����ļ��Ƿ�Ϊͼ������
* �����ߣ�
* �������:
* const char * pFile --�ļ���
* ���������
* ��
* ����ֵ:
* int  -- IMFILE_TYPE_ERROR:		����
*      -- IMFILE_TYPE_UNKNOWN:	�ļ�����Ԥ��ͼ������
*      -- (> 0):					����Ԥ��ͼ������ֵ
* ����: liuryu
* ʱ��: 2017-4-6
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
			printf("·����Ч��������֪��ͼ���ļ���ʽ\n");
			return 0;
		}
	} else if(!isDirExist(pFilePath)) {
		printf("·����Ч��·����Ϊ�ļ��л�ͼ���ļ������鵱ǰ·���Ƿ���Ч\n");
		return 0;
	}

	char srcPath[MAX_PATH*2];
	strcpy_s(srcPath, pFilePath);

	int nLen = strlen(srcPath);
	if('\\' != srcPath[nLen-1]) {
		srcPath[nLen] = '\\';
		srcPath[nLen+1] = '\0';
	}

	//ԴͼƬ���ڵ�Ŀ¼
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
			if(0 >= status)	// �˵���Ԥ��ͼ���ʽ�ļ�
				continue;

			TCHAR   SrcImgPath[MAX_PATH*2];
			lstrcpy(SrcImgPath, srcPath); 
			lstrcat(SrcImgPath, TEXT(wfd.cFileName));
			lstrcpy(stPathElemTemp.SrcImgPath, SrcImgPath); 

			ImgList.push_back(stPathElemTemp);
			nRet++;
		} while(FindNextFile(hFind, &wfd));
	} else {
		printf("����δԤ�ϵĴ���INVALID_HANDLE_VALUE��������[findImageFiles]\n");
		return -1;
	}

nExit:
	if(0 < nRet) {
		printf("�����·���°���ͼ���ļ�%d��\n", nRet);
	} else {
		printf("�����·����δ�ҵ�ͼ���ļ�\n");
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

	if(0 < isImageFile(pSrcImgPath)) {	// ����·��Ϊͼ���ļ�
		lstrcpy(stPathElemTemp.SrcImgPath, pSrcImgPath);
		ImgList.push_back(stPathElemTemp);
		nRet = 0;
		goto nExit;
	} else if(FILE_TYPE_NOTYPE != getFileTypeR(pSrcImgPath, ext)) {	// ����·��Ϊ��Ԥ��ͼ������
		printf("·����Ч��·����Ϊ�ļ��л�ͼ���ļ������鵱ǰ·���Ƿ���Ч\n");
		return 0;
	}

	//ԴͼƬ���ڵ�Ŀ¼
	TCHAR   szFileT1[MAX_PATH*2];
	lstrcpy(szFileT1,TEXT(pSrcImgPath));
	lstrcat(szFileT1, TEXT("\\*.*"));

	//���ͼƬ��ŵ�Ŀ¼
	TCHAR   RstAddr[MAX_PATH*2]; 
	lstrcpy(RstAddr,TEXT(pSrcImgPath));
	lstrcat(RstAddr,TEXT(pRstImgPath));
	_mkdir(RstAddr);   //�����ļ���

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
			if(0 >= status)	// �˵���Ԥ��ͼ���ʽ�ļ�
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
		printf("�������·���£�����ͼ���ļ�%d��\n", nRet);
	} else {
		printf("�������·���£�δ�ҵ�ͼ���ļ�\n", nRet);
	}

	return nRet;
}
*/
