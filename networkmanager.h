#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrl>
#include <QUrlQuery>

//城市编码转换
const QString CITY = "https://kv3h2pm69m.re.qweatherapi.com/geo/v2/city/lookup";
// 和风天气 API 基础 URL（免费版示例，按需替换）
const QString HEFENG_API_URL = "https://kv3h2pm69m.re.qweatherapi.com/v7/weather/now";
// const QString ONTIMEWEATHER_API_URL="https://devapi.qweather.com/v7/weather/now";
const QString SEVENDAYSWEATHER_API_URL="https://devapi.qweather.com/v7/weather/7d";

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr)
        : QObject(parent),
        m_network(new QNetworkAccessManager(this))
    {}

    //城市搜索时传入token
    void searchCityCode(const QString &cityName,const QString &token);
    // 发起天气请求（此时已接收token参数）
    void requestWeather(const QString &city, const QString &token);
signals:
    // 数据成功解析后发射（对外暴露 JSON 对象）
    void weatherDataReceived(const QJsonObject &data);
    // 网络或解析错误时发射
    void weatherErrorOccurred(const QString &errorMsg);

    void citySearchError(const QString &errorMsg);
    void cityCodeFound(const QString &cityName,const QString &locationId);
private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *m_network;
};

#endif // NETWORKMANAGER_H
