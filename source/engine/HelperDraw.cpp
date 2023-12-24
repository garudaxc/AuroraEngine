#include "stdheader.h"
#include "HelperDraw.h"
#include "CGeometry.h"
#include "RendererObject.h"
#include "Renderer.h"
#include "Shader.h"

namespace Aurora
{
	class CDrawLineVertexShader : public BaseShader
	{
	public:
		void	Initialize();
	};

	void CDrawLineVertexShader::Initialize()
	{
		AddShaderParameterBuffer(CViewShaderParameterBuffer::Instance);
		InitBase(BaseShader::VERTEX_SHADER, "..\\dev\\data\\shader\\Helper.shader");
	}



	class CDrawLinePixelShader : public BaseShader
	{
	public:
		void	Initialize();
	};

	void CDrawLinePixelShader::Initialize()
	{
		InitBase(BaseShader::PIXEL_SHADER, "..\\dev\\data\\shader\\Helper.shader");
	}

	CDrawLineVertexShader	DrawLineVertexShader;
	CDrawLinePixelShader	DrawLinePixelShader;


	// struct CVertexLayoutPositonColor
	// {
	// 	vector<VertexLayoutItem> items = {
	// 		{Vertex::TYPE_FLOAT3, Vertex::USAGE_POSITION, 0},
	// 		{Vertex::TYPE_UBYTE4_UNORM, Vertex::USAGE_COLOR, 0}
	// 	};
	//
	// 	Handle  mVertexLayout = -1;
	// 	int32	Stride = 0;
	//
	// 	void Init() {
	// 		mVertexLayout = GRenderDevice->CreateVertexLayoutHandle(items);
	// 		Stride = CGeometry::CalcVertexStride(items);
	// 	}
	// };
	//
	// CVertexLayoutPositonColor   VertexLayoutPositonColor_;



	const int32		MaxVertexCount = 100 * 1024;
	const int32		MaxIndexCount = 100 * 1024;


	enum class EHelperDrawType : int8
	{
		Line,
		LineList,
		AABB,
		Circle
	};

	struct HelperLineVertex
	{
		Vector3f	position;
		uint32		color;
	};


	struct FHelperLine
	{
		EHelperDrawType  type = EHelperDrawType::Line;
		Color	color;
		float	life;
		Vector3f a;
		Vector3f b;

		void	AddToBuffer(HelperLineVertex* vertex, uint32* index, int32& vertexBase, int32& indexBase)
		{
			uint32 col = color.ToRGBA8();
			vertex[vertexBase].position = a;
			vertex[vertexBase].color = col;

			vertex[vertexBase + 1].position = b;
			vertex[vertexBase + 1].color = col;

			index[indexBase] = vertexBase;
			index[indexBase + 1] = vertexBase + 1;

			vertexBase += 2;
			indexBase += 2;
		}
	};

	struct FHelperLineList
	{
		EHelperDrawType  type = EHelperDrawType::Line;
		Color	color;
		float	life;
		int32	count = 0;
	};

	struct FHelperAABB
	{
		EHelperDrawType  type = EHelperDrawType::AABB;
		Color	color;
		float	life;
		Vector3f min_;
		Vector3f max_;
	};

	struct FHelperCircle
	{
		EHelperDrawType  type = EHelperDrawType::Circle;
		Color	color;
		float	life;
	};
	

	const	int32 FigureBufferSize = 10 * 1024;
	int8	FigureBuffer[FigureBufferSize];
	int32	FigureBufferPointer = 0;


	template<typename T>
	int32 AddFigure(const T& figure, int32 pointer)
	{
		assert(pointer + sizeof(T) < FigureBufferSize);
		memcpy(FigureBuffer + pointer, &figure, sizeof(T));
		return pointer + sizeof(T);
	}

	template<typename T>
	int32 GetFigure(T& figure, int32 pointer)
	{
		T* p = (T*)(FigureBuffer + pointer);
		figure = *p;
		return pointer + sizeof(T);
	}

	EHelperDrawType GetFigureType(int32 pointer)
	{
		EHelperDrawType* type = (EHelperDrawType*)(FigureBuffer + pointer);
		return *type;
	}


	CHelperDraw HelperDraw;


	CHelperDraw::CHelperDraw()
	{
	}

	CHelperDraw::~CHelperDraw()
	{
	}

	void CHelperDraw::Init()
	{
		DrawLineVertexShader.Initialize();
		DrawLinePixelShader.Initialize();

		// VertexLayoutPositonColor_.Init();
		// VertexBuffer_ = GRenderDevice->CreateVertexBufferHandle(nullptr, MaxVertexCount * VertexLayoutPositonColor_.Stride);
		// IndexBuffer_ = GRenderDevice->CreateIndexBufferHandle(nullptr, MaxIndexCount * 4);
	}

	void CHelperDraw::AddLine(const Vector3f& a, const Vector3f& b, const Color& color, float life)
	{
		FHelperLine line;
		line.a = a;
		line.b = b;
		line.color = color;
		line.life = life;
		FigureBufferPointer = AddFigure(line, FigureBufferPointer);
	}

	void CHelperDraw::AddLineList(const Vector3f* pPos, uint nNumPoint, const Color& color, int pickupId)
	{
	}

	void CHelperDraw::AddCicle(const Vector3f& center, float radius, const Vector3f& dir, const Color& color, int pickupId)
	{
	}

	void CHelperDraw::Flush()
	{
		int32 vertexBase = 0;
		int32 indexBase = 0;
		{
			ResourceBufferMapper vbMapper(VertexBuffer_);
			ResourceBufferMapper ibMapper(IndexBuffer_);
			if (vbMapper.Ptr() == nullptr || ibMapper.Ptr() == nullptr) {
				FigureBufferPointer = 0;
				return;
			}


			int32 pos = 0;
			while (pos != FigureBufferPointer) {
				auto type = GetFigureType(pos);
				if (type == EHelperDrawType::Line) {
					FHelperLine line;
					pos = GetFigure(line, pos);

					line.AddToBuffer((HelperLineVertex*)vbMapper.Ptr(), (uint32*)ibMapper.Ptr(), vertexBase, indexBase);
				}
				else if (type == EHelperDrawType::LineList) {

				}
			}
		}

		FigureBufferPointer = 0;

		DrawLineVertexShader.BindShader();
		DrawLinePixelShader.BindShader();
		
		// RenderOperator op(VertexLayoutPositonColor_.mVertexLayout, VertexBuffer_, IndexBuffer_,
		// 	RenderOperator::PT_LINELIST, 0, 0, indexBase, VertexLayoutPositonColor_.Stride);
		//
		// GRenderDevice->ExecuteOperator(op);
	}

}