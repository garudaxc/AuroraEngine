#pragma once

#include "stdheader.h"

namespace Aurora
{


	void InitPlatform();

	bool IsFileExist(const std::string& pathName);


	class ILock
	{
	public:
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void Release() = 0;
	};

	template<class LockType>
	class ScopedLock 
	{
	public:

		ScopedLock(LockType* pLock):m_pLock(pLock)
		{
			m_pLock->Lock();
		}

		~ScopedLock()
		{
			m_pLock->Unlock();
		}

	private:
		LockType* m_pLock;
	};

	typedef ScopedLock<ILock> LockObject;


	class ITask
	{
	public:

		virtual void Run(void* param) = 0;
	};

	enum EThreadState
	{
		TS_RUNNING,
		TS_APPENDDING,
		TS_FINISHED,
	};

	// 
	class IThread
	{
	public:
		virtual EThreadState GetState() const = 0;

		virtual void AddTask(ITask* task, void* param) = 0;

		virtual uint GetNumTask() const = 0;

		virtual void FinishAllTasks(int timeout) = 0;

		virtual void Release() = 0;
	};

	ILock* CreateThreadLock();

	IThread* CreateThread(ITask* task, void* param, bool bAutoFinish);



	
	class CScreen
	{
	public:
		RectSize GetSize() const
		{
			return  mScreenSize;
		}
		
		RectSize mScreenSize;
	};

	class CPlatform
	{
	public:
		enum PlatformType
		{
			WINDOWS,
			ANDROID,
		};

		static CScreen*	MainScreen;

		static std::string		GetWorkingPath();		
	};


}