#include "stdafx.h"

#include "ryucv.h"

#define _DEBUG_WAVEFORM_ANALYSIS_
#ifdef  _DEBUG_WAVEFORM_ANALYSIS_
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "image_ui.h"
#endif  _DEBUG_WAVEFORM_ANALYSIS_

#include "Exwaybill_cutout.h"

const int gnWaveAnaly_respondW = 16;

const int gnWaveAnaly_bridgeW = 0;


int ryuWaveformAnalysis(int * wave, int lenth, int seek_step)
{
	int ret_val = 0;
	int k = 0;

	int curPos = 0;
	int baseVal = 0;
	int stepSum = 0;
	int curStat = 0;

	while (curPos < lenth) 
	{
		stepSum = 0;
		curStat = 0;
		if(0 == curPos)
			stepSum = wave[0];
		for(k = 0; k < seek_step; k++) {
			stepSum += (wave[curPos+k+1] - wave[curPos+k]);
			if(stepSum) {
				curStat = 1;
			}
		}
	}

nExit:
	return ret_val;
}


