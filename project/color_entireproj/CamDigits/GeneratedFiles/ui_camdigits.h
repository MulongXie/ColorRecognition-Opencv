/********************************************************************************
** Form generated from reading UI file 'camdigits.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMDIGITS_H
#define UI_CAMDIGITS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CamDigitsClass
{
public:
    QWidget *centralWidget;
    QPushButton *OpenBtn;
    QTextEdit *textEdit;
    QComboBox *serial;
    QWidget *canny;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CamDigitsClass)
    {
        if (CamDigitsClass->objectName().isEmpty())
            CamDigitsClass->setObjectName(QStringLiteral("CamDigitsClass"));
        CamDigitsClass->resize(1062, 690);
        centralWidget = new QWidget(CamDigitsClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        OpenBtn = new QPushButton(centralWidget);
        OpenBtn->setObjectName(QStringLiteral("OpenBtn"));
        OpenBtn->setGeometry(QRect(50, 20, 93, 28));
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(710, 70, 331, 311));
        serial = new QComboBox(centralWidget);
        serial->setObjectName(QStringLiteral("serial"));
        serial->setGeometry(QRect(270, 20, 171, 22));
        canny = new QWidget(centralWidget);
        canny->setObjectName(QStringLiteral("canny"));
        canny->setGeometry(QRect(-10, 50, 701, 531));
        canny->setMinimumSize(QSize(320, 240));
        CamDigitsClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(CamDigitsClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1062, 23));
        CamDigitsClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CamDigitsClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        CamDigitsClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(CamDigitsClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        CamDigitsClass->setStatusBar(statusBar);

        retranslateUi(CamDigitsClass);

        QMetaObject::connectSlotsByName(CamDigitsClass);
    } // setupUi

    void retranslateUi(QMainWindow *CamDigitsClass)
    {
        CamDigitsClass->setWindowTitle(QApplication::translate("CamDigitsClass", "CamDigits", 0));
        OpenBtn->setText(QApplication::translate("CamDigitsClass", "\346\211\223\345\274\200", 0));
    } // retranslateUi

};

namespace Ui {
    class CamDigitsClass: public Ui_CamDigitsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMDIGITS_H
