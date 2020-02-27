#include "raytracerApp.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RayTracerApp w;
	w.show();
	return a.exec();
}
