#ifndef __EXWAYBILL_CUTOUT_H__
#define __EXWAYBILL_CUTOUT_H__

#include "ryucv.h"

#ifdef  EXWAYBILLCUTOUT_EXPORTS
#define EXWAYBILLCUTOUT_API __declspec(dllexport)
#else
#define EXWAYBILLCUTOUT_API __declspec(dllimport)
#endif


/****************************************************************************************\
*									DLL Export										     *
\****************************************************************************************/
EXWAYBILLCUTOUT_API int ExwaybillCutout_init(int width, int height);

EXWAYBILLCUTOUT_API int ExwaybillCutout_process(RyuImage * im);

EXWAYBILLCUTOUT_API void ExwaybillCutout_release();


/****************************************************************************************\
*									Private sapce									     *
\****************************************************************************************/
typedef struct stWaveAnalyInterval
{
	int lPos;
	int rPos;
} WaveAnalyInterval;

#endif __EXWAYBILL_CUTOUT_H__

