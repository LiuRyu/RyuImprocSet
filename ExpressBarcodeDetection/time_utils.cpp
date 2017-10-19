#include "stdafx.h"  
#include <time.h>  
#include <stdio.h>  
#include <stdlib.h>   
#include <stdarg.h>  
#include <windows.h>

#include "types.h"
#include "general_utils.h"

LARGE_INTEGER s_time[10] = {{0}}, e_time[10] = {{0}};

/*********************************************************************
* 函数名称: 
* 说明: 
* 调用者： 
* 输入参数: 
* const char *pStrPath -- 用户指定的根路径
* 输出参数：
* 无
* 返回值:
* int  -- CREAT_FILEDIR_FAILED:  失败
*      -- CREAT_FILEDIR_SUCCESS: 成功
* 作者: liuryu
* 时间: 2017-3-31
*********************************************************************/ 
void ryuTimerStart(int seq) 
{
	if(TIMER_SEQ_0 > seq || TIMER_SEQ_9 < seq) {
#if _PRINT_GENERAL_UTILITIES_
		printf("Error! Invalid input. [GeneralUti../time../TimerStart]. seq=%d\n", seq);
#endif
		return;
	}
	LARGE_INTEGER frequency = {0};
	QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&s_time[seq]);
	return;
}

long ryuTimerStop(int seq)
{
	long TimeCost = 0;

	if(TIMER_SEQ_0 > seq || TIMER_SEQ_9 < seq) {
#if _PRINT_GENERAL_UTILITIES_
		printf("Error! Invalid input. [GeneralUti../time../TimerStart]. seq=%d\n", seq);
#endif
		return 0;
	}
	QueryPerformanceCounter((LARGE_INTEGER*) &e_time[seq]);

	LARGE_INTEGER frequency = {0};
	QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
	TimeCost = 1000000 * (e_time[seq].QuadPart - s_time[seq].QuadPart) / frequency.QuadPart;

	return TimeCost;
}

