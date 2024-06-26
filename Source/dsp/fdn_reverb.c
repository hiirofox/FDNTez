#include "fdn_reverb.h"

#ifdef __cplusplus
extern "C" {
#endif

	void FDNInit(FDNData* pdat)
	{
		memset(pdat->datal, 0, sizeof(float) * FDN_MAX_DELAY);
		memset(pdat->datar, 0, sizeof(float) * FDN_MAX_DELAY);
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			pdat->posl[i] = 0;
			pdat->posr[i] = 0;
		}
	}

	StereoFloat FDNProc(FDNData* pdat, StereoFloat input)
	{
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//输入延迟线
		{
			int input_posl = (pdat->posl[i] + pdat->dlytimel[i]) % FDN_MAX_DELAY;
			int input_posr = (pdat->posr[i] + pdat->dlytimer[i]) % FDN_MAX_DELAY;
			float fdbkl = pdat->fdbkl[i] * pdat->g[i];//这个fdbk还要处理过才能用
			float fdbkr = pdat->fdbkr[i] * pdat->g[i];
			pdat->datal[i][input_posl] = input.l * pdat->b[i] + fdbkl * (1.0 - pdat->sep) + fdbkr * (pdat->sep);
			pdat->datar[i][input_posr] = input.r * pdat->b[i] + fdbkr * (1.0 - pdat->sep) + fdbkl * (pdat->sep);
		}

		StereoFloat out;
		out.l = out.r = 0;
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//输出
		{
			int output_posl = pdat->posl[i] % FDN_MAX_DELAY;
			int output_posr = pdat->posr[i] % FDN_MAX_DELAY;
			float outl = pdat->datal[i][output_posl];
			float outr = pdat->datar[i][output_posr];
			out.l += outl * pdat->c[i];
			out.r += outr * pdat->c[i];
			pdat->fdbkl[i] = outl;
			pdat->fdbkr[i] = outr;
		}
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//更新pos
		{
			pdat->posl[i] = pdat->posl[i] + 1;
			pdat->posr[i] = pdat->posr[i] + 1;
		}

		float tmpl[FDN_DELAY_NUM];
		float tmpr[FDN_DELAY_NUM];
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//fdbk乘矩阵
		{
			tmpl[i] = 0;
			tmpr[i] = 0;
			for (int n = 0; n < FDN_DELAY_NUM; ++n)
			{
				tmpl[i] = pdat->mat[i][n] * pdat->fdbkl[n];
				tmpr[i] = pdat->mat[i][n] * pdat->fdbkr[n];
			}
		}
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//复制回去
		{
			pdat->fdbkl[i] = tmpl[i];
			pdat->fdbkr[i] = tmpr[i];
		}

		out.l = out.l * pdat->wet + input.l * pdat->dry;
		out.r = out.r * pdat->wet + input.r * pdat->dry;
		return out;
	}

	void SetFDNRoomSize(FDNData* pdat, float size_l, float size_r, float mode)
	{
		srand(200002017);
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{

			float rnd = (float)(rand() % 10000) / 10000.0;
			float a = 1.0 - pow(rnd, mode);
			int bl = a * FDN_MAX_DELAY * size_l;
			int br = a * FDN_MAX_DELAY * size_r;
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

	void SetFDNFeedback(FDNData* pdat, float feedback)
	{
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			pdat->b[i] = 1.0;//关于b和c咋调，文档也没有说明。。。
			pdat->c[i] = 1.0;
			pdat->g[i] = feedback;
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
					pdat->mat[i][j] = __hadamard_mat4x4[i][j] * 0.5;//这个0.5也是为了归一化
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
				power += rnd * rnd;//用来计算能量
				tmp[i][j] = rnd;
			}
		}
		power = sqrt(power);//这个矩阵现在的能量
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			for (int j = 0; j < FDN_DELAY_NUM; ++j)
			{
				pdat->mat[i][j] = tmp[i][j] / power;//归一化
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