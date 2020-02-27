#include "stdHeader.h"
#include "RayTracer.h"
#include "GaScene.h"
#include "GaLight.h"
#include "ResourceLoader.h"
#include "Sampler.h"
#include "KDTree.h"
#include "Grid.h"

using namespace Ga;




vector<Attachable*>	lights;



DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	RayTracer* pTracer = (RayTracer*)lpParameter;
	pTracer->RunThread();

	return 0;
}




RayTracer::RayTracer():m_nWidth(0),m_nHeight(0),m_pKDTree(NULL),m_bUseKDTree(true),
m_nStartTime(0),m_nEndTime(),m_bRunning(false)
{
	InitializeCriticalSection(&m_cs);

	InitSampler();
}

RayTracer::~RayTracer()
{
}


bool RayTracer::IsRunning() const
{
	return m_bRunning;
}

unsigned int RayTracer::GetTimeUsed() const
{
	return m_nEndTime - m_nStartTime;
}

int RayTracer::GetProgress() const
{
	return m_nPercent;
}

void RayTracer::StartTimer()
{
	m_nStartTime = timeGetTime();
	m_nPercent = 0;
	m_bRunning = true;
}

void RayTracer::EndTimer()
{
	m_nEndTime = timeGetTime();
	m_nPercent = 100;
	m_bRunning = false;
}


bool RayTracer::Init(int width, int height)
{
	static Scene scene;
	Global::SetSceneManager(&scene);

	LoadXmlScene("test.SAN");

	vector<Attachable*> cameras;
	GetEngine()->GetScene()->GetRootNode()->GetAttachableRecursive(cameras, Attachable::TYPE_CAMERA);
	m_pCamera = (Camera*)cameras[0];


	float aspect = width / (float)height;
	m_pCamera->SetAspect(aspect);

	float fov = m_pCamera->GetFov();
	m_pCamera->SetFov(fov / aspect);


	GetEngine()->GetScene()->UpdateWorld(0.0f, true);
	m_pCamera->UpdateFromParentNode();


	GetEngine()->GetScene()->GetRootNode()->GetAttachableRecursive(lights, Attachable::TYPE_LIGHT);


	m_nWidth	= width;
	m_nHeight	= height;
	m_Image.resize(width * height);

#ifdef SHVERTEX
	InitSphericalHarmonic(4);
	int sampleSqrt = 30;
	m_vSHSample.resize(sampleSqrt * sampleSqrt);
	SH_setup_spherical_samples(&m_vSHSample[0], sampleSqrt);
#endif

	m_pKDTree = new KDTree;
	m_pKDTree->Init(GetLoadedMeshes());

	m_pGrid = new Grid;
	m_pGrid->Init(GetLoadedMeshes());

	return true;
}


void RayTracer::Clear(const Color& color)
{
	for (vector<Color>::iterator it = m_Image.begin(); it != m_Image.end(); ++it)
	{
		*it = color;
	}
}

const Color& RayTracer::GetPixelColor(int x, int y)
{
	return m_Image[y * m_nWidth + x];
}


void RayTracer::SetPixel(int x, int y, const Color& color)
{
	m_Image[y * m_nWidth + x] = color;
}


void RayTracer::RunThread()
{
	while (1)
	{
		EnterCriticalSection(&m_cs);

		if (m_curSampleY == m_nHeight)
		{
			LeaveCriticalSection(&m_cs);
			break;
		}

		int samplex = m_curSampleX;
		int sampley = m_curSampleY;

		m_curSampleX++;
		if (m_curSampleX == m_nWidth)
		{
			m_curSampleX = 0;
			m_curSampleY++;
		}

		int coloredPixel = m_curSampleY * m_nWidth + m_curSampleX;
		int percent = (int)((coloredPixel / (float)(m_nWidth * m_nHeight)) * 100);
		m_nPercent = percent;

		LeaveCriticalSection(&m_cs);

		if (m_curSampleY == m_nHeight)
		{
			EndTimer();
			break;
		}

		TracePixel(samplex, sampley);
	}
}

void RayTracer::TracePixel(int x, int y)
{
	Ray ray = GetRay(x, y);

	Color color = TraceRay(ray);
	SetPixel(x, y, color);	
}


void RayTracer::Run(int nThreads, int sampleSqrt)
{
	m_curSampleX = 0;
	m_curSampleY = 0;

	StartTimer();

#ifdef SHVERTEX
	CalcVertexColor();
	CalcLightCoeff();
#endif

	m_ThreadHandles.resize(nThreads);
	for (int i = 0; i < nThreads; i++)
	{
		m_ThreadHandles[i] = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
	}

	m_nSampleSqrt = sampleSqrt;

	//LeaveCriticalSection(&m_cs);
	//WaitForMultipleObjects(2, m_hThreads, TRUE, INFINITE);
	//assert(m_curSampleX == 0 && m_curSampleY == m_nHeight);
	//EnterCriticalSection(&m_cs);

	//PostProcess();
}


Ray RayTracer::GetRay(int x, int y)
{
	float xPos = (x / (float)m_nWidth) * 2.0f - 1.0f;
	float yPos = (((float)m_nHeight - y) / (float)m_nHeight) * 2.0f - 1.0f;

	Vector3f lookDir = m_pCamera->GetLookDir();
	Vector3f upDir = m_pCamera->GetUpDir();
	Vector3f rightDir = m_pCamera->GetRightDir();


	Matrix4f matProj = m_pCamera->GetProjMatrix();

	float u = xPos / matProj._11;
	float v = yPos / matProj._22;

	Vector3f dir = lookDir + rightDir * u + upDir * v;
	dir.Normalize();
	
	Ray ray(m_pCamera->GetEyePos(), dir);
	return ray;
}


Color RayTracer::TraceRay(const Ray& ray)
{
	Intersection intersection;
	intersection.towSide = true;

	vector<Mesh*> meshes = GetLoadedMeshes();

	bool bIsIntersect = false;
	if (m_bUseKDTree)
	{
		//bIsIntersect = m_pKDTree->TestRay(ray, intersection);
		bIsIntersect = m_pGrid->TestRay(ray, intersection);
	}
	else
	{
		for (int i = 0; i < meshes.size(); i++)
		{
			if (meshes[i]->TestIntersect(intersection, ray))
			{
				bIsIntersect = true;
			}
		}
	}

	if (bIsIntersect)
	{
		return Shade(ray, intersection);
		//return ShadeByVertexColor(ray, intersection);
	}

	return Color::BLACK;
}

Vector3f ReflectRay(const Vector3f& r, const Vector3f& n)
{
	return r + n * -2.0f * DotProduct(r, n);
}

bool TestRay(const Ray& pathRay)
{
	Intersection test;
	test.towSide = false;
	test.backFace = false;

	vector<Mesh*> meshes = GetLoadedMeshes();

	for (int i = 0; i < meshes.size(); i++)
	{
		if (meshes[i]->m_brdf.type == BRDF::BRDF_Light)
		{
			continue;
		}
		if (meshes[i]->TestIntersect(test, pathRay))
		{
			return true;
		}
	}
	
	return false;
}


Vector3f TangentToWorld(const Vector3f& dir, const DifferentialGeometry& dg)
{
	return dg.tangent * dir.x + dg.binormal * dir.y + dg.normal * dir.z;
}



Color RayTracer::Shade(const Ray& ray, const Intersection& inter)
{
	/*LightObject* pLight = (LightObject*)lights[0];

	Vector3f lightDir;
	Quaternionf qRot = pLight->GetParentNode()->GetWorldRotation();
	Vector3Rotate(lightDir, Vector3f::UNIT_Z, qRot);

	Color light = pLight->GetColor();*/

	if (inter.pTri->brdf->type == BRDF::BRDF_Light)
	{
		return inter.pTri->brdf->diffuseColor;
	}


	//return IntegrateAreaLight(inter);


	DifferentialGeometry dg;
	CalcDifferentialGeometry(inter, dg);

	return IntegrateAmbientLight(dg);

}


Color IntegrateTriangleLight(const Triangle* pTri, const DifferentialGeometry& dg, int nSampleSqrt)
{
	float lightPower = 0.0f;

	float u, v, s, t;
	for (int j = 0; j < nSampleSqrt; j++)
	{
		for (int i = 0; i < nSampleSqrt; i++)
		{
			UniformSample(u, v);

			u = (u + j) / (float)nSampleSqrt;
			v = (v + i) / (float)nSampleSqrt;

			UniformSampleTriangle(u, v, s, t);

			Vector3f samplePos = pTri->p0->pos * (1.0f - s - t) + pTri->p1->pos * s + pTri->p2->pos * t;
			Vector3f lightNormal = pTri->p0->normal;


			Vector3f dirToLight = samplePos - dg.pos;
			float r = dirToLight.Length();
			dirToLight /= r;

			Ray pathRay(dg.pos, dirToLight);

			if (!TestRay(pathRay))
			{
				float inv_pdf = (Mathf::Max(DotProduct(-dirToLight, lightNormal), 0.0f) * pTri->area) / (r * r);


				lightPower += Mathf::Max(DotProduct(dg.normal, dirToLight), 0.0f) * inv_pdf;
			}
		}
	}


	lightPower /= (float)(nSampleSqrt * nSampleSqrt);


	Color light(2.f, 2.f, 2.f, 1.0f);
	light *= lightPower;

	return light;
}



Color RayTracer::IntegrateAreaLight(const DifferentialGeometry& dg)
{
	vector<Mesh*> meshes = GetLoadedMeshes();

	Color c = Color::BLACK;

	for (int i = 0; i < meshes.size(); i++)
	{
		if (meshes[i]->m_brdf.type != BRDF::BRDF_Light)
		{
			continue;
		}

		int nNumTri = meshes[i]->GetNumTriangle();
		for (int j = 0; j < nNumTri; j++)
		{
			const Triangle* pTri = meshes[i]->GetTriangle(j);

			c += IntegrateTriangleLight(pTri, dg, m_nSampleSqrt);
		}
	}
	
	return c;
}

Color RayTracer::IntegrateAmbientLight(const DifferentialGeometry& dg)
{
	float lightPower = 0.0f;

	float u, v;
	for (int j = 0; j < m_nSampleSqrt; j++)
	{
		for (int i = 0; i < m_nSampleSqrt; i++)
		{
			UniformSample(u, v);

			u = (u + j) / (float)m_nSampleSqrt;
			v = (v + i) / (float)m_nSampleSqrt;

			Vector3f sampleDirTangent;
			float pdf;
			
			CosineSampleHemiSphere(u, v, sampleDirTangent, pdf);
			Vector3f sampleDir = TangentToWorld(sampleDirTangent, dg);
			Ray pathRay(dg.pos, sampleDir);
			pathRay.origin = dg.pos + sampleDir * 0.001f;

			Intersection inter;
			inter.towSide = true;
			bool bInterset = false;
			if (m_bUseKDTree)
			{
				//bInterset = m_pKDTree->TestRay(pathRay, inter);
				bInterset = m_pGrid->TestRay(pathRay, inter);
			}
			else
			{
				bInterset = TestRay(pathRay);
			}

			if (!bInterset)
			{
				lightPower += Mathf::Max(DotProduct(dg.normal, sampleDir), 0.0f) / pdf;
			}
		}
	}

	lightPower /= (float)(m_nSampleSqrt * m_nSampleSqrt);


	Color light(0.3f, 0.3f, 0.3f, 1.0f);
	light *= lightPower;



	//Ray shadowRay;
	//shadowRay.origin = inter.pos + lightDir * 0.001f;
	//shadowRay.dir = lightDir;

	//Intersection shadowInter;
	//shadowInter.towSide = false;
	//shadowInter.backFace = true;

	//vector<Mesh*> meshes = GetLoadedMeshes();

	//for (int i = 0; i < meshes.size(); i++)
	//{
	//	meshes[i]->TestIntersect(shadowInter, shadowRay);
	//}
	//if (shadowInter.IsIntersected())
	//{
	//	light *= 0.0f;
	//}
	//else
	//{
	//	light *= Mathf::Max(DotProduct(lightDir, inter.normal), 0.0f);
	//}


	Color color;
	ColorModulate(color, light, dg.pTri->brdf->diffuseColor);

	/*if (inter.pMesh->brdf.type == BRDF::BRDF_Reflect)
	{
		Vector3f r = ReflectRay(ray.dir, inter.normal);
		Ray reflectRay;

		reflectRay.origin = inter.pos + r * 0.001f;
		reflectRay.dir = r;

		color = color * 0.7f + TraceRay(reflectRay) * 0.5f;
	}*/

	return  color;
}


void RayTracer::PostProcess()
{
	/*for (int y = 0; y < m_nHeight; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			Color c = m_Image[y * m_nWidth + x];
			float r = c.r / (c.r + 0.187f) * 1.035f;
			float g = c.g / (c.g + 0.187f) * 1.035f;
			float b = c.b / (c.b + 0.187f) * 1.035f;
			m_Image[y * m_nWidth + x].Set(r, g, b, 1.0f);
		}
	}*/

}


void RayTracer::TracePixelKDTree(int x, int y)
{
	m_bUseKDTree = true;
	Ray ray = GetRay(x, y);

	Color color = TraceRay(ray);
	SetPixel(x, y, color);

	m_bUseKDTree = false;
}

































void RayTracer::ShaderVertex(Vertex* pVertex, Mesh* pMesh)
{
	DifferentialGeometry dg;
	dg.pos = pVertex->pos;
	dg.normal = pVertex->normal;
	dg.tangent = pVertex->tangent;
	dg.binormal = pVertex->binormal;

	float lightPower = 0.0f;
	int numSamplex = 16;
	int numSampley = 16;

	int nNumCoeff = GetNumCoeff();

	for (int i = 0; i < m_vSHSample.size(); i++)
	{
		Vector3f sampleDirTangent = m_vSHSample[i].vec;
		float pdf = 1.0f / (4.0f * Mathf::PI);
		//UniformSampleHemiSphere(u, v, sampleDirTangent, pdf);
		//CosineSampleHemiSphere(u, v, sampleDirTangent, pdf);
		Vector3f sampleDir = sampleDirTangent;//TangentToWorld(sampleDirTangent, inter);
		Ray pathRay(dg.pos, sampleDir);

		if (!TestRay(pathRay))
		{
			lightPower += Mathf::Max(DotProduct(dg.normal, sampleDir), 0.0f) / pdf;

			float p = Mathf::Max(DotProduct(dg.normal, sampleDir), 0.0f);
			for (int j = 0; j < nNumCoeff; j++)
			{
				pVertex->coeff[j] += m_vSHSample[i].coeff[j] * p;
			}
		}

	}


	float factor = (4.0f * Mathf::PI) / (float)(m_vSHSample.size());
	for (int j = 0; j < nNumCoeff; j++)
	{
		pVertex->coeff[j] = pVertex->coeff[j] * factor;
	}



	lightPower /= (float)(numSamplex * numSampley);


	Color light(0.3f, 0.3f, 0.3f, 1.0f);
	light *= lightPower;


	Color color;
	ColorModulate(color, light, pMesh->m_brdf.diffuseColor);

	pVertex->color.Set(color.r, color.g, color.b);
}


void RayTracer::CalcVertexColor()
{
	vector<Mesh*> meshes = GetLoadedMeshes();

	for (int i = 0; i < meshes.size(); i++)
	{
		int numVert = meshes[i]->GetNumVertex();
		for (int v = 0; v < numVert; v++)
		{
			Vertex* pVertex = meshes[i]->GetVertex(v);

			pVertex->coeff = new float[GetNumCoeff()];
			for (int j = 0; j < GetNumCoeff(); j++)
			{
				pVertex->coeff[j] = 0.0f;
			}

			ShaderVertex(pVertex, meshes[i]);
		}
	}
}


void RayTracer::CalcLightCoeff()
{
	int nNumCoeff = GetNumCoeff();
	m_pLightCoeff = new float[nNumCoeff];

	memset(m_pLightCoeff, 0, sizeof(float) * nNumCoeff);

	for (int i = 0; i < m_vSHSample.size(); i++)
	{
		Vector3f sampleDirTangent = m_vSHSample[i].vec;
		float pdf = 1.0f / (4.0f * Mathf::PI);
		//UniformSampleHemiSphere(u, v, sampleDirTangent, pdf);
		//CosineSampleHemiSphere(u, v, sampleDirTangent, pdf);
		Vector3f sampleDir = sampleDirTangent;//TangentToWorld(sampleDirTangent, inter);
		
		float p = 0.3f;
		for (int j = 0; j < nNumCoeff; j++)
		{
			m_pLightCoeff[j] += m_vSHSample[i].coeff[j] * p;
		}
	}


	float factor = (4.0f * Mathf::PI) / (float)(m_vSHSample.size());
	for (int j = 0; j < nNumCoeff; j++)
	{
		m_pLightCoeff[j] = m_pLightCoeff[j] * factor;
	}
}


Vector3f CalcVertexColorBySH(const Vertex* pVertex, const float* pLightCoeff)
{
	float lightTransfer = 0.0f;
	for (int i = 0; i < GetNumCoeff(); i++)
	{
		lightTransfer += pVertex->coeff[i] * pLightCoeff[i];
	}

	lightTransfer = Mathf::Max(0.0f, lightTransfer);

	return Vector3f(lightTransfer, lightTransfer, lightTransfer);
}



Color RayTracer::ShadeByVertexColor(const Ray& ray, const Intersection& inter)
{
	Vector3f c0 = CalcVertexColorBySH(inter.pTri->p0, m_pLightCoeff);
	Vector3f c1 = CalcVertexColorBySH(inter.pTri->p1, m_pLightCoeff);
	Vector3f c2 = CalcVertexColorBySH(inter.pTri->p2, m_pLightCoeff);

	Vector3f color = c0 * (1.0f - inter.u - inter.v) + c1 * inter.u +c2 * inter.v;


	Color brdf = inter.pTri->brdf->diffuseColor;

	return Color(color.x * brdf.r, color.y * brdf.g, color.z * brdf.b, 1.0f);



	//return Color(inter.color.x, inter.color.y, inter.color.z, 1.0f);
}
