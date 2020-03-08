#ifndef GASKELETON_H
#define GASKELETON_H



namespace Aurora
{

	//class Joint
	//{
	//public:
	//	string		name;
	//	int			parent;
	//	Matrix4f	invTransfrom;
	//};

	//class JointPose
	//{
	//public:
	//	Quaternionf		qRot;
	//	Vector3f		vTrans;
	//	float			fScale;
	//};


	//class Skeleton
	//{
	//public:


	//private:
	//	vector<Joint>	m_Joints;

	//};



	class Skeleton
	{
	public:
		class Bone
		{
		public:
			friend Skeleton;
			Bone();
			~Bone();

			void			SetParent(Bone* pParent) { m_pParent = pParent; }
			void			AddSbiling(Bone* pSibling);
			void			AddChild(Bone* pChild);

			int				GetCount() const;
			void			UpdateTransformation();

			int				GetParentIndex() const		{	return m_nParentIndex;	}

			float			m_fScaleLocal;
			Vector3f		m_vTransLocal;
			Quaternionf		m_qRotationLocal;


			float			m_fScaleDerived;
			Vector3f		m_vTransDerived;
			Quaternionf		m_qRotationDerived;

			Matrix4f		m_matOffset;

		private:
			Bone*			m_pParent;
			Bone*			m_pSibling;
			Bone*			m_pFirstChild;

			int			m_nParentIndex;
		};


		Skeleton(void);
		~Skeleton(void);

		int							GetNumBones() const { return (int)m_aBones.size(); }

		const std::string&			GetBoneName(int nBone) const { return m_aBoneNames[nBone]; }
		void						SetBoneName(int nBone, const std::string& name) { m_aBoneNames[nBone] = name; }

		Bone*						GetBone(int nBone) { return &m_aBones[nBone]; }

		bool						SetupHierarchy(int nBone, int* pBoneParent);
		void						UpdateTransformation() { m_pRootBone->UpdateTransformation(); }
		void						GetTransformations(Matrix4f* pMatrix);

	private:
		void						Allocate(int nBones);

		Bone*						m_pRootBone;
		std::vector<Bone>			m_aBones;
		std::vector<std::string>	m_aBoneNames;

	};





}// end of namespace

#endif