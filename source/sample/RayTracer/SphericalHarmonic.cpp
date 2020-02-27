#include "SphericalHarmonic.h"
#include <time.h>



int GetNumCoeff(int nBands) {
	return nBands * (nBands + 2) + 1;
}

int factorial(int n);
float P(int l,int m,float x);
float K(int l, int m);
float SH(int l, int m, float theta, float phi);


float random()
{
	return rand() / (float)RAND_MAX;
}


void UniformSampleSphere(float u, float v, Vector3f& vec, float& theta, float& phi)
{
	float z = 1.f - 2.f * u;
	float r = Mathf::Sqrt(Mathf::Max(0.f, 1.f - z * z));
	phi = 2.f * Mathf::PI * v;
	float x = r * Mathf::Cos(phi);
	float y = r * Mathf::Sin(phi);
	vec.Set(x, y, z);

	theta = Mathf::ACos(z);
}



void SetupSphericalHarmonicSample(SHSample* pSamples, int nBand, int nNumSamples)
{
	//float PI = 3.1415926536;

	srand( (unsigned)time(NULL)); //生成种子

	int nNumSqrtSamples = (int)Mathf::Pow((float)nNumSamples, 0.5f);

	float inv = 1.0f / (float)nNumSqrtSamples;
	int idx = 0;
	for (int i = 0; i < nNumSqrtSamples; i++){
		for (int j = 0; j < nNumSqrtSamples; j++){
			float u = (random() + i) * inv;
			float v = (random() + j) * inv;

			Vector3f vec;
			float theta, phi;
			UniformSampleSphere(u, v, vec, theta, phi);

		/*	float t = (i + (rand() / (float)RAND_MAX)) * inv;
			float theta = (float)(2.0 * PI * t);

			t = (j + rand() / (float)RAND_MAX) * inv;
			t = t * 2.0 - 1.0;
			float phi = acos(t);

			float r = sin(phi);
			float x = (float)(cos(theta) * r);
			float z = (float)(sin(theta) * r);
			float y = (float)cos(phi);*/


			//pSamples[idx].sph.Set(phi, theta, 1.0f);
			pSamples[idx].vec = vec;
			// precompute all SH coefficients for this sample
			for(int l = 0; l <= nBand; ++l) {
				for(int m = -l; m <= l; ++m){
					int index = l * (l + 1) + m;
					pSamples[idx].coeff[index] = SH(l, m, theta, phi);
				}
			}
			idx++;
		}
	}

}



//void SH_setup_spherical_samples(SHSample* samples, int sqrt_n_samples)
//{
//	// fill an N*N*2 array with uniformly distributed
//	// samples across the sphere using jittered stratification
//	int i=0; // array index
//	float oneoverN = 1.0/sqrt_n_samples;
//	for(int a=0; a<sqrt_n_samples; a++) {
//		for(int b=0; b<sqrt_n_samples; b++) {
//			// generate unbiased distribution of spherical coords
//			float x = (a + random()) * oneoverN; // do not reuse results
//			float y = (b + random()) * oneoverN; // each sample must be random
//			float theta = 2.0f * acos(sqrt(1.0f - x));
//			float phi = 2.0f * Mathf::PI * y;
////			samples[i].sph = Vector3f(theta,phi,1.0);
//			// convert spherical coords to unit vector
//			Vector3f vec(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
//			samples[i].vec = vec;
//
//
//			int n = GetNumCoeff();
//			samples[i].coeff = new float[n];
//			// precompute all SH coefficients for this sample
//			for(int l=0; l<= GetBand(); ++l) {
//				for(int m=-l; m<=l; ++m) {
//					int index = l*(l+1)+m;
//					samples[i].coeff[index] = SH(l,m,theta,phi);
//					
//				}
//			}
//			++i;
//		}
//	}
//}




float P(int l,int m,float x)
{
	// evaluate an Associated Legendre Polynomial P(l,m,x) at x
	float pmm = 1.0f;
	if (m > 0)	{
		float somx2 = sqrt((1.0f - x) * (1.0f + x));
		float fact = 1.0f;
		for(int i = 1; i <= m; i++)		{
			pmm *= (-fact) * somx2;
			fact += 2.0;
		}
	}
	if (l == m)
		return pmm;

	float pmmp1 = x * (2.0f * m + 1.0f) * pmm;

	if (l == m + 1)
		return pmmp1;

	float pll = 0.0f;

	for(int ll = m + 2; ll <= l; ++ll)	{
		pll = ( (2.0f * ll - 1.0f) * x * pmmp1-(ll + m - 1.0f) * pmm ) / (ll - m);
		pmm = pmmp1;
		pmmp1 = pll;
	}
	return pll;
}

int factorial(int n)
{
	if (n == 0)	{
		return 1;
	}
	int res = n;
	int nn = n - 1;
	while (nn > 0){
		res *= nn--;
	}
	return res;
}

float K(int l, int m)
{
	// renormalisation constant for SH function
	float temp = ((2.0f * l + 1.0f) * factorial(l - m)) / (4.0f * Mathf::PI * factorial(l + m));
	return sqrt(temp);
}
float SH(int l, int m, float theta, float phi)
{
	// return a point sample of a Spherical Harmonic basis function
	// l is the band, range [0..N]
	// m in the range [-l..l]
	// theta in the range [0..Pi]
	// phi in the range [0..2*Pi]
	const float sqrt2 = sqrt(2.0f);
	if (m == 0) {
		return K(l, 0) * P(l, m, cos(theta));
	}else if (m > 0) {
		return sqrt2 * K(l, m) * cos(m * phi) * P(l, m, cos(theta));
	}else {
		return sqrt2 * K(l, -m) * sin(-m * phi) * P(l, -m, cos(theta));
	}
}



float* projCoeff;

void SphericalHarmonicProjection(SHSample* pSamples, int nBand, int nNumSamples)
{
	int n = GetNumCoeff(nBand);
	projCoeff = new float[n];

	float weight = 4.0f * Mathf::PI / nNumSamples;
	for (int j = 0; j < n; j++)	{
		float sum = 0.0;
		for (int k = 0; k < nNumSamples; k++){
			sum += pSamples[k].origFun * pSamples[k].coeff[j];
		}
		sum *= weight;
		projCoeff[j] = sum;

		//stringstream ss;
		//ss << "i=" << i << " j=" << j << " sum=" << sum << "\n";
		//Console::AppendText(ss.str().c_str());
	}
}


void SphericalHarmonicUnProjection(SHSample* pSamples, int nBand, int nNumSamples)
{
	int n = GetNumCoeff(nBand);
	for (int i = 0; i < nNumSamples; i++){
		float sum = 0.0;
		for (int j = 0; j < n; j++)	{
			sum += projCoeff[j] * pSamples[i].coeff[j];
		}
		pSamples[i].reConsFun = sum;
	}
}




void MatrixMultiply(float* out, const float* m0, const float* m1, int rank)
{
	for (int i = 0; i < rank; i++) {
		for (int j = 0; j < rank; j++) {
			float v = 0.f;
			for (int k = 0; k < rank; k++) {
				v += m0[i * rank + k] * m1[k * rank + j];
			}
			out[i * rank + j] = v;
		}
	}
}

void SHRotateMatrixZ(float* out, float angle)
{
	float s = Mathf::Sin(angle);
	float c = Mathf::Cos(angle);
	float s2 = Mathf::Sin(angle * 2.f);
	float c2 = Mathf::Cos(angle * 2.f);

	int n = GetNumCoeff(2);
	memset(out, 0, sizeof(float) * n * n);
	out[0 * n + 0] = 1.f;

	out[1 * n + 1] = c;
	out[1 * n + 3] = s;
	out[2 * n + 2] = 1.f;
	out[3 * n + 1] = -s;
	out[3 * n + 3] = c;

	out[4 * n + 4] = c2;
	out[4 * n + 8] = s2;
	out[5 * n + 5] = c;
	out[5 * n + 7] = s;
	out[6 * n + 6] = 1.f;
	out[7 * n + 5] = -s;
	out[7 * n + 7] = c;
	out[8 * n + 4] = -s2;
	out[8 * n + 8] = c2;
}

void SHRotateMatrix(float* out, const float eulerZYZ[3])
{
	float tmp0[81];
	float tmp1[81];

	float sqrt3 = Mathf::Sqrt(3.f);

	float m[81];
	int n = GetNumCoeff(2);
	memset(m, 0, sizeof(float) * n * n);
	m[0 * n + 0] = 1.f;

	m[1 * n + 2] = 1.f;
	m[2 * n + 1] = -1.f;
	m[3 * n + 3] = 1.f;

	m[4 * n + 7] = 1.f;
	m[7 * n + 4] = -1.f;
	m[5 * n + 5] = -1.f;
	m[6 * n + 6] = -0.5f;
	m[6 * n + 8] = -0.5f * sqrt3;
	m[8 * n + 6] = -0.5f * sqrt3;
	m[8 * n + 8] = 0.5f;

	SHRotateMatrixZ(tmp0, eulerZYZ[0]);
	MatrixMultiply(tmp1, tmp0, m, 9);

	SHRotateMatrixZ(tmp0, eulerZYZ[1]);
	MatrixMultiply(out, tmp1, tmp0, 9);


	memset(m, 0, sizeof(float) * n * n);
	m[0 * n + 0] = 1.f;

	m[1 * n + 2] = -1.f;
	m[2 * n + 1] = 1.f;
	m[3 * n + 3] = 1.f;

	m[4 * n + 7] = -1.f;
	m[7 * n + 4] = 1.f;
	m[5 * n + 5] = -1.f;
	m[6 * n + 6] = -0.5f;
	m[6 * n + 8] = -0.5f * sqrt3;
	m[8 * n + 6] = -0.5f * sqrt3;
	m[8 * n + 8] = 0.5f;

	MatrixMultiply(tmp0, out, m, 9);
	SHRotateMatrixZ(tmp1, eulerZYZ[2]);

	MatrixMultiply(out, tmp0, tmp1, 9);
}

void SHRotate(float* out, const float* in, const float* matrix, int nRank)
{
	float result[9];
	for (int i = 0; i < nRank; i++) {
		float v = 0.f;
		for (int j = 0; j < nRank; j++) {
			v += in[j] * matrix[j * nRank + i];
		}
		result[i] = v;
	}

	memcpy(out, result, sizeof(float) * nRank);
}




