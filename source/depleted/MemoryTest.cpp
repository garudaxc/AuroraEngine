// MemoryTest.cpp : 定义控制台应用程序的入口点。
//
#include "stdHeader.h"
#include "vmquery.h"

#ifdef USE_TBB
#include "tbb/tbbmalloc_proxy.h"
#endif // USE_TBB

inline ULONGLONG KB(ULONGLONG byte)
{
	return byte / 1024;
}

inline ULONGLONG MB(ULONGLONG byte)
{
	return byte / (1024 * 1024);
}


inline int SmallMemorySize()
{
	const int minSize = 1;
	const int maxSize = 48;

	return (rand() % (maxSize - minSize)) + minSize;
}

inline int BigMemorySize()
{
	const int minSize = 256;
	const int maxSize = 1024 * 256;

	return (rand() % (maxSize - minSize)) + minSize;
}

const int SmallAllocCount = 3000;
const int BigAllocCount = 100;
const int AllocLoop = 1000;

const int MaxReleaseCache = 2048;
void* releaseCache[2048];
int releaseCount = 0;

void AllocMemory()
{
	srand(88);

	for (int i = 0; i < AllocLoop; i++)
	{
		for (int j = 0; j < SmallAllocCount; j++)
		{
			int size = SmallMemorySize();
			void* p = malloc(size);
			if (j % 2 == 0 && releaseCount < MaxReleaseCache)
			{
				releaseCache[releaseCount++] = p;
			}			
		}

		for (int j = 0; j < BigAllocCount; j++)
		{
			int size = BigMemorySize();
			void* p = malloc(size);
			if (j % 2 == 0 && releaseCount < MaxReleaseCache)
			{
				releaseCache[releaseCount++] = p;
			}
		}

		for (int j = 0; j < releaseCount; j++)
		{
			free(releaseCache[j]);
		}
		releaseCount = 0;
	}
}




//int _tmain(int argc, _TCHAR* argv[])
//{
//	DWORD t0 = GetTickCount();
//	AllocMemory();
//	DWORD t = GetTickCount() - t0;
//
//	MemoryData data;
//	GetMemoryData(data);
//
//	printf("time\t%u\n", t);
//	printf("free\t%uMB\n", MB(data.m_freeMem));
//	printf("reserved\t%uMB\n", MB(data.m_reservedMem));
//	printf("commited\t%uMB\n", MB(data.m_commitedMem));
//	printf("biggest block\t%uMB\n", MB(data.m_biggestFreeBlock));
//	printf("fragmention\t%.2f", (float)data.m_biggestFreeBlock / (float)data.m_freeMem);
//
//	getchar();
//
//	return 0;
//}

