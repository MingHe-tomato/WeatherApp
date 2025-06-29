import QtQuick 2.15
import QtQuick.Particles 2.15

Item {
    width: 200
    height: 200
    layer.enabled: true // 启用图层优化动画

    // 雨滴颜色 - 根据主题变化
    property color rainColor: theme === "dark" ? "#64b5f6" : "#1976d2"

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
            color: "#F8F9F9"
        }

        Rectangle {
            width: 70
            height: 60
            radius: 30
            x: 100
            y: -10
            color: "#E0E0E0"
        }
    }

    // 粒子系统
    ParticleSystem {
        anchors.fill: parent
        running: true // 始终运行动画

        // 雨滴发射器
        Emitter {
            id: rainEmitter
            width: cloud.width
            height: 5
            // anchors.top: cloud.bottom
            // anchors.horizontalCenter: cloud.horizontalCenter

            y: cloud.y + cloud.height - 15  // 云的底部再往下 5px，贴合视觉
            x: cloud.x + cloud.width/2 - width/2     // 水平居中云

            emitRate: 30 // 雨强度
            lifeSpan: 1800 // 雨滴生命周期（毫秒）
            size: 6 // 雨滴大小
            sizeVariation: 0.1 // 大小随机变化
            velocity: PointDirection { y: 150; yVariation: 50 } // 下落速度
            system: parent // 关联粒子系统
        }

        // 雨滴粒子
        ParticleGroup {
            ImageParticle {
                source: "qrc:/icons/img/raindrop.png" // 雨滴图片（需自行准备透明png）
                alpha: 0.6 // 透明度
                rotation: -15 // 倾斜角度
                rotationVariation: 30 // 角度随机变化
                scale: 1.2 // 缩放比例
                // scaleVariation: 0.2 // 缩放随机变化
            }
        }
    }
}
