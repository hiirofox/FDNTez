#ifndef _HDSP_FREQ_SHIFT_
#define _HDSP_FREQ_SHIFT_

#include <stdint.h>
#include "apf.h"

typedef struct _FREQ_SHIFT_DATA_
{
	apf32 a1, a2, a3, a4;
	apf32 a5, a6, a7, a8;
	float* costable;
	float* sintable;
	uint32_t pos;//表的指针
	uint32_t spd;//转的速度
}FreqShiftData;

static const float __fs_re[4] = { 0.4021921162426, 0.8561710882420, 0.9722909545651, 0.9952884791278 };  //hilbert iir apf常数
static const float __fs_im[4] = { 0.6923878000000, 0.9360654322959, 0.9882295226860, 0.9987488452737 };

#ifdef __cplusplus
extern "C" {
#endif

	float FreqShiftProc(FreqShiftData* pdat, float in);
	void FreqShiftSetTablePtr(FreqShiftData* pdat, float* costable, float* sintable);
	void FreqShiftSetFreq(FreqShiftData* pdat, float freq);
#ifdef __cplusplus
}
#endif

#endif