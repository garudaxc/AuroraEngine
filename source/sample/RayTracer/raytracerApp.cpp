#include "raytracerApp.h"
#include <QColorDialog>
#include <QStatusBar>
#include <QMessageBox>

RayTracerApp::RayTracerApp(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	QSize size = ui.mainWindow->size();
	m_Tracer.Init(size.width(), size.height());

	ui.mainWindow->SetTracer(&m_Tracer);

	m_ClearColor.Set(0.2f, 0.25f, 0.2f, 1.0f);

	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(UndateProgressBar()));

	/*int a = qRegisterMetaType<RayTracer>("RayTracer");
	bool b = QObject::connect(&m_Tracer, SIGNAL(ReportProgress(int)), ui.progressBar, SLOT(setValue(int)));*/
	//assert(b);
	//statusBar()->ShowMessage("Ready");

}

RayTracerApp::~RayTracerApp()
{
	if (m_pTimer)
	{
		delete m_pTimer;
		m_pTimer = NULL;
	}
}

void RayTracerApp::OnClear()
{
	m_Tracer.Clear(m_ClearColor);
	ui.timeEdit->setText("");

	int numThread = ui.spinBoxThread->value();
	
	bool toIntOk = true;
	int sampleSqrt = ui.lineEditNumSample->text().toInt(&toIntOk);
	assert(toIntOk);

	m_Tracer.Run(numThread, sampleSqrt);

	m_pTimer->start(500);


	//QMessageBox msgBox;
	//msgBox.setText(buff);
	//msgBox.exec();
}


void RayTracerApp::OnSelectColor()
{
	QColorDialog dlg(this);

	//dlg.setCurrentColor(m_ClearColor);

	dlg.exec();
	//dlg.open();
	//m_ClearColor = dlg.selectedColor();
}


void RayTracerApp::UndateProgressBar()
{
	if (m_Tracer.IsRunning())
	{
		ui.progressBar->setValue(m_Tracer.GetProgress());
	}
	else
	{
		m_pTimer->stop();

		ui.progressBar->setValue(0);
		unsigned int ellapsed = m_Tracer.GetTimeUsed();
		float time = ellapsed / 1000.0f;
		char buff[64];
		sprintf_s(buff, sizeof(buff), "%.1fs", time);
		ui.timeEdit->setText(buff);

		ui.mainWindow->update();
	}
}