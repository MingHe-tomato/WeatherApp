import QtQuick 2.15

Item {
    id: cloudsRoot
    property string theme: "light"
    property bool animated: true

    //云后面的太阳
    Rectangle {
        id: sun
        width: 80
        height: 80
        radius: width/2
        anchors.centerIn: parent
        color: isDay ? "#FFD700" : "#E0E0E0"
    }

    // 主云
    Rectangle {
        id: mainCloud
        width: 100
        height: 50
        radius: 25
        anchors.centerIn: parent
        color: theme === "dark" ? "#7F8C8D" : "#ECF0F1"

        // 云朵动画
        SequentialAnimation on x {
            running: animated
            loops: Animation.Infinite
            NumberAnimation { from: 0; to: -10; duration: 3000; easing.type: Easing.InOutQuad }
            NumberAnimation { from: -10; to: 10; duration: 6000; easing.type: Easing.InOutQuad }
            NumberAnimation { from: 10; to: 0; duration: 3000; easing.type: Easing.InOutQuad }
        }
    }

    // 小云1
    Rectangle {
        width: 70
        height: 40
        radius: 20
        x: mainCloud.x - 60
        y: mainCloud.y - 20
        color: theme === "dark" ? "#95A5A6" : "#F8F9F9"

        SequentialAnimation on x {
            running: animated
            loops: Animation.Infinite
            NumberAnimation { from: mainCloud.x - 60; to: mainCloud.x - 70; duration: 3000; easing.type: Easing.InOutQuad }
            NumberAnimation { from: mainCloud.x - 70; to: mainCloud.x - 50; duration: 6000; easing.type: Easing.InOutQuad }
            NumberAnimation { from: mainCloud.x - 50; to: mainCloud.x - 60; duration: 3000; easing.type: Easing.InOutQuad }
        }
    }

    // 小云2
    Rectangle {
        width: 60
        height: 35
        radius: 17
        x: mainCloud.x + 80
        y: mainCloud.y + 15
        color: theme === "dark" ? "#AAB7B8" : "#FDFEFE"

        SequentialAnimation on x {
            running: animated
            loops: Animation.Infinite
            NumberAnimation { from: mainCloud.x + 80; to: mainCloud.x + 90; duration: 3000; easing.type: Easing.InOutQuad }
            NumberAnimation { from: mainCloud.x + 90; to: mainCloud.x + 70; duration: 6000; easing.type: Easing.InOutQuad }
            NumberAnimation { from: mainCloud.x + 70; to: mainCloud.x + 80; duration: 3000; easing.type: Easing.InOutQuad }
        }
    }
}
