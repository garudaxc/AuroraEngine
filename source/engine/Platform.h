#pragma once
#include <string>


namespace Aurora
{

	enum cpuid_t {
		CPUID_NONE							= 0x00000,
		CPUID_UNSUPPORTED					= 0x00001,	// unsupported (386/486)
		CPUID_GENERIC						= 0x00002,	// unrecognized processor
		CPUID_INTEL							= 0x00004,	// Intel
		CPUID_AMD							= 0x00008,	// AMD
		CPUID_MMX							= 0x00010,	// Multi Media Extensions
		CPUID_3DNOW							= 0x00020,	// 3DNow!
		CPUID_SSE							= 0x00040,	// Streaming SIMD Extensions
		CPUID_SSE2							= 0x00080,	// Streaming SIMD Extensions 2
		CPUID_SSE3							= 0x00100,	// Streaming SIMD Extentions 3 aka Prescott's New Instructions
		CPUID_ALTIVEC						= 0x00200,	// AltiVec
		CPUID_HTT							= 0x01000,	// Hyper-Threading Technology
		CPUID_CMOV							= 0x02000,	// Conditional Move (CMOV) and fast floating point comparison (FCOMI) instructions
		CPUID_FTZ							= 0x04000,	// Flush-To-Zero mode (denormal results are flushed to zero)
		CPUID_DAZ							= 0x08000,	// Denormals-Are-Zero mode (denormal source operands are set to zero)
		CPUID_XENON							= 0x10000,	// Xbox 360
		CPUID_CELL							= 0x20000	// PS3
	};

	struct cpuInfo_t {
		int processorPackageCount;
		int processorCoreCount;
		int logicalProcessorCount;
		int numaNodeCount;
		struct cacheInfo_t {
			int count;
			int associativity;
			int lineSize;
			int size;
		} cacheLevel[3];
	};



	struct Win32Info
	{
		OSVERSIONINFOEX	osversion;
		cpuid_t			cpuid;
		cpuInfo_t		cpuInfo;
	};

	extern Win32Info win32Info;


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


}