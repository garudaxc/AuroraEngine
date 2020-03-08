#ifndef _COMMAND_SYSTEM_
#define _COMMAND_SYSTEM_

#include "stdHeader.h"

namespace Aurora
{

	// 封装command的参数
	class Arguments
	{
	public:
		Arguments(const H3DVector<H3DString>& args);
		~Arguments();

		int GetNumArgs() const	{	return m_args.size();	}
		const H3DString& GetArg(int index) const;

	private:
		H3DVector<H3DString> m_args;		
	};

	typedef void (*CommandFunc)(const Arguments& args);



	// 用于各系统注册功能函数
	// 命令名大小写不敏感
	class CommandSystem
	{
	public:
		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual void AddCommand(const H3DString& name, CommandFunc func, const H3DString& desc) = 0;

		// 调用command，可以有多个command,用'\n'分隔
		virtual void ExecuteCommandText(const char* text) = 0;

		virtual bool ExecuteFile(const H3DString& filename) = 0;
	};

	extern CommandSystem* GCommandSystem;


	class CommandInitializer
	{
	public:
		CommandInitializer(const H3DString& name, CommandFunc func, const H3DString& desc);

		H3DString		Name;
		CommandFunc		Func;
		H3DString		Desc;

		CommandInitializer* pNext;

		static CommandInitializer* Link;
	};

}

#endif