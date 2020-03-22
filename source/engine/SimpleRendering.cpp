#include "stdheader.h"
#include "SimpleRendering.h"
#include "Renderer.h"
#include "Camera.h"
#include "Util.h"
#include "Entity.h"
#include "Renderer.h"
#include "SceneView.h"
#include "Material.h"
#include "Model.h"
#include "Texture.h"
#include "RenderState.h"
#include "HelperDraw.h"
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

	class GlobalShaderParameter
	{
	public:
		GlobalShaderParameter();
		~GlobalShaderParameter();

		void		CreateBinding();
		void		Bind();

		Matrix4f	MatrixView;
		Matrix4f	MatrixInvView;
		Matrix4f	MatrixProj;
		Matrix4f	MatrixViewProj;

		DirectionalLightInfo	DirectionLight;
	private:


		ShaderParamterBindings	bindings_;
		Handle	Handle_ = -1;

	};

	GlobalShaderParameter::GlobalShaderParameter()
	{
	}

	GlobalShaderParameter::~GlobalShaderParameter()
	{
	}

	void GlobalShaderParameter::CreateBinding()
	{
		bindings_.Name = "GlobalParameter";

		//ShaderParamterBindingItem 
		bindings_.Bindings.push_back(ShaderParamterBindingItem{ "matView", 0, MatrixView.Ptr() });
		bindings_.Bindings.push_back(ShaderParamterBindingItem{ "matInvView", 0, MatrixInvView.Ptr() });
		bindings_.Bindings.push_back(ShaderParamterBindingItem{ "matProj", 0, MatrixProj.Ptr() });
		bindings_.Bindings.push_back(ShaderParamterBindingItem{ "matViewProj", 0, MatrixViewProj.Ptr() });

		bindings_.Bindings.push_back(ShaderParamterBindingItem{ "DirectionLight0", 0, DirectionLight.Direction.Ptr() });
		bindings_.Bindings.push_back(ShaderParamterBindingItem{ "LightColor0", 0, DirectionLight.LightColor.Ptr() });

		bindings_.handle = CreateShaderParameterBinding(0, bindings_);
	}


	void GlobalShaderParameter::Bind()
	{
		MatrixView.TransposeSelf();
		MatrixInvView = MatrixView.Inverse();

		MatrixProj.TransposeSelf();
		MatrixViewProj.TransposeSelf();
		UpdateShaderParameter(bindings_.handle);
		BindGlobalParameter(bindings_.handle);
	}


	ModelShaderVS modelvs_;
	ModelShaderPS modelps_;

	RasterizerState<true> rasterState;


	SimpleRendering GSimpleRendering;

	Texture* pTexture = nullptr;


	Texture* TextureEnvCube = nullptr;


	GlobalShaderParameter globalShaderParam_;


	extern vector<VertexLayoutItem>	VertexLayoutPosNormTangentTex;
	Handle VertexLayoutPosNormTangentTexHandle_ = -1;
	int32  VertexLayoutPosNormTangentTexStride = 0;



	class CSimpleVisitor : public RenderableVisitor
	{
	public:
		virtual void Visit(RenderOperator& op)
		{
			op.VertexLayout_ = VertexLayoutPosNormTangentTexHandle_;
			op.VertexStride_ = VertexLayoutPosNormTangentTexStride;
			GRenderDevice->ExecuteOperator(op);
		}
	};


	CSimpleVisitor    SimpleVisitor_;

	void InitRandomSampleing();

	void SimpleRendering::Initialize()
	{
		VertexLayoutPosNormTangentTexHandle_ = CreateVertexLayoutHandle(VertexLayoutPosNormTangentTex);
		VertexLayoutPosNormTangentTexStride = Geometry::CalcVertexStride(VertexLayoutPosNormTangentTex);

		HelperDraw.Init();


		modelvs_.Initialize();
		modelps_.Initialize();
		rasterState.CreateDeviceObject();

		globalShaderParam_.CreateBinding();

		pTexture =  GTextureManager.GetTexture("resource:Model/T_White_GrenadeLauncher_D.TGA");
		TextureEnvCube = GTextureManager.GetTexture("resource:sky_cube_mipmap.dds");
		//pTexture =  GTextureManager.GetTexture("resource:test cube.dds");

		InitRandomSampleing();

	}

	void SimpleRendering::RenderSceneView(SceneView* view)
	{
		GRenderDevice->Clear(IRenderDevice::CLEAR_FRAME_BUFFER | IRenderDevice::CLEAR_DEPTH_BUFFER, Color(0.2f, 0.2f, 0.2f, 0.2f));

		globalShaderParam_.MatrixView = view->matView;
		globalShaderParam_.MatrixProj = view->matProj;
		globalShaderParam_.MatrixViewProj = view->matViewProj;

		Vector3f lightDir = { 1.f, -1.f, 1.f };
		lightDir.Normalize();
		globalShaderParam_.DirectionLight.Direction = lightDir;

		globalShaderParam_.Bind();
		

		modelvs_.BindShader();
		modelps_.BindShader();
		modelps_.BindTextureToRender(pTexture, TextureEnvCube);
		rasterState.BindToDevice();

		DrawingEntity* pEntity = view->pDrawingEntity;
		while (pEntity)
		{
			modelvs_.matWorld = pEntity->mWorld;
			modelvs_.matWorld.TransposeSelf();

			modelvs_.CommitShaderParameter();
			modelps_.CommitShaderParameter();

			Entity* pRenderEntity = pEntity->pEntity;
			pRenderEntity->Accept(SimpleVisitor_);

			pEntity = pEntity->pNext;
		}


		HelperDraw.AddLine(Vector3f::ZERO, Vector3f::UNIT_X * 5.f, Color::GREEN, 0.f);
		HelperDraw.AddLine(Vector3f::ZERO, Vector3f::UNIT_Z * 5.f, Color::BLUE, 0.f);
		HelperDraw.Flush();

	}


	float HammersleySample[2048];


	void InitRandomSampleing()
	{
		for (int i = 0; i < 2048; i++) {
			HammersleySample[i] = Mathf::Rand01();
		}

		ShaderCode code;
		char* shaderCode = "                     \
												 \
		cbuffer StaticBuffer: register(b1)	 \
		{										 \
			float2	HammersleySample[1024];				 \
		};										 \
		float4 Main(float4 pos : POSITION) : SV_POSITION		\
		{													\
			pos.xy += HammersleySample[0].xy;				\
			return pos;						\
		}														\
			";

		code.name = "Static Buffer";
		code.text = shaderCode;
		code.type = BaseShader::VERTEX_SHADER;

		Handle handle = GRenderDevice->CreateShader(code);

		ShaderParamterBindings bindings;
		bindings.Name = "StaticBuffer";
		bindings.Bindings.push_back(ShaderParamterBindingItem{ "HammersleySample", 0, HammersleySample });
		bindings.handle = CreateShaderParameterBinding(handle, bindings);
		
		UpdateShaderParameter(bindings.handle);
		BindGlobalParameter(bindings.handle);
	}
	




}