#include "weathermanager.h"

#include <QApplication>
#include <QHostInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置自定义DNS解析器
    QHostInfo::lookupHost("geoapi.qweather.com", [](const QHostInfo &host) {
        if (host.error() != QHostInfo::NoError) {
            qDebug() << "DNS lookup error:" << host.errorString();
        } else {
            qDebug() << "Resolved IP addresses:";
            for (const QHostAddress &address : host.addresses()) {
                qDebug() << address.toString();
            }
        }
    });

    WeatherManager w;
    w.setHefengToken("f8a3fcb9fb9241c7a8b0f8ef34bfed71");
    w.show();

    return a.exec();
}
