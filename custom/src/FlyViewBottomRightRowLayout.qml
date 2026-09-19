import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlyView

import Custom.Widgets

// Stealth Ops override of the stock bottom-right row: the fact-value grid and
// compass/attitude panel are replaced by the Stealth telemetry bar and HUD.
RowLayout {
    spacing: ScreenTools.defaultFontPixelWidth * 0.75

    StealthTelemetryBar {
        Layout.alignment: Qt.AlignBottom
    }

    StealthHud {
        Layout.alignment: Qt.AlignBottom
    }
}
