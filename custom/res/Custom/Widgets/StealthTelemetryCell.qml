import QtQuick

import QGroundControl
import QGroundControl.Controls

/// One label-over-value cell of the Stealth telemetry bar.
Item {
    id:     cell
    width:  Math.max(ScreenTools.defaultFontPixelWidth * 11, column.width + ScreenTools.defaultFontPixelWidth * 2)
    height: column.height + ScreenTools.defaultFontPixelHeight * 0.6

    property string label:      ""
    property var    fact:       null
    property bool   accent:     false
    property bool   showUnits:  true

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    Column {
        id:                 column
        anchors.centerIn:   parent
        spacing:            ScreenTools.defaultFontPixelHeight * 0.1

        QGCLabel {
            text:               cell.label
            font.pointSize:     ScreenTools.smallFontPointSize * 0.85
            font.capitalization: Font.AllUppercase
            font.letterSpacing: 1.5
            opacity:            0.6
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            spacing: ScreenTools.defaultFontPixelWidth * 0.3
            anchors.horizontalCenter: parent.horizontalCenter

            QGCLabel {
                text:               cell.fact ? cell.fact.valueString : "--"
                font.family:        ScreenTools.fixedFontFamily
                font.pointSize:     ScreenTools.largeFontPointSize * 1.2
                color:              cell.accent ? qgcPal.buttonHighlight : qgcPal.text
                anchors.baseline:   parent.bottom
            }
            QGCLabel {
                text:               cell.showUnits && cell.fact ? cell.fact.units : ""
                font.pointSize:     ScreenTools.smallFontPointSize
                opacity:            0.6
                anchors.baseline:   parent.bottom
                anchors.baselineOffset: -ScreenTools.defaultFontPixelHeight * 0.15
            }
        }
    }

    Rectangle {
        anchors.right:  parent.right
        width:          1
        height:         parent.height * 0.7
        anchors.verticalCenter: parent.verticalCenter
        color:          qgcPal.groupBorder
    }
}
