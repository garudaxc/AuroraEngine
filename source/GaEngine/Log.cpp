#include "stdHeader.h"
#include "Log.h"
#include "Platform.h"

namespace Aurora
{

	static Log gLog;
	Log* GLog = &gLog;

	Log::Log(void) {
	}

	Log::~Log(void) {
	}

	void Log::Initialize()
	{
		m_file.open("GaEngine.log", std::ios_base::out | std::ios_base::trunc);
		assert(m_file);

		m_pLock = CreateThreadLock();
	}

	void Log::Finalize()
	{
		m_file.close();

		m_pLock->Release();
		m_pLock = NULL;
	}	

	void Log::Error(const char* text, ...)
	{
		LockObject lock(m_pLock);

		va_list argptr;
		va_start( argptr, text );
		vsnprintf_s(m_TextBuffer, sizeof(m_TextBuffer), 
			sizeof(m_TextBuffer) - 1, text, argptr);
		va_end( argptr );

		//sprintf_s(m_TextBuffer, sizeof(m_TextBuffer), text);
		m_file << "Error : " << m_TextBuffer << endl;
		m_file.flush();
	}


	void Log::Warning(const char* text, ...)
	{
		LockObject lock(m_pLock);

		va_list argptr;
		va_start(argptr, text);
		vsnprintf_s(m_TextBuffer, sizeof(m_TextBuffer),
			sizeof(m_TextBuffer) - 1, text, argptr);
		va_end(argptr);

		//sprintf_s(m_TextBuffer, sizeof(m_TextBuffer), text);
		m_file << "Warning : " << m_TextBuffer << endl;
		m_file.flush();
	}


	void Log::Info(const char* text, ...)
	{
		LockObject lock(m_pLock);

		va_list argptr;
		va_start( argptr, text );
		vsnprintf_s(m_TextBuffer, sizeof(m_TextBuffer), 
			sizeof(m_TextBuffer) - 1, text, argptr);
		va_end( argptr );

		m_file << "Info : " << m_TextBuffer << endl;
		m_file.flush();
	}

}