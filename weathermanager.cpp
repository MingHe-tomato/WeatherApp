#include "weathermanager.h"
#include "ui_weathermanager.h"
#include <QApplication> // 用于 qApp->setStyleSheet
#include <QPushButton>

WeatherManager::WeatherManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WeatherManager)
    , m_network(new NetworkManager(this))
{
    ui->setupUi(this);

    // 关联网络管理器的信号（成功 + 失败）
    connect(m_network, &NetworkManager::weatherDataReceived,
            this, &WeatherManager::handleWeatherData);
    connect(m_network, &NetworkManager::weatherErrorOccurred,
            this, &WeatherManager::handleWeatherError);


    connect(m_network, &NetworkManager::cityCodeFound,
            this, &WeatherManager::handleCityCodeFound);
    connect(m_network, &NetworkManager::citySearchError,
            this, &WeatherManager::showWeatherError);

    // connect(ui->searchButton,&QPushButton::clicked,this,&WeatherManager::on_searchButton_clicked);

    // // 初始化 Token（也可从配置文件、界面输入获取）
    // m_hefengToken = "KAGXV5DY5M"; // 替换为实际 Token
}

//-------------------------设置token
void WeatherManager::setHefengToken(const QString &token) {
    m_hefengToken = token; // 直接赋值，不再硬编码
}

WeatherManager::~WeatherManager()
{
    delete ui;
}

// void WeatherManager::fetchWeatherData(const QString &city) {
//     // 调用和风 API，传入城市名 + Token
//     if(m_hefengToken.isEmpty()){
//         showWeatherError("Please set your Hefeng Token first");
//         return;
//     }
//     m_network->requestWeather(city, m_hefengToken);
// }

//-------------------------处理天气数据
void WeatherManager::handleWeatherData(const QJsonObject &data) {
    // data 是和风天气返回的 now 字段内容（已过滤无效数据）
    qDebug() << "进入 handleWeatherData";
    updateCurrentWeather(data);
    qDebug()<<"天气数据："<<data;
}


//-------------------------处理天气数据的报错
void WeatherManager::handleWeatherError(const QString &errorMsg) {
    showWeatherError(errorMsg);
}


//---------------------------tempLabel的更新
void WeatherManager::updateCurrentWeather(const QJsonObject &data) {
    // 和风天气返回示例：{"temp":"25", "feelsLike":"28", "icon":"101", ...}
    qDebug()<<"tempLabel指针："<<ui->tempLabel;
    if (data.isEmpty()) {
        showWeatherError("Empty weather data");
        return;
    }

    // qDebug() << "温度值：" << temp;

    // 更新温度
    QString temp = data.value("temp").toString();
    ui->tempLabel->setText(temp + "°C");

    // 更新天气图标（和风 icon 码 -> 本地资源）
    QString iconCode = data.value("icon").toString();
    QPixmap icon(":/icons/hefeng_" + iconCode + ".png"); // 假设资源文件已映射
    if (icon.isNull()) {
        icon.load(":/icons/default.png"); // 备用图标
    }
    ui->weatherIcon->setPixmap(icon.scaled(
        ui->weatherIcon->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
}


//------------------------展示天气数据报错
void WeatherManager::showWeatherError(const QString &message) {
    ui->tempLabel->setText("Error");
    ui->weatherIcon->setPixmap(QPixmap(":/icons/error.png"));
    QMessageBox::warning(this, "Weather Error", message);
}

// -----------------以下是界面交互槽函数（保持原有逻辑） ------
void WeatherManager::on_cityComboBox_currentIndexChanged(const QString &city) {
    fetchWeatherData(city);
}

// void WeatherManager::on_searchButton_clicked() {
//     QString city = ui->cityLineEdit->text().trimmed();
//     if(city.isEmpty()){
//         QMessageBox::warning(this,"提示","请输入有效的城市名称");
//         return;
//     }
//     fetchWeatherData(city);
// }


//--------------------------切换主题
void WeatherManager::on_themeButton_clicked() {
    static bool darkMode = false;
    darkMode = !darkMode;
    if (darkMode) {
        qApp->setStyleSheet("QWidget{background: #333; color: #FFF;}");
    } else {
        qApp->setStyleSheet("");
    }
}


//------------------------搜索相应地区天气
void WeatherManager::on_searchButton_clicked() {
    qDebug() << "Button clicked!";
    QString cityName = ui->cityLineEdit->text().trimmed();
    if (cityName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入有效的城市名称");
        return;
    }
    //顺便传了一下token
    m_network->searchCityCode(cityName,m_hefengToken); // 先搜索城市编码，而非直接请求天气

}


//------------------------------查找城市编码
void WeatherManager::handleCityCodeFound(const QString &cityName, const QString &locationId) {
    qDebug() << "City code found:" << locationId; // 添加日志
    if (!locationId.isEmpty()) {
        fetchWeatherData(locationId); // 传递编码而非名称
    } else {
        showWeatherError("未能获取城市编码");
    }
}


//--------------------------------获取天气数据
void WeatherManager::fetchWeatherData(const QString &locationId) {
    qDebug() << "Token: " << m_hefengToken;
    // 参数改为 locationId
    if (m_hefengToken.isEmpty()) {
        showWeatherError("Please set your Hefeng Token first");
        return;
    }
    m_network->requestWeather(locationId, m_hefengToken); // 传递编码
}


//-----------------------------comboBox槽函数
void WeatherManager::on_cityComboBox_currentIndexChanged(int index) {
    QString city = ui->cityComboBox->itemText(index); // 通过索引取文字
    fetchWeatherData(city);
}


//语音播报功能
void WeatherManager::on_voiceButton_clicked() {
    // 语音功能待实现
}
