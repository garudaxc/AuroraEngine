//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
//				3DXI file exporter project template
//				For a more in-depth exemple of a 3DXI exporter,
//				please refer to maxsdk\samples\igame\export.
// AUTHOR:		Jean-Francois Yelle - created Mar.20.2007
//***************************************************************************/

#include "MaxExporter.h"
#include "3dsmaxsdk_preinclude.h"


#define GaMaxExporter_CLASS_ID	Class_ID(0x7e1d6233, 0xd0208cf)

class GaMaxExporter : public SceneExport 
{
	public:
		
		virtual int				ExtCount();					// Number of extensions supported
		virtual const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		virtual const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		virtual const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		virtual const TCHAR *	AuthorName();				// ASCII Author name
		virtual const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		virtual const TCHAR *	OtherMessage1();			// Other message #1
		virtual const TCHAR *	OtherMessage2();			// Other message #2
		virtual unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		virtual void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		virtual BOOL SupportsOptions(int ext, DWORD options);
		virtual int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		GaMaxExporter();
		virtual ~GaMaxExporter();


	protected:
		int GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);

private:
	
};



class AuroraMaxExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new GaMaxExporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return GaMaxExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("AuroraMaxExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetGaMaxExporterDesc() { 
	static AuroraMaxExporterClassDesc AuroraMaxExporterDesc;
	return &AuroraMaxExporterDesc; 
}

INT_PTR CALLBACK GaMaxExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
int GetOption(int id);


//--- GaMaxExporter -------------------------------------------------------
GaMaxExporter::GaMaxExporter()
{
}

GaMaxExporter::~GaMaxExporter() 
{

}

int GaMaxExporter::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *GaMaxExporter::Ext(int n)
{	
	return _T("aur");
}

const TCHAR *GaMaxExporter::LongDesc()
{
	return _T("Aurora Engine Exporter Proxy");
}
	
const TCHAR *GaMaxExporter::ShortDesc() 
{
	return _T("Aurora Engine Exporter Proxy");
}

const TCHAR *GaMaxExporter::AuthorName()
{
	return _T("Garuda");
}

const TCHAR *GaMaxExporter::CopyrightMessage() 
{	
	return _T("");
}

const TCHAR *GaMaxExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *GaMaxExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int GaMaxExporter::Version()
{				
	//#pragma message	(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 10;
}

void GaMaxExporter::ShowAbout(HWND hWnd)
{	
}

BOOL GaMaxExporter::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int GaMaxExporter::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
{
	int i;
	if (i_currentNode == NULL){
		i_currentNode = GetCOREInterface()->GetRootNode();
	}else {
		// IGame will crash 3ds Max if it is initialised with the root node.
	    i_nodeTab.AppendNode(i_currentNode);
	}
	for (i = 0; i < i_currentNode->NumberOfChildren(); i++){
		GetSceneNodes(i_nodeTab, i_currentNode->GetChildNode(i));
	}
	return i_nodeTab.Count();
}

//////////////////////////////////////////////////////////////////////////

extern "C" 
{ 
	//int Exporter( void ); 
	typedef int(*ExportProxyFn)( const TCHAR *name, ExpInterface *ei, Interface *i,BOOL suppressPrompts, DWORD options ); 
} 


int GaMaxExporter::DoExport( const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options ) 
{ 
	HMODULE hModule = ::LoadLibraryEx( "plugins/AuroraMaxExporter_d.dll", NULL, 0 ); 
	if( hModule == NULL ) { 
		hModule = ::LoadLibraryEx( "AuroraMaxExporter_d.dll", NULL, 0 ); 
		if( hModule == NULL ) { 
			MessageBox( NULL, "加载模块AuroraMaxExporter_d.dll失败.", "", 0 ); 
			return FALSE; 
		} 
	}

	ExportProxyFn pfnExporter = NULL;
	int nRet = 0;
	pfnExporter = (ExportProxyFn)GetProcAddress( hModule, "ExportProxy" ); 
	if( pfnExporter == NULL ) {
		MessageBox( NULL, "获取导出函数地址失败.", "", 0 );
		nRet = FALSE;
	} else {
		nRet = pfnExporter( name, ei, i, suppressPrompts, options ); 
	}

	::FreeLibrary( hModule ); 
	return nRet; 
}
 
