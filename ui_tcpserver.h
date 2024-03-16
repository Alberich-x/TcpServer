/********************************************************************************
** Form generated from reading UI file 'tcpserver.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPSERVER_H
#define UI_TCPSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_tcpServer
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QTextBrowser *textBrowser;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *tcpServer)
    {
        if (tcpServer->objectName().isEmpty())
            tcpServer->setObjectName(QString::fromUtf8("tcpServer"));
        tcpServer->resize(373, 381);
        centralwidget = new QWidget(tcpServer);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 101, 21));
        textBrowser = new QTextBrowser(centralwidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(20, 40, 331, 281));
        textBrowser->setFrameShape(QFrame::Box);
        tcpServer->setCentralWidget(centralwidget);
        menubar = new QMenuBar(tcpServer);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 373, 23));
        tcpServer->setMenuBar(menubar);
        statusbar = new QStatusBar(tcpServer);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        tcpServer->setStatusBar(statusbar);

        retranslateUi(tcpServer);

        QMetaObject::connectSlotsByName(tcpServer);
    } // setupUi

    void retranslateUi(QMainWindow *tcpServer)
    {
        tcpServer->setWindowTitle(QCoreApplication::translate("tcpServer", "tcpServer", nullptr));
        label->setText(QCoreApplication::translate("tcpServer", "\345\267\262\350\277\236\346\216\245ip:", nullptr));
        textBrowser->setHtml(QCoreApplication::translate("tcpServer", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class tcpServer: public Ui_tcpServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPSERVER_H
