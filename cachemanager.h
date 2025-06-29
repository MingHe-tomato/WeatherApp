#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDir>
#include <QString>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSqlError>
#include <QTimer>

// class CacheManager
// {
// public:
//     CacheManager();
// };
// 数据库管理类
class CacheManager : public QObject {
    Q_OBJECT
public:

    explicit CacheManager(QObject *parent = nullptr);
    ~CacheManager();

    // 初始化数据库
    bool initDatabase();

    // 城市编码缓存操作
    QString getCityCode(const QString &cityName);
    void cacheCityCode(const QString &cityName, const QString &cityCode);

    // 天气数据缓存操作
    QJsonObject getWeatherData(const QString &cityCode);
    void cacheWeatherData(const QString &cityCode, const QJsonObject &weatherData);

    void cleanExpiredCache();
    void startCacheMaintenance();

    //调试数据库
    void printCacheContents();

private:
    QSqlDatabase m_db;
    const int WEATHER_CACHE_VALIDITY = 1800; // 30分钟缓存有效期（秒）
    const int CITY_CODE_CACHE_VALIDITY = 86400 * 7; // 城市编码缓存7天

    // 创建数据库表
    void createTables();
};


#endif // CACHEMANAGER_H
