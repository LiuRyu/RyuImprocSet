#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"


static unsigned char solveLinearSystem(double * A, double * B, double * X, int n)	// n=8
{
	double tmp = 0, cof = 0;
	int i = 0, row = 0, tmprow = 0, j = 0, k = 0;

	for(i=0; i<n; ++i)
	{
		row=i*n;
		for(j=i; j<n; ++j)
		{
			tmprow=j*n;
			if(A[tmprow+i]!=0)
			{
				cof=A[tmprow+i];
				for(k=i; k<n; ++k)
					A[tmprow+k]/=cof;
				B[j]/=cof;
				if(j!=i)
				{
					for(k=i; k<n; ++k)
					{
						tmp=A[tmprow+k];
						A[tmprow+k]=A[row+k];
						A[row+k]=tmp;
					}
					tmp=B[j];
					B[j]=B[i];
					B[i]=tmp;
				}
				break;
			}
		}
		if(j==n)
			return 0;

		for(j=i+1; j<n; ++j)
		{
			tmprow=j*n;
			cof=A[tmprow+i];
			for(k=i; k<n; ++k)
			{
				A[tmprow+k] -= A[row +k] * cof;
			}
			B[j] -= B[i]*cof;
		}
	}

	for(i=n-1; i>0; --i)
	{
		row=i*n;
		tmprow;
		for(j=i-1; j>=0; --j)
		{
			tmprow=j*n;
			B[j] -= B[i]*A[tmprow+i];
		}
	}

	for(i=0; i<n; ++i)
		X[i]=B[i];

	return 1;
}


int getPerspectiveTransformMat(const int * src, const int * dst, double * mat)
{
	double A[64], B[8];
	int i = 0, j = 0, k = 0;

	mat[8] = 1;
	memset(A, 0, 64 * sizeof(double));
	memset(B, 0, 8 * sizeof(double));

	for(i = 0; i < 4; ++i) {
		j = i << 3;
		k = i << 1;
		A[j] = A[j+35] = src[k];
		A[j+1] = A[j+36] = src[k+1];
		A[j+2] = A[j+37] = 1;
		A[j+6] = -src[k] * dst[k];
		A[j+7] = -src[k+1] * dst[k];
		A[j+38] = -src[k] * dst[k+1];
		A[j+39] = -src[k+1] * dst[k+1];
		B[i] = dst[k];
		B[i+4] = dst[k+1];
	}
	if(0 >= solveLinearSystem(A, B, mat, 8))
		return -1;

	return 1;
}

int warpPerspectiveTransformFixed(RyuImage * src, RyuImage * dst, double * matrix)
{
	int nRet = 0, i = 0;

	unsigned char * srcMat1 = 0, * srcMat2 = 0, * dstMat = 0, * pDst = 0;
	int y = 0, x = 0, tmp = 0;
	int vxd = 0, vxu = 0, vyd = 0, vyu = 0;

	int st = 0;
	int s1 = 0, s2 = 0, s3 = 0, s4 = 0;

	int vx = 0, vy = 0;
	int dx = 0, ux = 0, dy = 0, uy = 0;
	int thre1 = 0, thre2 = 0;

	int lTmp1 = 0, lTmp2 = 0, lTmp3 = 0;
	int lTMP1 = 0, lTMP2 = 0, lTMP3 = 0;
	int matrix_int[9];

	if (!src || !dst || !matrix) {
		nRet = -1;
		goto nExit;
	}

	if(src->width <= 0 || src->height <= 0 || dst->width <= 0 || dst->height <= 0) {
		nRet = -1;
		goto nExit;
	}

	matrix_int[0] = (int)(matrix[0] * (1<<18));
	matrix_int[1] = (int)(matrix[1] * (1<<18));
	matrix_int[2] = (int)(matrix[2] * (1<<18));
	matrix_int[3] = (int)(matrix[3] * (1<<18));
	matrix_int[4] = (int)(matrix[4] * (1<<18));
	matrix_int[5] = (int)(matrix[5] * (1<<18));
	matrix_int[6] = (int)(matrix[6] * (1<<14));
	matrix_int[7] = (int)(matrix[7] * (1<<14));

	thre1 = src->width - 1;
	thre2 = src->height - 1;

	dstMat = (unsigned char *)dst->imageData;
	lTMP1 = matrix_int[2];
	lTMP2 = matrix_int[5];
	lTMP3 = (1<<14);

	for(y = dst->height; y > 0; y--) {
		lTmp1 = lTMP1;
		lTmp2 = lTMP2;
		lTmp3 = lTMP3;
		pDst  = dstMat;
		for(x = dst->width; x > 0; x--) {
			//vx = (x * matrix_int[0] + y * matrix_int[1] + matrix_int[2]) / (x * matrix_int[6] + y * matrix_int[7] + (1<<16));
			//vy = (x * matrix_int[3] + y * matrix_int[4] + matrix_int[5]) / (x * matrix_int[6] + y * matrix_int[7] + (1<<16));

			vx = lTmp1 / lTmp3;
			vy = lTmp2 / lTmp3;

			lTmp1 += matrix_int[0];
			lTmp2 += matrix_int[3];
			lTmp3 += matrix_int[6];

			//双线性插值算法
			vxd = vx >> 4;
			dx  = vx - (vxd << 4);
			ux = 16 - dx;

			vyd = vy >> 4;
			dy  = vy - (vyd << 4);
			uy = 16 - dy;

			st = ((0 <= vxd) && (0 <= vyd) && (thre1 > vxd) && (thre2 > vyd)) ? 1 : 0;

			srcMat1 = (unsigned char *)src->imageData + vyd * src->widthStep + vxd;
			srcMat2 = srcMat1 + src->widthStep;

			s1 = (st) ? srcMat2[1] : 0;
			s2 = (st) ? srcMat1[1] : 0;
			s3 = (st) ? srcMat2[0] : 0;
			s4 = (st) ? srcMat1[0] : 0;

			tmp = (dx * dy * s1 + dx * uy * s2 + ux * dy * s3 + ux * uy * s4) >> 8;

			*(pDst++) = (tmp > 255) ? 255 : tmp;
		}
		lTMP1  += matrix_int[1];
		lTMP2  += matrix_int[4];
		lTMP3  += matrix_int[7];
		dstMat += dst->widthStep;
	}

	nRet = 1;
nExit:
	return nRet;
}
