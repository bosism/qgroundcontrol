import QtQuick

import QGroundControl
import QGroundControl.Controls

/// Stealth Ops telemetry bar: fixed set of large mono values with small labels above.
Item {
    id:     control
    width:  row.width
    height: row.height

    property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

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

    Row {
        id: row

        StealthTelemetryCell { label: qsTr("Alt rel");     fact: control._vehicle ? control._vehicle.altitudeRelative : null }
        StealthTelemetryCell { label: qsTr("Ground spd");  fact: control._vehicle ? control._vehicle.groundSpeed : null }
        StealthTelemetryCell { label: qsTr("Vert spd");    fact: control._vehicle ? control._vehicle.climbRate : null }
        StealthTelemetryCell { label: qsTr("Dist home");   fact: control._vehicle ? control._vehicle.distanceToHome : null }
        StealthTelemetryCell { label: qsTr("Heading");     fact: control._vehicle ? control._vehicle.heading : null; accent: true }
        StealthTelemetryCell { label: qsTr("Flight time"); fact: control._vehicle ? control._vehicle.flightTime : null; showUnits: false }
    }
}
