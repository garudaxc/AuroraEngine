#pragma once


#include <QImage>
#include <QWidget>


class RayTracer;


class OutputWindow : public QWidget
{
	Q_OBJECT
		
public:
	OutputWindow(QWidget *parent = 0);
	~OutputWindow();

	void							SetTracer(RayTracer* pTracer);

protected:
	virtual void					paintEvent(QPaintEvent * event);
	virtual void					mousePressEvent(QMouseEvent *event);


private:
	RayTracer*						m_pTracer;
	QImage*							m_pImage;

};