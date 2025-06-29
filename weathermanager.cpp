#include "weathermanager.h"
#include "ui_weathermanager.h"
#include <QApplication>
#include <QPushButton>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QFile>
#include <QQuickItem>
#include <QVBoxLayout>
#include <QTimer> // 添加定时器支持
#include <sstream>

WeatherManager::WeatherManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WeatherManager)
    , m_network(new NetworkManager(this))
    , m_cacheManager(new CacheManager(this))
    , cacheStatusLabel(nullptr)
    , weatherIconWidget(nullptr)
    , m_weatherIconItem(nullptr)
{
    ui->setupUi(this);

    // 创建QQuickWidget
    weatherIconWidget = new QQuickWidget(this);
    weatherIconWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    weatherIconWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    weatherIconWidget->setFixedSize(180, 180);




    // 关键修改：确保完全透明
    weatherIconWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    weatherIconWidget->setClearColor(Qt::transparent);
    weatherIconWidget->setStyleSheet("background: transparent; border: none;");

    // 设置透明背景属性
    QPalette palette = weatherIconWidget->palette();
    palette.setColor(QPalette::Window, Qt::transparent);
    weatherIconWidget->setPalette(palette);
    weatherIconWidget->setAutoFillBackground(false);






    // 检查QML文件是否存在
    QStringList possiblePaths = {
        ":/qml/components/WeatherIcon.qml",
        ":/qml/WeatherIcon.qml"
    };

    QString foundPath;
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            foundPath = path;
            qDebug() << "Found QML file at:" << path;
            break;
        }
    }

    // 替换原加载逻辑
    if (!foundPath.isEmpty()) {
        // 资源路径格式：qrc:/qml/WeatherIcon.qml
        QString qrcPath = foundPath.replace(":/", "qrc:/");
        weatherIconWidget->setSource(QUrl(qrcPath));
        qDebug() << "Setting QML source to:" << qrcPath;
    } else {
        qCritical() << "QML file not found in any location!";
        QLabel *errorLabel = new QLabel("QML文件缺失", this);
        ui->weatherIcon->layout()->addWidget(errorLabel);
    }

    // 添加状态变化处理
    connect(weatherIconWidget, &QQuickWidget::statusChanged, [this](QQuickWidget::Status status) {
        qDebug() << "QML Status changed:" << status;

        if (status == QQuickWidget::Ready) {
            qDebug() << "QML loaded successfully!";
            m_weatherIconItem = weatherIconWidget->rootObject();
            qDebug() << "Root object:" << (m_weatherIconItem ? "Valid" : "Invalid");

            if (m_weatherIconItem) {
                qDebug() << "QML properties:";
                qDebug() << "  iconCode:" << m_weatherIconItem->property("iconCode").toString();
                qDebug() << "  theme:" << m_weatherIconItem->property("theme").toString();
                qDebug() << "  animated:" << m_weatherIconItem->property("animated").toBool();
                qDebug() << "  isDay:" << m_weatherIconItem->property("isDay").toBool();
            }
        }
        else if (status == QQuickWidget::Error) {
            qCritical() << "QML load error:";
            for (const QQmlError &error : weatherIconWidget->errors()) {
                qCritical() << "  " << error.toString();
            }

            // 创建替代内容
            QLabel *fallback = new QLabel("QML加载失败", this);
            fallback->setAlignment(Qt::AlignCenter);
            fallback->setFixedSize(180, 180);
            ui->weatherIcon->layout()->addWidget(fallback);
        }
    });

    // 添加到UI布局
    QLayout *iconLayout = ui->weatherIcon->layout();
    if (!iconLayout) {
        iconLayout = new QVBoxLayout(ui->weatherIcon);
        iconLayout->setContentsMargins(0, 0, 0, 0);
    }
    iconLayout->addWidget(weatherIconWidget);

    // 延迟检查QML状态
    QTimer::singleShot(500, [this]() {
        if (!m_weatherIconItem && weatherIconWidget->status() == QQuickWidget::Ready) {
            m_weatherIconItem = weatherIconWidget->rootObject();
            qDebug() << "Delayed root object retrieval:"
                     << (m_weatherIconItem ? "Valid" : "Invalid");
        }
    });

    cacheStatusLabel = new QLabel("缓存状态: 初始化中...", this);
    cacheStatusLabel->setStyleSheet("color: gray; font-size: 10px;");
    ui->verticalLayout_2->addWidget(cacheStatusLabel);

    m_cacheManager->initDatabase();

    // 关联网络管理器的信号
    connect(m_network, &NetworkManager::weatherDataReceived,
            this, [this](const QJsonObject &data) {
                this->handleWeatherData(data);
            });

    connect(m_network, &NetworkManager::weatherErrorOccurred,
            this, &WeatherManager::handleWeatherError);

    connect(m_network, &NetworkManager::cityCodeFound,
            this, &WeatherManager::handleCityCodeFound);

    connect(m_network, &NetworkManager::citySearchError,
            this, &WeatherManager::showWeatherError);
}


//----------------------------设置API key----------------------------------------
void WeatherManager::setHefengToken(const QString &token) {
    m_hefengToken = token;
}

//------------------------------析构函数-------------------------------------------
WeatherManager::~WeatherManager()
{
    delete ui;
}

//----------------------------处理天气信息------------------------------------------
void WeatherManager::handleWeatherData(const QJsonObject &data) {
    qDebug() << "处理天气数据:";
    qDebug() << "  完整数据:" << data;

    if (!m_currentLocationId.isEmpty()) {
        qDebug() << "缓存天气数据到位置ID:" << m_currentLocationId;
        m_cacheManager->cacheWeatherData(m_currentLocationId, data);
    }
    updateCurrentWeather(data);
}


//--------------------------处理天气错误-------------------------------------------
void WeatherManager::handleWeatherError(const QString &errorMsg) {
    showWeatherError(errorMsg);
}

//---------------------------------更新当前天气--------------------------------------
void WeatherManager::updateCurrentWeather(const QJsonObject &data) {
    if (data.isEmpty()) {
        showWeatherError("Empty weather data");
        return;
    }

    // 更新温度
    QString temp = data.value("temp").toString();
    ui->tempLabel->setText(temp + "°C");

    // 获取天气状态和图标代码
    QString weatherText = data.value("text").toString(); // 天气状态文字描述
    QString iconCode = data.value("icon").toString();    // 天气图标代码
    bool isDay = true;

    // 更精确的白天/黑夜判断
    if (data.contains("isDay")) {
        isDay = data.value("isDay").toBool();
    } else if (!iconCode.isEmpty()) {
        // 根据图标代码判断白天黑夜
        isDay = !iconCode.endsWith("n");
    }

    qDebug() << "更新天气数据 - 状态:" << weatherText
             << " 图标代码:" << iconCode
             << " 白天:" << isDay;

    // 更新QML天气图标
    if (m_weatherIconItem) {
        // 设置天气图标属性
        m_weatherIconItem->setProperty("iconCode", iconCode);
        m_weatherIconItem->setProperty("isDay", isDay);
        m_weatherIconItem->setProperty("animated", true);

        // // 根据天气状态更新UI【不用这个了，qml里面有相似显示】
        // if (weatherText.contains("雨")) {
        //     // 雨天特殊处理
        //     ui->weatherDescLabel->setText("雨天");
        //     ui->weatherDescLabel->setStyleSheet("color: blue;");
        // } else if (weatherText.contains("雪")) {
        //     // 雪天特殊处理
        //     ui->weatherDescLabel->setText("雪天");
        //     ui->weatherDescLabel->setStyleSheet("color: white;");
        // } else {
        //     // 默认处理
        //     ui->weatherDescLabel->setText(weatherText);
        //     ui->weatherDescLabel->setStyleSheet("");
        // }
    } else {
        qWarning() << "天气图标对象未初始化!";
    }
}
//------------------------天气相关报错展示--------------------------------
void WeatherManager::showWeatherError(const QString &message) {
    ui->tempLabel->setText("Error");
    QMessageBox::warning(this, "Weather Error", message);
}

//---------------------------改变主题-----------------------------------
void WeatherManager::on_themeButton_clicked() {
    static bool darkMode = false;
    darkMode = !darkMode;

    // 更新QML主题
    if (m_weatherIconItem) {
        m_weatherIconItem->setProperty("theme", darkMode ? "dark" : "light");
    }

    // 更新应用主题
    if (darkMode) {
        qApp->setStyleSheet("QWidget{background: #333; color: #FFF;}");
    } else {
        qApp->setStyleSheet("");
    }
}

// 其他函数保持不变...

// -----------------以下是界面交互槽函数（保持原有逻辑） -----------------------------
// void WeatherManager::on_cityComboBox_currentIndexChanged(const QString &city) {
//     fetchWeatherData(city);
// }
//------------------------搜索相应地区天气----------------------------------
void WeatherManager::on_searchButton_clicked() {
    qDebug() << "Button clicked!";
    QString cityName = ui->cityLineEdit->text().trimmed();
    if (cityName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入有效的城市名称");
        return;
    }
    // 1. 先尝试从缓存获取城市编码
    QString cityCode = m_cacheManager->getCityCode(cityName);

    // //顺便传了一下token
    // m_network->searchCityCode(cityName,m_hefengToken); // 先搜索城市编码，而非直接请求天气

    if (cityCode.isEmpty()) {
        // 2. 缓存中没有有效编码，从API获取
        cacheStatusLabel->setText("从API获取城市编码...");
        cacheStatusLabel->setStyleSheet("color: orange;");

        QString token = m_hefengToken.trimmed().remove(' ');
        if (token.isEmpty() || token.length() != 32) {
            QMessageBox::warning(this, "错误", "Token格式无效");
            return;
        }

        m_network->searchCityCode(cityName, token);
    } else {
        // 3. 有缓存的城市编码，直接获取天气
        fetchWeatherData(cityCode);
        cacheStatusLabel->setText("城市编码命中缓存!");
        cacheStatusLabel->setStyleSheet("color: green;");
    }
}


//------------------------------查找城市编码-------------------------------
void WeatherManager::handleCityCodeFound(const QString &cityName, const QString &locationId) {
    qDebug() << "City code found:" << locationId; // 添加日志
    if (!locationId.isEmpty()) {
        // 缓存城市编码
        m_cacheManager->cacheCityCode(cityName, locationId);
        //获取天气
        fetchWeatherData(locationId); // 传递编码而非名称
    } else {
        showWeatherError("未能获取城市编码");
    }
}


//--------------------------------获取天气数据----------------------------
void WeatherManager::fetchWeatherData(const QString &locationId) {
    qDebug() << "Token: " << m_hefengToken;
    m_currentLocationId = locationId;
    // 1. 先尝试从缓存获取天气数据
    QJsonObject cachedWeather = m_cacheManager->getWeatherData(locationId);

    if (!cachedWeather.isEmpty()) {
        // 使用缓存数据
        handleWeatherData(cachedWeather);
        return;
    }

    // 2. 缓存中没有有效数据，从API获取
    if (m_hefengToken.isEmpty()) {
        showWeatherError("Please set your Hefeng Token first");
        return;
    }

    m_network->requestWeather(locationId, m_hefengToken);
}


//-----------------------------comboBox槽函数---------------------------------
void WeatherManager::on_cityComboBox_currentIndexChanged(int index) {
    QString city = ui->cityComboBox->itemText(index); // 通过索引取文字
    fetchWeatherData(city);
}

void WeatherManager::on_testCacheButton_clicked() {
    m_cacheManager->printCacheContents();
}

//语音播报功能
void WeatherManager::on_voiceButton_clicked() {
    // 语音功能待实现
}
