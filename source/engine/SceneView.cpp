#include "stdHeader.h"
#include "SceneView.h"
#include "Entity.h"

namespace Aurora
{
	
	//////////////////////////////////////////////////////////////////////////

	const int FRAME_BUFFER_SIZE = 1024 * 1024;

	FrameMemoryBuffer gFrameMemoryBuffer;
	FrameMemoryBuffer* GFrameMemoryBuffer = &gFrameMemoryBuffer;

	FrameMemoryBuffer::FrameMemoryBuffer()
	{
		m_pBufferA = new char[FRAME_BUFFER_SIZE];
		m_pBufferB = new char[FRAME_BUFFER_SIZE];

		m_pBuffer = m_pBufferA;
		m_pPos = m_pBuffer;
	}

	FrameMemoryBuffer::~FrameMemoryBuffer()
	{
		delete []m_pBufferA;
		delete []m_pBufferB;
		m_pBuffer = nullptr;
		m_pPos = nullptr;
	}

	void* FrameMemoryBuffer::Alloc(size_t size)
	{
		assert(m_pPos + size < m_pBuffer + FRAME_BUFFER_SIZE);
		void* pMem = (void*)m_pPos;
		m_pPos += size;
		return pMem;
	}

	SceneView* FrameMemoryBuffer::AllocSceneView()
	{
		return (SceneView*)Alloc(sizeof(SceneView));
	}

	DrawingEntity* FrameMemoryBuffer::AllocDrawingEntity()
	{
		return (DrawingEntity*)Alloc(sizeof(DrawingEntity));
	}

	DrawingLight* FrameMemoryBuffer::AllocDrawingLight()
	{
		return (DrawingLight*)Alloc(sizeof(DrawingLight));
	}

	// clear once a frame
	void FrameMemoryBuffer::Clear()
	{
		m_pPos = m_pBuffer;
	}

	void FrameMemoryBuffer::ClearAndSwap()
	{
		m_pBuffer = (m_pBuffer == m_pBufferA) ? m_pBufferB : m_pBufferA;
		m_pPos = m_pBuffer;
	}


}