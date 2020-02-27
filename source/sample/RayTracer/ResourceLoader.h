#pragma once
#include "stdHeader.h"







const string& GetResourcePath();

void LoadXmlScene(const string& pathname);
//
//Vector3f ParseVec3(const string& str);
//Quaternionf ParseRotation(const string& str);
//Color ParseColor(const string& str);

class Mesh;
vector<Mesh*>& GetLoadedMeshes();