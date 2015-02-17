import QtQuick 2.3

Canvas {
    id: root

    property Item p0
    property Item p1

    anchors.fill: parent

    property var dummy: {
        p0.x; p0.y; p1.x; p1.y
        requestPaint()
    }

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        ctx.lineWidth = 1
        ctx.strokeStyle = "black"
        ctx.beginPath()
        ctx.moveTo(p0.x + p0.width/2, p0.y + p0.height/2)
        ctx.lineTo(p1.x + p1.width/2, p1.y + p1.height/2)
        ctx.closePath()
        ctx.stroke()
    }
}
