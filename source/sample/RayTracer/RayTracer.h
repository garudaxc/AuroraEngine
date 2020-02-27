#pragma once
#include "GaColor.h"
#include "GaCamera.h"
#include "Primitive.h"
#include <Windows.h>
#include "SphericalHarmonic.h"

using namespace std;

class KDTree;
class Grid;

class RayTracer
{
public:

	RayTracer();
	RayTracer(const RayTracer& ob)	{}
	~RayTracer();

	bool				Init(int width, int height);

	void				Clear(const Ga::Color& color);

	void				TracePixel(int x, int y);

	void				TracePixelKDTree(int x, int y);

	void				RunThread();
	void				Run(int nThreads, int sampleSqrt);

	const Ga::Color&	GetPixelColor(int x, int y);
	void				SetPixel(int x, int y, const Ga::Color& color);

	void				PostProcess();

	void				CalcVertexColor();
	void				CalcLightCoeff();
	void				ShaderVertex(Vertex* pVertex, Mesh* pMesh);

	bool				IsRunning() const;
	unsigned int		GetTimeUsed() const;
	int					GetProgress() const;


private:


	Ray					GetRay(int x, int y);

	Ga::Color			TraceRay(const Ray& ray);
	Ga::Color			Shade(const Ray& ray, const Intersection& inter);
	Ga::Color			ShadeByVertexColor(const Ray& ray, const Intersection& inter);

	Ga::Color			IntegrateAreaLight(const DifferentialGeometry& dg);
	Ga::Color			IntegrateAmbientLight(const DifferentialGeometry& dg);

	int						m_nWidth;
	int						m_nHeight;
	vector<Ga::Color>		m_Image;
	int						m_nSampleSqrt;

	Ga::Camera*				m_pCamera;
	bool					m_bUseKDTree;
	KDTree*					m_pKDTree;
	Grid*					m_pGrid;


	CRITICAL_SECTION		m_cs;
	vector<HANDLE>			m_ThreadHandles;
	int						m_curSampleX;
	int						m_curSampleY;

	// timer;
	unsigned int			m_nStartTime;
	unsigned int			m_nEndTime;
	int						m_nPercent;
	bool					m_bRunning;
	void					StartTimer();
	void					EndTimer();

	vector<SHSample>		m_vSHSample;
	float*					m_pLightCoeff;
};


void CalcVertexColor();