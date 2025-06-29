import QtQuick 2.15

Item {
    id: sunRoot
    width: 200
    height: 200
    property string theme: "light"
    property bool isDay: true
    property bool animated: true
    property int rayCount: 8  // 8个光芒
    property real raySize: 20 // 三角形大小（边长）
    property real rayDistance: 15 // 三角形与太阳的距离

    // 旋转的太阳容器
    Rectangle {
        id: sunContainer
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        color: "transparent"

        // 旋转动画
        RotationAnimation on rotation {
            running: animated
            from: 0
            to: 360
            duration: 20000
            loops: Animation.Infinite
        }

        // 太阳圆形
        Rectangle {
            id: sun
            width: 80
            height: 80
            radius: width/2
            anchors.centerIn: parent
            color: isDay ? "#FFD700" : "#E0E0E0"
        }

        // 三角形光芒
        Repeater {
            model: rayCount
            delegate: Canvas {
                id: triangle
                width: raySize
                height: raySize * 0.866 // 正三角形高度公式: h = a * √3/2 ≈ a * 0.866

                // 计算位置：太阳半径 + 距离 + 三角形高度/2
                property real distance: sun.width/2 + rayDistance + height/2

                x: parent.width/2 - width/2
                y: parent.height/2 - distance

                transform: Rotation {
                    origin.x: width/2
                    origin.y: distance
                    angle: index * (360/rayCount)
                }

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.fillStyle = isDay ? "#FFD700" : "#E0E0E0"

                    // 绘制正三角形（顶点朝外）
                    ctx.beginPath()
                    ctx.moveTo(width/2, 0)        // 顶点
                    ctx.lineTo(width, height)     // 右下角
                    ctx.lineTo(0, height)         // 左下角
                    ctx.closePath()
                    ctx.fill()
                }

                // 组件加载时和属性变化时重绘
                // Component.onCompleted: requestPaint()
                // onIsDayChanged: requestPaint()
            }
        }
    }
}
