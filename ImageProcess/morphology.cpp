#include "stdafx.h" 

#include "types.h"
#include "core.h"
#include "improc.h"

int ryuDilate(RyuImage * src, RyuImage * dst)
{
	int nh = src->height - 2; 
	int nw = src->width - 2;

	int i = 0, j = 0;
	unsigned char t = 0, t_l = 0, t_r = 0;

	unsigned char * pIn = src->imageData;
	unsigned char * pOut = dst->imageData;
	unsigned char * lOut = 0;

	unsigned char * loffset, * loffset_t, * loffset_b;
	unsigned char * poffset, * poffset_t, * poffset_b;

	// ��ʼ��
	poffset   = loffset		= pIn;
	poffset_t = loffset_t	= pIn - src->width;
	poffset_b = loffset_b	= pIn + src->width;
	lOut	  = pOut;

	// ���д���
	// -���Ͻ�
	t = RYUMAX(poffset[0], poffset_b[0]);
	t_r = RYUMAX(poffset[1], poffset_b[1]);
	*pOut = RYUMAX(t, t_r);
	t_l = t;
	t = t_r;
	poffset++;	
	poffset_b++;	
	pOut++;
	// -�м䲿��
	for(j = nw; j > 0; j--) {
		t_r = RYUMAX(poffset[1], poffset_b[1]);
		*pOut = RYUMAX(t_l, RYUMAX(t, t_r));
		t_l = t;
		t = t_r;
		poffset++;	
		poffset_b++;	
		pOut++;
	}
	// -���Ͻ�
	*pOut = RYUMAX(t_l, t);

	// �м䲿��
	for(i = nh; i > 0; i--)	
	{
		loffset		+= src->width;
		loffset_t	+= src->width;
		loffset_b	+= src->width;
		poffset		= loffset;
		poffset_t	= loffset_t;
		poffset_b	= loffset_b;

		lOut += dst->width;
		pOut = lOut;

		// -����
		t = RYUMAX(poffset_t[0], RYUMAX(poffset[0], poffset_b[0]));
		t_r = RYUMAX(poffset_t[1], RYUMAX(poffset[1], poffset_b[1]));
		*pOut = RYUMAX(t, t_r);
		t_l = t;
		t = t_r;
		poffset_t++;
		poffset++;	
		poffset_b++;	
		pOut++;
		// -�м䲿��
		for(j = nw; j > 0; j--)
		{
			t_r = RYUMAX(poffset_t[1], RYUMAX(poffset[1], poffset_b[1]));
			*pOut = RYUMAX(t_l, RYUMAX(t, t_r));
			t_l = t;
			t = t_r;
			poffset_t++;
			poffset++;	
			poffset_b++;	
			pOut++;
		}
		// -��ĩ
		*pOut = RYUMAX(t_l, t);
	}

	// ĩ�д���
	poffset		= loffset;
	poffset_b	= loffset_b;
	lOut += dst->width;
	pOut = lOut;
	// -���½�
	t = RYUMAX(poffset[0], poffset_b[0]);
	t_r = RYUMAX(poffset[1], poffset_b[1]);
	*pOut = RYUMAX(t, t_r);
	t_l = t;
	t = t_r;
	poffset++;	
	poffset_b++;	
	pOut++;
	// -�м䲿��
	for(j = nw; j > 0; j--) {
		t_r = RYUMAX(poffset[1], poffset_b[1]);
		*pOut = RYUMAX(t_l, RYUMAX(t, t_r));
		t_l = t;
		t = t_r;
		poffset++;	
		poffset_b++;	
		pOut++;
	}
	// -���Ͻ�
	*pOut = RYUMAX(t_l, t);

	return 1;
}

int ryuErode(RyuImage * src, RyuImage * dst)
{
	int nh = src->height - 2; 
	int nw = src->width - 2;

	int i = 0, j = 0;
	unsigned char t = 0, t_l = 0, t_r = 0;

	unsigned char * pIn = src->imageData;
	unsigned char * pOut = dst->imageData;
	unsigned char * lOut = 0;

	unsigned char * loffset, * loffset_t, * loffset_b;
	unsigned char * poffset, * poffset_t, * poffset_b;

	// ��ʼ��
	poffset   = loffset		= pIn;
	poffset_t = loffset_t	= pIn - src->width;
	poffset_b = loffset_b	= pIn + src->width;
	lOut	  = pOut;

	// ���д���
	// -���Ͻ�
	t = RYUMIN(poffset[0], poffset_b[0]);
	t_r = RYUMIN(poffset[1], poffset_b[1]);
	*pOut = RYUMIN(t, t_r);
	t_l = t;
	t = t_r;
	poffset++;	
	poffset_b++;	
	pOut++;
	// -�м䲿��
	for(j = nw; j > 0; j--) {
		t_r = RYUMIN(poffset[1], poffset_b[1]);
		*pOut = RYUMIN(t_l, RYUMIN(t, t_r));
		t_l = t;
		t = t_r;
		poffset++;	
		poffset_b++;	
		pOut++;
	}
	// -���Ͻ�
	*pOut = RYUMIN(t_l, t);

	// �м䲿��
	for(i = nh; i > 0; i--)	
	{
		loffset		+= src->width;
		loffset_t	+= src->width;
		loffset_b	+= src->width;
		poffset		= loffset;
		poffset_t	= loffset_t;
		poffset_b	= loffset_b;

		lOut += dst->width;
		pOut = lOut;

		// -����
		t = RYUMIN(poffset_t[0], RYUMIN(poffset[0], poffset_b[0]));
		t_r = RYUMIN(poffset_t[1], RYUMIN(poffset[1], poffset_b[1]));
		*pOut = RYUMIN(t, t_r);
		t_l = t;
		t = t_r;
		poffset_t++;
		poffset++;	
		poffset_b++;	
		pOut++;
		// -�м䲿��
		for(j = nw; j > 0; j--)
		{
			t_r = RYUMIN(poffset_t[1], RYUMIN(poffset[1], poffset_b[1]));
			*pOut = RYUMIN(t_l, RYUMIN(t, t_r));
			t_l = t;
			t = t_r;
			poffset_t++;
			poffset++;	
			poffset_b++;	
			pOut++;
		}
		// -��ĩ
		*pOut = RYUMIN(t_l, t);
	}

	// ĩ�д���
	poffset		= loffset;
	poffset_b	= loffset_b;
	lOut += dst->width;
	pOut = lOut;
	// -���½�
	t = RYUMIN(poffset[0], poffset_b[0]);
	t_r = RYUMIN(poffset[1], poffset_b[1]);
	*pOut = RYUMIN(t, t_r);
	t_l = t;
	t = t_r;
	poffset++;	
	poffset_b++;	
	pOut++;
	// -�м䲿��
	for(j = nw; j > 0; j--) {
		t_r = RYUMIN(poffset[1], poffset_b[1]);
		*pOut = RYUMIN(t_l, RYUMIN(t, t_r));
		t_l = t;
		t = t_r;
		poffset++;	
		poffset_b++;	
		pOut++;
	}
	// -���Ͻ�
	*pOut = RYUMIN(t_l, t);

	return 1;
}

