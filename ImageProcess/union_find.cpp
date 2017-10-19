#include "stdafx.h"

#include "types.h"
#include "core.h"
#include "improc.h"


static int UnionFind_IdxValid(int arr_size, int idx);

static void UnionFind_SetParent(int * arr, int idx, int new_root);

//- parent���������֣�
//-		1�� ��-1������ʼ���ݣ���ʾ�Լ�Ϊ���ڵ�
//-		2�� >=0����ʾ�ڵ㡰*��Ϊ��ǰ�ڵ�ĸ��ڵ�
//-		3�� ��-x������ʾ�Լ�Ϊ���ڵ㣬���С�x�����ڵ����Լ���ͳ����
// int	*	m_arrParentUF	= 0;
// int		m_nArrSizeUF	= 0;	// �ڴ�ߴ�
// int		m_maxIdxUF		= 0;	// ʵ�ʵ����ڵ����
// 
// int ryuInitUnionFindMemStorage(int arr_size)
// {
// 	if(0 >= arr_size) {
// 		return -1;
// 	}
// 	if(m_maxIdxUF) {
// 		ryuReleaseUnionFindMemStorage();
// 	}
// 
// 	m_nArrSizeUF = arr_size;
// 	m_arrParentUF = (int *)malloc(m_nArrSizeUF * sizeof(int));
// 	if(!m_arrParentUF) {
// 		return -1;
// 	}
// 
// 	ryuUnionFindReset();
// 	return 1;
// }
// 
// void ryuReleaseUnionFindMemStorage()
// {
// 	m_nArrSizeUF = 0;
// 	if(m_arrParentUF) {
// 		free(m_arrParentUF);
// 		m_arrParentUF = 0;
// 	}
// }

void ryuUnionFindReset(int * arr, int arr_size)
{
	int i;
	if(NULL == arr)
		return;
	for(i = 0; i < arr_size; i++) {
		arr[i] = -1;
	}
}

int ryuUnionFind(int * arr, int arr_size, int idx)
{
	if(NULL == arr || 0 > UnionFind_IdxValid(arr_size, idx))
		return -1;

	if(arr[idx] < 0)
		return idx;
	else
		return ryuUnionFind(arr, arr_size, arr[idx]);
}

int ryuUnionFindEx(int * arr, int arr_size, int idx)
{
	if(NULL == arr || 0 > UnionFind_IdxValid(arr_size, idx))
		return -1;

	if(arr[idx] < 0)
		return idx;
	UnionFind_SetParent(arr, idx, ryuUnionFindEx(arr, arr_size, arr[idx]));
	return arr[idx];
}

void ryuUnionFind_UnionRank(int * arr, int arr_size, int idx1, int idx2)
{
	if(NULL == arr || 0 > UnionFind_IdxValid(arr_size, idx1) 
		|| 0 > UnionFind_IdxValid(arr_size, idx2))
		return;

	//int root1 = Find(idx1);
	//int root2 = Find(idx2);
	int root1 = ryuUnionFindEx(arr, arr_size, idx1);
	int root2 = ryuUnionFindEx(arr, arr_size, idx2);
	if(root1 == root2) 
		return;

	int node_count = arr[root1] + arr[root2];
	if(arr[root1] < arr[root2]) { 	// root1ӵ�и���Ľڵ�
		arr[root2] = root1;
		arr[root1] = node_count;
	}
	else {
		arr[root1] = root2;
		arr[root2] = node_count;
	}
}

void ryuUnionFind_UnionOrder(int * arr, int arr_size, int idx1, int idx2)
{
	if(idx1 == idx2)
		return;

	//int root1 = Find(idx1);
	//int root2 = Find(idx2);
	int root1 = ryuUnionFindEx(arr, arr_size, idx1);
	int root2 = ryuUnionFindEx(arr, arr_size, idx2);
	if(root1 == root2) 
		return;

	int node_count = arr[root1] + arr[root2];
	if(root1 < root2) { 	// root1����Ž�С����root2�ϲ���root1
		arr[root2] = root1;
		arr[root1] = node_count;
	}
	else {
		arr[root1] = root2;
		arr[root2] = node_count;
	}
}

// void ryuUnionFind_SetRootCount(const int max_idx)
// {
// 	m_maxIdxUF = max_idx;
// }

int ryuUnionFind_GetRootCount(int * arr, int arr_size)
{
	int i, counter = 0;
	if(NULL == arr)
		return -1;
//	for(i = 0; i < m_maxIdxUF; i++) {
	for(i = 0; i < arr_size; i++) {
		if(arr[i] < 0)
			counter++;
	}
	return counter;
}

int UnionFind_IdxValid(int arr_size, int idx)
{
	return ((idx <= arr_size - 1) ? idx : -1);
}

void UnionFind_SetParent(int * arr, int idx, int new_root)
{
	arr[idx] = new_root;
}




