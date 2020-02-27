#pragma once


class MeshBuildData
{
public:

	virtual int GetNumVertex() const = 0;
	virtual int GetNumFace() const = 0;
	virtual int GetNumElement() const = 0;
	virtual int GetNumElementFace(int nElement) const = 0;

	virtual const int* GetIndexData() const = 0;

	virtual const float* GetPositionData() const = 0;
	virtual const float* GetNormalData() const = 0;
	virtual const float* GetTexcoordData() const = 0;

	virtual const int* GetBlendIndex() const = 0;
	virtual const float* GetBlendWeight() const = 0;

	virtual void			Release() = 0;
};



class SkeletonDataInterface
{
public:
	
	virtual int GetNumJoint() const = 0;

	virtual int GetParent(int nJoint) const = 0;

	virtual const char* GetJointName(int nJoint) const = 0;

	virtual const float* GetTransform(int nJoint) const = 0;

	virtual void Release() = 0;
};


class AnimClipDataInterface
{
public:

};



namespace Aurora
{

	




}