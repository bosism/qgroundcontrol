import QtQuick

import QGroundControl
import QGroundControl.Controls

/// One row of the Stealth status board: label, monospace value, square lamp.
Item {
    id:     row
    width:  parent ? parent.width : implicitWidth
    height: ScreenTools.defaultFontPixelHeight * 1.35

    property string label:  ""
    property string value:  ""
    property color  lamp:   "transparent"

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    QGCLabel {
        id:                     labelText
        anchors.left:           parent.left
        anchors.leftMargin:     ScreenTools.defaultFontPixelWidth
        anchors.verticalCenter: parent.verticalCenter
        width:                  ScreenTools.defaultFontPixelWidth * 7
        text:                   row.label
        font.capitalization:    Font.AllUppercase
        font.letterSpacing:     1.2
        font.pointSize:         ScreenTools.smallFontPointSize * 0.9
        opacity:                0.6
    }

    QGCLabel {
        anchors.left:           labelText.right
        anchors.right:          lampRect.left
        anchors.rightMargin:    ScreenTools.defaultFontPixelWidth
        anchors.verticalCenter: parent.verticalCenter
        text:                   row.value
        font.family:            ScreenTools.fixedFontFamily
        font.pointSize:         ScreenTools.smallFontPointSize
        elide:                  Text.ElideRight
    }

    Rectangle {
        id:                     lampRect
        anchors.right:          parent.right
        anchors.rightMargin:    ScreenTools.defaultFontPixelWidth
        anchors.verticalCenter: parent.verticalCenter
        width:                  ScreenTools.defaultFontPixelHeight * 0.55
        height:                 width
        color:                  row.lamp
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left:   parent.left
        anchors.right:  parent.right
        height:         1
        color:          qgcPal.groupBorder
        opacity:        0.5
    }
}
