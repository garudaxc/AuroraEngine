#pragma once

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
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "IGame/IGame.h"
#include "IGame/IGameModifier.h"
#include "tinyxml.h"
#include <map>
#include <string>

using namespace std;

//SIMPLE TYPE


typedef unsigned short ushort;
typedef unsigned int uint;


struct AnimInfo
{
	string		nodeName;
	IGameControl*	control;
};

extern string FilePath;
extern string SceneName;



extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;


// transform max's time to our need (to real time, to frame number, etc.)
float TransformTime(float time);
bool TestKeyFrameExport(int time);

TiXmlElement* LinkNewXmlElement(TiXmlElement* pParent, const char* name);
TiXmlText* LinkNewXmlText(TiXmlElement* pParent, const char* text);

const char* ValidateName(const MCHAR* pName);

void ExportProperty(IGameProperty * prop, TiXmlElement* pParentElem, bool bExportAnim);
