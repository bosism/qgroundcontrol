import QtQuick

import QGroundControl
import QGroundControl.Controls

/// Stealth Ops head-up display: heading tape, speed and altitude tapes,
/// artificial horizon with pitch ladder. Replaces the stock compass/attitude panel.
Item {
    id:     control
    width:  ScreenTools.defaultFontPixelWidth * 46
    height: ScreenTools.defaultFontPixelHeight * 12

    property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle

    property real _heading:   _vehicle ? _vehicle.heading.rawValue : 0
    property real _roll:      _vehicle ? _vehicle.roll.rawValue : 0
    property real _pitch:     _vehicle ? _vehicle.pitch.rawValue : 0
    property real _speed:     _vehicle && !isNaN(_vehicle.groundSpeed.value) ? _vehicle.groundSpeed.value : 0
    property real _altitude:  _vehicle && !isNaN(_vehicle.altitudeRelative.value) ? _vehicle.altitudeRelative.value : 0
    property real _climb:     _vehicle && !isNaN(_vehicle.climbRate.value) ? _vehicle.climbRate.value : 0
    property string _speedUnits: _vehicle ? _vehicle.groundSpeed.units : ""
    property string _altUnits:   _vehicle ? _vehicle.altitudeRelative.units : ""

    property real _fontPx:    ScreenTools.defaultFontPixelHeight * 0.8
    property real _tapeW:     ScreenTools.defaultFontPixelWidth * 7
    property real _hdgH:      ScreenTools.defaultFontPixelHeight * 1.9
    property real _footH:     ScreenTools.defaultFontPixelHeight * 1.4

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    property color _accent: qgcPal.buttonHighlight

    Rectangle {
        anchors.fill:   parent
        color:          qgcPal.window
        opacity:        0.88
        border.color:   qgcPal.groupBorder
        border.width:   1
    }

    // amber corner ticks, as in the mockups
    Rectangle { width: ScreenTools.defaultFontPixelWidth; height: 2; color: control._accent }
    Rectangle { width: 2; height: ScreenTools.defaultFontPixelWidth; color: control._accent }
    Rectangle { width: ScreenTools.defaultFontPixelWidth; height: 2; color: control._accent; anchors.right: parent.right; anchors.bottom: parent.bottom }
    Rectangle { width: 2; height: ScreenTools.defaultFontPixelWidth; color: control._accent; anchors.right: parent.right; anchors.bottom: parent.bottom }

    Canvas {
        id:             canvas
        anchors.fill:   parent
        anchors.margins: 1
        renderStrategy: Canvas.Cooperative

        onPaint: {
            var ctx = getContext("2d")
            var w = width, h = height
            ctx.clearRect(0, 0, w, h)
            ctx.font = control._fontPx + "px '" + ScreenTools.fixedFontFamily + "'"
            ctx.lineWidth = 1

            var accent = qgcPal.buttonHighlight
            var text = qgcPal.text
            var border = qgcPal.groupBorder
            var ok = qgcPal.colorGreen

            var hudTop = control._hdgH
            var hudBottom = h - control._footH
            var hudH = hudBottom - hudTop
            var centerX = w / 2
            var centerY = hudTop + hudH / 2

            // ---------------- heading tape
            ctx.fillStyle = Qt.rgba(0, 0, 0, 0.25)
            ctx.fillRect(0, 0, w, hudTop)
            ctx.strokeStyle = border
            ctx.beginPath(); ctx.moveTo(0, hudTop + 0.5); ctx.lineTo(w, hudTop + 0.5); ctx.stroke()
            var pxPerDeg = w / 90
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"
            for (var d = -50; d <= 50; d += 5) {
                var deg = Math.round(control._heading / 5) * 5 + d
                var x = centerX + (deg - control._heading) * pxPerDeg
                if (x < 0 || x > w) continue
                var major = (deg % 10) === 0
                ctx.strokeStyle = text
                ctx.beginPath()
                ctx.moveTo(x, hudTop - (major ? control._hdgH * 0.35 : control._hdgH * 0.18))
                ctx.lineTo(x, hudTop)
                ctx.stroke()
                if ((deg % 30) === 0) {
                    var n = ((deg % 360) + 360) % 360
                    ctx.fillStyle = text
                    ctx.fillText(("00" + n).slice(-3), x, control._hdgH * 0.38)
                }
            }
            // heading readout
            var boxW = control._fontPx * 3.6, boxH = control._hdgH * 0.72
            ctx.fillStyle = qgcPal.windowShadeDark
            ctx.fillRect(centerX - boxW / 2, 2, boxW, boxH)
            ctx.strokeStyle = accent
            ctx.strokeRect(centerX - boxW / 2 + 0.5, 2.5, boxW, boxH)
            ctx.fillStyle = accent
            ctx.fillText(("00" + Math.round(((control._heading % 360) + 360) % 360)).slice(-3), centerX, 2 + boxH / 2)
            ctx.beginPath()
            ctx.moveTo(centerX - 6, hudTop); ctx.lineTo(centerX + 6, hudTop); ctx.lineTo(centerX, hudTop + 6)
            ctx.closePath(); ctx.fill()

            // ---------------- artificial horizon (clipped to the centre area)
            var hx0 = control._tapeW, hx1 = w - control._tapeW
            ctx.save()
            ctx.beginPath(); ctx.rect(hx0, hudTop, hx1 - hx0, hudH); ctx.clip()
            ctx.translate(centerX, centerY)
            ctx.rotate(-control._roll * Math.PI / 180)
            var pxPerPitch = hudH / 45
            ctx.translate(0, control._pitch * pxPerPitch)
            ctx.fillStyle = Qt.rgba(0.16, 0.20, 0.24, 0.55)
            ctx.fillRect(-w, -w, 2 * w, w)
            ctx.fillStyle = Qt.rgba(0.16, 0.14, 0.09, 0.55)
            ctx.fillRect(-w, 0, 2 * w, w)
            ctx.strokeStyle = text
            ctx.lineWidth = 1.5
            ctx.beginPath(); ctx.moveTo(-w, 0); ctx.lineTo(w, 0); ctx.stroke()
            ctx.lineWidth = 1
            ctx.fillStyle = text
            for (var p = -30; p <= 30; p += 10) {
                if (p === 0) continue
                var y = -p * pxPerPitch
                var len = (p % 20 === 0) ? hudH * 0.22 : hudH * 0.12
                ctx.beginPath()
                ctx.moveTo(-len, y); ctx.lineTo(-len * 0.3, y)
                ctx.moveTo(len * 0.3, y); ctx.lineTo(len, y)
                ctx.stroke()
                ctx.textAlign = "right"; ctx.fillText(Math.abs(p), -len - 4, y)
                ctx.textAlign = "left";  ctx.fillText(Math.abs(p), len + 4, y)
            }
            ctx.restore()

            // fixed aircraft symbol
            ctx.strokeStyle = accent
            ctx.lineWidth = 2
            ctx.beginPath()
            ctx.moveTo(centerX - 40, centerY); ctx.lineTo(centerX - 14, centerY)
            ctx.lineTo(centerX - 6, centerY + 8); ctx.lineTo(centerX, centerY)
            ctx.lineTo(centerX + 6, centerY + 8); ctx.lineTo(centerX + 14, centerY)
            ctx.lineTo(centerX + 40, centerY)
            ctx.stroke()
            ctx.lineWidth = 1
            // roll scale
            var rr = hudH * 0.42
            ctx.strokeStyle = border
            ctx.beginPath(); ctx.arc(centerX, centerY, rr, Math.PI * 1.25, Math.PI * 1.75); ctx.stroke()
            ctx.fillStyle = accent
            var ra = -Math.PI / 2 - control._roll * Math.PI / 180
            var rx = centerX + Math.cos(ra) * rr, ry = centerY + Math.sin(ra) * rr
            ctx.beginPath(); ctx.moveTo(rx, ry); ctx.lineTo(rx - 5, ry + 9); ctx.lineTo(rx + 5, ry + 9); ctx.closePath(); ctx.fill()

            // ---------------- speed (left) and altitude (right) tapes
            function tape(x0, rightSide, value, step, label) {
                ctx.fillStyle = Qt.rgba(0, 0, 0, 0.25)
                ctx.fillRect(x0, hudTop, control._tapeW, hudH)
                ctx.strokeStyle = border
                var edge = rightSide ? x0 + 0.5 : x0 + control._tapeW - 0.5
                ctx.beginPath(); ctx.moveTo(edge, hudTop); ctx.lineTo(edge, hudBottom); ctx.stroke()
                var pxPerUnit = hudH / (step * 8)
                ctx.save()
                ctx.beginPath(); ctx.rect(x0, hudTop, control._tapeW, hudH); ctx.clip()
                ctx.fillStyle = text
                ctx.strokeStyle = text
                var base = Math.floor(value / step) * step
                for (var k = -6; k <= 6; k++) {
                    var v = base + k * step
                    var yy = centerY - (v - value) * pxPerUnit
                    var tickLen = control._tapeW * 0.18
                    ctx.beginPath()
                    if (rightSide) { ctx.moveTo(x0, yy); ctx.lineTo(x0 + tickLen, yy) }
                    else           { ctx.moveTo(x0 + control._tapeW - tickLen, yy); ctx.lineTo(x0 + control._tapeW, yy) }
                    ctx.stroke()
                    if (v >= 0 || label === "GS") {
                        ctx.textAlign = rightSide ? "left" : "right"
                        ctx.fillText(v, rightSide ? x0 + tickLen + 4 : x0 + control._tapeW - tickLen - 4, yy)
                    }
                }
                ctx.restore()
                // readout
                var bw = control._tapeW * 0.8, bh = control._fontPx * 1.7
                var bx = rightSide ? x0 + control._tapeW * 0.16 : x0 + control._tapeW * 0.04
                ctx.fillStyle = qgcPal.windowShadeDark
                ctx.fillRect(bx, centerY - bh / 2, bw, bh)
                ctx.strokeStyle = accent
                ctx.strokeRect(bx + 0.5, centerY - bh / 2 + 0.5, bw, bh)
                ctx.fillStyle = accent
                ctx.textAlign = "center"
                ctx.fillText(value.toFixed(1), bx + bw / 2, centerY)
                ctx.beginPath()
                if (rightSide) { ctx.moveTo(bx, centerY - 6); ctx.lineTo(bx - 7, centerY); ctx.lineTo(bx, centerY + 6) }
                else           { ctx.moveTo(bx + bw, centerY - 6); ctx.lineTo(bx + bw + 7, centerY); ctx.lineTo(bx + bw, centerY + 6) }
                ctx.closePath(); ctx.fill()
                ctx.fillStyle = qgcPal.text
                ctx.font = (control._fontPx * 0.75) + "px '" + ScreenTools.normalFontFamily + "'"
                ctx.textAlign = rightSide ? "left" : "right"
                ctx.fillText(label, rightSide ? x0 + 4 : x0 + control._tapeW - 4, hudTop + control._fontPx * 0.7)
                ctx.font = control._fontPx + "px '" + ScreenTools.fixedFontFamily + "'"
            }
            tape(0, false, control._speed, 2, "GS " + control._speedUnits)
            tape(w - control._tapeW, true, control._altitude, 10, "ALT " + control._altUnits)

            // ---------------- footer
            ctx.strokeStyle = border
            ctx.beginPath(); ctx.moveTo(0, hudBottom + 0.5); ctx.lineTo(w, hudBottom + 0.5); ctx.stroke()
            ctx.font = (control._fontPx * 0.8) + "px '" + ScreenTools.normalFontFamily + "'"
            ctx.fillStyle = text
            ctx.textAlign = "left"
            ctx.fillText("VS " + (control._climb >= 0 ? "+" : "") + control._climb.toFixed(1), 6, hudBottom + control._footH / 2)
            ctx.textAlign = "right"
            ctx.fillText("ROLL " + Math.round(control._roll) + "°  PITCH " + Math.round(control._pitch) + "°", w - 6, hudBottom + control._footH / 2)
            ctx.textAlign = "center"
            ctx.fillStyle = ok
            ctx.fillText(control._vehicle ? control._vehicle.flightMode.toUpperCase() : "NO LINK", centerX, hudBottom + control._footH / 2)
        }
    }

    // Repaint at a steady rate instead of on every fact change.
    Timer {
        interval:   100
        running:    control.visible
        repeat:     true
        onTriggered: canvas.requestPaint()
    }
}
