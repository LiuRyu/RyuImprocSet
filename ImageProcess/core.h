#ifndef __RYU_CORE_H__
#define __RYU_CORE_H__

#ifdef IMAGEPROCESSING_EXPORTS
#define IMAGEPROCESSING_API __declspec(dllexport)
#else
#define IMAGEPROCESSING_API __declspec(dllimport)
#endif

#include "types.h"

IMAGEPROCESSING_API RyuPoint ryuPoint(int x, int y);

IMAGEPROCESSING_API RyuRect ryuRect(int x, int y, int width, int height);

IMAGEPROCESSING_API RyuSize ryuSize(int width, int height);

IMAGEPROCESSING_API RyuPoint ryuDivideIntPoint(int point);

IMAGEPROCESSING_API int ryuDistanceBtPoints(RyuPoint pt1, RyuPoint pt2);


IMAGEPROCESSING_API RyuROI * ryuCreateROI(int xOffset, int yOffset, int width, int height);

IMAGEPROCESSING_API void ryuSetImageROI(RyuImage* image, RyuRect rect);

IMAGEPROCESSING_API RyuRect ryuGetImageROI(const RyuImage * image);

IMAGEPROCESSING_API void ryuResetImageROI(RyuImage * image);

IMAGEPROCESSING_API RyuImage * ryuCreateImageHeader(RyuSize size, int depth, int channels);

IMAGEPROCESSING_API void * ryuInitImageHeader(RyuImage * image, RyuSize size, int depth, int channels);

IMAGEPROCESSING_API RyuImage * ryuCreateImage(RyuSize size, int depth, int channels);

IMAGEPROCESSING_API void ryuReleaseImageHeader( RyuImage ** image );

IMAGEPROCESSING_API void ryuReleaseImage( RyuImage ** image );

IMAGEPROCESSING_API void * ryuSetImage( RyuImage * image, RyuSize size, int depth, int channels );

IMAGEPROCESSING_API void ryuZero( RyuImage * image );

IMAGEPROCESSING_API RyuSize ryuGetSize(RyuImage * image);

IMAGEPROCESSING_API int ryuGetPixel(RyuImage * image, RyuPoint pt);

IMAGEPROCESSING_API int ryuSetPixel(RyuImage * image, RyuPoint pt, unsigned char val);

IMAGEPROCESSING_API int ryuResizeImage(RyuImage * img_in, RyuImage * img_out);

IMAGEPROCESSING_API int ryuSub(RyuImage * src1, RyuImage * src2, RyuImage * dst);

/****************************************************************************************\
*										Math Fast									     *
\****************************************************************************************/
IMAGEPROCESSING_API int  ryuAtan180_10SH(int dy, int dx);

IMAGEPROCESSING_API int ryuCycle(int a, int range);

IMAGEPROCESSING_API int ryuCycleDistance(int a, int b, int cycle);

IMAGEPROCESSING_API int ryuSin_10SH(int angel);

IMAGEPROCESSING_API int ryuCos_10SH(int angel);

#endif  __RYU_CORE_H__