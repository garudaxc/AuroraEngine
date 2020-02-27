#include "Max.h"
#include "resource.h"
#include "tinyxml.h"

#include <string>
using namespace std;

//MAKEWPARAM


TiXmlElement* LinkNewXmlElement(TiXmlElement* pParent, const char* name);
TiXmlText* LinkNewXmlText(TiXmlElement* pParent, const char* text);





struct ExportOptions
{
	int			id;
	int			state;
	const char*	name;
	HWND		hWnd;
};


static ExportOptions gOptions[] = 
{
	{IDC_RADIO_ALL, 1, "ExportAll", 0},
	{IDC_RADIO_SELECT, 0, "ExportSelect", 0},
};



int GetNumOptions()
{
	return sizeof(gOptions) / sizeof(gOptions[0]);
}


int GetOption(int id)
{
	for (int i = 0; i < GetNumOptions(); i++)
	{
		if (id == gOptions[i].id)
		{
			return gOptions[i].state;
		}
	}
	return 0;
}




basic_string<MCHAR> GetConfigPathName()
{
	return basic_string<MCHAR>(GetCOREInterface()->GetDir(APP_PLUGCFG_DIR)) + "\\SceneAnimConfig.xml";
}



void SaveConfig()
{
	TiXmlDocument doc;

	TiXmlElement* pXmlConfig = (TiXmlElement*)doc.LinkEndChild(new TiXmlElement("Config"));

	for (int i = 0; i < GetNumOptions(); i++)
	{
		LinkNewXmlText(LinkNewXmlElement(pXmlConfig, gOptions[i].name), gOptions[i].state ? "1" : "0");
	}

	doc.SaveFile(GetConfigPathName().c_str());
}


void LoadConfig()
{
	TiXmlDocument doc;
	if (!doc.LoadFile(GetConfigPathName().c_str()))
	{
		return;
	}

	TiXmlElement* pConfig = doc.FirstChildElement("Config");

	TiXmlNode* pNode = pConfig->FirstChild();
	int i = 0;
	while (pNode)
	{
		if (pNode->ToElement() && strcmp(pNode->ToElement()->Value(), gOptions[i].name) == 0)
		{
			if (strcmp(pNode->FirstChild()->ToText()->Value(), "1") == 0)
			{
				gOptions[i].state = 1;
			}
			else
			{
				gOptions[i].state = 0;
			}
		}

		i++;
		pNode = pNode->NextSibling();
	}
}




void InitDlgItemState(HWND hWnd)
{
	for (int i = 0; i < GetNumOptions(); i++)
	{
		gOptions[i].hWnd = GetDlgItem(hWnd, gOptions[i].id);
		SendMessage(gOptions[i].hWnd, (UINT)BM_SETCHECK, gOptions[i].state ? BST_CHECKED : BST_UNCHECKED, 0);  
	}
}

void UpdateDlgItemState(HWND hWnd)
{
	for (int i = 0; i < GetNumOptions(); i++)
	{
		LRESULT res = SendMessage(gOptions[i].hWnd, (UINT)BM_GETCHECK, 0, 0);
		if (res == BST_CHECKED)
		{
			gOptions[i].state = 1;
		}
		else
		{
			gOptions[i].state = 0;
		}		
	}
}




INT_PTR CALLBACK GaMaxExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{

	switch(message) {
		case WM_INITDIALOG:
			//imp = (GaMaxExporter *)lParam;
			//GaMaxExporter::SetParamDialog(hWnd);
			CenterWindow(hWnd,GetParent(hWnd));
			LoadConfig();
			InitDlgItemState(hWnd);
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
		case WM_COMMAND:
			{
				WORD id = LOWORD(wParam);
				if (id == IDOK || id == IDCANCEL)
				{
					if (id == IDOK)
					{
						UpdateDlgItemState(hWnd);
						SaveConfig();
					}
					EndDialog(hWnd, id);
				}
			}
			break;
	}
	return 0;
}


