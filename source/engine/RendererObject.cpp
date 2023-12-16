#include "stdHeader.h"
#include "RendererObject.h"
#include "CGeometry.h"
#include "Util.h"
#include "Console.h"
#include "Renderer.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Camera.h"
#include "Pipeline.h"


namespace Aurora
{
    VertexLayout* VertexLayout::PositionNormalTangentTexture = nullptr;    
    VertexLayout* VertexLayout::PositionTexture = nullptr;
    VertexLayout* VertexLayout::Position = nullptr;

}
