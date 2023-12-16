#ifndef GARENDEREROBJECT_H
#define GARENDEREROBJECT_H
#include "Types.h"
#include "ResourceManager.h"

namespace Aurora
{

	class CGeometry;
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

		static VertexLayout* PositionNormalTangentTexture;
		static VertexLayout* PositionTexture;
		static VertexLayout* Position;
		
		
		struct Stream
		{
			vector<VertexLayoutItem>	items;
			uint			nVertexStride;
		};

		Stream			stream;

		void* HALHandle = nullptr;
	};


}

#endif