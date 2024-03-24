#pragma once
#include "stdheader.h"

namespace Aurora
{

	class RenderStateObject;

	enum class BlendFactor : uint8
	{
		Zero = 0,
		One,
		SrcColor,
		InvSrcColor,
		DestColor,
		InvDestColor,
		SrcAlpha,
		InvSrcAlpha,
		DestAplha,
		InvDestAlpha,
		BlendFactor,
		InvBlendFactor,
	};


	enum class BlendOp : uint8
	{
		Add = 0,
		Subtract,
		RevSubtract,
		Min,
		Max,
	};

	enum class CullMode : uint8
	{
		Front = 0,
		Back,
		None,
	};

	enum class FillMode : uint8
	{
		WireFrame = 0,
		Solid,
	};

	enum class CompareFunc : uint8
	{
		Never = 0,
		Equal,
		NotEuqal,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Always,
	};



	enum class StencilOp : uint8
	{
		Keep = 0,
		Zero,
		Replace,
		IncrSat,
		DecrSat,
		Invert,
		Incr,
		Decr,
	};

//	
//
//
//
//
//	class DepthStencilStateOjbect : public RenderStateObject
//	{
//	public:
//		struct Desc
//		{
//			bool								DepthEable;
//			bool								DepthWriteEnable;
//			RenderStateObject::CompareFunc		DepthFunc;
//			bool								StencilEnable;
//			uint8								StencilReadMask;
//			uint8								StencilWriteMask;
//			RenderStateObject::StencilOp		StencilOpFrontFace;
//			RenderStateObject::StencilOp		StencilOpBackFace;
//
//
//			Desc() :DepthEable(true), DepthWriteEnable(true), DepthFunc(RenderStateObject::CompareLessEqual),
//				StencilEnable(false), StencilReadMask(0), StencilWriteMask(0),
//				StencilOpFrontFace(RenderStateObject::StencilOpZero), StencilOpBackFace(RenderStateObject::StencilOpZero) {}
//
//		};
//
//		Desc				desc;
//
//		//DepthStencilStateOjbect(const Desc& d):desc(d)	{}
//
//			/* BOOL DepthEnable;
//			D3D10_DEPTH_WRITE_MASK DepthWriteMask;
//			D3D10_COMPARISON_FUNC DepthFunc;
//			BOOL StencilEnable;
//			UINT8 StencilReadMask;
//			UINT8 StencilWriteMask;
//			D3D10_DEPTH_STENCILOP_DESC FrontFace;
//			D3D10_DEPTH_STENCILOP_DESC BackFace;*/
//
//	};
//
//
//	class RasterStateObject : public RenderStateObject
//	{
//	public:
//		struct Desc
//		{
//			RenderStateObject::FillMode			FillMode;
//			RenderStateObject::CullMode			CullMode;
//			bool								FrontClockwise;
//			int									DepthBias;
//			float								DepthBiasClamp;
//			float								Slope;
//			bool								ScissorEnable;
//			bool								MultisampleEnable;
//			bool								AntialiasedLineEable;
//
//			Desc() :FillMode(RenderStateObject::FillSolid), CullMode(RenderStateObject::CullBack),
//				FrontClockwise(false), DepthBias(0), DepthBiasClamp(0.0f), Slope(0.0f), ScissorEnable(false),
//				MultisampleEnable(false), AntialiasedLineEable(false) {}
//		};
//
//
//		Desc				desc;
//		/*D3D10_FILL_MODE FillMode;
//			D3D10_CULL_MODE CullMode;
//			BOOL FrontCounterClockwise;
//			INT DepthBias;
//			FLOAT DepthBiasClamp;
//			FLOAT SlopeScaledDepthBias;
//			BOOL DepthClipEnable;
//			BOOL ScissorEnable;
//			BOOL MultisampleEnable;
//			BOOL AntialiasedLineEnable;
//*/
//	};
//



//		};
//	};
//
//	class SamplerStateObject : public RenderStateObject
//	{
//	public:
//		struct Desc
//		{
//			/*D3D10_FILTER Filter;
//			D3D10_TEXTURE_ADDRESS_MODE AddressU;
//			D3D10_TEXTURE_ADDRESS_MODE AddressV;
//			D3D10_TEXTURE_ADDRESS_MODE AddressW;
//			FLOAT MipLODBias;
//			UINT MaxAnisotropy;
//			D3D10_COMPARISON_FUNC ComparisonFunc;
//			FLOAT BorderColor[4];
//			FLOAT MinLOD;
//			FLOAT MaxLOD;*/
//
//		};
//	};



	class RenderStateBase
	{
	public:
		RenderStateBase() {}
		~RenderStateBase()	{}

		virtual		void	CreateDeviceObject() {}


	private:

	};



	template<bool DepthEnable = true>
	class DepthStencilState : public RenderStateBase
	{
	public:
		bool	DepthEable_;
		bool	DepthWriteEnable_;

		
		DepthStencilState();

		virtual void	CreateDeviceObject();
	};


	
	template<
		bool BlendEnable = false, BlendFactor SrcBlend = BlendFactor::One,
		BlendFactor DestBlend = BlendFactor::Zero, BlendOp Op = BlendOp::Add,
		uint8 WriteMask = 0xff, bool AlphaToCoverageEnable = false>
	class BlendState : public RenderStateBase
	{
	public:
		BlendState()
		{			
		}

		virtual		void	CreateDeviceObject()
		{
			if (StaticHandle_ ==  nullptr) {
				handle_ = GRenderDevice->CreateBlendState(BlendEnable_, SrcBlend_, DestBlend_, BlendOp_, WriteMask_, AlphaToCoverageEnable_);
				StaticHandle_ = handle_;
			} else {
				handle_ = StaticHandle_;
			}
		}

		void BindToDevice()
		{
			if (handle_ != nullptr) {
				GRenderDevice->BindBlendState(handle_, Color::WHITE);
			}
		}


	private:

		bool	BlendEnable_	= BlendEnable;
		BlendFactor SrcBlend_	= SrcBlend;
		BlendFactor DestBlend_ = DestBlend;
		BlendOp  BlendOp_ = Op;
		uint8 WriteMask_ = WriteMask;
		bool AlphaToCoverageEnable_ = AlphaToCoverageEnable;
					   

		static RenderStateObject*  StaticHandle_;
		RenderStateObject* handle_ = -1;
	};

	   

	template< bool BlendEnable, BlendFactor SrcBlend,
		BlendFactor DestBlend, BlendOp Op,
		uint8 WriteMask, bool AlphaToCoverageEnable>
		RenderStateObject* BlendState<BlendEnable, SrcBlend, DestBlend, Op, WriteMask, AlphaToCoverageEnable>::StaticHandle_ = nullptr;








	template <
		bool FrontCounterClockwise = false, CullMode cullMode = CullMode::Back,
		FillMode fillMode = FillMode::Solid, int DepthBias = 0, int SlopeScaledDepthBias = 0,
		bool AntialiasedLineEnable = false>
	class RasterizerState : public RenderStateBase
	{
	public:
		RasterizerState()	{}
		

		virtual		void	CreateDeviceObject()
		{
			if (StaticHandle_ == nullptr) {
				handle_ = GRenderDevice->CreateRasterizerState(FrontCounterClockwise_, cullMode_,
					fillMode_, DepthBias_, SlopeScaledDepthBias_, AntialiasedLineEnable_);
				StaticHandle_ = handle_;
			}
			else {
				handle_ = StaticHandle_;
			}
		}

		void BindToDevice()
		{
			if (handle_ != nullptr) {
				GRenderDevice->BindRasterizerState(handle_);
			}
		}


	private:
		bool FrontCounterClockwise_ = FrontCounterClockwise;
		int SlopeScaledDepthBias_	= SlopeScaledDepthBias;
		bool AntialiasedLineEnable_	= AntialiasedLineEnable;
		CullMode cullMode_	= cullMode;
		FillMode fillMode_	= fillMode;
		int DepthBias_	= DepthBias;


		static RenderStateObject*  StaticHandle_;
		RenderStateObject* handle_ = nullptr;

	};




	template <
		bool FrontCounterClockwise, CullMode cullMode, FillMode fillMode, 
		int DepthBias, int SlopeScaledDepthBias, bool AntialiasedLineEnable>
		RenderStateObject* RasterizerState<FrontCounterClockwise, cullMode, fillMode, 
		DepthBias, SlopeScaledDepthBias, AntialiasedLineEnable>::StaticHandle_ = nullptr;






}