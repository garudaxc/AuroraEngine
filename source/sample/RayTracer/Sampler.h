#pragma once
#include "stdHeader.h"


void InitSampler();



void UniformSample(float& u, float& v);






void UniformSampleHemiSphere(float u, float v, Ga::Vector3f& vSample, float& pdf);


void CosineSampleHemiSphere(float u, float v, Ga::Vector3f& vSample, float& pdf);


void UniformSampleTriangle(float s0, float s1, float& u, float &v);