#include "fdn_reverb.h"

#ifdef __cplusplus
extern "C" {
#endif

	void FDNInit(FDNData* pdat)
	{
		memset(pdat, 0, sizeof(FDNData));//�򵥴ֱ�
	}

	StereoFloat FDNProc(FDNData* pdat, StereoFloat input)
	{
		float matl[FDN_DELAY_NUM];//�����mat֮��ķ���
		float matr[FDN_DELAY_NUM];
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//fdbk�˾���
		{
			matl[i] = 0;
			matr[i] = 0;
			for (int n = 0; n < FDN_DELAY_NUM; ++n)
			{
				matl[i] = pdat->mat[i][n] * pdat->fdbkl[n];
				matr[i] = pdat->mat[i][n] * pdat->fdbkr[n];
			}
		}

		for (int i = 0; i < FDN_DELAY_NUM; ++i)//�����ӳ���
		{
			int input_posl = (pdat->posl[i] + pdat->dlytimel[i]) % FDN_MAX_DELAY;
			int input_posr = (pdat->posr[i] + pdat->dlytimer[i]) % FDN_MAX_DELAY;
			float fdbkl = matl[i] * pdat->g1[i] + pdat->fdbkl[i] * pdat->g2[i];
			float fdbkr = matr[i] * pdat->g1[i] + pdat->fdbkr[i] * pdat->g2[i];
			pdat->datal[i][input_posl] = input.l * pdat->b + fdbkl * (1.0 - pdat->sep) + fdbkr * (pdat->sep);
			pdat->datar[i][input_posr] = input.r * pdat->b + fdbkr * (1.0 - pdat->sep) + fdbkl * (pdat->sep);
		}

		StereoFloat out;
		out.l = out.r = 0;
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//���
		{
			int output_posl = pdat->posl[i] % FDN_MAX_DELAY;
			int output_posr = pdat->posr[i] % FDN_MAX_DELAY;
			float outl = pdat->datal[i][output_posl];
			float outr = pdat->datar[i][output_posr];
			out.l += outl * pdat->c;
			out.r += outr * pdat->c;
			pdat->fdbkl[i] = outl;
			pdat->fdbkr[i] = outr;
		}
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//����pos
		{
			pdat->posl[i] = pdat->posl[i] + 1;
			pdat->posr[i] = pdat->posr[i] + 1;
		}

		out.l = out.l * pdat->wet + input.l * pdat->dry;
		out.r = out.r * pdat->wet + input.r * pdat->dry;
		return out;
	}

	void SetFDNRoomSize(FDNData* pdat, float size_l, float size_r, float mode)
	{
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			//int bl = __delay_M16[i] * size_l;//FDN_DELAY_NUM=16ʱ������ 
			//int br = __delay_M16[i] * size_r;
			float a = (float)(i + 1) / FDN_DELAY_NUM;
			int bl = (pow(mode + 1.0, a) - 1.0) * size_l * FDN_MAX_DELAY;//���Ҳ����ѧ 
			int br = (pow(mode + 1.0, a) - 1.0) * size_r * FDN_MAX_DELAY;
			if (bl >= FDN_MAX_DELAY)bl = FDN_MAX_DELAY - 1;
			if (br >= FDN_MAX_DELAY)br = FDN_MAX_DELAY - 1;
			if (bl < 0)bl = 0;
			if (br < 0)br = 0;

			pdat->dlytimel[i] = bl;
			pdat->dlytimer[i] = br;
		}
	}

	void SetFDNSeparate(FDNData* pdat, float separate)
	{
		pdat->sep = separate;
	}

	void SetFDNDryWet(FDNData* pdat, float dry, float wet)
	{
		pdat->dry = dry;
		pdat->wet = wet;
	}

	void SetFDNFeedback(FDNData* pdat, float feedback1, float feedback2)
	{

		pdat->b = 1.0;//����b��cզ�����ĵ�Ҳû��˵��������
		pdat->c = 1.0;
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			pdat->g1[i] = feedback1;
			pdat->g2[i] = exp((double)pdat->dlytimel[i] / 48000.0 * log(feedback2) / log(2.71828182));
			//���feedback��һ���Ļ������ڲ�ͬ���ȵ�delay��ÿ��delay��˥��ʱ�䲻һ��
			//�������Ϊ��ƽ��˥��ʱ��
		}
	}

	void FDNApplyHadamardMatrix(FDNData* pdat)
	{
		if (FDN_DELAY_NUM == 4)
		{
			for (int i = 0; i < FDN_DELAY_NUM; ++i)
			{
				for (int j = 0; j < FDN_DELAY_NUM; ++j)
				{
					pdat->mat[i][j] = __hadamard_mat4x4[i][j] * 0.5;//���0.5Ҳ��Ϊ�˹�һ��
				}
			}
		}

	}

	void FDNApplyRandomMatrix(FDNData* pdat)
	{
		float tmp[FDN_DELAY_NUM][FDN_DELAY_NUM];
		float power = 0;
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			for (int j = 0; j < FDN_DELAY_NUM; ++j)
			{
				float rnd = (float)(rand() % 10000) / 10000.0 * (rand() % 2 ? 1 : -1);
				power += rnd * rnd;//������������
				tmp[i][j] = rnd;
			}
		}
		power = sqrt(power);//����������ڵ�����
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			for (int j = 0; j < FDN_DELAY_NUM; ++j)
			{
				pdat->mat[i][j] = tmp[i][j] / power;//��һ��
			}
		}
	}

	void FDNApplyHouseholderMatrix(FDNData* pdat)
	{
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			for (int j = 0; j < FDN_DELAY_NUM; ++j)
			{
				if (i != j)
					pdat->mat[i][j] = -1 * 0.5;
				else
					pdat->mat[i][j] = 1 * 0.5;
			}
		}
	}
#ifdef __cplusplus
}
#endif