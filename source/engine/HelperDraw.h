#pragma once



namespace Aurora
{


	class CHelperDraw
	{
	public:
		CHelperDraw();
		~CHelperDraw();

		void	Init();


		void	AddLine(const Vector3f& a, const Vector3f& b, const Color& color, float life);
		void	AddLineList(const Vector3f* pPos, uint nNumPoint, const Color& color, int pickupId);
		void	AddCicle(const Vector3f& center, float radius, const Vector3f& dir, const Color& color, int pickupId);

		void	Flush();

	private:

		Handle		VertexBuffer_	= -1;
		Handle		IndexBuffer_	= -1;
	};


	extern CHelperDraw HelperDraw;

}