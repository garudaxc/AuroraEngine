#ifndef GAHELPERRENDERER_H
#define GAHELPERRENDERER_H

#include "Renderer.h"
#include "MaterialInstance.h"

namespace Aurora
{

	class OcclusionQuery;

class HelperRenderer
{
public:
	HelperRenderer();
	~HelperRenderer();

	bool Init();

	void AddLine(const Vector3f& a, const Vector3f& b, const Color& color);

	void AddAABB(const Vector3f& vMin, const Vector3f& vMax, const Color& color);

	//void DrawScreenQuad(MaterialInstance* pMtlInst);
private:

	class LineRenderer* m_pLineRenderer;
	class ScreenQuadRenderer* m_pQuadRenderer;
};

extern HelperRenderer GHelperRenderer;


//class TextRenderer
//{
//public:
//
//	TextRenderer(const string& strTexture, uint nRow, uint nCol);
//	~TextRenderer();
//
//	bool				Init();
//	void				DrawText(const char* pText, uint xPos, uint yPos, uint cw, uint ch, const Color& color);
//
//private:
//	static const int				MAX_TEXT = 10;
//	string							m_strTexture;
//	MaterialInstance*				m_pMatInst;
//
//	vector<VertexPT>				m_VertBuff;
//	vector<ushort>					m_Index;
//
//	uint							m_nCol;
//	uint							m_nRow;
//
//	RenderOperator					m_op;
//};


}


#endif