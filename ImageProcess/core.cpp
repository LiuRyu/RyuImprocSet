#include "stdafx.h"
#include <stdio.h>

#include <malloc.h>
#include <math.h>

#include "types.h"
#include "core.h"


RyuPoint ryuPoint(int x, int y)
{
	RyuPoint p;

	p.x = x;
	p.y = y;

	return p;
}

RyuRect ryuRect(int x, int y, int width, int height)
{
	RyuRect r;

	r.x = x;
	r.y = y;
	r.width = width;
	r.height = height;

	return r;
}


RyuSize ryuSize(int width, int height)
{
	RyuSize s;

	s.width = width;
	s.height = height;

	return s;
}

RyuPoint ryuDivideIntPoint(int point)
{
	int dx = 0, dy = 0;
	int s = 0;

	RyuPoint pt;

	dx = point >> 16;
	s = dx & 0x8000;
	pt.x = (0 == s) ? (dx & 0x0000ffff) : (dx | 0xffff0000);

	dy = point & 0x0000ffff;
	s = dy & 0x8000;
	pt.y = (0 == s) ? (dy) : (dy | 0xffff0000);

	return pt;
}

int ryuDistanceBtPoints(RyuPoint pt1, RyuPoint pt2)
{
	int d = 0;

	d = (pt1.x - pt2.x) * (pt1.x - pt2.x) + (pt1.y - pt2.y) * (pt1.y - pt2.y);
	d = (int)(sqrt(d * 1.0) + 0.5);

	return d;
}


RyuROI * ryuCreateROI( int xOffset, int yOffset, int width, int height )
{
	RyuROI * roi = 0;

	roi = (RyuROI *) malloc( sizeof(RyuROI) );
	if( !roi ) {
		printf("ERROR! Bad alloc_ptr of ryuCreateROI, roi = 0x%x\n", roi);
		return 0;
	}

	roi->xOffset = xOffset;
	roi->yOffset = yOffset;
	roi->width = width;
	roi->height = height;

	return roi;
}

void ryuSetImageROI( RyuImage* image, RyuRect rect )
{
	if( !image ) {
		printf( "ERROR! Invalid input of ryuSetImageROI, image = 0x%x\n", image );
		return;
	}

	rect.width += rect.x;
	rect.height += rect.y;

	rect.x = RYUMAX( rect.x, 0 );
	rect.y = RYUMAX( rect.y, 0 );
	rect.width = RYUMIN( rect.width, image->width );
	rect.height = RYUMIN( rect.height, image->height );

	rect.width -= rect.x;
	rect.height -= rect.y;

	if( image->roi ) {
		image->roi->xOffset = rect.x;
		image->roi->yOffset = rect.y;
		image->roi->width = rect.width;
		image->roi->height = rect.height;
	}
	else
		image->roi = ryuCreateROI( rect.x, rect.y, rect.width, rect.height );

	return;
}

RyuRect ryuGetImageROI( const RyuImage * image )
{
	RyuRect rect = ryuRect( 0, 0, 0, 0 );

	if( !image ) {
		printf( "ERROR! Invalid input of ryuGetImageROI, image = 0x%x\n", image );
		return rect;
	}

	if( image->roi )
		rect = ryuRect( image->roi->xOffset, image->roi->yOffset,
		image->roi->width, image->roi->height );
	else
		rect = ryuRect( 0, 0, image->width, image->height );

	return rect;
}

void cvResetImageROI( RyuImage * image )
{
	if( !image ) {
		printf( "ERROR! Invalid input of cvResetImageROI, image = 0x%x\n", image );
		return;
	}

	if( image->roi ) {
		free( image->roi );
		image->roi = 0;
	}
	return;
}


RyuImage * ryuCreateImageHeader( RyuSize size, int depth, int channels )
{
	RyuImage * img = 0;

	img = ( RyuImage * ) malloc( sizeof(RyuImage) );
	if( !img ) {
		printf("ERROR! Bad alloc_ptr of ryuCreateImageHeader, img = 0x%x\n", img);
		return 0;
	}

	ryuInitImageHeader( img, size, depth, channels );

	return img;
}

void * ryuInitImageHeader( RyuImage * image, RyuSize size, int depth, int channels )
{
	if( !image ) {
		printf( "ERROR! Invalid input of ryuInitImageHeader, image = 0x%x\n", image );
		return 0;
	}

	memset( image, 0, sizeof(*image) );
	image->nSize = sizeof( *image );

	if( size.width < 0 || size.height < 0 ) {
		printf( "ERROR! Bad input of ryuInitImageHeader, size.width = %d, size.height = %d\n",
			size.width, size.height );
		return 0;
	}

	if( (depth != (int)RYU_DEPTH_8C && depth != (int)RYU_DEPTH_16S &&
		depth != (int)RYU_DEPTH_32N) || channels < 0 ) {
			printf( "ERROR! Bad input of ryuInitImageHeader, depth = %d, channels = %d\n",
				depth, channels );
			return 0;
	}

	image->width = size.width;
	image->height = size.height;

	if( image->roi ) {
		image->roi->xOffset = image->roi->yOffset = 0;
		image->roi->width = size.width;
		image->roi->height = size.height;
	}

	image->nChannels = RYUMAX( channels, 1 );
	image->depth = depth;
	image->widthStep = image->width * image->nChannels * (depth >> 3);

	image->imageSize = image->widthStep * image->height;

	return image;
}

RyuImage * ryuCreateImage(RyuSize size, int depth, int channels)
{
	RyuImage * img = ryuCreateImageHeader( size, depth, channels );
	if( !img ) {
		printf( "ERROR! Bad alloc_ptr of ryuCreateImage, img = 0x%x\n", img );
		return 0;
	}

	img->imageData = (unsigned char *) malloc( img->imageSize );
	if( !img->imageData ) {
		printf( "ERROR! Bad alloc_ptr of ryuCreateImage, imagedata = 0x%x\n", img->imageData );
		return 0;
	}

	return img;
}

void ryuReleaseImageHeader( RyuImage ** image )
{
	if( !image ) {
		printf( "ERROR! Invalid input of ryuReleaseImageHeader, image = 0x%x\n", image );
		return;
	}

	if( *image ) {
		RyuImage * img = *image;
		*image = 0;
		if( img->roi ) {
			free( img->roi );
			img->roi = 0;
		}
		free( img );
		img = 0;
	}
	return;
}

void ryuReleaseImage( RyuImage ** image )
{
	if( !image ) {
		printf( "ERROR! Invalid input of ryuReleaseImage, image = 0x%x\n", image );
		return;
	}

	if( *image ) {
		RyuImage * img = *image;
		*image = 0;
		free( img->imageData );
		ryuReleaseImageHeader( &img );
	}
}

void * ryuSetImage( RyuImage * image, RyuSize size, int depth, int channels )
{
	int step = 0;
	if( !image ) {
		printf( "ERROR! Invalid input of ryuSetImage, image = 0x%x\n", image );
		return 0;
	}

	if( size.width < 0 || size.height < 0 ) {
		printf( "ERROR! Bad input of ryuSetImage, size.width = %d, size.height = %d\n",
			size.width, size.height );
		return 0;
	}

	if( (depth != (int)RYU_DEPTH_8C && depth != (int)RYU_DEPTH_16S &&
		depth != (int)RYU_DEPTH_32N) || channels <= 0 ) {
			printf( "ERROR! Bad input of ryuSetImage, depth = %d, channels = %d\n",
				depth, channels );
			return 0;
	}

	step = size.width * channels * (depth >> 3);
	if( step * size.height > image->imageSize ) {
		printf( "ERROR! Bad value, too large size for imagedata, set size = %d\n",
			step * size.height );
		return 0;
	}

	image->width = size.width;
	image->height = size.height;
	image->nChannels = channels;
	image->depth = depth;
	image->widthStep = step;

	return image;
}

void ryuZero( RyuImage * image )
{
	RyuRect rect = ryuRect( 0, 0, 0, 0 );

	int i = 0, base = 0, setcount = 0;

	if( !image ) {
		printf( "ERROR! Invalid input of ryuSetImage, image = 0x%x\n", image );
		return;
	}

	if( !image->imageData ) {
		printf( "ERROR! Bad address of ryuSetImage, imagedata = 0x%x\n", image->imageData );
		return;
	}

	if( !image->roi ) {
		memset( image->imageData, 0, image->widthStep * image->height );
	} else {
		base = image->roi->yOffset * image->widthStep + image->roi->xOffset * image->nChannels * (image->depth>>3);
		setcount = image->roi->width* image->nChannels * (image->depth>>3);
		for( i = 0; i < rect.height; i++ ) {
			memset( image->imageData+base, 0, setcount );
			base += image->widthStep;
		}
	}
	return;
}

RyuSize ryuGetSize(RyuImage * image)
{
	RyuSize sz;
	sz.width = image->width;
	sz.height = image->height;

	return sz;
}

int ryuGetPixel(RyuImage * image, RyuPoint pt)
{
	if( !image ) {
		printf( "ERROR! Invalid input of ryuSetImage, image = 0x%x\n", image );
		return -1;
	}

	if( !image->imageData ) {
		printf( "ERROR! Bad address of ryuSetImage, imagedata = 0x%x\n", image->imageData );
		return -2;
	}

	return ((int)(image->imageData + pt.y * image->widthStep + pt.x));
}


int ryuSetPixel(RyuImage * image, RyuPoint pt, unsigned char val)
{
	if( !image ) {
		printf( "ERROR! Invalid input of ryuSetImage, image = 0x%x\n", image );
		return -1;
	}

	if( !image->imageData ) {
		printf( "ERROR! Bad address of ryuSetImage, imagedata = 0x%x\n", image->imageData );
		return -2;
	}

	*(image->imageData + pt.y * image->widthStep + pt.x) = val;

	return 1;
}

// 调整图像大小，目前只具备缩小功能
int ryuResizeImage(RyuImage * img_in, RyuImage * img_out)
{
	double ZoomRatioW = 0.0;
	double ZoomRatioH = 0.0;
	double ZoomAccW = 0.0, ZoomAccH = 0.0;

	int i = 0, j = 0;
	unsigned char * pIn = 0, * pOut = 0;
	//unsigned char * pInL = 0, * pOutL = 0;

	if( !img_in || !img_out ) {
		printf( "ERROR! Invalid input of ryuSetImage, img_in = 0x%x, img_out = 0x%x\n",
			img_in, img_out );
		return -1;
	}

	if( !img_in->imageData || !img_out->imageData ) {
		printf( "ERROR! Bad address of ryuSetImage, img_in data = 0x%x, img_out data = 0x%x\n",
			img_in->imageData, img_out->imageData );
		return -2;
	}

	ZoomRatioW = img_in->width * 1.0 / img_out->width;
	ZoomRatioH = img_in->height * 1.0 / img_out->height;

	for(j = 0; j < img_out->height; j++) {
		ZoomAccW = 0.0;
		pIn = img_in->imageData + (int)(ZoomAccH+0.5) * img_in->widthStep;
		pOut = img_out->imageData + j * img_out->widthStep;
		for(i = 0; i < img_out->width; i++) {
			pOut[i] = pIn[(int)(ZoomAccW+0.5)];
			ZoomAccW = ZoomAccW + ZoomRatioW;
		}
		ZoomAccH = ZoomAccH + ZoomRatioH;
	}

	return 1;
}

int ryuSub(RyuImage * src1, RyuImage * src2, RyuImage * dst)
{
	int i = 0, j = 0;
	unsigned char * pIn1 = 0, * pIn2 = 0, * pOut = 0;
	unsigned char * pInL1 = 0, * pInL2 = 0, * pOutL = 0;

	if( !src1 || !src2 || !dst) {
		printf("ERROR! Invalid input of ryuSetImage, src1 = 0x%x, src2 = 0x%x,\
			   dst = 0x%x\n", 
			   src1, src2, dst);
		return -1;
	}

	if(src1->width != src2->width || src1->width != dst->width
		|| src1->height != src2->height || src1->height != dst->height) {
		printf("ERROR! Invalid image sizes, image must be same scale.\n");
		return -1;
	}

	if( !src1->imageData || !src2->imageData || !dst->imageData) {
		printf("ERROR! Bad address of ryuSetImage, src1 data = 0x%x, \
			   src2 data = 0x%x, dst data = 0x%x\n",
			src1->imageData, src2->imageData, dst->imageData);
		return -1;
	}

	pInL1 = src1->imageData;
	pInL2 = src2->imageData;
	pOutL = dst->imageData;

	for(j = 0; j < src1->height; j++) {
		pIn1 = pInL1;
		pIn2 = pInL2;
		pOut = pOutL;
		for(i = 0; i < src1->width; i++) {
			*pOut = (*pIn1 > *pIn2) ? (*pIn1 - *pIn2) : 0;
			pIn1++;
			pIn2++;
			pOut++;
		}
		pInL1 += src1->widthStep;
		pInL2 += src2->widthStep;
		pOutL += dst->widthStep;
	}

	return 1;
}