#ifndef GARENDEREROBJECT_H
#define GARENDEREROBJECT_H
#include "Types.h"
#include "ResourceManager.h"

namespace Aurora
{

	class CGeometry;
	class IRenderDevice;
	class RenderOperator;
	

	class RenderableVisitor
	{
	public:
		RenderableVisitor() = default;
		
		virtual void Visit(RenderOperator& op) {}
	};



}

#endif