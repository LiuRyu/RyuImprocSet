#ifndef __CODE_LOCATION_TYPES_C_H__
#define __CODE_LOCATION_TYPES_C_H__

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
*									Gradient Mode								         *
\****************************************************************************************/
// 标记类型
enum {
	RYU_GRADMODE_W3S3		= 0,
	RYU_GRADMODE_W4S4		= 1
};


#endif  __CODE_LOCATION_TYPES_C_H__