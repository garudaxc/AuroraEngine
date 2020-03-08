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

		// 输出文本到日志窗口
		void AppendText(const char* text);

	private:


	};


	extern DebugConsole GDebugConsole;


}

#endif