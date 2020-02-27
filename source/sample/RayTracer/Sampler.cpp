#include "Sampler.h"
#include <stdlib.h>
#include <time.h>


using namespace Ga;


void InitSampler()
{
	srand( (unsigned)time( NULL ) );
}


float Randf()
{
	return rand() / (float)RAND_MAX;
}




void UniformSample(float& u, float& v)
{
	u = Randf();
	v = Randf();
}







Ga::Vector3f UniformSampleSphere()
{
	float x, y, z;
	do 
	{
		x = Randf();
		y = Randf();
		z = Randf();

	} while (x * x + y * y + z * z > 1.0f);

	Vector3f v(x, y, z);
	v.Normalize();

	return v;
}



void UniformSampleHemiSphere(float u, float v, Ga::Vector3f& vSample, float& pdf)
{
	float r = Ga::Mathf::Sqrt(1.0f - u * u);  
	float phi = 2.0f * Ga::Mathf::PI * v;  
	vSample.Set(Ga::Mathf::Cos(phi) * r,  Ga::Mathf::Sin(phi) * r, u);

	static float inv_two_pi = 1.0f / (2.0f * 3.14159265f);
	pdf = inv_two_pi;
}

void CosineSampleHemiSphere(float u, float v, Ga::Vector3f& vSample, float& pdf)
{
	float r = Mathf::Sqrt(u);  
	float theta = 2 * Mathf::PI * v;  

	float x = r * Mathf::Cos(theta);  
	float y = r * Mathf::Sin(theta);
	float z = Mathf::Sqrt(Mathf::Max(0.00001f, 1 - x * x - y * y));

	vSample.Set(x, y, z);

	pdf = z / Mathf::PI;
}



void UniformSampleTriangle(float s0, float s1, float& u, float &v)
{
	float tmp = Mathf::Sqrt(s0);
	u = 1.0f - tmp;
	v = s1 * tmp;
}