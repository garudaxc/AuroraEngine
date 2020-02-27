#pragma once
#include <fstream>
#include <string>
#include "Platform.h"


namespace Aurora
{

class Log
{
public:
	Log(void);
	~Log(void);

	void Initialize();
	void Finalize();

	void Error(const char* text, ...);

	void Info(const char* text, ...);

private:
	std::ofstream m_file;

	char m_TextBuffer[512];

	class ILock* m_pLock;
};

extern Log* GLog;

}