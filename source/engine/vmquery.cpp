/******************************************************************************  
Module:  VMQuery.cpp  
Notices: Copyright (c) 2000 Jeffrey Richter  
******************************************************************************/   
#include "VMQuery.h" 
#include "umconverter.h"
#include <iostream>
#include <fstream>
//#include <client_utilities/um_converter.h>   
#include   <TlHelp32.h> 
#include <tchar.h>
#include <stdlib.h>

// C����ʱͷ�ļ�
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cassert>

// ��ֹ ����strcmp ��Ҫ�������°汾 strcmp_s֮��ľ���
#pragma warning( disable : 4996 )
// ��ֹ double --> float ת������
#pragma warning( disable : 4305 )
#pragma warning( disable : 4244 )

using namespace std;
//void globalLog(char* info);
///////////////////////////////////////////////////////////////////////////////    

// Helper structure    
typedef struct    
{   
   SIZE_T RgnSize;   
   DWORD  dwRgnStorage;     // MEM_*: Free, Image, Mapped, Private    
   DWORD  dwRgnBlocks;   
   DWORD  dwRgnGuardBlks;   // If > 0, region contains thread stack    
   BOOL   fRgnIsAStack;     // TRUE if region contains thread stack    
} VMQUERY_HELP;   
   
// This global, static variable holds the allocation granularity value for     
// this CPU platform. Initialized the first time VMQuery is called.    
static DWORD gs_dwAllocGran = 0;   
      
///////////////////////////////////////////////////////////////////////////////    
   
   
// Iterates through a region's blocks and returns findings in VMQUERY_HELP    
static BOOL VMQueryHelp(HANDLE hProcess, LPCVOID pvAddress, VMQUERY_HELP *pVMQHelp)    
{   
   // Each element contains a page protection    
   // (i.e.: 0=reserved, PAGE_NOACCESS, PAGE_READWRITE, etc.)    
   DWORD dwProtectBlock[4] = { 0 };    
   
   ZeroMemory(pVMQHelp, sizeof(*pVMQHelp));   
   
   // Get address of region containing passed memory address.    
   MEMORY_BASIC_INFORMATION mbi;   
   BOOL fOk = (VirtualQueryEx(hProcess, pvAddress, &mbi, sizeof(mbi)) == sizeof(mbi));   
   
   if (!fOk)   
      return(fOk);   // Bad memory address, return failure    
   
   // Walk starting at the region's base address (which never changes)    
   PVOID pvRgnBaseAddress = mbi.AllocationBase;   
   
   // Walk starting at the first block in the region (changes in the loop)    
   PVOID pvAddressBlk = pvRgnBaseAddress;   
   
   // Save the memory type of the physical storage block.    
   pVMQHelp->dwRgnStorage = mbi.Type;   
   
   for (;;)    
   {   
      // Get info about the current block.    
      fOk = (VirtualQueryEx(hProcess, pvAddressBlk, &mbi, sizeof(mbi)) == sizeof(mbi));   
      if (!fOk)   
         break;   // Couldn't get the information, end loop.    
   
      // Is this block in the same region?    
      if (mbi.AllocationBase != pvRgnBaseAddress)   
         break;   // Found a block in the next region; end loop.    
   
      // We have a block contained in the region.    
   
      // The following if statement is for detecting stacks in Windows 98.    
      // A Windows 98 stack region's last 4 blocks look like this:    
      //    reserved block, no access block, read-write block, reserved block    
      if (pVMQHelp->dwRgnBlocks < 4)    
      {   
         // 0th through 3rd block, remember the block's protection    
         dwProtectBlock[pVMQHelp->dwRgnBlocks] =    
            (mbi.State == MEM_RESERVE) ? 0 : mbi.Protect;   
      }    
      else    
      {   
         // We've seen 4 blocks in this region.    
         // Shift the protection values down in the array.    
         MoveMemory(&dwProtectBlock[0], &dwProtectBlock[1], sizeof(dwProtectBlock) - sizeof(DWORD));   
   
         // Add the new protection value to the end of the array.    
         dwProtectBlock[3] = (mbi.State == MEM_RESERVE) ? 0 : mbi.Protect;   
      }   
   
      pVMQHelp->dwRgnBlocks++;             // Add another block to the region    
      pVMQHelp->RgnSize += mbi.RegionSize; // Add block's size to region size    
   
      // If block has PAGE_GUARD attribute, add 1 to this counter    
      if ((mbi.Protect & PAGE_GUARD) == PAGE_GUARD)   
         pVMQHelp->dwRgnGuardBlks++;   
   
      // Take a best guess as to the type of physical storage committed to the    
      // block. This is a guess because some blocks can convert from MEM_IMAGE    
      // to MEM_PRIVATE or from MEM_MAPPED to MEM_PRIVATE; MEM_PRIVATE can    
      // always be overridden by MEM_IMAGE or MEM_MAPPED.    
      if (pVMQHelp->dwRgnStorage == MEM_PRIVATE)   
         pVMQHelp->dwRgnStorage = mbi.Type;   
   
      // Get the address of the next block.    
      pvAddressBlk = (PVOID) ((PBYTE) pvAddressBlk + mbi.RegionSize);   
   }   
   
   // After examining the region, check to see whether it is a thread stack    
   // Windows 2000: Assume stack if region has at least 1 PAGE_GUARD block    
   // Windows 9x:   Assume stack if region has at least 4 blocks with    
   //               3rd block from end: reserved    
   //               2nd block from end: PAGE_NOACCESS    
   //               1st block from end: PAGE_READWRITE    
   //               block at end: another reserved block.    
   pVMQHelp->fRgnIsAStack =   
      (pVMQHelp->dwRgnGuardBlks > 0)         ||   
      ((pVMQHelp->dwRgnBlocks >= 4)          &&   
       (dwProtectBlock[0] == 0)              &&    
       (dwProtectBlock[1] == PAGE_NOACCESS)  &&   
       (dwProtectBlock[2] == PAGE_READWRITE) &&   
       (dwProtectBlock[3] == 0));   
   
   return(TRUE);   
}   
   
   
///////////////////////////////////////////////////////////////////////////////    
   
   
BOOL VMQuery(HANDLE hProcess, LPCVOID pvAddress, PVMQUERY pVMQ)    
{   
   if (gs_dwAllocGran == 0)    
   {   
      // Set allocation granularity if this is the first call    
      SYSTEM_INFO sinf;   
      GetSystemInfo(&sinf);   
      gs_dwAllocGran = sinf.dwAllocationGranularity;   
   }   
   
   ZeroMemory(pVMQ, sizeof(*pVMQ));   
   
   //Get the MEMORY_BASIC_INFORMATION for the passed address.    
   MEMORY_BASIC_INFORMATION mbi;   
   BOOL fOk = (VirtualQueryEx(hProcess, pvAddress, &mbi, sizeof(mbi)) == sizeof(mbi));   
   
   if (!fOk)   
      return(fOk);   // Bad memory address, return failure    
   
   // The MEMORY_BASIC_INFORMATION structure contains valid information.    
   // Time to start setting the members of our own VMQUERY structure.    
   
   // First, fill in the block members. We'll fill the region members later.    
   switch (mbi.State)    
   {   
      case MEM_FREE:       // Free block (not reserved)    
         pVMQ->pvBlkBaseAddress = nullptr;   
         pVMQ->BlkSize = 0;   
         pVMQ->dwBlkProtection = 0;   
         pVMQ->dwBlkStorage = MEM_FREE;   
         break;   
   
      case MEM_RESERVE:    // Reserved block without committed storage in it.    
         pVMQ->pvBlkBaseAddress = mbi.BaseAddress;   
         pVMQ->BlkSize = mbi.RegionSize;   
   
         // For an uncommitted block, mbi.Protect is invalid. So we will     
         // show that the reserved block inherits the protection attribute     
         // of the region in which it is contained.    
         pVMQ->dwBlkProtection = mbi.AllocationProtect;     
         pVMQ->dwBlkStorage = MEM_RESERVE;   
         break;   
   
      case MEM_COMMIT:     // Reserved block with committed storage in it.    
         pVMQ->pvBlkBaseAddress = mbi.BaseAddress;   
         pVMQ->BlkSize = mbi.RegionSize;   
         pVMQ->dwBlkProtection = mbi.Protect;      
         pVMQ->dwBlkStorage = mbi.Type;   
         break;   
   
      default:   
          DebugBreak();   
          break;   
   }   
   
   // Now fill in the region data members.    
   VMQUERY_HELP VMQHelp;   
   switch (mbi.State)    
   {   
      case MEM_FREE:       // Free block (not reserved)    
         pVMQ->pvRgnBaseAddress = mbi.BaseAddress;   
         pVMQ->dwRgnProtection  = mbi.AllocationProtect;   
         pVMQ->RgnSize          = mbi.RegionSize;   
         pVMQ->dwRgnStorage     = MEM_FREE;   
         pVMQ->dwRgnBlocks      = 0;   
         pVMQ->dwRgnGuardBlks   = 0;   
         pVMQ->fRgnIsAStack     = FALSE;   
         break;   
   
      case MEM_RESERVE:    // Reserved block without committed storage in it.    
         pVMQ->pvRgnBaseAddress = mbi.AllocationBase;   
         pVMQ->dwRgnProtection  = mbi.AllocationProtect;   
   
         // Iterate through all blocks to get complete region information.             
         VMQueryHelp(hProcess, pvAddress, &VMQHelp);   
   
         pVMQ->RgnSize          = VMQHelp.RgnSize;   
         pVMQ->dwRgnStorage     = VMQHelp.dwRgnStorage;   
         pVMQ->dwRgnBlocks      = VMQHelp.dwRgnBlocks;   
         pVMQ->dwRgnGuardBlks   = VMQHelp.dwRgnGuardBlks;   
         pVMQ->fRgnIsAStack     = VMQHelp.fRgnIsAStack;   
         break;   
   
      case MEM_COMMIT:     // Reserved block with committed storage in it.    
         pVMQ->pvRgnBaseAddress = mbi.AllocationBase;   
         pVMQ->dwRgnProtection  = mbi.AllocationProtect;   
   
         // Iterate through all blocks to get complete region information.             
         VMQueryHelp(hProcess, pvAddress, &VMQHelp);   
   
         pVMQ->RgnSize          = VMQHelp.RgnSize;   
         pVMQ->dwRgnStorage     = VMQHelp.dwRgnStorage;   
         pVMQ->dwRgnBlocks      = VMQHelp.dwRgnBlocks;   
         pVMQ->dwRgnGuardBlks   = VMQHelp.dwRgnGuardBlks;   
         pVMQ->fRgnIsAStack     = VMQHelp.fRgnIsAStack;   
         break;   
   
      default:   
          DebugBreak();   
          break;   
   }   
   
   return(fOk);   
}   

// GetMappedFileName is only on Windows 2000 in PSAPI.DLL    
// If this function exists on the host system, we'll use it    
typedef DWORD (WINAPI* PFNGETMAPPEDFILENAME)(HANDLE, PVOID, PTSTR, DWORD);   
static PFNGETMAPPEDFILENAME g_pfnGetMappedFileName = nullptr;  

static PCTSTR GetMemStorageText(DWORD dwStorage)    
{   
	PCTSTR p = TEXT("Unknown");   
	switch (dwStorage)    
	{   
	case MEM_FREE:    p = TEXT("Free   "); break;   
	case MEM_RESERVE: p = TEXT("Reserve"); break;   
	case MEM_IMAGE:   p = TEXT("Image  "); break;   
	case MEM_MAPPED:  p = TEXT("Mapped "); break;   
	case MEM_PRIVATE: p = TEXT("Private"); break;   
	}   
	return(p);   
}   


///////////////////////////////////////////////////////////////////////////////    


static PTSTR GetProtectText(DWORD dwProtect, PTSTR szBuf, BOOL fShowFlags)    
{   
	PCTSTR p = TEXT("Unknown");   
	switch (dwProtect & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE))    
	{   
	case PAGE_READONLY:          p = TEXT("-R--"); break;   
	case PAGE_READWRITE:         p = TEXT("-RW-"); break;   
	case PAGE_WRITECOPY:         p = TEXT("-RWC"); break;   
	case PAGE_EXECUTE:           p = TEXT("E---"); break;   
	case PAGE_EXECUTE_READ:      p = TEXT("ER--"); break;   
	case PAGE_EXECUTE_READWRITE: p = TEXT("ERW-"); break;   
	case PAGE_EXECUTE_WRITECOPY: p = TEXT("ERWC"); break;   
	case PAGE_NOACCESS:          p = TEXT("----"); break;   
	}   
	_tcscpy(szBuf, p);   
	if (fShowFlags)    
	{   
		_tcscat(szBuf, TEXT(" "));   
		_tcscat(szBuf, (dwProtect & PAGE_GUARD)        ? TEXT("G") : TEXT("-"));   
		_tcscat(szBuf, (dwProtect & PAGE_NOCACHE)      ? TEXT("N") : TEXT("-"));   
		_tcscat(szBuf, (dwProtect & PAGE_WRITECOMBINE) ? TEXT("W") : TEXT("-"));   
	}   
	return(szBuf);   
}   

static HANDLE m_hSnapshot =  INVALID_HANDLE_VALUE; 
static BOOL   CToolHelpModuleFirst(PMODULEENTRY32   pme){ 
	return   Module32First(m_hSnapshot,pme); 
} 
static BOOL   CToolHelpModuleNext(PMODULEENTRY32   pme){ 
	return   Module32Next(m_hSnapshot,pme); 
} 
static BOOL   CToolHelpModuleFind(PVOID   pvBaseAddr,PMODULEENTRY32   pme)   { 

	m_hSnapshot =  INVALID_HANDLE_VALUE;

	m_hSnapshot =   CreateToolhelp32Snapshot(TH32CS_SNAPALL,GetCurrentProcessId()); 

	BOOL   bFound =   FALSE; 
	for(BOOL   fOK=CToolHelpModuleFirst(pme);fOK;fOK=CToolHelpModuleNext(pme)){ 
		bFound =(pme-> modBaseAddr==pvBaseAddr); 
		if(bFound)break; 
	} 
	
	if(m_hSnapshot !=  INVALID_HANDLE_VALUE)
		CloseHandle(m_hSnapshot);

	return   bFound; 
} 
static int  CToolHelpHeapType(PVOID   pvBaseAddr)   { 
	HANDLE defaultHeap = GetProcessHeap();

	int heapType = 0;
	HANDLE hSnapshot =  INVALID_HANDLE_VALUE;

	hSnapshot =   CreateToolhelp32Snapshot(TH32CS_SNAPALL,GetCurrentProcessId()); 

	HEAPLIST32 heapListNode;
	heapListNode.dwSize = sizeof(HEAPLIST32);

	BOOL   bFound =   FALSE; 
	for(BOOL   fOK=Heap32ListFirst(hSnapshot, &heapListNode);fOK;fOK=Heap32ListNext(hSnapshot, &heapListNode)){ 
		{
			HEAPENTRY32 he;
			ZeroMemory(&he, sizeof(HEAPENTRY32));
			he.dwSize = sizeof(HEAPENTRY32);

			if( Heap32First( &he, GetCurrentProcessId(), heapListNode.th32HeapID ) )
			{
				if((PVOID) he.th32HeapID == pvBaseAddr)
				{
					if((ULONG_PTR)defaultHeap == heapListNode.th32HeapID)
					{
						heapType = 1;//default heap
						break;
					}
					else
					{
						heapType = 2;//share heap
						break;
					}
				}
			}
		} 
	} 
	CloseHandle(hSnapshot);
	return   heapType; 
} 

static void ConstructRgnInfoLine(HANDLE hProcess, PVMQUERY pVMQ, PTSTR szLine, int nMaxLen)    
{   
	_stprintf(szLine, TEXT("%p     %s  %16u  "),   
		pVMQ->pvRgnBaseAddress,   
		GetMemStorageText(pVMQ->dwRgnStorage),   
		pVMQ->RgnSize);   

	if (pVMQ->dwRgnStorage != MEM_FREE)    
	{   
		wsprintf(_tcschr(szLine, 0), TEXT("%5u  "), pVMQ->dwRgnBlocks);   
		GetProtectText(pVMQ->dwRgnProtection, _tcschr(szLine, 0), FALSE);   
	}  

	_tcscat(szLine, TEXT("     "));   

	// Try to obtain the module pathname for this region.    
	int nLen = _tcslen(szLine);   
	if (pVMQ->pvRgnBaseAddress != nullptr)    
	{   
		MODULEENTRY32 me = { sizeof(me) };   

		if (CToolHelpModuleFind(pVMQ->pvRgnBaseAddress, &me))    
		{   
			lstrcat(&szLine[nLen], me.szExePath);   
		}    
		else    
		{   
			{
				// This is not a module; see if it's a memory-mapped file    
				if (g_pfnGetMappedFileName != nullptr)    
				{   
					DWORD d = g_pfnGetMappedFileName(hProcess, pVMQ->pvRgnBaseAddress, szLine + nLen, nMaxLen - nLen);   
					if (d == 0)    
					{   
						// NOTE: GetMappedFileName modifies the string when it fails    
						szLine[nLen] = 0;   
					}   
				}  
			}
 
		}   
	}   

	if (pVMQ->fRgnIsAStack)    
	{   
		_tcscat(szLine, TEXT("Thread Stack"));   
	}   
}   


///////////////////////////////////////////////////////////////////////////////    


static void ConstructBlkInfoLine(PVMQUERY pVMQ, PTSTR szLine, int nMaxLen)    
{   
	_stprintf(szLine, TEXT("   %p  %s  %16u         "),   
		pVMQ->pvBlkBaseAddress,   
		GetMemStorageText(pVMQ->dwBlkStorage),   
		pVMQ->BlkSize);   

	if (pVMQ->dwBlkStorage != MEM_FREE)    
	{   
		GetProtectText(pVMQ->dwBlkProtection, _tcschr(szLine, 0), TRUE);   
	}   
}   
static CHAR *                      //   return error message
	getLastErrorText(                  // converts "Lasr Error" code into text
	CHAR *pBuf,                        //   message buffer
	ULONG bufSize)                     //   buffer size
{
	DWORD retSize;
	LPTSTR pTemp=nullptr;

	if (bufSize < 16) {
		if (bufSize > 0) {
			pBuf[0]='\0';
		}
		return(pBuf);
	}
	retSize=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM|
		FORMAT_MESSAGE_ARGUMENT_ARRAY,
		nullptr,
		GetLastError(),
		LANG_NEUTRAL,
		(LPTSTR)&pTemp,
		0,
		nullptr );
	if (!retSize || pTemp == nullptr) {
		pBuf[0]='\0';
	}
	else {
		pTemp[_tcslen(pTemp)-2]='\0'; //remove cr and newline character
		sprintf(pBuf,"%0.*s (0x%x)",bufSize-16,pTemp,GetLastError());
		LocalFree((HLOCAL)pTemp);
	}
	return(pBuf);
}
static void Refresh(MemoryData& memData,const char* preStr, DWORD dwProcessId, BOOL fExpandRegions)    
{   
	memData.clear();

	ofstream myfile;
	static int s_count = 0;

	if(preStr)
	{
		time_t	timet;
		time( &timet );
		tm *ptm = localtime( &timet );

		char dmpfile[128];
		_snprintf( dmpfile, 127, "./%s-%04d-%02d-%02d-%02d-%02d-%02d-%d.txt", preStr,
			1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, 
			ptm->tm_sec, s_count );
		++s_count;

		myfile.open (dmpfile,ios::out | ios::trunc);
		myfile << "BeginMemOutput.\n";
		myfile.flush();
	}

	// Delete contents of list box & add a horizontal scroll bar    
	//ListBox_ResetContent(hwndLB);   
	//ListBox_SetHorizontalExtent(hwndLB, 300 * LOWORD(GetDialogBaseUnits()));   

	// Is the process still running?    
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);   

	if (hProcess == nullptr)    
	{   
		//globalLog("vm Refresh hProcess == nullptr");
		//ListBox_AddString(hwndLB, TEXT(""));   // Blank line, looks better    
		//ListBox_AddString(hwndLB, TEXT("    The process ID identifies a process that is not running"));   
		return;   
	}   

	// Grab a new snapshot of the process    
	//g_toolhelp.CreateSnapshot(TH32CS_SNAPALL, dwProcessId);   

	// Walk the virtual address space, adding entries to the list box.    
	BOOL fOk = TRUE;   
	PVOID pvAddress = nullptr;   

	HANDLE hHeaps[100] = {0};
	DWORD dwHeaps = GetProcessHeaps(100,hHeaps);
	PVOID pHeaps[100] = {0};
	HANDLE defaultHeap = GetProcessHeap();

	//SetWindowRedraw(hwndLB, FALSE);   
	while (fOk)    
	{   
		VMQUERY vmq;   
		fOk = VMQuery(hProcess, pvAddress, &vmq);   

		if (fOk)    
		{   
			// Construct the line to be displayed, and add it to the list box.    
			TCHAR szLine[1024];   
			ConstructRgnInfoLine(hProcess, &vmq, szLine, sizeof(szLine));   

			if(vmq.dwRgnStorage == MEM_FREE && vmq.RgnSize < 2000000000)//�ӽ�2G�Ĳ�ͳ��
			{
				memData.m_freeMem += vmq.RgnSize; 
				if(vmq.RgnSize > memData.m_biggestFreeBlock)
				{
					memData.m_biggestFreeBlock = vmq.RgnSize;
				}
				memData.m_freeBlockList.push_back(vmq.RgnSize);
			}
			//ListBox_AddString(hwndLB, szLine);   

			if(preStr)
			{
				UMConverter cvt(szLine);
				std::string outStr = cvt.GetMBCS();
				myfile << outStr.c_str();
				for(unsigned int i=0;i<dwHeaps ;i++)
				{
					if(vmq.pvRgnBaseAddress == (PVOID)hHeaps[i])
					{
						if(vmq.pvRgnBaseAddress == (PVOID)defaultHeap)
						{
							myfile << "Default heap"; 
							break;
						}
						else
						{
							myfile << "Shared heap"; 
							break;
						}
					}
				}
				myfile << "\n";
				myfile.flush();
			}


			if (fExpandRegions)    
			{   
				for (DWORD dwBlock = 0; fOk && (dwBlock < vmq.dwRgnBlocks);   dwBlock++)   
				{   
					ConstructBlkInfoLine(&vmq, szLine, sizeof(szLine));  

					if(vmq.dwRgnStorage != MEM_FREE)
					{
						if(vmq.dwBlkStorage == MEM_RESERVE)
						{
							memData.m_reservedMem += vmq.BlkSize;
						}
						else 
						{
							memData.m_commitedMem += vmq.BlkSize;
						}
					}

					//ListBox_AddString(hwndLB, szLine); 
					if(preStr)
					{
						UMConverter cvt(szLine);
						std::string outStr = cvt.GetMBCS();
						myfile << outStr.c_str();
						myfile << "\n";
						myfile.flush();
					}


					// Get the address of the next region to test.    
					pvAddress = ((PBYTE) pvAddress + vmq.BlkSize);   
					if (dwBlock < vmq.dwRgnBlocks - 1)     
					{   
						// Don't query the memory info after the last block.    
						fOk = VMQuery(hProcess, pvAddress, &vmq);   
					}   
				}   
			}   
			// Get the address of the next region to test.    
			pvAddress = ((PBYTE) vmq.pvRgnBaseAddress + vmq.RgnSize);   
		}   
	}   
	//SetWindowRedraw(hwndLB, TRUE);   
	CloseHandle(hProcess);   

	if(preStr)
	{
		myfile << "EndMemOutput.\n";
		myfile.flush();
		myfile.close();
	}
} 
void GetMemoryData(MemoryData& memData,DWORD dwProcessId,const char* outputFilePreStr)
{
	HMODULE hmodPSAPI = LoadLibrary(TEXT("PSAPI"));   
	if (hmodPSAPI != nullptr)    
	{   
#ifdef UNICODE    
		g_pfnGetMappedFileName = (PFNGETMAPPEDFILENAME)    
			GetProcAddress(hmodPSAPI, "GetMappedFileNameW");   
#else    
		g_pfnGetMappedFileName = (PFNGETMAPPEDFILENAME)    
			GetProcAddress(hmodPSAPI, "GetMappedFileNameA");   
#endif          
	}   

	if (dwProcessId == 0)    
	{   
		dwProcessId = GetCurrentProcessId();   
	}   

	Refresh(memData,outputFilePreStr, dwProcessId, true);
}

void dumpHeapMemInfo()
{
	time_t	timet;
	time( &timet );
	tm *ptm = localtime( &timet );

	static int s_count = 0;

	char dmpfile[128];
	_snprintf( dmpfile, 127, "./HeapMemDump-%04d-%02d-%02d-%02d-%02d-%02d-%d.txt",
		1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, 
		ptm->tm_sec, s_count );
	std::ofstream dump_stream;
	dump_stream.open(dmpfile);

	dump_stream << "=========BEIGIN DUMP===================\n";

	HANDLE* handle_arr = new HANDLE[256];
	int num = GetProcessHeaps(256, handle_arr);

	for (int i = 0; i < num; ++i)
	{
		PROCESS_HEAP_ENTRY heap_entry;
		heap_entry.lpData = nullptr;
		heap_entry.iRegionIndex = 0;
		heap_entry.wFlags = 0;

		HANDLE handle = handle_arr[i];
		HeapLock(handle);
		BOOL ret = FALSE;
		dump_stream << "\nHEAP HANDLE:" << handle << "\n";
		while (ret = HeapWalk(handle, &heap_entry))
		{
			if (heap_entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
			{
				dump_stream << "USED BLOCK address:" << heap_entry.lpData << " data size:" << 
					heap_entry.cbData << " header size:" << (int)heap_entry.cbOverhead << " region index:" 
					<< (int)heap_entry.iRegionIndex << "\n";
			}
			else if (heap_entry.wFlags & PROCESS_HEAP_REGION)
			{
				dump_stream << "\nREGION address:" << heap_entry.lpData << " data size:" << 
					heap_entry.cbData << " header size:" << (int)heap_entry.cbOverhead << " region index:" 
					<< (int)heap_entry.iRegionIndex << " commited size:" << heap_entry.Region.dwCommittedSize
					<< " uncommited size:" << heap_entry.Region.dwUnCommittedSize << " first block:" <<
					heap_entry.Region.lpFirstBlock << " last block:"<< heap_entry.Region.lpLastBlock << "\n";
			}
			else if (heap_entry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE)
			{
				dump_stream << "UNCOMMITTED address:" << heap_entry.lpData << " data size:" << 
					heap_entry.cbData << " header size:" << (int)heap_entry.cbOverhead << " region index:" 
					<< (int)heap_entry.iRegionIndex << "\n";
			}
			else
			{
				dump_stream << "FREE BLOCK address:" << heap_entry.lpData << " data size:" << 
					heap_entry.cbData << " header size:" << (int)heap_entry.cbOverhead << " region index:" 
					<< (int)heap_entry.iRegionIndex << "\n";
			}

			heap_entry.iRegionIndex = 0;
		}

		HeapUnlock(handle);
		int err = GetLastError();

		if (err != ERROR_NO_MORE_ITEMS)
		{
			dump_stream << " error \n";
		}
	}

	dump_stream << "\n=========END DUMP===================\n\n";

	dump_stream.flush();
	dump_stream.close();
}
