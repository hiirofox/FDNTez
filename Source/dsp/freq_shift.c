#include "freq_shift.h"

float FreqShiftProc(FreqShiftData* pdat, float re)
{
	float im = re;

	re = APFProc(&pdat->a1, re, 0.4021921162426);//È«Í¨90¡ãÍøÂç
	re = APFProc(&pdat->a2, re, 0.8561710882420);
	re = APFProc(&pdat->a3, re, 0.9722909545651);
	re = APFProc(&pdat->a4, re, 0.9952884791278);

	im = APFProc(&pdat->a4, im, 0.6923878000000);
	im = APFProc(&pdat->a4, im, 0.9360654322959);
	im = APFProc(&pdat->a4, im, 0.9882295226860);
	im = APFProc(&pdat->a4, im, 0.9987488452737);

	re *= pdat->costable[pdat->pos >> 16];//freq shift
	im *= pdat->sintable[pdat->pos >> 16];

	pdat->pos += pdat->spd;

	return re + im;
}

void FreqShiftSetTablePtr(FreqShiftData* pdat, float* costable, float* sintable)
{
	pdat->costable = costable;
	pdat->sintable = sintable;
}

void FreqShiftSetFreq(FreqShiftData* pdat, float freq)
{
	pdat->spd = (double)((1 << 32) / 48000) * freq;
}
