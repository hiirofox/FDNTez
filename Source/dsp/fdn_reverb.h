/*
* Hiirofox 2024/06/24
*/
#ifndef _HDSP_FDN_REVERB_
#define _HDSP_FDN_REVERB_

#include <stdint.h>
#include <math.h>
#include "freq_shift.h"

#define FDN_DELAY_NUM 12     //fdn�����delay line�ĸ���
#define FDN_MAX_DELAY 12000 //fdbÿ��delay line����󳤶� 

typedef struct _STEREO_FLOAT_
{
	float l, r;
}StereoFloat;

typedef struct _FDN_DATA_
{
	float datal[FDN_DELAY_NUM][FDN_MAX_DELAY];//������������
	float datar[FDN_DELAY_NUM][FDN_MAX_DELAY];
	float fdbkl[FDN_DELAY_NUM];//��������ʱ��
	float fdbkr[FDN_DELAY_NUM];
	float totfbl;//һ����(��freq shift)
	float totfbr;
	FreqShiftData freqshiftl;
	FreqShiftData freqshiftr;
	int32_t posl[FDN_DELAY_NUM];//�ӳٵ�λ��
	int32_t posr[FDN_DELAY_NUM];

	//param
	int32_t dlytimel[FDN_DELAY_NUM];//�ӳ�ʱ��
	int32_t dlytimer[FDN_DELAY_NUM];//�ӳ�ʱ��
	float b;//����
	float c;//���
	float g1[FDN_DELAY_NUM], g2[FDN_DELAY_NUM];//����
	float g3;
	float mat[FDN_DELAY_NUM][FDN_DELAY_NUM];//�����˸�����
	float sep;//�������������
	//����ѷ����߲������һ������Ϳ�����Ϊ����������������
	float dry, wet;
}FDNData;

//Hadamard���� 
static float __hadamard_mat2x2[2][2] = { {1, 1},
										 {1,-1} };
static float __hadamard_mat4x4[4][4] = { {1, 1, 1, 1},
										 {1,-1, 1,-1},
										 {1, 1,-1,-1},
										 {1,-1,-1, 1} };

//16���ӳٵĴ�Ų���
static int32_t __delay_M16[16] = { 441, 513, 593, 738, 890, 1135, 1388, 1689, 2059, 2446, 2874, 3388, 3906, 4466, 5089, 5755 };

#ifdef __cplusplus
extern "C" {
#endif

	void FDNInit(FDNData* pdat);//��ʼ��
	StereoFloat FDNProc(FDNData* pdat, StereoFloat input);//����������
	void SetFDNRoomSize(FDNData* pdat, float size_l, float size_r, float mode);//���÷����С[0,1]���Լ��ֲ���ʽ[0,1]
	void SetFDNSeparate(FDNData* pdat, float separate);//�������������[0,1]
	void SetFDNDryWet(FDNData* pdat, float dry, float wet);//��ʪ��[0,1]
	void SetFDNFeedback(FDNData* pdat, float feedback1, float feedback2, float feedback3);//����[0,1]
	void SetFDNFreqShift(FDNData* pdat, float freq);//����freq shift����
	//�����Ƿ��������ѡ��
	void FDNApplyHadamardMatrix(FDNData* pdat);//Ӧ��Hadamard����(��˵�Ǻܺõ�һ��)
	void FDNApplyRandomMatrix(FDNData* pdat);  //Ӧ���������(����������FDN_DELAY_NUM)
	void FDNApplyHouseholderMatrix(FDNData* pdat);  //Ӧ��Householder����(��˵�����ﵽһ���Ļ����ܶ�)
#ifdef __cplusplus
}
#endif


#endif