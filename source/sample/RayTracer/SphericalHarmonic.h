#pragma once


#include "Vector.h"
#include "AurMath.h"
#include "Color.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "MathFunction.h"
#include "Types.h"


using namespace Aurora;

struct SHSample
{
	Vector3f		sph;
	Vector3f		vec;
	float			origFun;
	float			reConsFun;
	float*			coeff;
};

int GetNumCoeff(int nBands);

void SetupSphericalHarmonicSample(SHSample* pSamples, int nBand, int nNumSamples);

void SphericalHarmonicProjection(SHSample* pSamples, int nBand, int nNumSamples);

void SphericalHarmonicUnProjection(SHSample* pSamples, int nBand, int nNumSamples);

void SHRotateMatrix(float* out, const float eulerZYZ[3]);
void SHRotate(float* out, const float* in, const float* matrix, int nRank);


