import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 180
    height: 180

    // 公开属性
    property string iconCode: "100"
    property string theme: "light"
    property bool animated: true
    property bool isDay: true

    // 动态加载天气组件
    Loader {
        id: weatherLoader
        anchors.fill: parent
        sourceComponent: getWeatherComponent()
    }

    // 根据天气代码选择组件
    function getWeatherComponent() {
        // 晴天 (100, 150)
        if (iconCode === "100" || iconCode === "150") {
            return sunComponent;
        }
        // 多云 (101, 102, 103)
        else if (iconCode === "101" || iconCode === "102" || iconCode === "103") {
            return cloudsComponent;
        }
        // 雨 (300-399)
        else if (iconCode.startsWith("3")) {
            return rainComponent;
        }
        // 雪 (400-499)
        else if (iconCode.startsWith("4")) {
            return snowComponent;
        }
        // 默认晴天
        return sunComponent;
    }

    // 组件定义
    Component {
        id: sunComponent
        SunComponent {
            theme: root.theme
            isDay: root.isDay
            animated: root.animated
        }
    }

    Component {
        id: cloudsComponent
        CloudsComponent {
            theme: root.theme
            animated: root.animated
        }
    }

    Component {
        id: rainComponent
        RainComponent {
            theme: root.theme
            animated: root.animated
        }
    }

    Component {
        id: snowComponent
        SnowComponent {
            theme: root.theme
            animated: root.animated
        }
    }
}
