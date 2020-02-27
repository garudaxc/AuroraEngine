#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "stdheader.h"
#include <QtGui/QMainWindow>
#include <QTimer>
#include "qmainwindow.h"
#include "../../src/gui/widgets/qmainwindow.h"
#include "ui_raytracer.h"
#include "RayTracer.h"







class RayTracerApp : public QMainWindow
{
	Q_OBJECT

public:
	RayTracerApp(QWidget *parent = 0, Qt::WFlags flags = 0);
	~RayTracerApp();

private slots:
	void				OnClear();
	void				OnSelectColor();
	void				UndateProgressBar();



private:
	Ui::RayTracerClass ui;

	RayTracer			m_Tracer;

	QTimer*				m_pTimer;

	Ga::Color			m_ClearColor;

};

#endif // RAYTRACER_H
