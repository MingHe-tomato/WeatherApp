#include "networkmanager.h"
#include "weathermanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//----------------------------------请求天气信息---------------------------------
void NetworkManager::requestWeather(const QString &city, const QString &token) { // 修正参数列表
    QUrl url(HEFENG_API_URL);
    QUrlQuery query;

     // 添加参数
    query.addQueryItem("location", city);
    query.addQueryItem("key", token);

    url.setQuery(query);

    QNetworkRequest request(url);

    qDebug() << "请求天气 URL:" << url.toString();

    QNetworkReply *reply = m_network->get(request);//发送GET请求
    
    connect(reply, &QNetworkReply::finished, this, &NetworkManager::onReplyFinished);

    qDebug() << "🌤️ 请求天气:" << city;
}


//---------------------------------------响应结束----------------------------------
void NetworkManager::onReplyFinished() {

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        emit weatherErrorOccurred("Invalid reply object");
        return;
    }

    // 后续逻辑不变，使用 reply 指针处理响应
    if (reply->error() != QNetworkReply::NoError) {
        // 网络错误
        emit weatherErrorOccurred("Network Error: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    // 读取响应数据到变量，避免重复调用 readAll()
    QByteArray data = reply->readAll();
    // qDebug() << "API Response:" << data;
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        emit weatherErrorOccurred("Invalid JSON response");
        reply->deleteLater();
        return;
    }

    QJsonObject rootObj = doc.object();
    // 和风天气返回格式：{"code":"200", "now":{"temp":"25", ...}}
    QString code = rootObj.value("code").toString();
    if (code != "200") {
        // API 返回错误（如 Token 无效、城市不存在）
        QString errorMsg = rootObj.value("reason").toString();
        emit weatherErrorOccurred("API Error (" + code + "): " + errorMsg);
        reply->deleteLater();
        return;
    }

    // 提取有效数据（now 字段是实时天气）
    QJsonObject weatherObj = rootObj.value("now").toObject();
    //确保包含必要的字段
    if (!weatherObj.contains("text") || !weatherObj.contains("icon")) {
        qWarning() << "API响应缺少必要字段";
        emit weatherErrorOccurred("API响应不完整");
        reply->deleteLater();
        return;
    }

    // 添加白天/黑夜信息
    QString iconCode = weatherObj.value("icon").toString();
    bool isDay = !iconCode.endsWith("n"); // 以"n"结尾表示夜晚

    // 创建增强数据对象
    QJsonObject enhancedData;
    enhancedData["text"] = weatherObj.value("text");
    enhancedData["icon"] = iconCode;
    enhancedData["temp"] = weatherObj.value("temp");
    enhancedData["isDay"] = isDay;

    // 添加更多需要的字段
    if (weatherObj.contains("humidity")) {
        enhancedData["humidity"] = weatherObj.value("humidity");
    }
    if (weatherObj.contains("windDir")) {
        enhancedData["windDir"] = weatherObj.value("windDir");
    }

    emit weatherDataReceived(weatherObj);
    qDebug()<<"weatherDataReceived执行";

    reply->deleteLater();
}


//---------------------------城市搜索--------------------------------------------
void NetworkManager::searchCityCode(const QString &cityName,const QString &token) {
    qDebug() << "[NETWORK] Starting city search for:" << cityName;

    QUrl url(CITY);
    QUrlQuery query;
    query.addQueryItem("location", cityName);
    query.addQueryItem("key", token); // 使用传入的 token
    // query.addQueryItem("lang", "zh-Hans");
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply *reply = m_network->get(request);

    qDebug() << "[NETWORK] Request URL:" << url.toString();

    connect(reply, &QNetworkReply::finished, this,[this, reply,cityName](){
        qDebug() << "[NETWORK] City search reply received";

        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        if (!reply){
            qDebug() << "[NETWORK] Reply is null in city search callback";
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "[NETWORK] Error:" << reply->errorString();
            emit citySearchError(reply->errorString()); // 自定义城市搜索错误信号
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        /*qDebug() << "[NETWORK]API Response:" << data;*/ // 输出天气预报API的调用结果

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) {
            emit citySearchError("Invalid city search response");
            reply->deleteLater();
            return;
        }

        QJsonObject rootObj = doc.object();
        QString code = rootObj.value("code").toString();
        if (code != "200") {
            QString errorMsg = rootObj.value("message").toString();
            qDebug() << "API Error:" << errorMsg;

            emit citySearchError("City search failed: " + errorMsg);
            reply->deleteLater();
            return;
        }

        // 解析城市编码（可能返回多个结果，取第一个）
        QJsonArray location = rootObj.value("location").toArray();
        if (location.isEmpty()) {
            emit citySearchError("No city found for \"" + cityName + "\"");
            reply->deleteLater();
            return;
        }else{
            QString locationId = location.first().toObject().value("id").toString();
            emit cityCodeFound(cityName, locationId);
        }

        QString locationId = location.first().toObject().value("id").toString();
        emit cityCodeFound(cityName, locationId); // 发射城市编码找到的信号
        reply->deleteLater();
    });
}
