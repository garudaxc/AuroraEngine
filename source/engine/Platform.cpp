#include "stdheader.h"
#include "Platform.h"
#include <Windows.h>
#include <io.h>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <intrin.h>

#include "Log.h"

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

	Win32Info win32Info;


	class InstructionSet
	{
		// forward declarations
		class InstructionSet_Internal;

	public:
		// getters
		static std::string Vendor(void) { return CPU_Rep.vendor_; }
		static std::string Brand(void) { return CPU_Rep.brand_; }

		static bool SSE3(void) { return CPU_Rep.f_1_ECX_[0]; }
		static bool PCLMULQDQ(void) { return CPU_Rep.f_1_ECX_[1]; }
		static bool MONITOR(void) { return CPU_Rep.f_1_ECX_[3]; }
		static bool SSSE3(void) { return CPU_Rep.f_1_ECX_[9]; }
		static bool FMA(void) { return CPU_Rep.f_1_ECX_[12]; }
		static bool CMPXCHG16B(void) { return CPU_Rep.f_1_ECX_[13]; }
		static bool SSE41(void) { return CPU_Rep.f_1_ECX_[19]; }
		static bool SSE42(void) { return CPU_Rep.f_1_ECX_[20]; }
		static bool MOVBE(void) { return CPU_Rep.f_1_ECX_[22]; }
		static bool POPCNT(void) { return CPU_Rep.f_1_ECX_[23]; }
		static bool AES(void) { return CPU_Rep.f_1_ECX_[25]; }
		static bool XSAVE(void) { return CPU_Rep.f_1_ECX_[26]; }
		static bool OSXSAVE(void) { return CPU_Rep.f_1_ECX_[27]; }
		static bool AVX(void) { return CPU_Rep.f_1_ECX_[28]; }
		static bool F16C(void) { return CPU_Rep.f_1_ECX_[29]; }
		static bool RDRAND(void) { return CPU_Rep.f_1_ECX_[30]; }

		static bool MSR(void) { return CPU_Rep.f_1_EDX_[5]; }
		static bool CX8(void) { return CPU_Rep.f_1_EDX_[8]; }
		static bool SEP(void) { return CPU_Rep.f_1_EDX_[11]; }
		static bool CMOV(void) { return CPU_Rep.f_1_EDX_[15]; }
		static bool CLFSH(void) { return CPU_Rep.f_1_EDX_[19]; }
		static bool MMX(void) { return CPU_Rep.f_1_EDX_[23]; }
		static bool FXSR(void) { return CPU_Rep.f_1_EDX_[24]; }
		static bool SSE(void) { return CPU_Rep.f_1_EDX_[25]; }
		static bool SSE2(void) { return CPU_Rep.f_1_EDX_[26]; }

		static bool FSGSBASE(void) { return CPU_Rep.f_7_EBX_[0]; }
		static bool BMI1(void) { return CPU_Rep.f_7_EBX_[3]; }
		static bool HLE(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[4]; }
		static bool AVX2(void) { return CPU_Rep.f_7_EBX_[5]; }
		static bool BMI2(void) { return CPU_Rep.f_7_EBX_[8]; }
		static bool ERMS(void) { return CPU_Rep.f_7_EBX_[9]; }
		static bool INVPCID(void) { return CPU_Rep.f_7_EBX_[10]; }
		static bool RTM(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[11]; }
		static bool AVX512F(void) { return CPU_Rep.f_7_EBX_[16]; }
		static bool RDSEED(void) { return CPU_Rep.f_7_EBX_[18]; }
		static bool ADX(void) { return CPU_Rep.f_7_EBX_[19]; }
		static bool AVX512PF(void) { return CPU_Rep.f_7_EBX_[26]; }
		static bool AVX512ER(void) { return CPU_Rep.f_7_EBX_[27]; }
		static bool AVX512CD(void) { return CPU_Rep.f_7_EBX_[28]; }
		static bool SHA(void) { return CPU_Rep.f_7_EBX_[29]; }

		static bool PREFETCHWT1(void) { return CPU_Rep.f_7_ECX_[0]; }

		static bool LAHF(void) { return CPU_Rep.f_81_ECX_[0]; }
		static bool LZCNT(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_ECX_[5]; }
		static bool ABM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[5]; }
		static bool SSE4a(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[6]; }
		static bool XOP(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[11]; }
		static bool TBM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[21]; }

		static bool SYSCALL(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[11]; }
		static bool MMXEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[22]; }
		static bool RDTSCP(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[27]; }
		static bool _3DNOWEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[30]; }
		static bool _3DNOW(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[31]; }

	private:
		static const InstructionSet_Internal CPU_Rep;

		class InstructionSet_Internal
		{
		public:
			InstructionSet_Internal()
				: nIds_{ 0 },
				nExIds_{ 0 },
				isIntel_{ false },
				isAMD_{ false },
				f_1_ECX_{ 0 },
				f_1_EDX_{ 0 },
				f_7_EBX_{ 0 },
				f_7_ECX_{ 0 },
				f_81_ECX_{ 0 },
				f_81_EDX_{ 0 },
				data_{},
				extdata_{}
			{
				//int cpuInfo[4] = {-1};
				std::array<int, 4> cpui;

				// Calling __cpuid with 0x0 as the function_id argument
				// gets the number of the highest valid function ID.
				__cpuid(cpui.data(), 0);
				nIds_ = cpui[0];

				for (int i = 0; i <= nIds_; ++i)
				{
					__cpuidex(cpui.data(), i, 0);
					data_.push_back(cpui);
				}

				// Capture vendor string
				char vendor[0x20];
				memset(vendor, 0, sizeof(vendor));
				*reinterpret_cast<int*>(vendor) = data_[0][1];
				*reinterpret_cast<int*>(vendor + 4) = data_[0][3];
				*reinterpret_cast<int*>(vendor + 8) = data_[0][2];
				vendor_ = vendor;
				if (vendor_ == "GenuineIntel")
				{
					isIntel_ = true;
				}
				else if (vendor_ == "AuthenticAMD")
				{
					isAMD_ = true;
				}

				// load bitset with flags for function 0x00000001
				if (nIds_ >= 1)
				{
					f_1_ECX_ = data_[1][2];
					f_1_EDX_ = data_[1][3];
				}

				// load bitset with flags for function 0x00000007
				if (nIds_ >= 7)
				{
					f_7_EBX_ = data_[7][1];
					f_7_ECX_ = data_[7][2];
				}

				// Calling __cpuid with 0x80000000 as the function_id argument
				// gets the number of the highest valid extended ID.
				__cpuid(cpui.data(), 0x80000000);
				nExIds_ = cpui[0];

				char brand[0x40];
				memset(brand, 0, sizeof(brand));

				for (int i = 0x80000000; i <= nExIds_; ++i)
				{
					__cpuidex(cpui.data(), i, 0);
					extdata_.push_back(cpui);
				}

				// load bitset with flags for function 0x80000001
				if (nExIds_ >= 0x80000001)
				{
					f_81_ECX_ = extdata_[1][2];
					f_81_EDX_ = extdata_[1][3];
				}

				// Interpret CPU brand string if reported
				if (nExIds_ >= 0x80000004)
				{
					memcpy(brand, extdata_[2].data(), sizeof(cpui));
					memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
					memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
					brand_ = brand;
				}
			};

			int nIds_;
			int nExIds_;
			std::string vendor_;
			std::string brand_;
			bool isIntel_;
			bool isAMD_;
			std::bitset<32> f_1_ECX_;
			std::bitset<32> f_1_EDX_;
			std::bitset<32> f_7_EBX_;
			std::bitset<32> f_7_ECX_;
			std::bitset<32> f_81_ECX_;
			std::bitset<32> f_81_EDX_;
			std::vector<std::array<int, 4>> data_;
			std::vector<std::array<int, 4>> extdata_;
		};
	};

	// Initialize static member data
	const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;



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
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = nullptr;
	PCACHE_DESCRIPTOR Cache;
	LPFN_GLPI	glpi;
	BOOL		done = FALSE;
	DWORD		returnLength = 0;
	DWORD		byteOffset = 0;

	memset( & cpuInfo, 0, sizeof( cpuInfo ) );

	glpi = (LPFN_GLPI)GetProcAddress( GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation" );
	if ( nullptr == glpi ) {
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


	// check for Multi Media Extensions
	if (InstructionSet::MMX() ) {
		flags |= CPUID_MMX;
	}

	// check for 3DNow!
	if (InstructionSet::_3DNOW() ) {
		flags |= CPUID_3DNOW;
	}

	// check for Streaming SIMD Extensions
	if (InstructionSet::SSE() ) {
		flags |= CPUID_SSE | CPUID_FTZ;
	}

	// check for Streaming SIMD Extensions 2
	if (InstructionSet::SSE2() ) {
		flags |= CPUID_SSE2;
	}

	// check for Streaming SIMD Extensions 3 aka Prescott's New Instructions
	if (InstructionSet::SSE3() ) {
		flags |= CPUID_SSE3;
	}

	// check for Hyper-Threading Technology
	//if (InstructionSet::HTT() ) {
	//	flags |= CPUID_HTT;
	//}

	// check for Conditional Move (CMOV) and fast floating point comparison (FCOMI) instructions
	if (InstructionSet::CMOV() ) {
		flags |= CPUID_CMOV;
	}

	// check for Denormals-Are-Zero mode
	//if (InstructionSet::DAZ() ) {
	//	flags |= CPUID_DAZ;
	//}

	return (cpuid_t)flags;
}


void InitPlatform()
{
	Win32Info win32Info;
	CoInitialize( nullptr );

	//
	// Windows version
	//
	win32Info.osversion.dwOSVersionInfoSize = sizeof( win32Info.osversion );

	if ( !GetVersionEx( (LPOSVERSIONINFO)&win32Info.osversion ) )
		GLog->Error( "Couldn't get OS info" );

	string osVersion;

	// on win10, GetVersionEx have been deprecated, so we can only get win8 here
	// https://docs.microsoft.com/zh-cn/windows/win32/sysinfo/targeting-your-application-at-windows-8-1

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

	//if ( win32Info.cpuid & CPUID_AMD ) {
	//	cpuInfo += "AMD CPU";
	//} else if ( win32Info.cpuid & CPUID_INTEL ) {
	//	cpuInfo += "Intel CPU";
	//} else if ( win32Info.cpuid & CPUID_UNSUPPORTED ) {
	//	cpuInfo += "unsupported CPU";
	//} else {
	//	cpuInfo += "generic CPU";
	//}

	cpuInfo += InstructionSet::Vendor();
	cpuInfo += "  ";
	cpuInfo += InstructionSet::Brand();


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
	//	HANDLE hFile = CreateFileA(pathName.c_str(), 0, 0, nullptr, 0, OPEN_EXISTING, nullptr);

	//	return true;
	//}

	static CPlatform _GPlatform;
	CPlatform* GPlatform = &_GPlatform;

	 bool CPlatform::IsFileExist(const String& pathName) const
	 {
	 	if (_access(pathName.c_str(), 0) == -1)
	 	{
	 		return false;
	 	}
	
	 	return true;
	 }


	// DIR *dir;
	// struct dirent *ent;
	// if ((dir = opendir ("c:\\src\\")) != NULL) {
	// 	/* print all the files and directories within directory */
	// 	while ((ent = readdir (dir)) != NULL) {
	// 		printf ("%s\n", ent->d_name);
	// 	}
	// 	closedir (dir);
	// } else {
	// 	/* could not open directory */
	// 	perror ("");
	// 	return EXIT_FAILURE;
	// }
	//
	
	 Array<String> CPlatform::ListFile(const String& InPath, const String& InFilter) const
	 {
	 	WIN32_FIND_DATA FindFileData;

	 	Array<String> result;

	 	String path = InPath + '/';
	 	path += InFilter.empty() ? "*.*" : InFilter;


	 	HANDLE hFind = FindFirstFile(path.c_str(), &FindFileData);
	 	if (hFind == INVALID_HANDLE_VALUE) 
	 	{
	 		GLog->Error("FindFirstFile failed  in path %s", InPath.c_str());
	 		return result;
	 	}
	 	
	    do
	    {
		    if (! (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		    {
			    result.push_back(FindFileData.cFileName);
		    }
	    } while (::FindNextFile(hFind, &FindFileData));

	    FindClose(hFind);

	 	return result;	 	
	 }
	 String CPlatform::GetShaderFilePath() const
	 {
	 	return "../dev/data/shader";
	 }

	 String CPlatform::GetWorkingPath() const
	 {
	 	return ".";
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
			m_hMutex = CreateMutex(nullptr, FALSE, nullptr);
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

		Event():m_hEvent(nullptr)	{}
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
			m_hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
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
			m_hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
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
		m_hThread		= nullptr;
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
			nullptr,              // default security attributes
			0,                 // use default stack size  
			ThreadProc,        // thread function 
			(LPVOID)this,             // argument to thread function 
			0/*CREATE_SUSPENDED*/,         // use default creation flags 
			(DWORD*)&m_nThreadId);   // returns the thread identifier 
		assert(m_hThread != nullptr);

		if (m_hThread == nullptr)
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
			return pair<ITask*, void*>((ITask*)nullptr, (void*)nullptr);
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

		m_hThread = nullptr;
		m_nThreadId = 0;
	}

	void WorkingThread::Release()
	{
		FinishAllTasks(0);
		delete this;
	}

	IThread* CreateThread(ITask* task, void* param, bool bAutoFinish)
	{
		if (task == nullptr && bAutoFinish)
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

	CScreen* CPlatform::MainScreen;

}