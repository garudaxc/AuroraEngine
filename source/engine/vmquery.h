/****************************************************************************** 
Module:  VMQuery.h 
Notices: Copyright (c) 2000 Jeffrey Richter 
******************************************************************************/ 
#pragma once
#include <windows.h>
#include <list>

struct MemoryData
{
	//这里的数据单位是字节
	ULONGLONG m_commitedMem;
	ULONGLONG m_reservedMem;
	ULONGLONG m_freeMem;
	ULONGLONG m_biggestFreeBlock;

	std::list<unsigned long> m_freeBlockList;

	void clear()
	{
		m_freeMem = 0;
		m_commitedMem = 0;
		m_reservedMem = 0;
		m_biggestFreeBlock = 0;
		m_freeBlockList.clear();
	}
	MemoryData()
	{
		clear();
	}
};


void GetMemoryData(MemoryData& memData,DWORD g_dwProcessId = 0,const char* outputFilePreStr = NULL);

/// dump堆信息
void dumpHeapMemInfo();

typedef struct  
{ 
	// Region information 
	PVOID  pvRgnBaseAddress; 
	DWORD  dwRgnProtection;  // PAGE_* 
	SIZE_T RgnSize; 
	DWORD  dwRgnStorage;     // MEM_*: Free, Image, Mapped, Private 
	DWORD  dwRgnBlocks; 
	DWORD  dwRgnGuardBlks;   // If > 0, region contains thread stack 
	BOOL   fRgnIsAStack;     // TRUE if region contains thread stack 

	// Block information 
	PVOID  pvBlkBaseAddress; 
	DWORD  dwBlkProtection;  // PAGE_* 
	SIZE_T BlkSize; 
	DWORD  dwBlkStorage;     // MEM_*: Free, Reserve, Image, Mapped, Private 
} VMQUERY, *PVMQUERY; 

/////////////////////////////////////////////////////////////////////////////// 

BOOL VMQuery(HANDLE hProcess, LPCVOID pvAddress, PVMQUERY pVMQ); 



 
//////////////////////////////// End of File ////////////////////////////////// 