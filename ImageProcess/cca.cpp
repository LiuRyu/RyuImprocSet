#include "stdafx.h"

#include "types.h"
#include "core.h"
#include "improc.h"

/*
//- 第一次遍历，标注
static int Label_1st(unsigned char *img, unsigned char *ori, const int w, const int h, const int step, 
			  const unsigned char spe_val);
//- 第二次遍历，合并标号
static void Label_2nd(const int w, const int h);

static void ResetLocal();
static void LocalAdd(const int new_label);
static int  GetProperLabel(int * cur_label);


//------------------------------
//- Member variables
int	*		m_pLabelCCA	= 0;	// 标号矩阵：存储区域的标号
int			m_maxWidthCCA = 0;	// 设定最大图像宽度
int			m_maxHeightCCA = 0;	// 设定最大图像高度
int			m_maxCCN	= 0;	// 内存中可用的最大连通域个数，预先分配得到。
int			m_nRealCCN	= 0;	// 连通域分析得到的连通域个数，有可能大于m_nMaxCCN
int			m_nCCN		= 0;	// 用户后续使用m_pCCN时实际会操作的连通域数目，m_nCCN = min(m_nRealCCN, m_nMaxCCN)

int			m_isCCAMemStorage = 0;

//----------------------------------------------------------
//- 处理局部连通关系，以便给当前中心像素得到一个合适的label
int			m_nLocalLabel;
int			m_LocalLabel[4];
int			m_MinLocalLabel;


int ryuCreateCCAMemStorage(CvSize max_size, const int max_CCN)
{
	CvMemStorage
	int status = 0;

	if(m_isCCAMemStorage)
		ryuReleaseCCAMemStorage();

	if(2 >= max_size.width || 2 >= max_size.height || 0 >= max_CCN) {
		return -1;
	}

	m_maxWidthCCA	= max_size.width;
	m_maxHeightCCA	= max_size.height;
	m_pLabelCCA	= (int *)malloc(m_maxWidthCCA * m_maxHeightCCA * sizeof(int));
	if(!m_pLabelCCA) {
		return -1;
	}

	m_maxCCN = max_CCN;
	status = ryuCreateUnionFindMemStorage(max_CCN + 10);
	if(1 != status) {
		return -1;
	}

	m_isCCAMemStorage = 1;
	return 1;
}

void ryuReleaseCCAMemStorage()
{
	if(m_pLabelCCA) {
		delete[] m_pLabelCCA;
		m_pLabelCCA = 0;
	}
	m_maxWidthCCA	= 0;
	m_maxHeightCCA	= 0;

	m_nCCN 	= 0;

	ryuReleaseUnionFindMemStorage();

	m_isCCAMemStorage = 0;
}

int ClassicCCA_LabelImage(IplImage * im, IplImage * label, const unsigned char spe_val, 
						  const int max_CCN, int * storage)
{
	if(m_isCCAMemStorage != 1) {
		return -1;
	}

	if(!img 
		|| !m_pLabelCCA
		|| w > m_maxWidthCCA
		|| h > m_maxHeightCCA)
		return -1;

	ryuUnionFindReset();
	m_nRealCCN = Label_1st(img, ori, w, h, step, spe_val, ori_thre);
	m_nCCN = m_nRealCCN;
	if(m_nCCN > m_maxCCN)
		m_nCCN = m_maxCCN;

	Label_2nd(w, h);

	return m_nCCN;
}

int * ClassicCCA_GetLabelMat()
{
	return m_pLabelCCA;
}

//- 返回有效的CC的数目，所谓有效是说当实际标注的连通域个数m_nRealCCN大于m_nMaxCCN时，会被截短为m_nMaxCCN，即
//-		如果 (m_nRealCCN > m_nMaxCCN)
//-			则 m_nCCN = m_nMaxCCN
//-		如果 (m_nRealCCN <= m_nMaxCCN)
//-			则 m_nCCN = m_nRealCCN
int ClassicCCA_GetCount()
{
	return m_nCCN;
}

void ClassicCCA_SetCount(int count)
{
	if(count > 0 && count <= m_maxCCN)
	{
		m_nCCN = count;
	}
	else
		m_nCCN = 0;
}

//- 重置，即清空
void ClassicCCA_Reset()
{
	m_nCCN		= 0;
	m_nRealCCN	= 0;
}

int ClassicCCA_GetMaxCount()
{
	return m_maxCCN;
}
int ClassicCCA_GetRealCount()		// 与GetCount()相比，它返回的数字是实际执行CCA时的个数，它可能比m_nMaxCCN还要大
{
	return m_nRealCCN;
}

int Label_1st(unsigned char *img, unsigned char *ori, const int w, const int h, const int step, 
				const unsigned char spe_val, const unsigned char ori_thre)
{
	int x, y, i;
	unsigned char *p, *p2;
	unsigned char *q, *q2;
	int *pl;
	const int noffset0	= 2;   //此掩模适用于每行第一个像素
	int offset0[2]		// = {-step, 1-step};
	{
		offset0[0] = -step;
		offset0[1] = 1-step;
	}
	int offset0_lb[2]	//= {-w, 1-w};
	{
		offset0_lb[0] = -w;
		offset0_lb[1] = 1-w;
	}

	const int noffset1	= 4;   //此掩模适用于每行中间的像素
	int offset1[4]		//= {-1-step, -step, 1-step, -1};
	{
		offset1[0] = -1-step;
		offset1[1] = -step;
		offset1[2] = 1-step;
		offset1[3] = -1;
	}
	int offset1_lb[4]	//= {-1-w, -w, 1-w, -1};
	{
		offset1_lb[0] = -1-w;
		offset1_lb[1] = -w;
		offset1_lb[2] = 1-w;
		offset1_lb[3] = -1;
	}

	const int noffset2	= 3;   //此掩模适用于每行最后一个像素
	int offset2[3]		//= {-1-step, -step, -1};
	{
		offset2[0] = -1-step;
		offset2[1] = -step;
		offset2[2] = -1;
	}
	int offset2_lb[3]	//= {-1-w, -w, -1};
	{
		offset2_lb[0] = -1-w;
		offset2_lb[1] = -w;
		offset2_lb[2] = -1;
	}

	int *label_mat = m_pLabelCCA;
	memset(label_mat, 0, sizeof(int) * w * h);
	int cur_label	= 1;
//	int label_bk	= 0;

	// 1st line
	p	= img;
	p2	= img + w;
	q	= ori;
	q2	= ori + w;
	pl	= label_mat;
	//- 1st pixel in 1st line
	if(*p == spe_val) {
		*pl = cur_label;
		cur_label++;
	}

	for(p++, q++, pl++; p < p2; p++, q++, pl++) { 	//计算得到了第一行的区域联通情况
		if(*p == spe_val) {
			if(*(p-1) == spe_val && 1 == verifyInMainOrientScope_fixed(*q, *(q-1), ori_thre, GRAD_ORIENT_ANGLE_RANGE)) {
				*pl = *(pl-1);
			}
			else {
				*pl = cur_label;
				cur_label++;
			}
		}
	}

	for(y=1; y<h; y++) {
		p	= img + y * step;
		p2	= p + w - 1;
		q   = ori + y * step;
		q2	= q + w - 1;
		pl	= label_mat + y * w;
		// 1st pixel in the line
		if(*p == spe_val) {
			ResetLocal();
			for(i = 0; i < noffset0; i++) {
				if(*(p+offset0[i]) == spe_val && 1 == verifyInMainOrientScope_fixed(*q, *(q+offset0[i]), ori_thre, GRAD_ORIENT_ANGLE_RANGE)) {
					LocalAdd(*(pl + offset0_lb[i]));
				}
			}
			*pl = GetProperLabel(&cur_label);
		}

		// Pixels
		for(x = 1, p++, q++, pl++; p < p2; p++, q++, pl++, x++) {
			if(*p == spe_val) {
				ResetLocal();
				for(i = 0; i < noffset1; i++) {
					if(*(p + offset1[i]) == spe_val && 1 == verifyInMainOrientScope_fixed(*q, *(q+offset1[i]), ori_thre, GRAD_ORIENT_ANGLE_RANGE)) {
						LocalAdd(*(pl + offset1_lb[i]));
					}
				}
				*pl = GetProperLabel(&cur_label);
			}
		}

		// last pixel in the line
		if(*p == spe_val)
		{
			ResetLocal();
			for(i = 0; i < noffset2; i++) {
				if(*(p + offset2[i]) == spe_val && 1 == verifyInMainOrientScope_fixed(*q, *(q+offset2[i]), ori_thre, GRAD_ORIENT_ANGLE_RANGE)) {
					LocalAdd(*(pl + offset2_lb[i]));
				}
			}
			*pl = GetProperLabel(&cur_label);
		}
	}

	return (cur_label - 1);
}

void Label_2nd(const int w, const int h)
{
	int * label_mat = m_pLabelCCA;
	int y;
	int * p, * p2;
	const int label_bk	= 0;

	for(y=0; y<h; y++) {
		p	= label_mat + y * w;
		p2	= p + w;
		for(; p<p2; p++) {
			if(*p > label_bk) {
				//*p = m_pUF->Find(*p);
				*p = ryuUnionFindEx(*p);
			}
		}
	}
}

void ResetLocal()
{
	m_nLocalLabel	= 0;
	m_MinLocalLabel	= 10000000;
}
void LocalAdd(const int new_label)
{
	if(m_MinLocalLabel > new_label)
		m_MinLocalLabel = new_label;
	m_LocalLabel[m_nLocalLabel] = new_label;
	m_nLocalLabel++;
}
int GetProperLabel(int * cur_label)
{
	int i;
	if(m_nLocalLabel > 0)
	{
		for(i = 1; i < m_nLocalLabel; i++)
			ryuUnionFind_UnionOrder(m_LocalLabel[0], m_LocalLabel[i]);
		return m_MinLocalLabel;
	}
	else
	{
		(*cur_label) += 1;
		return (*cur_label - 1);
	}
}
*/

