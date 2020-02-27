#include "3dsmaxsdk_preinclude.h"
#include "MaxExporter.h"
#include <string>
#include <vector>
#include <fstream>
using namespace std;


void ExportAnimation(const vector<AnimInfo>& animations)
{
	IGameScene * pIgame = GetIGameInterface();

	string filename = FilePath + SceneName + ".anim";
	ofstream fout(filename.c_str());

	fout << "NumAnimations " << animations.size() << endl;

	for (vector<AnimInfo>::const_iterator it = animations.begin(); it != animations.end(); ++it) {
		fout << it->nodeName << endl;
		IGameControl* pControl = it->control;	

		bool bRelative = true;
		IGameKeyTab keys;
		bool bRet = pControl->GetFullSampledKeys(keys, 1, IGAME_TM, bRelative);
		if (!bRet) {
			fout << "sample failed" << endl;
			continue;
		}

		fout << "NumKeys " << keys.Count() << endl;

		for (int i = 0; i < keys.Count(); i++) {
			const GMatrix& matLocal = keys[i].sampleKey.gval;
			Quat qRot = matLocal.Rotation().Inverse();
			Point3 vPos = matLocal.Translation();
			Point3 vScale = matLocal.Scaling();

			fout << "rot " << qRot.x << " " << qRot.y << " " << qRot.z << " " << qRot.w << endl;
			//fout << "translate " << vPos.x << " " << vPos.y << " " << vPos.z << endl;
		}

		return;
	}

}