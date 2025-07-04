QT += core gui widgets network sql quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += core gui network widgets

QT += qml quick quickcontrols2

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = WeatherApp
TEMPLATE = app

SOURCES += \
    cachemanager.cpp \
    main.cpp \
    networkmanager.cpp \
    weathermanager.cpp

HEADERS += \
    cachemanager.h \
    networkmanager.h \
    weathermanager.h

Resources += resources.qrc

FORMS += \
    weathermanager.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
