#include "stdafx.h"
#include <stdio.h>

#include <math.h>

#include "types.h"
#include "core.h"

const int cnRyuAtanHLUT_10SH[90] = {
	9, 27, 45, 63, 81, 99, 117, 135, 153, 171, 
	190, 208, 227, 246, 265, 284, 303, 323, 343, 363,
	383, 403, 424, 445, 467, 488, 511, 533, 556, 579,
	603, 628, 652, 678, 704, 730, 758, 786, 815, 844,
	875, 906, 938, 972, 1006, 1042, 1079, 1117, 1157, 1199,
	1242, 1287, 1335, 1384, 1436, 1490, 1547, 1607, 1671, 1738,
	1810, 1886, 1967, 2054, 2147, 2247, 2355, 2472, 2600, 2739,
	2892, 3060, 3248, 3457, 3692, 3960, 4265, 4619, 5033, 5525,
	6119, 6852, 7778, 8988, 10635, 13011, 16742, 23453, 39105, 117339
};

const int cnRyuSinLUT_10SH[91] = {
	0, 17, 35, 53, 71, 89, 107, 124, 142, 160,
	177, 195, 212, 230, 247, 265, 282, 299, 316, 333, 
	350, 366, 383, 400, 416, 432, 448, 464, 480, 496,
	511, 527, 542, 557, 572, 587, 601, 616, 630, 644,
	658, 671, 685, 698, 711, 724, 736, 748, 760, 772,
	784, 795, 806, 817, 828, 838, 848, 858, 868, 877,
	886, 895, 904, 912, 920, 928, 935, 942, 949, 955,
	962, 968, 973, 979, 984, 989, 993, 997, 1001, 1005,
	1008, 1011, 1014, 1016, 1018, 1020, 1021, 1022, 1023, 1023, 1024
};


int  ryuAtan180_10SH(int dy, int dx)
{
	int i = 0;

	int start = 0, end = 0;

	int t_sign = 0, t_abs = 0;

	if(dy == 0)
		return 0;

	if(dx == 0)
		return 90;

	t_sign = ((dy ^ dx) >= 0) ? 1 : (-1);
	t_abs = ((abs(dy)) << 10) / abs(dx);

	if(t_abs < cnRyuAtanHLUT_10SH[0]) {
		return 0;
	}

	if(t_abs >= cnRyuAtanHLUT_10SH[89]) {
		return 90;
	}

	for(start = 0, end = 89; start < end; ) {
		i = start + ((end - start)>>1);
		if(t_abs >= cnRyuAtanHLUT_10SH[i] && t_abs < cnRyuAtanHLUT_10SH[i+1]) {
			if(t_sign == 1) 
				return (i + 1);
			else if(t_sign == -1)
				return (179 - i);
		}
		else if(t_abs < cnRyuAtanHLUT_10SH[i]) {
			end = i;
		}
		else {
			start = i + 1;
		}
	}

	return -1;
}

int ryuCycle(int a, int range)
{
	int d = a;
	while(d < 0) {
		d += range;
	}

	while(d >= range) {
		d -= range;
	}

	return d;
}

int ryuCycleDistance(int a, int b, int cycle)
{
	int d = abs(a - b);

	if(a < 0 || b < 0 || a >= cycle || b >= cycle) {
#ifdef	_PRINT_PROMPT
		printf("\nWARN! Bad input of ryuCycleDistance, a=%d, b=%d, cycle=%d\n", 
			a, b, cycle);
#endif
		return -1;
	}

	d = (d < cycle - d) ? d : (cycle - d);

	return d;
}

int ryuSin_10SH(int angel)
{
	int theta = ryuCycle(angel, 360);

	if(90 >= theta)
		return cnRyuSinLUT_10SH[theta];
	else if(90 < theta && 180 >= theta)
		return cnRyuSinLUT_10SH[180-theta];
	else if(180 < theta && 270 >= theta)
		return (0-cnRyuSinLUT_10SH[theta-180]);
	else
		return (0-cnRyuSinLUT_10SH[360-theta]);
}

int ryuCos_10SH(int angel)
{
	int theta = ryuCycle(angel, 360);

	if(90 >= theta)
		return cnRyuSinLUT_10SH[90-theta];
	else if(90 < theta && 180 >= theta)
		return (0 - cnRyuSinLUT_10SH[theta-90]);
	else if(180 < theta && 270 >= theta)
		return (0-cnRyuSinLUT_10SH[270-theta]);
	else
		return cnRyuSinLUT_10SH[theta-270];
}

