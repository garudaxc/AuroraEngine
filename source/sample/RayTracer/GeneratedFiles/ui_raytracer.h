/********************************************************************************
** Form generated from reading UI file 'raytracer.ui'
**
** Created: Sun Aug 15 14:29:08 2010
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAYTRACER_H
#define UI_RAYTRACER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QProgressBar>
#include <QtGui/QSpinBox>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include "outputwindow.h"

QT_BEGIN_NAMESPACE

class Ui_RayTracerClass
{
public:
    QAction *actionClear;
    QAction *actionSelectColor;
    QWidget *centralWidget;
    OutputWindow *mainWindow;
    QProgressBar *progressBar;
    QLineEdit *timeEdit;
    QLabel *label;
    QLineEdit *lineEditNumSample;
    QLabel *label_2;
    QSpinBox *spinBoxThread;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *RayTracerClass)
    {
        if (RayTracerClass->objectName().isEmpty())
            RayTracerClass->setObjectName(QString::fromUtf8("RayTracerClass"));
        RayTracerClass->resize(783, 547);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RayTracerClass->sizePolicy().hasHeightForWidth());
        RayTracerClass->setSizePolicy(sizePolicy);
        actionClear = new QAction(RayTracerClass);
        actionClear->setObjectName(QString::fromUtf8("actionClear"));
        actionSelectColor = new QAction(RayTracerClass);
        actionSelectColor->setObjectName(QString::fromUtf8("actionSelectColor"));
        centralWidget = new QWidget(RayTracerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        mainWindow = new OutputWindow(centralWidget);
        mainWindow->setObjectName(QString::fromUtf8("mainWindow"));
        mainWindow->setGeometry(QRect(0, 0, 640, 480));
        mainWindow->setMaximumSize(QSize(640, 480));
        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(10, 490, 531, 21));
        progressBar->setValue(0);
        timeEdit = new QLineEdit(centralWidget);
        timeEdit->setObjectName(QString::fromUtf8("timeEdit"));
        timeEdit->setEnabled(true);
        timeEdit->setGeometry(QRect(560, 490, 51, 20));
        timeEdit->setFrame(false);
        timeEdit->setReadOnly(true);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(650, 50, 61, 20));
        lineEditNumSample = new QLineEdit(centralWidget);
        lineEditNumSample->setObjectName(QString::fromUtf8("lineEditNumSample"));
        lineEditNumSample->setGeometry(QRect(730, 50, 41, 20));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(650, 20, 54, 21));
        spinBoxThread = new QSpinBox(centralWidget);
        spinBoxThread->setObjectName(QString::fromUtf8("spinBoxThread"));
        spinBoxThread->setGeometry(QRect(730, 20, 42, 22));
        spinBoxThread->setReadOnly(false);
        spinBoxThread->setMinimum(1);
        spinBoxThread->setMaximum(4);
        spinBoxThread->setValue(2);
        RayTracerClass->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(RayTracerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        RayTracerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        mainToolBar->addAction(actionSelectColor);
        mainToolBar->addAction(actionClear);

        retranslateUi(RayTracerClass);
        QObject::connect(actionClear, SIGNAL(triggered()), RayTracerClass, SLOT(OnClear()));
        QObject::connect(actionSelectColor, SIGNAL(triggered()), RayTracerClass, SLOT(OnSelectColor()));

        QMetaObject::connectSlotsByName(RayTracerClass);
    } // setupUi

    void retranslateUi(QMainWindow *RayTracerClass)
    {
        RayTracerClass->setWindowTitle(QApplication::translate("RayTracerClass", "RayTracer", 0, QApplication::UnicodeUTF8));
        actionClear->setText(QApplication::translate("RayTracerClass", "Clear", 0, QApplication::UnicodeUTF8));
        actionSelectColor->setText(QApplication::translate("RayTracerClass", "SelectColor", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RayTracerClass", "Num Sample", 0, QApplication::UnicodeUTF8));
        lineEditNumSample->setInputMask(QApplication::translate("RayTracerClass", "D0; ", 0, QApplication::UnicodeUTF8));
        lineEditNumSample->setText(QApplication::translate("RayTracerClass", "8", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("RayTracerClass", "Thread", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RayTracerClass: public Ui_RayTracerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAYTRACER_H
