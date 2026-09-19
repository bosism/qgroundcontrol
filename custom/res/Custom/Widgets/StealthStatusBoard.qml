import QtQuick

import QGroundControl
import QGroundControl.Controls

import Custom.Stealth

/// Stealth Ops go/no-go board: one row per subsystem with a square status lamp.
Item {
    id:     control
    width:  ScreenTools.defaultFontPixelWidth * 26
    height: column.height + ScreenTools.defaultFontPixelHeight * 0.4

    property var _vehicle:   QGroundControl.multiVehicleManager.activeVehicle
    property var _battery:   _vehicle && _vehicle.batteries.count > 0 ? _vehicle.batteries.get(0) : null
    property var _gps:       _vehicle ? _vehicle.gps : null
    property var _ekf:       _vehicle ? _vehicle.estimatorStatus : null

    // 0 = no data, 1 = go, 2 = caution, 3 = no-go
    property int _gpsState:     !_gps ? 0 : (_gps.lock.rawValue >= 3 ? 1 : (_gps.lock.rawValue === 2 ? 2 : 3))
    property int _battState:    !_battery || isNaN(_battery.percentRemaining.rawValue) ? 0 :
                                (_battery.percentRemaining.rawValue > 40 ? 1 : (_battery.percentRemaining.rawValue > 20 ? 2 : 3))
    property int _ekfState:     !_ekf ? 0 : (_ekf.goodAttitudeEstimate.rawValue && _ekf.goodHorizPosRelEstimate.rawValue ? 1 : 2)
    property int _sensorState:  !_vehicle ? 0 : (_vehicle.allSensorsHealthy ? 1 : 3)
    property int _readyState:   !_vehicle ? 0 : (!_vehicle.readyToFlyAvailable ? 2 : (_vehicle.readyToFly ? 1 : 3))
    property int _linkState:    !_vehicle || !StealthLink.latencyValid ? 0 :
                                (StealthLink.latencyMs < 100 ? 1 : (StealthLink.latencyMs < 250 ? 2 : 3))
    property bool _allGo:       _gpsState === 1 && _battState === 1 && _sensorState === 1 && _readyState === 1 && _linkState !== 3

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    function lampColor(state) {
        switch (state) {
        case 1:  return qgcPal.colorGreen
        case 2:  return qgcPal.colorOrange
        case 3:  return qgcPal.colorRed
        default: return qgcPal.windowShadeLight
        }
    }

    Rectangle {
        anchors.fill:   parent
        color:          qgcPal.window
        opacity:        0.88
        border.color:   qgcPal.groupBorder
        border.width:   1
    }
    Rectangle { width: ScreenTools.defaultFontPixelWidth; height: 2; color: qgcPal.buttonHighlight }
    Rectangle { width: 2; height: ScreenTools.defaultFontPixelWidth; color: qgcPal.buttonHighlight }
    Rectangle { width: ScreenTools.defaultFontPixelWidth; height: 2; color: qgcPal.buttonHighlight; anchors.right: parent.right; anchors.bottom: parent.bottom }
    Rectangle { width: 2; height: ScreenTools.defaultFontPixelWidth; color: qgcPal.buttonHighlight; anchors.right: parent.right; anchors.bottom: parent.bottom }

    Column {
        id:             column
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.top:    parent.top
        anchors.topMargin: ScreenTools.defaultFontPixelHeight * 0.2

        // header
        Item {
            width:  parent.width
            height: ScreenTools.defaultFontPixelHeight * 1.5

            QGCLabel {
                anchors.left:           parent.left
                anchors.leftMargin:     ScreenTools.defaultFontPixelWidth
                anchors.verticalCenter: parent.verticalCenter
                text:                   qsTr("System status")
                font.capitalization:    Font.AllUppercase
                font.letterSpacing:     1.5
                font.pointSize:         ScreenTools.smallFontPointSize
                font.bold:              true
            }
            QGCLabel {
                anchors.right:          parent.right
                anchors.rightMargin:    ScreenTools.defaultFontPixelWidth
                anchors.verticalCenter: parent.verticalCenter
                text:                   !control._vehicle ? qsTr("NO LINK") : (control._allGo ? qsTr("ALL GO") : qsTr("CHECK"))
                font.pointSize:         ScreenTools.smallFontPointSize
                font.letterSpacing:     1.5
                color:                  !control._vehicle ? qgcPal.text : (control._allGo ? qgcPal.colorGreen : qgcPal.colorOrange)
            }
            Rectangle {
                anchors.bottom: parent.bottom
                width:          parent.width
                height:         1
                color:          qgcPal.groupBorder
            }
        }

        StealthStatusRow {
            label:  qsTr("GPS")
            value:  control._gps ? control._gps.count.valueString + " sat · " + control._gps.hdop.valueString : "--"
            lamp:   control.lampColor(control._gpsState)
        }
        StealthStatusRow {
            label:  qsTr("Battery")
            value:  control._battery ? control._battery.voltage.valueString + " " + control._battery.voltage.units + " · " + control._battery.percentRemaining.valueString + "%" : "--"
            lamp:   control.lampColor(control._battState)
        }
        StealthStatusRow {
            label:  qsTr("Link")
            value:  control._vehicle ? (StealthLink.latencyValid ? StealthLink.latencyMs + " ms" : qsTr("no ping reply")) : "--"
            lamp:   control.lampColor(control._linkState)
        }
        StealthStatusRow {
            label:  qsTr("EKF")
            value:  control._ekf ? (control._ekfState === 1 ? qsTr("Attitude · position") : qsTr("Estimating")) : "--"
            lamp:   control.lampColor(control._ekfState)
        }
        StealthStatusRow {
            label:  qsTr("Sensors")
            value:  control._vehicle ? (control._vehicle.allSensorsHealthy ? qsTr("All healthy") : qsTr("Fault")) : "--"
            lamp:   control.lampColor(control._sensorState)
        }
        StealthStatusRow {
            label:  qsTr("Ready")
            value:  control._vehicle ? control._vehicle.flightMode + (control._vehicle.armed ? qsTr(" · armed") : qsTr(" · safe")) : "--"
            lamp:   control.lampColor(control._readyState)
        }
    }
}
