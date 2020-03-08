#pragma once




namespace Aurora
{

	class SceneView;

	class SimpleRendering
	{
	public:

		void			Initialize();
		void			RenderSceneView(SceneView* view);

	};


	extern SimpleRendering GSimpleRendering;

}

