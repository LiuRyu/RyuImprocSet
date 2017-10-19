#ifndef __IMAGE_USER_INTERFACE_TYPES_C_H__
#define __IMAGE_USER_INTERFACE_TYPES_C_H__

#include <stdlib.h>

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
*									Label Image									         *
\****************************************************************************************/
// 标记类型
enum {
	RYU_LABELIM_POINT		= 1,
	RYU_LABELIM_LINE		= 2,
	RYU_LABELIM_FIXEDLINE	= 3,
	RYU_LABELIM_RECT		= 4,
	RYU_LABELIM_FIXEDRECT	= 5,
	RYU_LABELIM_ROTATERECT	= 6,
	RYU_LABELIM_CROSSPOINT	= 7
};


/****************************************************************************************\
*									Draw Diagram								         *
\****************************************************************************************/
// 绘制图表值预处理方式
enum {
	RYU_DRAWDIAG_VALUE_ORIN		= 0,
	RYU_DRAWDIAG_VALUE_ABS		= 1
};
// 绘制图表类型
enum {
	RYU_DRAWDIAG_TYPE_COLUMN	= 0,
	RYU_DRAWDIAG_TYPE_CURVE		= 1
};



#endif  __IMAGE_USER_INTERFACE_TYPES_C_H__

