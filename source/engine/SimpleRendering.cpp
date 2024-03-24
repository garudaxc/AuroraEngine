#include "stdheader.h"
#include "SimpleRendering.h"
#include "Renderer.h"
#include "Entity.h"
#include "FileSystem.h"
#include "SceneView.h"
#include "Texture.h"
#include "RenderState.h"
#include "HelperDraw.h"
#include "Shader.h"
namespace Aurora
{	

	struct DirectionalLightInfo
	{
		Vector3f	Direction = Vector3f::UNIT_X;
		uint32		Flag = 0;
		Color		LightColor = Color::WHITE;
	};

	struct PointLightInfo
	{
		Vector3f	Position = Vector3f::UNIT_X;
		uint32		Flag = 0;
		Color		LightColor = Color::WHITE;
	};
	

	ModelShaderVS modelvs_;
	ModelShaderPS modelps_;

	RasterizerState<true> rasterState;

	SimpleRendering GSimpleRendering;

	Texture* pTexture = nullptr;

	Texture* TextureEnvCube = nullptr;
	
	CViewShaderParameterBuffer ViewShaderParameter;

	class CSimpleVisitor : public RenderableVisitor
	{
	public:
		virtual void Visit(RenderOperator& op)
		{
			op.mVertexFactory = CVertexFactoryPNTT::Instance;
			GRenderDevice->ExecuteOperator(op);
		}
	};


	CSimpleVisitor    SimpleVisitor_;

	void InitRandomSampleing();

	void SimpleRendering::Initialize()
	{
		// BaseShader HsbShader;
		// HsbShader.InitBase(BaseShader::PIXEL_SHADER, "../dev/data/shader/hsb.shader");
	
		
		BaseShader glTestShader;
		glTestShader.InitBase(BaseShader::VERTEX_SHADER, "../dev/data/shader/shader330.vert");	

		
		bool Result = ViewShaderParameter.CreateDeviceObject();
		assert(Result);
		
		HelperDraw.Init();

		modelvs_.Initialize();
		modelps_.Initialize();
		rasterState.CreateDeviceObject();

		pTexture =  GTextureManager.GetTexture("resource:Model/T_White_GrenadeLauncher_D.TGA");
		TextureEnvCube = GTextureManager.GetTexture("resource:sky_cube_mipmap.dds");
		//pTexture =  GTextureManager.GetTexture("resource:test cube.dds");
		
		InitRandomSampleing();

	}

	void SimpleRendering::RenderSceneView(SceneView* view)
	{
		GRenderDevice->Clear(IRenderDevice::CLEAR_FRAME_BUFFER | IRenderDevice::CLEAR_DEPTH_BUFFER, Color(0.2f, 0.2f, 0.2f, 0.2f));

		ViewShaderParameter.mViewMatrix = view->matView;
		ViewShaderParameter.mViewMatrixInverse = view->matView.Inverse();
		ViewShaderParameter.mProjectionMatrix = view->matProj;
		ViewShaderParameter.mViewProjectionMatrix = view->matViewProj;
		ViewShaderParameter.Commit();

		Vector3f lightDir = { 1.f, -1.f, 1.f };
		lightDir.Normalize();
		// globalShaderParam_.DirectionLight.Direction = lightDir;
		ViewShaderParameter.DirectionLightDirection0 = lightDir;
		

		modelvs_.BindShader();
		modelps_.BindShader();
		// modelps_.BindTextureToRender(pTexture, TextureEnvCube);
		rasterState.BindToDevice();

		DrawingEntity* pEntity = view->pDrawingEntity;
		while (pEntity)
		{
			// modelvs_.matWorld = pEntity->mWorld;
			// modelvs_.matWorld.TransposeSelf();

			modelvs_.mWorldMatrix = pEntity->mWorld;

			modelvs_.CommitShaderParameter();
			modelps_.CommitShaderParameter();

			Entity* pRenderEntity = pEntity->pEntity;
			pRenderEntity->Accept(SimpleVisitor_);

			pEntity = pEntity->pNext;
		}


		// HelperDraw.AddLine(Vector3f::ZERO, Vector3f::UNIT_X * 5.f, Color::GREEN, 0.f);
		// HelperDraw.AddLine(Vector3f::ZERO, Vector3f::UNIT_Z * 5.f, Color::BLUE, 0.f);
		// HelperDraw.Flush();

	}


	float HammersleySample[2048];


	void InitRandomSampleing()
	{

		
		// for (int i = 0; i < 2048; i++) {
		// 	HammersleySample[i] = Mathf::Rand01();
		// }		
		//
		// ShaderCode code;
		// char* shaderCode = "                     \
		// 										 \
		// cbuffer StaticBuffer: register(b1)	 \
		// {										 \
		// 	float2	HammersleySample[1024];				 \
		// };										 \
		// float4 Main(float4 pos : POSITION) : SV_POSITION		\
		// {													\
		// 	pos.xy += HammersleySample[0].xy;				\
		// 	return pos;						\
		// }														\
		// 	";
		//
		// code.name = "Static Buffer";
		// code.text = shaderCode;
		// code.type = BaseShader::VERTEX_SHADER;
		//
		// GPUShaderObject* handle = GRenderDevice->CreateGPUShaderObject(code, nullptr);
		//
		// ShaderParameterBindings bindings;
		// bindings.Name = "StaticBuffer";
		// bindings.Bindings.push_back(ShaderParamterBindingItem{ "HammersleySample", 0, HammersleySample });
		// bindings.handle = GRenderDevice->CreateShaderParameterBinding(handle, bindings);
		//
		// GRenderDevice->UpdateShaderParameter(bindings.handle, nullptr);
		// GRenderDevice->BindGlobalParameter(bindings.handle);
	}
	




}