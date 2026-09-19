import QtQuick

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlyView

import Custom.Widgets

// Stealth Ops overlay for the Fly View: adds the system status board in the
// top-right corner, below whatever the stock top-right column occupies.
Item {
    id: _root

    property var parentToolInsets               // Insets from the stock widget layer
    property var totalToolInsets:   _toolInsets // Insets including this overlay's additions
    property var mapControl

    property real _toolsMargin: ScreenTools.defaultFontPixelWidth * 0.75

    // The stock top-right inset comes from the multi-vehicle panel, which keeps its
    // height even when hidden. Only honour it while that panel can actually show.
    property bool _multiVehiclePanelShown: QGroundControl.multiVehicleManager.vehicles.count > 1 &&
                                           QGroundControl.settingsManager.appSettings.enableMultiVehiclePanel.rawValue
    property real _topInset: _multiVehiclePanelShown ? parentToolInsets.topEdgeRightInset : 0

    QGCToolInsets {
        id:                     _toolInsets
        leftEdgeTopInset:       _root.parentToolInsets.leftEdgeTopInset
        leftEdgeCenterInset:    _root.parentToolInsets.leftEdgeCenterInset
        leftEdgeBottomInset:    _root.parentToolInsets.leftEdgeBottomInset
        rightEdgeTopInset:      statusBoard.visible ? _root.width - statusBoard.x : _root.parentToolInsets.rightEdgeTopInset
        rightEdgeCenterInset:   _root.parentToolInsets.rightEdgeCenterInset
        rightEdgeBottomInset:   _root.parentToolInsets.rightEdgeBottomInset
        topEdgeLeftInset:       _root.parentToolInsets.topEdgeLeftInset
        topEdgeCenterInset:     _root.parentToolInsets.topEdgeCenterInset
        topEdgeRightInset:      statusBoard.visible ? statusBoard.y + statusBoard.height : _root.parentToolInsets.topEdgeRightInset
        bottomEdgeLeftInset:    _root.parentToolInsets.bottomEdgeLeftInset
        bottomEdgeCenterInset:  _root.parentToolInsets.bottomEdgeCenterInset
        bottomEdgeRightInset:   _root.parentToolInsets.bottomEdgeRightInset
    }

    StealthStatusBoard {
        id:                 statusBoard
        anchors.right:      parent.right
        anchors.rightMargin: _root._toolsMargin
        anchors.top:        parent.top
        anchors.topMargin:  _root._topInset + _root._toolsMargin
        visible:            !QGroundControl.videoManager.fullScreen
    }
}
