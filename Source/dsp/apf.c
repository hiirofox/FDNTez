#include "apf.h"

float APFProc(apf32* pdat, float sign, float a)
{
	sign += pdat->z2 * a;
	a = pdat->z2 - sign * a;
	pdat->z2 = pdat->z1, pdat->z1 = sign;
	return a;
}
