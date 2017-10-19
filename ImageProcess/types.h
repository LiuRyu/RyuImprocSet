#ifndef __IMAGE_PROCESSING_TYPES_C_H__
#define __IMAGE_PROCESSING_TYPES_C_H__

#include <stdio.h>
#include <stdlib.h>


#ifdef IMAGEPROCESSING_EXPORTS
#define IMAGEPROCESSING_API __declspec(dllexport)
#else
#define IMAGEPROCESSING_API __declspec(dllimport)
#endif


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
*									macro Min & Max								         *
\****************************************************************************************/
#ifndef RYUMAX
#define RYUMAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef RYUMIN
#define RYUMIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif


/****************************************************************************************\
*										ryu's point								         *
\****************************************************************************************/
#ifndef RYUPOINT
#define RYUPOINT
typedef struct RyuPoint
{
	int x;
	int y;
}
RyuPoint;
#endif

/****************************************************************************************\
*										ryu's line								         *
\****************************************************************************************/
#ifndef RYULINE
#define RYULINE
typedef struct RyuLine
{
	RyuPoint pt0;
	RyuPoint pt1;
}
RyuLine;
#endif


#ifndef RYUHOUGHLINE
#define RYUHOUGHLINE
typedef struct RyuHoughLine
{
	int		 theta;
	int		 rho;
	int		 ptCnt;
}
RyuHoughLine;
#endif

#ifndef RYUFULLLINE
#define RYUFULLLINE
typedef struct RyuFullLine
{
	RyuPoint pt0;
	RyuPoint pt1;
	int		 theta;
	int		 rho;
	int		 ptCnt;
}
RyuFullLine;
#endif

/****************************************************************************************\
*										ryu's rect								         *
\****************************************************************************************/
#ifndef RYURECT
#define RYURECT
typedef struct RyuRect
{
	int x;
	int y;
	int width;
	int height;
}
RyuRect;
#endif

/****************************************************************************************\
*										ryu's size								         *
\****************************************************************************************/

#ifndef RYUSIZE
#define RYUSIZE
typedef struct RyuSize
{
	int width;
	int height;
}
RyuSize;
#endif

/****************************************************************************************\
*										ryu's memheap									 *
\****************************************************************************************/
#ifndef RYUMEMHEAP
#define RYUMEMHEAP
typedef struct RyuMemHeap
{
	int dataAlign;
	int dataCount;
	int dataStep;

	union
	{
		unsigned char * ptr;
		short * s;
		int	* i;
		float * fl;
		double * db;
	} data;
}
RyuMemHeap;
#endif

/****************************************************************************************\
*										ryu's image								         *
\****************************************************************************************/
#ifndef RYUIMAGEROI
#define RYUIMAGEROI
typedef struct RyuROI
{
	int  coi;		/* 0 - no COI (all channels are selected), 1 - 0th channel is selected ... */
	int  xOffset;
	int  yOffset;
	int  width;
	int  height;
}
RyuROI;
#endif

#ifndef RYUIMAGE
#define RYUIMAGE
typedef struct RyuImage
{
	int nSize;		/* sizeof(IplImage) */
	int align;		/* Alignment of image rows (4 or 8). */

	int imageSize;	/* Image data size in bytes
                       (==image->height*image->widthStep
                       in case of interleaved data) */

	unsigned char * imageData;	/* Pointer to aligned image data. */
	int width;		/* Image width in pixels. */
	int height;		/* Image height in pixels. */

	int depth;
	int nChannels;

	int widthStep;	/* Size of aligned image row in bytes. */

	RyuROI * roi;	/* Image ROI. If NULL, the whole image is selected. */
	unsigned char *imageDataOrigin;  /* Pointer to very origin of image data
										(not necessarily aligned) -
										needed for correct deallocation */
}
RyuImage;
#endif

#define RYU_DEPTH_8C	8
#define RYU_DEPTH_16S	16
#define RYU_DEPTH_32N	32
#define RYU_DEPTH_SIGN	0x80000000


/****************************************************************************************\
*										Color cvt								         *
\****************************************************************************************/
#define BGR2GRAY(B,G,R)		((R*38 + G*75 + B*15) >> 7)


#endif  __IMAGE_PROCESSING_TYPES_C_H__

