#pragma once

#include <string>
#include <vector>

namespace Aurora
{

	using namespace std;

	class Arguments
	{
	public:
		Arguments(const string& args);

		int NumArgs() const;
		
		const char* Args(int i) const;

	private:
		vector<string> m_args;
	};

	
	typedef void (*CommandFunction)(const Arguments& args);

	class CommandSystem
	{
	public:
		static void Initialize();
		static void Finalize();

		virtual void AddCommand(const string& name, CommandFunction func, const string& comment) = 0;

		virtual void ExecuteCommand(const char* command) = 0;
	};

	extern CommandSystem* GCommandSystem;


}