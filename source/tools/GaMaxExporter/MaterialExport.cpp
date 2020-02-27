#include "3dsmaxsdk_preinclude.h"
#include "MaxExporter.h"
#include <string>
#include <vector>
using namespace std;



//////////////////////////////////////////////////////////////////////////

/*

void IGameExporter::DumpMaterial(CComPtr<IXMLDOMNode> node,IGameMaterial * mat, int index, int matID )
{


CreateXMLNode(pXMLDoc,node,_T("Material"),&material);
buf.printf("%d",index);
AddXMLAttribute(material,_T("Index"),buf.data());


if(matID !=-1)
{
// we are a sub material
buf.printf("%d",matID);
AddXMLAttribute(material,_T("MaterialID"),buf.data());
}

AddXMLAttribute(material,_T("Name"), FixupName (mat->GetMaterialName()));

AddXMLAttribute (material, _T("Class"), FixupName (mat->GetMaterialClass()));

//WE ONLY WANT THE PROPERTIES OF THE ACTUAL MATERIAL NOT THE CONTAINER - FOR NOW.....
if(!mat->IsMultiType())
{

}

//do the textures if they are there

DumpTexture(material,mat);

if(mat->IsMultiType())
{
CComPtr <IXMLDOMNode> multi;
CreateXMLNode(pXMLDoc,material,_T("SubMaterials"),&multi);
buf.printf("%d",mat->GetSubMaterialCount());
AddXMLAttribute(multi,_T("Count"),buf.data());

for(int k=0;k<mat->GetSubMaterialCount();k++)
{
int mID = mat->GetMaterialID(k);
IGameMaterial * subMat = mat->GetSubMaterial(k);
DumpMaterial(multi,subMat,k, mID);
}
}
}
*/


// sepcial function for normal bump texture map
const TCHAR* GetNormalBumpTexName(IGameTextureMap* pGameTexMap)
{
	Texmap* pTexMap = pGameTexMap->GetMaxTexmap();

	if (pTexMap->ClassID() != Class_ID(BMTEX_CLASS_ID, 0x00))
	{
		pTexMap = pTexMap->GetSubTexmap(0);
	}

	return ((BitmapTex *)pTexMap)->GetMapName();
}



void ExportTextureMaps(IGameMaterial* pMat, TiXmlElement* pXmlTextures)
{
	static TCHAR* mapSlotNames[] = {
		_T("Ambient"),
		_T("Diffuse"),
		_T("Specular"),
		_T("Shininess"),
		_T("ShininessStrength"),
		_T("SelfIllumination"),
		_T("Opacity"),
		_T("FilterColor"),
		_T("Bump"),
		_T("Reflection"),
		_T("Refraction"),
		_T("Displacement"),
	};


	//#define ID_AM 0   //!< ambient
	//#define ID_DI 1   //!< diffuse
	//#define ID_SP 2   //!< specular
	//#define ID_SH 3   //! shininesNs
	//#define ID_SS 4   //!< shininess strength
	//#define ID_SI 5   //!< self-illumination
	//#define ID_OP 6   //!< opacity
	//#define ID_FI 7   //!< filter color
	//#define ID_BU 8   //!< bump 
	//#define ID_RL 9   //!< reflection
	//#define ID_RR 10  //!< refraction 
	//#define ID_DP 11  //!< displacement

	//int texCount = pMat->GetNumberOfTextureMaps();


	//StdMat* pMaxMat = (StdMat*)(pMat->GetMaxMaterial());


	//for(int i = 0;i < texCount; i++)
	//{
	//	IGameTextureMap * tex = pMat->GetIGameTextureMap(i);
	//	int mapSlot = tex->GetStdMapSlot();
	//	if (mapSlot == -1)
	//	{
	//		MessageBoxA(NULL, "只支持标准贴图", "", MB_OK);
	//		continue;
	//	}

	//	TiXmlElement* pXmlTex = LinkNewXmlElement(pXmlTextures, mapSlotNames[mapSlot]);


	//	float amount = 1.0f;
	//	amount = pMaxMat->GetTexmapAmt(mapSlot, 0);
	//	char buf[128];
	//	sprintf_s(buf, sizeof(buf), "%.2f", amount);


	//	pXmlTex->SetAttribute("Amount", buf);
	//	pXmlTex->SetAttribute("Channel", tex->GetMapChannel() - 1);

	//	const char* fileName = tex->GetBitmapFileName();
	//	// special case for normal bump map
	//	if (mapSlot == ID_BU)
	//	{
	//		fileName = GetNormalBumpTexName(tex);
	//	}
	//	fileName = FixFileName(fileName);

	//	LinkNewXmlText(pXmlTex, fileName);
	//}
}




void ExportMaterial(IGameMaterial* pMat, TiXmlElement* pXmlParent)
{
	Mtl* pMaxMat = pMat->GetMaxMaterial();

	if (pMat->IsMultiType())
	{
		MessageBoxA(NULL, "不支持子材质", "", MB_OK);
		return;
	}

	if (pMaxMat->ClassID() != Class_ID(DMTL_CLASS_ID, 0))
	{
		MessageBoxA(NULL, "只支持标准材质", "", MB_OK);
		return;
	}

	IGameProperty *prop = NULL;

	prop = pMat->GetDiffuseData();
	ExportProperty(prop, LinkNewXmlElement(pXmlParent, "Diffuse"), false);

	prop = pMat->GetAmbientData();
	ExportProperty(prop, LinkNewXmlElement(pXmlParent, "Ambient"), false);

	prop = pMat->GetSpecularData();
	ExportProperty(prop, LinkNewXmlElement(pXmlParent, "Specular"), false);

	prop = pMat->GetGlossinessData();
	ExportProperty(prop, LinkNewXmlElement(pXmlParent, "Glossiness"), false);

	prop = pMat->GetSpecularLevelData();
	ExportProperty(prop, LinkNewXmlElement(pXmlParent, "SpecularLevel"), false);

	prop = pMat->GetEmissiveData();
	ExportProperty(prop, LinkNewXmlElement(pXmlParent, "Emissive"), false);

	prop = pMat->GetOpacityData();
	ExportProperty(prop, LinkNewXmlElement(pXmlParent, "Opacity"), false);

	TiXmlElement* pXmlTextures = LinkNewXmlElement(pXmlParent, "Textures");
	ExportTextureMaps(pMat, pXmlTextures);
}


