#include "cachemanager.h"

CacheManager::CacheManager(QObject *parent):QObject(parent) {}

CacheManager::~CacheManager() {}

// 初始化数据库
bool CacheManager::initDatabase() {
    // 设置数据库路径（应用数据目录）
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    QString dbFile = dbPath + "/weather_cache.db";

    m_db = QSqlDatabase::addDatabase("QSQLITE", "weather_cache");
    m_db.setDatabaseName(dbFile);

    if (!m_db.open()) {
        qWarning() << "Failed to open cache database:" << m_db.lastError().text();
        return false;
    }

    createTables();
    return true;
}

// 创建数据库表
void CacheManager::createTables() {
    QSqlQuery query(m_db);

    // 创建城市编码缓存表
    query.exec("CREATE TABLE IF NOT EXISTS city_code_cache ("
               "city_name TEXT PRIMARY KEY, "
               "city_code TEXT NOT NULL, "
               "timestamp INTEGER NOT NULL)");

    // 创建天气数据缓存表
    query.exec("CREATE TABLE IF NOT EXISTS weather_data_cache ("
               "city_code TEXT PRIMARY KEY, "
               "weather_data TEXT NOT NULL, "
               "timestamp INTEGER NOT NULL)");
}

// 获取缓存的天气数据
QJsonObject CacheManager::getWeatherData(const QString &cityCode) {
    QSqlQuery query(m_db);
    query.prepare("SELECT weather_data, timestamp FROM weather_data_cache "
                  "WHERE city_code = ? AND timestamp > ?");
    query.addBindValue(cityCode);
    query.addBindValue(QDateTime::currentSecsSinceEpoch() - WEATHER_CACHE_VALIDITY);

    if (query.exec() && query.next()) {
        QString jsonStr = query.value(0).toString();
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        if (doc.isObject()) {
            return doc.object();
        }
    }
    return QJsonObject(); // 返回空对象表示缓存无效
}

// 缓存天气数据
void CacheManager::cacheWeatherData(const QString &cityCode, const QJsonObject &weatherData) {
    QSqlQuery query(m_db);
    query.prepare("INSERT OR REPLACE INTO weather_data_cache "
                  "(city_code, weather_data, timestamp) "
                  "VALUES (?, ?, ?)");

    QJsonDocument doc(weatherData);
    query.addBindValue(cityCode);
    query.addBindValue(doc.toJson(QJsonDocument::Compact));
    query.addBindValue(QDateTime::currentSecsSinceEpoch());

    if (!query.exec()) {
        qWarning() << "Failed to cache weather data:" << query.lastError().text();
    }
}

// 获取缓存的城市编码
QString CacheManager::getCityCode(const QString &cityName) {
    QSqlQuery query(m_db);
    query.prepare("SELECT city_code FROM city_code_cache "
                  "WHERE city_name = ? AND timestamp > ?");
    query.addBindValue(cityName);
    query.addBindValue(QDateTime::currentSecsSinceEpoch() - CITY_CODE_CACHE_VALIDITY);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString(); // 返回空字符串表示缓存无效
}

//--------------------------- 缓存城市编码---------------------------
void CacheManager::cacheCityCode(const QString &cityName, const QString &cityCode) {
    QSqlQuery query(m_db);
    query.prepare("INSERT OR REPLACE INTO city_code_cache "
                  "(city_name, city_code, timestamp) "
                  "VALUES (?, ?, ?)");

    query.addBindValue(cityName);
    query.addBindValue(cityCode);
    query.addBindValue(QDateTime::currentSecsSinceEpoch());

    if (!query.exec()) {
        qWarning() << "Failed to cache city code:" << query.lastError().text();
    }
}



// 在CacheManager中添加定期清理方法
void CacheManager::cleanExpiredCache() {
    qint64 now = QDateTime::currentSecsSinceEpoch();

    // 清理过期天气数据
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM weather_data_cache WHERE timestamp < ?");
    query.addBindValue(now - WEATHER_CACHE_VALIDITY);
    query.exec();

    // 清理过期城市编码
    query.prepare("DELETE FROM city_code_cache WHERE timestamp < ?");
    query.addBindValue(now - CITY_CODE_CACHE_VALIDITY);
    query.exec();
}

void CacheManager::printCacheContents() {
    if (!m_db.isOpen()) {
        qDebug() << "Database not open";
        return;
    }

    qDebug() << "=== City Code Cache ===";
    QSqlQuery cityQuery(m_db);
    cityQuery.exec("SELECT * FROM city_code_cache");
    while (cityQuery.next()) {
        qDebug() << "City:" << cityQuery.value("city_name").toString()
        << "Code:" << cityQuery.value("city_code").toString()
        << "Timestamp:" << cityQuery.value("timestamp").toString();
    }

    qDebug() << "\n=== Weather Data Cache ===";
    QSqlQuery weatherQuery(m_db);
    weatherQuery.exec("SELECT * FROM weather_data_cache");
    while (weatherQuery.next()) {
        qDebug() << "City Code:" << weatherQuery.value("city_code").toString()
        << "Timestamp:" << weatherQuery.value("timestamp").toString();
    }
}
// // 在应用启动时安排定期清理
// void WeatherManager::startCacheMaintenance() {
//     // 每小时清理一次过期缓存
//     QTimer *cleanupTimer = new QTimer(this);
//     connect(cleanupTimer, &QTimer::timeout, m_cacheManager, &CacheManager::cleanExpiredCache);
//     cleanupTimer->start(3600000); // 每小时清理一次
// }
