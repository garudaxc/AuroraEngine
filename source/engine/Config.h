#ifndef GACONFIG_H
#define GACONFIG_H


//#define __declspec(dllexport)
#pragma warning( disable : 4251 )

namespace Aurora
{
	class Config
	{
	public:
		Config();
		~Config();

		static void			Initialize();
		static void			Finalize();
		static Config*		Get();

		const string&		GetResourcePath() const;

		const string&		GetShaderPath() const;

		const string&		GetAssetPath() const;

	};
};



#endif