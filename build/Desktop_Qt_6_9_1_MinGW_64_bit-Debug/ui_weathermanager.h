/********************************************************************************
** Form generated from reading UI file 'weathermanager.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WEATHERMANAGER_H
#define UI_WEATHERMANAGER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WeatherManager
{
public:

    void setupUi(QWidget *WeatherManager)
    {
        if (WeatherManager->objectName().isEmpty())
            WeatherManager->setObjectName("WeatherManager");
        WeatherManager->resize(800, 600);

        retranslateUi(WeatherManager);

        QMetaObject::connectSlotsByName(WeatherManager);
    } // setupUi

    void retranslateUi(QWidget *WeatherManager)
    {
        WeatherManager->setWindowTitle(QCoreApplication::translate("WeatherManager", "WeatherManager", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WeatherManager: public Ui_WeatherManager {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WEATHERMANAGER_H
