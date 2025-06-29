import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 250
    height: 180

    // 可绑定属性
    property string iconCode: "400"  // 天气代码
    property string theme: "light"  // light/dark
    property bool animated: true    // 是否启用动画
    property bool isDay: true       // 白天/黑夜

    //判断天气类型
    property string weatherType: {
            // 晴天系列 (100-150)
            if (iconCode === "100" ||
                iconCode === "150" ||
                iconCode === "101d" ||
                iconCode === "102d") {
                return "sun";
            }
            // 阴天系列 (104是阴天)
            else if (iconCode === "104" ||
                     iconCode === "154" ||
                     iconCode === "204") {
                return "overcast";
            }
            // 多云系列 (101-104)
            else if (iconCode === "101" ||
                     iconCode === "102" ||
                     iconCode === "103" ||
                     iconCode.startsWith("10")) {
                return "clouds";
            }
            // 阴天系列 (104是阴天)
            else if (iconCode === "104" ||
                     iconCode === "154" ||
                     iconCode === "204") {
                return "overcast";
            }
            // 雨天系列 (300-399)
            else if (iconCode >= "300" && iconCode <= "399") {
                return "rain";
            }
            // 雪天系列 (400-499)
            else if (iconCode >= "400" && iconCode <= "499") {
                return "snow";
            }
            // 默认无
            return "";
    }
    //调试输出
    onIconCodeChanged: {
        console.log("图标代码变化:", iconCode, "天气类型:", weatherType);
    }


    // 主题颜色
    property color backgroundColor: theme === "dark" ? "#1a1a2e" : "#f0f8ff"
    property color primaryColor: theme === "dark" ? "#64b5f6" : "#1976d2"
    property color secondaryColor: theme === "dark" ? "#a7c5eb" : "#5dade2"

    // 背景圆形
    Rectangle {
        id: backgroundCircle
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.95
        height: width
        radius: width / 2
        color: "transparent"
        border.width: 2
        border.color: root.primaryColor
        opacity: 0.3
    }

    // 根据天气类型加载不同组件
    Loader {
        id: weatherLoader
        anchors.centerIn: parent
        width: parent.width * 0.8
        height: parent.height * 0.8
        source: {
            switch(weatherType) {
                case "sun": return "SunComponent.qml"
                case "clouds": return "CloudsComponent.qml"
                case "overcast": return "OvercastComponent.qml"
                case "rain": return "RainComponent.qml"
                case "snow": return "SnowComponent.qml"
                default: return "SunComponent.qml"
            }
        }

        // 将属性传递给加载的组件
        onLoaded: {
            console.log("Component loaded:", weatherLoader.source);
            item.theme = root.theme
            item.animated = root.animated
            item.isDay = root.isDay
        }
    }

    // 主题切换动画
    Behavior on theme {
        ColorAnimation {
            duration: root.animated ? 800 : 0
            properties: "backgroundColor, primaryColor, secondaryColor"
        }
    }

    // 根据天气类型显示文字描述
    Text {
        id: weatherText
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 10
        }
        text: {
            switch(weatherType) {
                case "sun": return isDay ? "晴天" : "晴朗"
                case "clouds": return "多云"
                case "overcast":return "阴天"
                case "rain": return "下雨"
                case "snow": return "下雪"
                default: return "晴天"
            }
        }
        font.pixelSize: 14
        color: root.primaryColor
        font.weight: Font.Medium
    }

    // 调试信息 - 实际使用时可以移除
    /*
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: "red"
        border.width: 1
        Text {
            text: `Code: ${iconCode}\nTheme: ${theme}\nAnimated: ${animated}\nDay: ${isDay}`
            color: root.primaryColor
            font.pixelSize: 10
            anchors.centerIn: parent
        }
    }
    */
}
