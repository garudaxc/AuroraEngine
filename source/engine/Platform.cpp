#include "stdheader.h"
#include "Platform.h"
#include <Windows.h>
#include <io.h>

namespace Aurora
{


static bool HasCPUID() {
	__asm 
	{
		pushfd						// save eflags
		pop		eax
		test	eax, 0x00200000		// check ID bit
		jz		set21				// bit 21 is not set, so jump to set_21
		and		eax, 0xffdfffff		// clear bit 21
		push	eax					// save new value in register
		popfd						// store new value in flags
		pushfd
		pop		eax
		test	eax, 0x00200000		// check ID bit
		jz		good
		jmp		err					// cpuid not supported
set21:
		or		eax, 0x00200000		// set ID bit
		push	eax					// store new value
		popfd						// store new value in EFLAGS
		pushfd
		pop		eax
		test	eax, 0x00200000		// if bit 21 is on
		jnz		good
		jmp		err
	}

err:
	return false;
good:
	return true;
}

#define _REG_EAX		0
#define _REG_EBX		1
#define _REG_ECX		2
#define _REG_EDX		3

/*
================
CPUID
================
*/
static void CPUID( int func, unsigned regs[4] ) {
	unsigned regEAX, regEBX, regECX, regEDX;

	__asm pusha
	__asm mov eax, func
	__asm __emit 00fh
	__asm __emit 0a2h
	__asm mov regEAX, eax
	__asm mov regEBX, ebx
	__asm mov regECX, ecx
	__asm mov regEDX, edx
	__asm popa

	regs[_REG_EAX] = regEAX;
	regs[_REG_EBX] = regEBX;
	regs[_REG_ECX] = regECX;
	regs[_REG_EDX] = regEDX;
}


/*
================
IsAMD
================
*/
static bool IsAMD() {
	char pstring[16];
	char processorString[13];

	// get name of processor
	CPUID( 0, ( unsigned int * ) pstring );
	processorString[0] = pstring[4];
	processorString[1] = pstring[5];
	processorString[2] = pstring[6];
	processorString[3] = pstring[7];
	processorString[4] = pstring[12];
	processorString[5] = pstring[13];
	processorString[6] = pstring[14];
	processorString[7] = pstring[15];
	processorString[8] = pstring[8];
	processorString[9] = pstring[9];
	processorString[10] = pstring[10];
	processorString[11] = pstring[11];
	processorString[12] = 0;

	if ( strcmp( processorString, "AuthenticAMD" ) == 0 ) {
		return true;
	}
	return false;
}

/*
================
HasCMOV
================
*/
static bool HasCMOV() {
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 15 of EDX denotes CMOV existence
	if ( regs[_REG_EDX] & ( 1 << 15 ) ) {
		return true;
	}
	return false;
}

/*
================
Has3DNow
================
*/
static bool Has3DNow() {
	unsigned regs[4];

	// check AMD-specific functions
	CPUID( 0x80000000, regs );
	if ( regs[_REG_EAX] < 0x80000000 ) {
		return false;
	}

	// bit 31 of EDX denotes 3DNow! support
	CPUID( 0x80000001, regs );
	if ( regs[_REG_EDX] & ( 1 << 31 ) ) {
		return true;
	}

	return false;
}

/*
================
HasMMX
================
*/
static bool HasMMX() {
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 23 of EDX denotes MMX existence
	if ( regs[_REG_EDX] & ( 1 << 23 ) ) {
		return true;
	}
	return false;
}

/*
================
HasSSE
================
*/
static bool HasSSE() {
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 25 of EDX denotes SSE existence
	if ( regs[_REG_EDX] & ( 1 << 25 ) ) {
		return true;
	}
	return false;
}

/*
================
HasSSE2
================
*/
static bool HasSSE2() {
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 26 of EDX denotes SSE2 existence
	if ( regs[_REG_EDX] & ( 1 << 26 ) ) {
		return true;
	}
	return false;
}

/*
================
HasSSE3
================
*/
static bool HasSSE3() {
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 0 of ECX denotes SSE3 existence
	if ( regs[_REG_ECX] & ( 1 << 0 ) ) {
		return true;
	}
	return false;
}

/*
================
LogicalProcPerPhysicalProc
================
*/
#define NUM_LOGICAL_BITS   0x00FF0000     // EBX[23:16] Bit 16-23 in ebx contains the number of logical
                                          // processors per physical processor when execute cpuid with 
                                          // eax set to 1
static unsigned char LogicalProcPerPhysicalProc() {
	unsigned int regebx = 0;
	__asm {
		mov eax, 1
		cpuid
		mov regebx, ebx
	}
	return (unsigned char) ((regebx & NUM_LOGICAL_BITS) >> 16);
}

/*
================
GetAPIC_ID
================
*/
#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
                                          // initial APIC ID for the processor this code is running on.
                                          // Default value = 0xff if HT is not supported
static unsigned char GetAPIC_ID() {
	unsigned int regebx = 0;
	__asm {
		mov eax, 1
		cpuid
		mov regebx, ebx
	}
	return (unsigned char) ((regebx & INITIAL_APIC_ID_BITS) >> 24);
}

/*
================
CPUCount

	logicalNum is the number of logical CPU per physical CPU
    physicalNum is the total number of physical processor
	returns one of the HT_* flags
================
*/
#define HT_NOT_CAPABLE				0
#define HT_ENABLED					1
#define HT_DISABLED					2
#define HT_SUPPORTED_NOT_ENABLED	3
#define HT_CANNOT_DETECT			4

int CPUCount( int &logicalNum, int &physicalNum ) {
	int statusFlag;
	SYSTEM_INFO info;

	physicalNum = 1;
	logicalNum = 1;
	statusFlag = HT_NOT_CAPABLE;

	info.dwNumberOfProcessors = 0;
	GetSystemInfo (&info);

	// Number of physical processors in a non-Intel system
	// or in a 32-bit Intel system with Hyper-Threading technology disabled
	physicalNum = info.dwNumberOfProcessors;  

	unsigned char HT_Enabled = 0;

	logicalNum = LogicalProcPerPhysicalProc();

	if ( logicalNum >= 1 ) {	// > 1 doesn't mean HT is enabled in the BIOS
		HANDLE hCurrentProcessHandle;
		DWORD  dwProcessAffinity;
		DWORD  dwSystemAffinity;
		DWORD  dwAffinityMask;

		// Calculate the appropriate  shifts and mask based on the 
		// number of logical processors.

		unsigned char i = 1, PHY_ID_MASK  = 0xFF, PHY_ID_SHIFT = 0;

		while( i < logicalNum ) {
			i *= 2;
 			PHY_ID_MASK  <<= 1;
			PHY_ID_SHIFT++;
		}
		
		hCurrentProcessHandle = GetCurrentProcess();
		GetProcessAffinityMask( hCurrentProcessHandle, &dwProcessAffinity, &dwSystemAffinity );

		// Check if available process affinity mask is equal to the
		// available system affinity mask
		if ( dwProcessAffinity != dwSystemAffinity ) {
			statusFlag = HT_CANNOT_DETECT;
			physicalNum = -1;
			return statusFlag;
		}

		dwAffinityMask = 1;
		while ( dwAffinityMask != 0 && dwAffinityMask <= dwProcessAffinity ) {
			// Check if this CPU is available
			if ( dwAffinityMask & dwProcessAffinity ) {
				if ( SetProcessAffinityMask( hCurrentProcessHandle, dwAffinityMask ) ) {
					unsigned char APIC_ID, LOG_ID, PHY_ID;

					Sleep( 0 ); // Give OS time to switch CPU

					APIC_ID = GetAPIC_ID();
					LOG_ID  = APIC_ID & ~PHY_ID_MASK;
					PHY_ID  = APIC_ID >> PHY_ID_SHIFT;

					if ( LOG_ID != 0 ) {
						HT_Enabled = 1;
					}
				}
			}
			dwAffinityMask = dwAffinityMask << 1;
		}
	        
		// Reset the processor affinity
		SetProcessAffinityMask( hCurrentProcessHandle, dwProcessAffinity );
	    
		if ( logicalNum == 1 ) {  // Normal P4 : HT is disabled in hardware
			statusFlag = HT_DISABLED;
		} else {
			if ( HT_Enabled ) {
				// Total physical processors in a Hyper-Threading enabled system.
				physicalNum /= logicalNum;
				statusFlag = HT_ENABLED;
			} else {
				statusFlag = HT_SUPPORTED_NOT_ENABLED;
			}
		}
	}
	return statusFlag;
}

/*
================
HasHTT
================
*/
static bool HasHTT() {
	unsigned regs[4];
	int logicalNum, physicalNum, HTStatusFlag;

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 28 of EDX denotes HTT existence
	if ( !( regs[_REG_EDX] & ( 1 << 28 ) ) ) {
		return false;
	}

	HTStatusFlag = CPUCount( logicalNum, physicalNum );
	if ( HTStatusFlag != HT_ENABLED ) {
		return false;
	}
	return true;
}

/*
================
HasHTT
================
*/
static bool HasDAZ() {
	__declspec(align(16)) unsigned char FXSaveArea[512];
	unsigned char *FXArea = FXSaveArea;
	DWORD dwMask = 0;
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 24 of EDX denotes support for FXSAVE
	if ( !( regs[_REG_EDX] & ( 1 << 24 ) ) ) {
		return false;
	}

	memset( FXArea, 0, sizeof( FXSaveArea ) );

	__asm {
		mov		eax, FXArea
		FXSAVE	[eax]
	}

	dwMask = *(DWORD *)&FXArea[28];						// Read the MXCSR Mask
	return ( ( dwMask & ( 1 << 6 ) ) == ( 1 << 6 ) );	// Return if the DAZ bit is set
}

/*
================================================================================================

	CPU

================================================================================================
*/

/*
========================
CountSetBits 
Helper function to count set bits in the processor mask.
========================
*/
DWORD CountSetBits( ULONG_PTR bitMask ) {
	DWORD LSHIFT = sizeof( ULONG_PTR ) * 8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    

	for ( DWORD i = 0; i <= LSHIFT; i++ ) {
		bitSetCount += ( ( bitMask & bitTest ) ? 1 : 0 );
		bitTest /= 2;
	}

	return bitSetCount;
}

typedef BOOL (WINAPI *LPFN_GLPI)( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD );

enum LOGICAL_PROCESSOR_RELATIONSHIP_LOCAL {
    localRelationProcessorCore,
    localRelationNumaNode,
    localRelationCache,
	localRelationProcessorPackage
};


/*
========================
GetCPUInfo
========================
*/
bool GetCPUInfo( cpuInfo_t & cpuInfo ) {
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	PCACHE_DESCRIPTOR Cache;
	LPFN_GLPI	glpi;
	BOOL		done = FALSE;
	DWORD		returnLength = 0;
	DWORD		byteOffset = 0;

	memset( & cpuInfo, 0, sizeof( cpuInfo ) );

	glpi = (LPFN_GLPI)GetProcAddress( GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation" );
	if ( NULL == glpi ) {
		//idLib::Printf( "\nGetLogicalProcessorInformation is not supported.\n" );
		return 0;
	}

	while ( !done ) {
		DWORD rc = glpi( buffer, &returnLength );

		if ( FALSE == rc ) {
			if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER ) {
				if ( buffer ) {
					free( buffer );
				}

				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc( returnLength );
			} else {
				//idLib::Printf( "Sys_CPUCount error: %d\n", GetLastError() );
				return false;
			}
		} else {
			done = TRUE;
		}
	}

	ptr = buffer;

	while ( byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength ) {
		switch ( (LOGICAL_PROCESSOR_RELATIONSHIP_LOCAL) ptr->Relationship ) {
			case localRelationProcessorCore:
				cpuInfo.processorCoreCount++;

				// A hyperthreaded core supplies more than one logical processor.
				cpuInfo.logicalProcessorCount += CountSetBits( ptr->ProcessorMask );
				break;

			case localRelationNumaNode:
				// Non-NUMA systems report a single record of this type.
				cpuInfo.numaNodeCount++;
				break;

			case localRelationCache:
				// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
				Cache = &ptr->Cache;
				if ( Cache->Level >= 1 && Cache->Level <= 3 ) {
					int level = Cache->Level - 1;
					if ( cpuInfo.cacheLevel[level].count > 0 ) {
						cpuInfo.cacheLevel[level].count++;
					} else {
						cpuInfo.cacheLevel[level].associativity = Cache->Associativity;
						cpuInfo.cacheLevel[level].lineSize = Cache->LineSize;
						cpuInfo.cacheLevel[level].size = Cache->Size;
					}
				}
				break;

			case localRelationProcessorPackage:
				// Logical processors share a physical package.
				cpuInfo.processorPackageCount++;
				break;

			default:
				//idLib::Printf( "Error: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n" );
				break;
		}
		byteOffset += sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
		ptr++;
	}

	free( buffer );

	return true;
}

/*
========================
Sys_GetCPUCacheSize
========================
*/
void Sys_GetCPUCacheSize( int level, int & count, int & size, int & lineSize ) {
	assert( level >= 1 && level <= 3 );
	cpuInfo_t cpuInfo;

	GetCPUInfo( cpuInfo );

	count = cpuInfo.cacheLevel[level - 1].count;
	size = cpuInfo.cacheLevel[level - 1].size;
	lineSize = cpuInfo.cacheLevel[level - 1].lineSize;
}

/*
========================
Sys_CPUCount

numLogicalCPUCores	- the number of logical CPU per core
numPhysicalCPUCores	- the total number of cores per package
numCPUPackages		- the total number of packages (physical processors)
========================
*/
void Sys_CPUCount( int & numLogicalCPUCores, int & numPhysicalCPUCores, int & numCPUPackages ) {
	cpuInfo_t cpuInfo;
	GetCPUInfo( cpuInfo );

	numPhysicalCPUCores = cpuInfo.processorCoreCount;
	numLogicalCPUCores = cpuInfo.logicalProcessorCount;
	numCPUPackages = cpuInfo.processorPackageCount;
}

/*
================
Sys_GetCPUId
================
*/
cpuid_t GetCPUId() {
	int flags;

	// verify we're at least a Pentium or 486 with CPUID support
	if ( !HasCPUID() ) {
		return CPUID_UNSUPPORTED;
	}

	// check for an AMD
	if ( IsAMD() ) {
		flags = CPUID_AMD;
	} else {
		flags = CPUID_INTEL;
	}

	// check for Multi Media Extensions
	if ( HasMMX() ) {
		flags |= CPUID_MMX;
	}

	// check for 3DNow!
	if ( Has3DNow() ) {
		flags |= CPUID_3DNOW;
	}

	// check for Streaming SIMD Extensions
	if ( HasSSE() ) {
		flags |= CPUID_SSE | CPUID_FTZ;
	}

	// check for Streaming SIMD Extensions 2
	if ( HasSSE2() ) {
		flags |= CPUID_SSE2;
	}

	// check for Streaming SIMD Extensions 3 aka Prescott's New Instructions
	if ( HasSSE3() ) {
		flags |= CPUID_SSE3;
	}

	// check for Hyper-Threading Technology
	if ( HasHTT() ) {
		flags |= CPUID_HTT;
	}

	// check for Conditional Move (CMOV) and fast floating point comparison (FCOMI) instructions
	if ( HasCMOV() ) {
		flags |= CPUID_CMOV;
	}

	// check for Denormals-Are-Zero mode
	if ( HasDAZ() ) {
		flags |= CPUID_DAZ;
	}

	return (cpuid_t)flags;
}





void InitPlatform()
{
	Win32Info win32Info;

	CoInitialize( NULL );

	//
	// Windows version
	//
	win32Info.osversion.dwOSVersionInfoSize = sizeof( win32Info.osversion );

	if ( !GetVersionEx( (LPOSVERSIONINFO)&win32Info.osversion ) )
		GLog->Error( "Couldn't get OS info" );

	string osVersion;
	if( win32Info.osversion.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
		if( win32Info.osversion.dwMajorVersion <= 4 ) {
			osVersion = "WinNT (NT)";
		} else if( win32Info.osversion.dwMajorVersion == 5 && win32Info.osversion.dwMinorVersion == 0 ) {
			osVersion = "Win2K (NT)";
		} else if( win32Info.osversion.dwMajorVersion == 5 && win32Info.osversion.dwMinorVersion == 1 ) {
			osVersion = "WinXP (NT)";
		} else if ( win32Info.osversion.dwMajorVersion == 6  && win32Info.osversion.dwMinorVersion == 0 ) {
			osVersion = "Windows Vista";
		} else if ( win32Info.osversion.dwMajorVersion == 6  && win32Info.osversion.dwMinorVersion == 1 ) {
			osVersion = "Windows 7";
		} else if ( win32Info.osversion.dwMajorVersion == 6  && win32Info.osversion.dwMinorVersion == 2 ) {
			osVersion = "Windows 8";
		} else {
			osVersion = "Unknown NT variant";
		}
	} else if( win32Info.osversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) {
		if( win32Info.osversion.dwMajorVersion == 4 && win32Info.osversion.dwMinorVersion == 0 ) {
			// Win95
			if( win32Info.osversion.szCSDVersion[1] == 'C' ) {
				osVersion = "Win95 OSR2 (95)";
			} else {
				osVersion = "Win95 (95)";
			}
		} else if( win32Info.osversion.dwMajorVersion == 4 && win32Info.osversion.dwMinorVersion == 10 ) {
			// Win98
			if( win32Info.osversion.szCSDVersion[1] == 'A' ) {
				osVersion = "Win98SE (95)";
			} else {
				osVersion = "Win98 (95)";
			}
		} else if( win32Info.osversion.dwMajorVersion == 4 && win32Info.osversion.dwMinorVersion == 90 ) {
			// WinMe
			osVersion = "WinMe (95)";
		} else {
			osVersion = "Unknown 95 variant";
		}
	} else {
		osVersion = "unknown Windows variant";
	}

	GLog->Info(osVersion.c_str());

	win32Info.cpuid = GetCPUId();
	GetCPUInfo(win32Info.cpuInfo);

	string cpuInfo;

	if ( win32Info.cpuid & CPUID_AMD ) {
		cpuInfo += "AMD CPU";
	} else if ( win32Info.cpuid & CPUID_INTEL ) {
		cpuInfo += "Intel CPU";
	} else if ( win32Info.cpuid & CPUID_UNSUPPORTED ) {
		cpuInfo += "unsupported CPU";
	} else {
		cpuInfo += "generic CPU";
	}

	cpuInfo += " with ";
	if ( win32Info.cpuid & CPUID_MMX ) {
		cpuInfo += "MMX ";
	}
	if ( win32Info.cpuid & CPUID_3DNOW ) {
		cpuInfo += "3DNow! ";
	}
	if ( win32Info.cpuid & CPUID_SSE ) {
		cpuInfo += "SSE ";
	}
	if ( win32Info.cpuid & CPUID_SSE2 ) {
		cpuInfo += "SSE2 ";
	}
	if ( win32Info.cpuid & CPUID_SSE3 ) {
		cpuInfo += "SSE3 ";
	}
	if ( win32Info.cpuid & CPUID_HTT ) {
		cpuInfo += "HTT";
	}

	GLog->Info(cpuInfo.c_str());

	GLog->Info("Processor Core : %d, Logic Processor : %d", 
		win32Info.cpuInfo.processorCoreCount,
		win32Info.cpuInfo.logicalProcessorCount);


	//common->Printf( "%s\n", win32Info.sys_cpustring.GetString() );
	//common->Printf( "%d MB System Memory\n", Sys_GetSystemRam() );
	//common->Printf( "%d MB Video Memory\n", Sys_GetVideoRam() );
	//if ( ( win32Info.cpuid & CPUID_SSE2 ) == 0 ) {
	//	common->Error( "SSE2 not supported!" );
	//}

}

















	//bool IsFileExist(const string& pathName)
	//{
	//	HANDLE hFile = CreateFileA(pathName.c_str(), 0, 0, NULL, 0, OPEN_EXISTING, NULL);

	//	return true;
	//}

	bool IsFileExist(const string& pathName)
	{
		if (_access(pathName.c_str(), 0) == -1)
		{
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	class SectionLock 
	{
	public:	

		SectionLock()
		{
			InitializeCriticalSection(&m_cs);
		}

		~SectionLock()
		{
			DeleteCriticalSection(&m_cs);
		}

		void Lock()
		{
			EnterCriticalSection(&m_cs);
		}

		void Unlock()
		{
			LeaveCriticalSection(&m_cs);
		}

	private:
		CRITICAL_SECTION		m_cs;
	};


	class MutexLock 
	{
	public:

		MutexLock()
		{
			m_hMutex = CreateMutex(NULL, FALSE, NULL);
		}

		~MutexLock()
		{
			CloseHandle(m_hMutex);
		}

		void Lock()
		{
			DWORD obj = WaitForSingleObject(m_hMutex, INFINITE);
		}

		void Unlock()
		{
			ReleaseMutex(m_hMutex);
		}
	private:
		HANDLE		m_hMutex;
	};



	enum
	{
		H3D_WAIT_NONE = 0,
		H3D_WAIT_TIMEOUT,
		H3D_WAIT_SIGNALED,
	};


	class Event 
	{
	public:

		Event():m_hEvent(NULL)	{}
		virtual ~Event()	{}

		inline HANDLE	GetHandle() const;

	protected:
		HANDLE	m_hEvent;

	};

	HANDLE Event::GetHandle() const
	{
		return m_hEvent;
	}



	class ManualResetEvent : public Event
	{
	public:
		ManualResetEvent()
		{
			m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			assert(m_hEvent);
		}

		virtual ~ManualResetEvent()
		{
			CloseHandle(m_hEvent);
		}

		inline void		Signal();
		inline void		Reset();
		inline int		WaitOn(int timeOut);
	};


	void ManualResetEvent::Signal()
	{
		SetEvent(m_hEvent);
	}

	void ManualResetEvent::Reset()
	{
		ResetEvent(m_hEvent);	
	}

	int ManualResetEvent::WaitOn(int timeOut)
	{
		uint result = WaitForSingleObject(m_hEvent, timeOut == 0 ? INFINITE : timeOut);

		if (result == WAIT_TIMEOUT)
		{
			return (int)H3D_WAIT_TIMEOUT;
		}

		if (result == WAIT_OBJECT_0)
		{
			return (int)H3D_WAIT_SIGNALED;
		}

		return (int)H3D_WAIT_NONE;
	}



	class AutoResetEvent : public Event
	{
	public:
		AutoResetEvent()
		{
			m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(m_hEvent);
		}

		virtual ~AutoResetEvent()
		{
			CloseHandle(m_hEvent);
		}

		inline void		SignalAndReset();

		inline int		WaitOn(int timeOut);
	};

	void AutoResetEvent::SignalAndReset()
	{
		SetEvent(m_hEvent);
	}

	int AutoResetEvent::WaitOn(int timeOut)
	{
		uint result = WaitForSingleObject(m_hEvent, timeOut == 0 ? INFINITE : timeOut);

		if (result == WAIT_TIMEOUT)
		{
			return (int)H3D_WAIT_TIMEOUT;
		}

		if (result == WAIT_OBJECT_0)
		{
			return (int)H3D_WAIT_SIGNALED;
		}

		return (int)H3D_WAIT_NONE;
	}


	class EventPool 
	{
	public:

		EventPool();
		~EventPool();

		void	RegisterEvent(Event* pEvent);
		void	UnRegisterEvent(Event* pEvent);

		int		GetNumEvent() const;

		int		WaitAll(int nMilliseconds);
		int		WaitAny(int nMilliseconds);

	private:
		vector<HANDLE>	m_Handles;
	};


	EventPool::EventPool()
	{

	}

	EventPool::~EventPool()
	{

	}

	void EventPool::RegisterEvent(Event* pEvent)
	{
		m_Handles.push_back(pEvent->GetHandle());
	}

	void EventPool::UnRegisterEvent(Event* pEvent)
	{
		vector<HANDLE>::iterator it = std::find(m_Handles.begin(), m_Handles.end(), pEvent->GetHandle());
		assert(it != m_Handles.end());

		m_Handles.erase(it);
	}

	int EventPool::GetNumEvent() const
	{
		return m_Handles.size();
	}

	int EventPool::WaitAll(int nMilliseconds)
	{
		if (m_Handles.empty())
		{
			return (int)H3D_WAIT_NONE;
		}
		uint result = WaitForMultipleObjects(m_Handles.size(), &m_Handles[0], TRUE, 
			nMilliseconds == 0 ? INFINITE : nMilliseconds);
		if (result == WAIT_TIMEOUT)
		{
			//LogInfo(BEAM_INFO,OutPut_Console, "WaitForMultipleObjects time out!\n");
			return (int)H3D_WAIT_TIMEOUT;
		}

		if (result - WAIT_OBJECT_0 < m_Handles.size())
		{
			return (int)H3D_WAIT_SIGNALED;
		}

		return (int)H3D_WAIT_NONE;
	}

	int EventPool::WaitAny(int nMilliseconds)
	{
		if (m_Handles.empty())
		{
			return (int)H3D_WAIT_NONE;
		}

		uint result = WaitForMultipleObjects(m_Handles.size(), &m_Handles[0], FALSE, 
			nMilliseconds == 0 ? INFINITE : nMilliseconds);
		if (result == WAIT_TIMEOUT)
		{
			//LogInfo(BEAM_INFO,OutPut_Console, "WaitForMultipleObjects time out!\n");
			return (int)H3D_WAIT_TIMEOUT;
		}

		if (result - WAIT_OBJECT_0 < m_Handles.size())
		{
			return (int)H3D_WAIT_SIGNALED;
		}

		return (int)H3D_WAIT_NONE;
	}


	//////////////////////////////////////////////////////////////////////////


	typedef __declspec(align(32)) LONG volatile AtomInt;



	class WorkingThread : public IThread
	{
	public:
		WorkingThread(bool bAutoFinish);
		~WorkingThread();

		virtual EThreadState GetState() const;
		virtual void AddTask(ITask* task, void* param);
		virtual uint GetNumTask() const;
		virtual void FinishAllTasks(int timeout);

		virtual void Release();

		void Run();

		static DWORD WINAPI ThreadProc(LPVOID lpThis);

	private:
		bool Init();
		pair<ITask*, void*> GetNextTask();

		typedef list<pair<ITask*, void*>> TaskList;
		TaskList m_Tasks;
		mutable SectionLock m_TaskListLock;
		ManualResetEvent	m_Event;

		bool m_bAutoFinish;
		AtomInt	m_nRunning;
		HANDLE m_hThread;
		uint m_nThreadId;
	};


	DWORD WINAPI WorkingThread::ThreadProc(LPVOID lpThis)
	{
		WorkingThread* pThread = (WorkingThread*)lpThis;
		pThread->Run();

		return 0;
	}


	WorkingThread::WorkingThread(bool bAutoFinish)
	{
		m_bAutoFinish	= bAutoFinish;
		m_hThread		= NULL;
		m_nRunning		= 0;
		m_nThreadId		= 0;
		Init();
	}

	WorkingThread::~WorkingThread()
	{
		FinishAllTasks(0);
	}

	bool WorkingThread::Init()
	{
		m_nRunning = 0;
		InterlockedIncrement(&m_nRunning);
		m_hThread = ::CreateThread( 
			NULL,              // default security attributes
			0,                 // use default stack size  
			ThreadProc,        // thread function 
			(LPVOID)this,             // argument to thread function 
			0/*CREATE_SUSPENDED*/,         // use default creation flags 
			(DWORD*)&m_nThreadId);   // returns the thread identifier 
		assert(m_hThread != NULL);

		if (m_hThread == NULL)
		{
			GLog->Error("Create thread failed");
			return false;
		}

		return true;
	}


	void WorkingThread::Run()
	{
		while (m_nRunning)
		{
			pair<ITask*, void*> task = GetNextTask();
			if (task.first)
			{
				task.first->Run(task.second);
				continue;
			}

			if (!m_bAutoFinish)
			{
				m_Event.WaitOn(0);
			}
		}
	}

	pair<ITask*, void*> WorkingThread::GetNextTask()
	{
		ScopedLock<SectionLock> lock(&m_TaskListLock);
			
		if (m_Tasks.empty())
		{
			return pair<ITask*, void*>((ITask*)NULL, (void*)NULL);
		}

		pair<ITask*, void*> task = m_Tasks.front();
		m_Tasks.pop_front();

		return task;
	}

	EThreadState WorkingThread::GetState() const
	{
		ScopedLock<SectionLock> lock(&m_TaskListLock);

		if (!m_nRunning)
		{
			return TS_FINISHED;
		}

		if (!m_Tasks.empty())
		{
			return TS_RUNNING;
		}

		return TS_APPENDDING;
	}

	void WorkingThread::AddTask(ITask* task, void* param)
	{
		ScopedLock<SectionLock> lock(&m_TaskListLock);

		m_Tasks.push_back(make_pair(task, param));
		m_Event.Signal();
	}

	uint WorkingThread::GetNumTask() const
	{
		ScopedLock<SectionLock> lock(&m_TaskListLock);
		return m_Tasks.size();
	}

	void WorkingThread::FinishAllTasks(int timeout)
	{
		if (!m_nRunning)
		{
			return;
		}

		InterlockedDecrement(&m_nRunning);
		WaitForSingleObject(m_hThread, timeout == 0 ? INFINITE : timeout);
		CloseHandle(m_hThread);

		m_hThread = NULL;
		m_nThreadId = 0;
	}

	void WorkingThread::Release()
	{
		FinishAllTasks(0);
		delete this;
	}

	IThread* CreateThread(ITask* task, void* param, bool bAutoFinish)
	{
		if (task == NULL && bAutoFinish)
		{
			GLog->Error("Thread with no task will terminate immediately");
		}

		IThread* pThread = new WorkingThread(bAutoFinish);
		if (task)
		{
			pThread->AddTask(task, param);
		}

		return pThread;
	}

	//////////////////////////////////////////////////////////////////////////

	class ThreadLock : public ILock
	{
	public:
		ThreadLock()	{}
		~ThreadLock()	{}

		virtual void Lock();
		virtual void Unlock();
		virtual void Release();
	private:
		SectionLock m_Lock;
	};

	void ThreadLock::Lock()
	{
		m_Lock.Lock();
	}

	void ThreadLock::Unlock()
	{
		m_Lock.Unlock();
	}

	void ThreadLock::Release()
	{
		delete this;
	}

	ILock* CreateThreadLock()
	{
		return new ThreadLock();
	}



}