#ifndef _DEBUG_CONSOLE_
#define _DEBUG_CONSOLE_


namespace H3D
{


	class DebugConsole
	{
	public:
		DebugConsole();
		~DebugConsole();

		void Init();
		void Destroy();

		void Show();

		// ����ı�����־����
		void AppendText(const char* text);

	private:


	};


	extern DebugConsole GDebugConsole;


}

#endif