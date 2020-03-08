#pragma once
#include "Renderer.h"

namespace Aurora
{

	class OcclusionQuery
	{
	public:
		enum State
		{
			State_Signaled,
			State_Building,
			State_Issued,
			State_End,
		};

		OcclusionQuery();
		~OcclusionQuery();

		void				Init();

		void				BeginQuery(int x0, int y0, int x1, int y1);
		int					EndQuery();

		void				BeginObject(int queryId);
		void				EndObject();

	private:
		int					m_nCurrentOccludedId;
		int					m_nCurrentObjectId;

		int					m_x0, m_y0, m_x1, m_y1;
		State				m_State;

		//HALOcclusionQuery	m_pQuery;

	};
}