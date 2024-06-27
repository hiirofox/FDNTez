#ifndef _HDSP_APF_
#define _HDSP_APF_

typedef struct _APF32_ {  //多相全通
	float z1, z2, y1, y2;
}apf32;

#ifdef __cplusplus
extern "C" {
#endif

	float APFProc(apf32* pdat, float sign, float a);

#ifdef __cplusplus
}
#endif

#endif