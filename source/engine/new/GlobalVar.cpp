#include "H3DSTLHeader.h"
#include "GlobalVar.h"
#include "CommandSystem.h"
#include "IH3DLogMng.h"
#include "Globals.h"

namespace H3D
{
	GlobalVar* GlobalVar::StaticInit = NULL;
	GlobalVar::GlobalVar(const H3DString& name, const H3DString& value, int flag, const H3DString& desc)
	{
		m_Name = name;
		m_DefaultValue = value;
		m_flag = flag;
		m_Desc = desc;

		m_pInternal = NULL;
		next = NULL;

		if (StaticInit != (GlobalVar*)0xffffffff)
		{
			next = StaticInit;
			StaticInit = this;
		}
		else
		{
			GGlobalVarManager->Register(this);
		}
	}

	//////////////////////////////////////////////////////////////////////////


	class GlobalVarInternal : public GlobalVar
	{
	public:
		GlobalVarInternal(GlobalVar* gvar);

		virtual void ResetValue();

		virtual void SetBool(bool value);
		virtual void SetInt(int value);
		virtual void SetFloat(float value);
		virtual void SetValue(const H3DString& value);

		virtual void SetCallBack(GlobalVarCallback pCallback);

		// only for GlobalVarManager use
		const H3DString& GetNameInternal() const	{	return m_Name;	}
		const H3DString& GetDescInternal() const	{	return m_Desc;	}
		int GetFlagInternal() const		{	return m_flag;	}
		int GetIntInternal() const		{	return nValue;	}
		float GetFloatInternal() const	{	return fValue;	}

		void SetValueInternal(const H3DString& value);
		//add ��ȡvalue
		H3DString GetValueInternal();

		void PrintToConsole(bool bDesc) const;
	};

	GlobalVarInternal::GlobalVarInternal(GlobalVar* gvar)
	{
		m_Name = gvar->GetName();
		m_DefaultValue = gvar->GetDefaultValue();
		m_flag = gvar->GetFlag();
		m_Desc = gvar->GetDesc();

		next		= NULL;
		m_pInternal	= NULL;
		m_pCallback	= NULL;

		ResetValue();
	}

	void GlobalVarInternal::ResetValue()
	{
		SetValueInternal(m_DefaultValue);
	}

	H3DString GlobalVarInternal::GetValueInternal()
	{
		char buf[32];
		memset(buf,0,32);
		if (m_flag & GVFLAG_BOOL)
		{
			if (nValue == 1)
				sprintf_s(buf,"%s","true");
			else
				sprintf_s(buf,"%s","true1");
		}
		else if (m_flag & GVFLAG_INT)
		{
			sprintf_s(buf,"%d",nValue);
		}
		else if (m_flag & GVFLAG_FLOAT)
		{
			sprintf_s(buf,"%f",fValue);
		}
		return H3DString(buf);
	}
	void GlobalVarInternal::SetValueInternal(const H3DString& value)
	{
		if (value.empty())
		{
			LogConsole("GlobalVar %s SetValue error : value=%s\n", m_Name.c_str(), value.c_str());
			return;
		}

		if (m_flag & GVFLAG_BOOL)
		{
			if (value[0] == '1' || value == "true")
			{
				nValue = 1;
			}
			else
			{
				nValue = 0;
			}
		}
		else if (m_flag & GVFLAG_INT)
		{
			nValue = atoi(value.c_str());
		}
		else if (m_flag & GVFLAG_FLOAT)
		{
			fValue = (float)atof(value.c_str());
		}

		if (m_pCallback)
		{
			m_pCallback();
		}
	}

	void GlobalVarInternal::SetBool(bool value)
	{
		if (value)
		{
			nValue = 1;
		}
		else
		{
			nValue = 0;
		}

		if (m_pCallback)
		{
			m_pCallback();
		}
	}

	void GlobalVarInternal::SetInt(int value)
	{
		nValue = value;

		if (m_pCallback)
		{
			m_pCallback();
		}
	}

	void GlobalVarInternal::SetFloat(float value)
	{
		fValue = value;

		if (m_pCallback)
		{
			m_pCallback();
		}
	}

	void GlobalVarInternal::SetValue(const H3DString& value)
	{
		SetValueInternal(value);
	}

	void GlobalVarInternal::SetCallBack(GlobalVarCallback pCallback)
	{
		m_pCallback = pCallback;
	}

	void GlobalVarInternal::PrintToConsole(bool bDesc) const
	{
		char buff[256];
		int flag = GetFlagInternal();
		if ((flag & GVFLAG_BOOL) || (flag & GVFLAG_INT))
		{
			if (bDesc)
			{
				sprintf(buff, "%s = %d\t%s\n", GetNameInternal().c_str(), 
					GetIntInternal(), GetDescInternal().c_str());
			}
			else
			{
				sprintf(buff, "%s = %d\n", GetNameInternal().c_str(), GetIntInternal());
			}
		}
		else if (flag & GVFLAG_FLOAT)
		{
			if (bDesc)
			{
				sprintf(buff, "%s = %f\t%s\n", GetNameInternal().c_str(), 
					GetFloatInternal(), GetDescInternal().c_str());
			}
			else
			{
				sprintf(buff, "%s = %f\n", GetNameInternal().c_str(), GetFloatInternal());
			}
		}

		LogConsole(buff);
	}

	//////////////////////////////////////////////////////////////////////////


	class GlobalVarManagerImpl : public GlobalVarManager
	{
	public:
		virtual void Init();
		virtual void Destroy();

		virtual void Register(GlobalVar* gvar);

		virtual void SetGlobalVar(const H3DString& name, const H3DString& value);
		//add ��ȡĳ��GV��value
		virtual H3DString GetGloabalVar(const H3DString& name);

		virtual bool OnCommand(const H3DString& cmd);

		void ListGlobalVar(const Arguments& args);

	private:
		GlobalVarInternal* Find(const H3DString& name);

		H3DVector<GlobalVarInternal*> m_InternalVars;
	};

	GlobalVarManagerImpl GVManagerImpl;
	GlobalVarManager* GGlobalVarManager = &GVManagerImpl;



	void GlobalVarManagerImpl::Destroy()
	{
		for (H3DVector<GlobalVarInternal*>::iterator it = m_InternalVars.begin(); it != m_InternalVars.end(); ++it)
		{
			delete (*it);
		}
		m_InternalVars.clear();
	}

	void GlobalVarManagerImpl::Register(GlobalVar* gvar)
	{
		gvar->SetInternal(gvar);
		GlobalVarInternal* internal = Find(gvar->GetName());
		if (internal == NULL)
		{
			internal = new GlobalVarInternal(gvar);
			m_InternalVars.push_back(internal);
		}
		gvar->SetInternal(internal);
	}

	GlobalVarInternal* GlobalVarManagerImpl::Find(const H3DString& name)
	{
		for (H3DVector<GlobalVarInternal*>::iterator it = m_InternalVars.begin(); it != m_InternalVars.end(); ++it)
		{		
			//if ((*it)->GetNameInternal() == name)
			// ��Сд������
			if (_stricmp((*it)->GetNameInternal().c_str(), name.c_str()) == 0)	
			{
				return (*it);
			}
		}
		
		return NULL;
	}

	H3DString GlobalVarManagerImpl::GetGloabalVar(const H3DString& name)
	{
		GlobalVarInternal* gvar=Find(name);
		if (gvar==NULL)
		{
			return "";
		}
		return gvar->GetValueInternal();
	}

	void GlobalVarManagerImpl::SetGlobalVar(const H3DString& name, const H3DString& value)
	{
		GlobalVarInternal* gvar = Find(name);
		if (gvar == NULL)
		{
			LogConsole("can not find GlobalVar %s\n", name.c_str());
			return;
		}

		if (gvar->GetFlagInternal() & GVFLAG_READONLY)
		{
			LogConsole("%s can not be set directly, use corresponding command instead\n", name.c_str());
			return;
		}

		gvar->SetValueInternal(value);
		gvar->PrintToConsole(false);
	}

	bool GlobalVarManagerImpl::OnCommand(const H3DString& cmd)
	{
		GlobalVarInternal* gvar = Find(cmd);
		if (gvar == NULL)
		{
			return false;
		}

		gvar->PrintToConsole(true);
		return true;
	}


	static void cmdListGlobalVar(const Arguments& args)
	{
		GVManagerImpl.ListGlobalVar(args);
	}

	static void cmdSetGlobalVarValue(const Arguments& args)
	{
		if (args.GetNumArgs() < 3)
		{
			LogConsole("format error : set GlobalVarName value\n");
			return;
		}

		GVManagerImpl.SetGlobalVar(args.GetArg(1), args.GetArg(2));
	}

	void GlobalVarManagerImpl::ListGlobalVar(const Arguments& args)
	{
		if (args.GetNumArgs() > 1 && args.GetArg(1) == "/?")
		{
			const char* helpText = "�������壺\n"
				"/bֻ���bool����GlobalVar\n"
				"/iֻ���������GlobalVar\n"
				"/fֻ���������GlobalVar\n"
				"/pֻ�������profile��GlobalVar\n"
				"/nd�����GlobalVar��ע��\n";
			LogConsole(helpText);
			return;
		}


		int flag = 0;
		bool bShowDesc = true;
		for (int i = 1; i < args.GetNumArgs(); i++)
		{
			if (args.GetArg(i) == "/b")
			{
				flag |= GVFLAG_BOOL;
			}
			else if (args.GetArg(i) == "/i")
			{
				flag |= GVFLAG_INT;
			}
			else if (args.GetArg(i) == "/f")
			{
				flag |= GVFLAG_FLOAT;
			}
			else if (args.GetArg(i) == "/p")
			{
				flag |= GVFLAG_PROFILE;
			}
			else if (args.GetArg(i) == "/nd")
			{
				bShowDesc = false;
			}
		}

		if (flag == 0)
		{
			flag = 0xFFFFFFFF;
		}

		int count = 0;
		for (H3DVector<GlobalVarInternal*>::iterator it = m_InternalVars.begin(); it != m_InternalVars.end(); ++it)
		{
			if ((*it)->GetFlagInternal() & flag)
			{
				(*it)->PrintToConsole(bShowDesc);
				count++;
			}
		}

		LogConsole("List %d GlobalVars\n", count);
	}

	bool SortGlobalVar(GlobalVarInternal* p0, GlobalVarInternal* p1)
	{
		int result = _stricmp(p0->GetNameInternal().c_str(), p1->GetNameInternal().c_str());
		return result < 0;
	}

	void GlobalVarManagerImpl::Init()
	{
		// init static GlobalVar
		for (GlobalVar* p = GlobalVar::StaticInit; p != NULL; p = p->next)
		{
			Register(p);
		}
		GlobalVar::StaticInit = (GlobalVar*)0xffffffff;

		std::sort(m_InternalVars.begin(), m_InternalVars.end(), SortGlobalVar);

		GCommandSystem->AddCommand("ListGlobalVar", cmdListGlobalVar, "�г����е�GlobalVar (����/? /b /f /i /p /nd)");
		GCommandSystem->AddCommand("set", cmdSetGlobalVarValue, "��һ��GlobalVar��ֵ����ʽ��set GlobalVar�� ֵ");
	}
}