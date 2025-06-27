#ifndef WEATHERMANAGER_H
#define WEATHERMANAGER_H

#include "networkmanager.h"
#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QMessageBox>

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
    void on_cityComboBox_currentIndexChanged(const QString &city);
    void on_searchButton_clicked();
    void on_themeButton_clicked();
    void on_voiceButton_clicked();
    void on_cityComboBox_currentIndexChanged(int index);

    // 处理网络管理器的成功信号
    void handleWeatherData(const QJsonObject &data);
    // 处理网络或 API 错误
    void handleWeatherError(const QString &errorMsg);

    void handleCityCodeFound(const QString &cityName, const QString &locationId);
private:
    Ui::WeatherManager *ui;
    NetworkManager *m_network;   // 自定义网络管理器
    QString m_hefengToken ;       // 和风天气 Token

    void fetchWeatherData(const QString &city);
    void updateCurrentWeather(const QJsonObject &data);
    void showWeatherError(const QString &message);
};

#endif // WEATHERMANAGER_H
