#ifndef WEATHERMANAGER_H
#define WEATHERMANAGER_H

#include "networkmanager.h"
#include "cachemanager.h"

#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QMessageBox>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlContext>

QT_BEGIN_NAMESPACE
namespace Ui { class WeatherManager; }
QT_END_NAMESPACE

class WeatherManager : public QWidget
{
    Q_OBJECT

public:
    WeatherManager(QWidget *parent = nullptr);
    ~WeatherManager();

    // 新增：设置和风天气 Token（从外部传入更灵活）
    void setHefengToken(const QString &token);


signals:
    void cityCodeFound(const QString &cityName,const QString &locationId);



private slots:
    //void on_cityComboBox_currentIndexChanged(const QString &city);
    void on_searchButton_clicked();
    void on_themeButton_clicked();
    void on_voiceButton_clicked();
    void on_cityComboBox_currentIndexChanged(int index);
    void on_testCacheButton_clicked();

    // 处理网络管理器的成功信号
    void handleWeatherData(const QJsonObject &data);
    // 处理网络或 API 错误
    void handleWeatherError(const QString &errorMsg);

    void handleCityCodeFound(const QString &cityName, const QString &locationId);
private:
    Ui::WeatherManager *ui;
    NetworkManager *m_network;   // 自定义网络管理器
    QString m_hefengToken ;       // 和风天气 Token
    CacheManager *m_cacheManager; // 新增缓存管理器
    QString m_currentLocationId;
    QLabel *cacheStatusLabel;

    QQuickWidget *weatherIconWidget;
    QObject *m_weatherIconItem; // QML根对象指针

    void fetchWeatherData(const QString &city);
    void updateCurrentWeather(const QJsonObject &data);
    void showWeatherError(const QString &message);
};


#endif // WEATHERMANAGER_H
