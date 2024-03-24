#include "Platform.h"
#include "FileSystem.h"
#include "zip/unzip.h"
#include "Log.h"
#include <dirent.h>


namespace Aurora
{

	string File::ReadAsString()
	{
		if (Size() == 0 || mBinary) {
			return string();
		}

		string s(Size() + 1, '\0');
		Read(&s[0], Size());

		return s;
	}

	class FileLocal : public File
	{
	public:
		FileLocal(FILE* pf, const string& pathname, bool Binary);
		virtual ~FileLocal();

		size_t	Size() const override;
		bool	Read(void* data, int size) override;
		void	Seek(size_t pos) override;
		void	Close() override;

		const string& Pathname() const override;

	private:
		FILE*	mPtr;
		size_t	mSize;
		string	mPathName;
	};

	FileLocal::FileLocal(FILE* pf, const string& pathname, bool Binary) :File(Binary), mPtr(pf), mPathName(pathname)
	{
		fseek(pf, 0, SEEK_END);
		mSize = ftell(pf);
		fseek(pf, 0, SEEK_SET);
	}

	FileLocal::~FileLocal()
	{
		if (mPtr != nullptr){
			fclose(mPtr);
			mPtr = nullptr;
		}
	}

	const string& FileLocal::Pathname() const
	{
		return mPathName;
	}

	size_t	FileLocal::Size() const
	{
		return mSize;
	}

	bool FileLocal::Read(void* data, int size)
	{
		if (mPtr == nullptr){
			return false;
		}

		int ReadSize = fread(data, 1, size, mPtr);
		
		return true;
	}

	void FileLocal::Seek(size_t pos)
	{
		fseek(mPtr, pos, SEEK_SET);
	}

	void FileLocal::Close()
	{
		delete this;
	}

	

	class FileInMemory : public File
	{
	public:
		FileInMemory(size_t size, const string& pathname);
		virtual ~FileInMemory();

		virtual size_t	Size() const
		{
			return size_;
		}

		bool	Read(void* data, int size) override;
		void	Seek(size_t pos) override;
		void	Close() override;
		const string& Pathname() const override;

		void* GetBuffer()
		{
			return buffer_;
		}

	private:
		void*	buffer_;
		size_t	size_;
		size_t	currPos_;
		string	pathname_;
	};

	FileInMemory::FileInMemory(size_t size, const string& pathname) :
		File(true),
		size_(size),
		currPos_(0),
		pathname_(pathname)
	{
		buffer_ = malloc(size);
	}

	FileInMemory::~FileInMemory()
	{
		if (buffer_ != nullptr) {
			free(buffer_);
			buffer_ = nullptr;
		}
	}

	const string& FileInMemory::Pathname() const
	{
		return pathname_;
	}

	bool FileInMemory::Read(void* data, int size)
	{
		if (currPos_ >= size_) {
			return false;
		}

		if (currPos_ + size > size_) {
			memcpy(data, (char*)buffer_ + currPos_, size_ - currPos_);
			currPos_ = size_;
			return false;
		}
		else {
			memcpy(data, (char*)buffer_ + currPos_, size);
			currPos_ += size;
			return true;
		}
	}
	void FileInMemory::Seek(size_t pos)
	{
		if (pos < size_) {
			currPos_ = pos;
		}
	}

	void FileInMemory::Close()
	{
		delete this;
	}


//=========================================================


	StringBuffer::StringBuffer(const string& pathName)
	{
		FilePtr file(GFileSys->OpenFile(pathName, false));
		ReadFile(file.get());
	}

	StringBuffer::StringBuffer(File* file)
	{
		ReadFile(file);
	}

	StringBuffer::~StringBuffer()
	{
		delete []buffer_;
	}

	void StringBuffer::ReadFile(File* file)
	{
		if (file == nullptr) {
			return;
		}

		size_ = (int32)file->Size() + 1;
		buffer_ = new char[size_];
		file->Read(buffer_, size_ - 1);
		buffer_[size_ - 1] = '\0';
	}
	
	String FileSystem::CombinePath(const String& InBasePath, const String& SubPath)
	{
		int Position = (int)InBasePath.find_last_not_of("\\/");
		return  InBasePath.substr(0, Position + 1) + '/' + SubPath;		
	}

	char* StringBuffer::Ptr()
	{
		return buffer_;
	}

	const char* StringBuffer::Ptr() const
	{
		return buffer_;
	}
	
	//========================================



	class FileSystemLocal : public FileSystem
	{
	public:
		FileSystemLocal();
		~FileSystemLocal();

		virtual void SetRootPath(const string& path, bool addPak);

		virtual bool AddPakFile(const string& pathname);

		virtual File* OpenFile(const string& Pathname, bool Binary = true);

		virtual bool FileExist(const string& pathname);

	private:
		void	NormalizeName(string& fullname, string& name, const string& _rawPath);

		string	rootPath_;
		vector<unzFile>	paks_;
	};

	FileSystemLocal::FileSystemLocal()
	{
	}

	FileSystemLocal::~FileSystemLocal()
	{
		for (auto it = paks_.begin(); it != paks_.end(); ++it) {
			if (unzClose(*it) != UNZ_OK) {
				GLog->Error("close zip file failed, may files not closed");
			}
		}
	}

	void FileSystemLocal::SetRootPath(const string& path, bool addPak)
	{
		rootPath_ = path;
		for (size_t i = 0; i < rootPath_.size(); i++) {
			if (rootPath_[i] == '\\') {
				rootPath_[i] = '/';
			}
		}

		if (!rootPath_.empty() && rootPath_[rootPath_.size() - 1] != '/') {
			rootPath_ += '/';
		}

		if (addPak) {
			DIR* dir = opendir(path.c_str());
			if (dir == nullptr) {
				GLog->Error("FileSystem open dir failed! %s", path.c_str());
				return;
			}

			while (true) {
				dirent* entry = readdir(dir);
				if (entry == nullptr) {
					break;
				}

				if (strstr(entry->d_name, ".zip") != nullptr ||
					strstr(entry->d_name, ".pak") != nullptr) {
					AddPakFile(entry->d_name);
				}
			}

			closedir(dir);
		}
	}

	bool FileSystemLocal::AddPakFile(const string& pathname)
	{
		unzFile pakfile = unzOpen(pathname.c_str());

		if (pakfile == nullptr) {
			string name = rootPath_ + pathname;
			pakfile = unzOpen(name.c_str());
		}

		if (pakfile == nullptr) {
			GLog->Error("Open pak file %s failed!", pathname.c_str());
			return false;
		}

		paks_.push_back(pakfile);
		GLog->Info("FileSystem add pak %s", pathname.c_str());

		return true;
	}

	void FileSystemLocal::NormalizeName(string& fullname, string& name, const string& _rawPath)
	{
		string rawPath = _rawPath;
		for (size_t i = 0; i < rawPath.size(); i++) {
			if (rawPath[i] == '\\') {
				rawPath[i] = '/';
			}
		}

		if (rawPath.find(rootPath_) == 0) {
			fullname = rawPath;
			name = fullname.substr(rootPath_.length());
		} else {
			fullname = rootPath_ + rawPath;
			name = rawPath;
		}
	}

	File* FileSystemLocal::OpenFile(const string& Pathname, bool Binary)
	{
		string name, fullname;
		NormalizeName(fullname, name, Pathname);

		FILE* pf = fopen(fullname.c_str(), Binary ? "rb" : "r");
		if (pf != nullptr){
			File* file = new FileLocal(pf, Pathname, Binary);
			return file;
		}

		// search pak
		unzFile pakFile = nullptr;
		for (auto it : paks_) {
			if (unzLocateFile(it, name.c_str(), 0) == UNZ_OK) {
				pakFile = it;
				break;
			}
		}

		if (pakFile != nullptr) {
			unz_file_info fileInfo;
			char filename[256];
			if (unzGetCurrentFileInfo(pakFile, &fileInfo,
				filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK) {
				GLog->Error("get %s file info in pak error!", Pathname.c_str());
				return nullptr;
			}

			if (unzOpenCurrentFile(pakFile) != UNZ_OK) {
				GLog->Error("open file %s in pak error!", Pathname.c_str());
				return nullptr;
			}

			FileInMemory* file = new FileInMemory(fileInfo.uncompressed_size, Pathname);
			int r = unzReadCurrentFile(pakFile, file->GetBuffer(), fileInfo.uncompressed_size);
			if (r != fileInfo.uncompressed_size) {
				GLog->Error("read file %s in pak error!", Pathname.c_str());
			}

			unzCloseCurrentFile(pakFile);

			return file;
		}

		GLog->Error("FileSystem::OpenFile failed! %s", Pathname.c_str());

		return nullptr;
	}

	bool FileSystemLocal::FileExist(const string& pathname)
	{
		string name, fullname;
		NormalizeName(fullname, name, pathname);

		if (_access(fullname.c_str(), 0) != -1) {
			return true;
		}

		for (auto it : paks_) {
			if (unzLocateFile(it, name.c_str(), 0) == UNZ_OK) {
				return true;
			}
		}

		return false;
	}


	static FileSystemLocal fileSysLocal;
	FileSystem* GFileSys = &fileSysLocal;

}