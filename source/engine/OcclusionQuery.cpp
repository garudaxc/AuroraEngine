#include "stdheader.h"
#include "OcclusionQuery.h"

namespace Aurora
{

	OcclusionQuery::OcclusionQuery():
m_nCurrentOccludedId(-1),m_nCurrentObjectId(-1)
,m_x0(0),m_y0(0),m_x1(0),m_y1(0),
m_State(State_End)//,m_pQuery(NULL)
	{

	}

	OcclusionQuery::~OcclusionQuery()
	{
		//HALReleaseOcclusionQuery(m_pQuery);		
	}

	void OcclusionQuery::Init()
	{
		//m_pQuery = HALCreateOcclusionQuery();
	}

	void OcclusionQuery::BeginQuery(int x0, int y0, int x1, int y1)
	{
		m_nCurrentOccludedId = -1;
		m_nCurrentObjectId = -1;

		m_x0 = x0;
		m_y0 = y0;
		m_x1 = x1;
		m_y1 = y1;
		m_State = State_Signaled;
	}

	int	OcclusionQuery::EndQuery()
	{
		assert(m_State == State_Issued);
		return m_nCurrentOccludedId;
	}

	void OcclusionQuery::BeginObject(int queryId)
	{
		//assert(m_State == State_Signaled || m_State == State_Building);

		m_State = State_Building;
		m_nCurrentObjectId = queryId;

		//HALSetScissorTest(true, m_x0, m_y0, m_x1, m_y1);
		//HALOcclusionQueryIssueBegin(m_pQuery);
	}

	void OcclusionQuery::EndObject()
	{
		//assert(m_State == State_Signaled || m_State == State_Building);

		m_State = State_Issued;
		//uint pixelDrawn = HALOcclusionQueryIssueEnd(m_pQuery);
		//HALSetScissorTest(false, 0, 0, 0, 0);

		//if (pixelDrawn > 0)
		//{
		//	m_nCurrentOccludedId = m_nCurrentObjectId;
		//}
	}

}