#include "stdHeader.h"
#include "Config.h"
namespace Aurora
{
	static Config* s_pConfig = nullptr;
	void Config::Initialize()
	{
		assert(s_pConfig == nullptr);
		s_pConfig = new Config();
	}

	void Config::Finalize()
	{
		assert(s_pConfig != nullptr);
		delete s_pConfig;
		s_pConfig = nullptr;
	}

	Config* Config::Get()
	{
		assert(s_pConfig != nullptr);
		return s_pConfig;
	}


	Config::Config()
	{

	}

	Config::~Config()
	{
	}

	const string& Config::GetResourcePath() const
	{
		static string resourcePath = "../res/";
		return resourcePath;		
	}

	const string& Config::GetShaderPath() const
	{
		static string shaderPath = "../dev/data/fx/";
		return shaderPath;
	}

	const string& Config::GetAssetPath() const
	{
		static string assetPath = "../dev/data/asset/";
		return assetPath;
	}

};