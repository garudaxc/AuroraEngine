#include <QtGui>
#include <QMouseEvent>
#include "stdheader.h"
#include "GaColor.h"
#include "OutputWindow.h"
#include "RayTracer.h"

OutputWindow::OutputWindow(QWidget *parent)
: QWidget(parent),m_pTracer(NULL),m_pImage(NULL)
{
}

OutputWindow::~OutputWindow()
{
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

void OutputWindow::SetTracer(RayTracer* pTracer)
{
	m_pTracer = pTracer;
	m_pImage = new QImage(width(), height(), QImage::Format_RGB32);

}


void OutputWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() ==  Qt::LeftButton)
	{
		//m_pTracer->TracePixel(event->x(), event->y());
		m_pTracer->TracePixelKDTree(event->x(), event->y());
	}

}

void OutputWindow::paintEvent (QPaintEvent * event)
{
	for (int h = 0; h < height(); h++)
	{
		for (int w = 0; w < width(); w++)
		{
			Ga::Color c = m_pTracer->GetPixelColor(w, h);
			c *= 255.0f;

			QRgb value = qRgb((int)c.r, (int)c.g, (int)c.b);
			m_pImage->setPixel(w, h, value);
		}
	}

	QPainter painter(this);
	painter.drawImage(QPoint(0, 0), *m_pImage);
}