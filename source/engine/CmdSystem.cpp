#include "stdheader.h"
#include "CmdSystem.h"

namespace Aurora
{


	class CommandInfo
	{
	public:
		CommandFunction	func;
		string name;
		string comment;
		CommandInfo* next;
	};


	Arguments::Arguments(const string& args)
	{
		size_t i = 0, j = 0;
		for (;;)
		{
			i = args.find_first_not_of(' ', j);
			if (i == string::npos)
				break;

			j = args.find_first_of(' ', i);
			if (j == string::npos)
				j = args.size();

			m_args.push_back(args.substr(i, j - i));
		}		
	}

	int Arguments::NumArgs() const
	{
		return m_args.size();
	}

	const char* Arguments::Args(int i) const
	{
		return m_args[i].c_str();
	}

	//////////////////////////////////////////////////////////////////////////



	void CommandSystem::Initialize()
	{
	}

	void CommandSystem::Finalize()
	{
	}


	class CommandSystemLocal : public CommandSystem
	{
	public:
		CommandSystemLocal();
		~CommandSystemLocal();

		virtual void AddCommand(const string& name, CommandFunction func, const string& comment);

		virtual void ExecuteCommand(const char* command);

		static void ListCommand(const Arguments& args);

	private:
		CommandInfo* m_pCmds;

	};


	static CommandSystemLocal cmdSystemLocal;
	CommandSystem* GCommandSystem = &cmdSystemLocal;


	void CommandSystemLocal::ListCommand(const Arguments& args)
	{

	}


	CommandSystemLocal::CommandSystemLocal()
	{
		m_pCmds = NULL;
	}

	CommandSystemLocal::~CommandSystemLocal()
	{
	}

	void CommandSystemLocal::AddCommand(const string& name, CommandFunction func, const string& comment)
	{
		CommandInfo* pCmd = new CommandInfo();
		pCmd->name = name;
		pCmd->func = func;
		pCmd->comment = comment;
		pCmd->next = m_pCmds;
		m_pCmds = pCmd;
	}

	void CommandSystemLocal::ExecuteCommand(const char* command)
	{
		Arguments args(command);

		if (args.NumArgs() == 0)
			return;

		for (CommandInfo* c = m_pCmds; c != NULL; c = c->next)
		{
			if (c->name == args.Args(0))
			{
				c->func(args);
				break;
			}
		}
		
		// not found
	}


}