import QtQuick 2.15
import QtQuick.Particles 2.15

Item {
    id: snowRoot
    property string theme: "light"
    property bool animated: true

    // 云朵
    Rectangle {
        id: cloud
        width: 140
        height: 60
        radius: 30
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        color: theme === "dark" ? "#5D6D7E" : "#ECF0F1"

        Rectangle {
            width: 90
            height: 60
            radius: 30
            x: -30
            y: -15
            color: parent.color
        }

        Rectangle {
            width: 70
            height: 60
            radius: 30
            x: 100
            y: -10
            color: parent.color
        }
    }

    // 雪花粒子系统
    ParticleSystem {
        id: snowSystem
        anchors.fill: parent  // 确保粒子系统覆盖整个可视区域
        running: true

        // 发射器
        Emitter {
            id: snowEmitter
            width: cloud.width
            height: 5
            y: cloud.y + cloud.height - 5  // 云的底部再往下 15px，贴合视觉
            x: cloud.x + cloud.width/2 - width/2     // 水平居中云
            emitRate: 1.5
            lifeSpan: 1800
            size: 100
            velocity: PointDirection {
                y: 30;
                yVariation: 10
            }
        }

        // 雪花
        ParticleGroup {
            ImageParticle {
                source: "qrc:/icons/img/snowflake.png" // 雨滴图片（需自行准备透明png）
                alpha: 0 // 透明度
                rotation: 0 // 倾斜角度
                rotationVariation: 0 // 角度随机变化
                scale: 1 // 缩放比例
                // scaleVariation: 0.2 // 缩放随机变化
            }
        }
    }
}
