#pragma once
#include "stdheader.h"

namespace Aurora
{

#define MAGIC(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
#define MAGIC_STR(str) (str[0] << 24 | str[1] << 16 | str[2] << 8 | str[3])

	class File
	{
	public:
		explicit File(bool  Binary):mBinary(Binary)
		{
			
		}
		
		virtual size_t	Size() const = 0;
		virtual bool	Read(void* data, int size) = 0;
		virtual void	Seek(size_t pos) = 0;
		virtual void	Close() = 0;
		virtual const string& Pathname() const = 0;

		string			ReadAsString();
		
		template<class type>
		bool	Read(type &c) {
			bool r = Read(&c, sizeof(c));
			return r;
		}

		template<class type>
		bool	ReadArray(type* array, int count){
			bool r = Read(array, sizeof(type)* count);
			return r;
		}

		bool mBinary = true;
	};

	class StringBuffer
	{
	public:
		StringBuffer(const string& pathname);
		StringBuffer(File* file);
		~StringBuffer();

		char* Ptr();
		const char* Ptr() const;

	private:
		void	ReadFile(File* file);

		char*	buffer_ = nullptr;
		int32	size_ = 0;

	};

	class FileDeleteFunc
	{
	public:
		void operator () (File* p) { if (p) p->Close(); }
	};

	typedef unique_ptr<File, FileDeleteFunc>   FilePtr;


	class FileSystem
	{
	public:
		static String CombinePath(const String& InBasePath, const String& SubPath);
		
		virtual void	SetRootPath(const string& path, bool addPak) = 0;
		virtual bool	AddPakFile(const string& pathname) = 0;

		virtual File*	OpenFile(const string& Pathname, bool Binary = true) = 0;
		virtual bool	FileExist(const string& pathname) = 0;
	};


	extern FileSystem* GFileSys;

}