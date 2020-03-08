#ifndef GARENDEREROBJECT_H
#define GARENDEREROBJECT_H
#include "Types.h"
#include "ResourceManager.h"

namespace Aurora
{

	class Geometry;
	class IRenderDevice;
	class RenderOperator;
	class VertexLayout;
	

	class RenderableVisitor
	{
	public:
		virtual void Visit(RenderOperator& op) {}
	};


	struct VertexLayoutItem
	{
		uint8	type;
		uint8	usage;
		uint8	usageIndex;
	};

	class VertexLayout
	{
	public:
		struct Stream
		{
			vector<VertexLayoutItem>	items;
			uint			nVertexStride;
		};

		Stream			stream;

		void* HALHandle = nullptr;

	};

	enum EVertexLayout : int8
	{
		VertexLayout_PNTT = 0,
		VertexLayout_PT,
		VertexLayout_P,
		VertexLayout_PNTTBB,
		VertexLayout_NUM,
	};



}

#endif