#include "stdafx.h"
#include <stdio.h>  
#include <stdlib.h>   
#include <string.h>  
#include <windows.h>

#include "types.h"
#include "general_utils.h"


void ryuThousandType(long val, char (&Dest)[32U])
{
	if(NULL == Dest) {
#if _PRINT_GENERAL_UTILITIES_
		printf("Error! Invalid input. [GeneralUti../print../ThousandType]. Dest=0x%x\n", Dest);
#endif
		return;
	}
	char stmp[32] = "0";
	int i = 0, tic = 0, len = 0;
	sprintf_s(stmp, "%d", val);

	for(i = strlen(stmp) - 1; i >= 0; i--) {
		Dest[len++] = stmp[i];
		if(!((++tic) % 3))
			Dest[len++] = ',';
	}
	if(',' == Dest[len-1]) {
		Dest[len-1] = '\0';
		len -= 1;
	}
	else
		Dest[len] = '\0';

	for(i = 0; i < (len >> 1); i++) {
		char c = Dest[i];
		Dest[i] = Dest[len-i-1];
		Dest[len-i-1] = c;
	}
	return;
}


