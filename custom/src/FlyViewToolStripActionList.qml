import QtQml.Models

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlyView

// Stealth FPV override of the Fly View tool strip: manual flying only, so no
// takeoff, no 3D viewer and no gripper. Land and return stay as safety actions.
ToolStripActionList {
    id: _root

    signal displayPreFlightChecklist

    model: [
        PreFlightCheckListShowAction { onTriggered: _root.displayPreFlightChecklist() },
        GuidedActionLand { },
        GuidedActionRTL { },
        GuidedActionPause { },
        FlyViewAdditionalActionsButton { }
    ]
}
