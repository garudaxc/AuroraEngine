#include "stdHeader.h"
#include "CommandSystem.h"
#include "FilePathStringHelper.h"
#include "Globals.h"
#include "GlobalVar.h"
namespace Aurora
{
	CommandInitializer* CommandInitializer::Link = NULL;
	CommandInitializer::CommandInitializer(const H3DString& name, CommandFunc func, const H3DString& desc):
		Name(name),
		Func(func),
		Desc(desc)
	{
		pNext = Link;
		Link = this;
	}

	//////////////////////////////////////////////////////////////////////////

	Arguments::Arguments(const H3DVector<H3DString>& args):m_args(args)
	{	
	}

	Arguments::~Arguments()
	{
	}

	const H3DString& Arguments::GetArg(int index) const
	{
		return m_args[index];
	}

	//////////////////////////////////////////////////////////////////////////

	struct CmdFunInfo
	{
		H3DString name;
		CommandFunc func;
		H3DString desc;
	};

	class CommandSystemImpl : public CommandSystem
	{
	public:
		virtual void Init();
		virtual void Destroy();

		virtual void AddCommand(const H3DString& name, CommandFunc func, const H3DString& desc);

		virtual void ExecuteCommandText(const char* text);

		virtual bool ExecuteFile(const H3DString& filename);

		void ListCommand() const;

	private:
		CmdFunInfo* FindCommand(const H3DString& cmdName);

		H3DVector<CmdFunInfo>	m_cmds;
	};

	CommandSystemImpl cmdSys;
	CommandSystem* GCommandSystem = &cmdSys;


	static void cmdListCommand(const Arguments& args)
	{
		cmdSys.ListCommand();
	}

	void CommandSystemImpl::Init()
	{
		CommandInitializer* pInit = CommandInitializer::Link;
		while (pInit != NULL)
		{
			AddCommand(pInit->Name, pInit->Func, pInit->Desc);
			pInit = pInit->pNext;
		}
		CommandInitializer::Link = NULL;


		// init static command
		AddCommand("ListCommand", cmdListCommand, "列出当前所有Command");
	}

	void CommandSystemImpl::Destroy()
	{
	}

	void CommandSystemImpl::AddCommand(const H3DString& name, CommandFunc func, const H3DString& desc)
	{
		CmdFunInfo info;
		info.name = name;
		info.func = func;
		info.desc = desc;
		
		m_cmds.push_back(info);
	}

	CmdFunInfo* CommandSystemImpl::FindCommand(const H3DString& cmdName)
	{
		for (H3DVector<CmdFunInfo>::iterator it = m_cmds.begin(); it != m_cmds.end(); ++it)
		{
			if (_stricmp(it->name.c_str(), cmdName.c_str()) == 0)
			{
				return &(*it);
			}
		}

		return NULL;
	}

	void CommandSystemImpl::ExecuteCommandText(const char* text)
	{
		H3DString strText = text;

		H3DVector<H3DString> commandLines;
		FilePathStringHelper::split_string(commandLines, strText, "\n");

		if (commandLines.empty() || commandLines[0].empty())
		{
			return;
		}

		for (size_t i = 0; i < commandLines.size(); i++)
		{			
			if (commandLines[i].size() >= 2 && commandLines[i][0] == '/' && commandLines[i][1] == '/')
			{
				// 跳过注释
				continue;
			}

			H3DVector<H3DString> tokens;
			FilePathStringHelper::split_string(tokens, commandLines[i], " ");

			if (tokens.empty())
			{
				continue;
			}

			CmdFunInfo* pCmdInfo = FindCommand(tokens[0]);
			if (pCmdInfo != NULL)
			{
				Arguments args(tokens);
				pCmdInfo->func(args);
				continue;
			}

			// if is a globlaVar
			if (!GGlobalVarManager->OnCommand(tokens[0]))
			{
				LogConsole("can not find command %s\n", tokens[0].c_str());
			}
		}
	}

	void CommandSystemImpl::ListCommand() const
	{
		for (H3DVector<CmdFunInfo>::const_iterator it = m_cmds.begin(); it != m_cmds.end(); ++it)
		{
			char buff[255];
			sprintf_s(buff, "%s\t%s\n", it->name.c_str(), it->desc.c_str());
			LogConsole(buff);
		}
	}

	bool CommandSystemImpl::ExecuteFile(const H3DString& filename)
	{
		FILE* pf = fopen(filename.c_str(), "r");
		if (pf == NULL)
		{
			LogConsole("CommandSystem can not find command file %s\n", filename.c_str());
			return false;
		}

		fseek(pf, 0, SEEK_END);
		size_t size = ftell(pf);
		fseek(pf, 0, SEEK_SET);

		char* buff = new char[size + 1];
		size_t readSize = fread(buff, 1, size, pf);
		buff[readSize] = '\0';

		ExecuteCommandText(buff);

		delete []buff;
		fclose(pf);

		return true;
	}
	

}