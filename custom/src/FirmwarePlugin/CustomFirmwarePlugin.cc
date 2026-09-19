#include "CustomFirmwarePlugin.h"
#include "CustomAutoPilotPlugin.h"
#include "px4_custom_mode.h"
#include "Vehicle.h"

CustomFirmwarePlugin::CustomFirmwarePlugin()
{
    for (auto &mode: _flightModeList) {
        //-- Narrow the flight mode options to only these
        if ((mode.mode_name != pauseFlightMode()) && (mode.mode_name != rtlFlightMode()) && (mode.mode_name != missionFlightMode())) {
            // No other flight modes can be set
            mode.canBeSet = false;
        }
    }
}

AutoPilotPlugin* CustomFirmwarePlugin::autopilotPlugin(Vehicle *vehicle) const
{
    return new CustomAutoPilotPlugin(vehicle, vehicle);
}

const QVariantList& CustomFirmwarePlugin::toolIndicators(const Vehicle *vehicle)
{
    if (_toolIndicatorList.size() == 0) {
        // Start from the stock list so upstream additions still appear, then drop what a
        // small-tablet FPV pilot does not need: GPS, remote ID, gimbal, ESC and multi-vehicle
        // indicators. Telemetry and RC link, battery and joystick stay.
        _toolIndicatorList = FirmwarePlugin::toolIndicators(vehicle);
        static const QStringList removed = {
            QStringLiteral("qrc:/qml/QGroundControl/Toolbar/VehicleGPSIndicator.qml"),
            QStringLiteral("qrc:/qml/QGroundControl/Toolbar/GPSResilienceIndicator.qml"),
            QStringLiteral("qrc:/qml/QGroundControl/Toolbar/RemoteIDIndicator.qml"),
            QStringLiteral("qrc:/qml/QGroundControl/Toolbar/GimbalIndicator.qml"),
            QStringLiteral("qrc:/qml/QGroundControl/Toolbar/EscIndicator.qml"),
            QStringLiteral("qrc:/qml/QGroundControl/Toolbar/MultiVehicleSelector.qml"),
        };
        for (const QString &url : removed) {
            (void) _toolIndicatorList.removeOne(QVariant::fromValue(QUrl::fromUserInput(url)));
        }
    }

    return _toolIndicatorList;
}

bool CustomFirmwarePlugin::hasGimbal(Vehicle* /*vehicle*/, bool &rollSupported, bool &pitchSupported, bool &yawSupported) const
{
    rollSupported = false;
    pitchSupported = true;
    yawSupported = true;

    return true;
}

void CustomFirmwarePlugin::updateAvailableFlightModes(FlightModeList &modeList)
{

    for (auto &mode: modeList) {
        const PX4CustomMode::Mode cMode = static_cast<PX4CustomMode::Mode>(mode.custom_mode);
        // Update Multi Rotor
        switch (cMode) {
        case PX4CustomMode::MANUAL:
        case PX4CustomMode::STABILIZED:
        case PX4CustomMode::ACRO:
        case PX4CustomMode::RATTITUDE:
        case PX4CustomMode::ALTCTL:
        case PX4CustomMode::OFFBOARD:
        case PX4CustomMode::SIMPLE:
        case PX4CustomMode::POSCTL_POSCTL:
        case PX4CustomMode::AUTO_LOITER:
        case PX4CustomMode::AUTO_MISSION:
        case PX4CustomMode::AUTO_RTL:
        case PX4CustomMode::AUTO_FOLLOW_TARGET:
        case PX4CustomMode::AUTO_LAND:
        case PX4CustomMode::AUTO_PRECLAND:
        case PX4CustomMode::AUTO_READY:
        case PX4CustomMode::AUTO_RTGS:
        case PX4CustomMode::AUTO_TAKEOFF:
            mode.multiRotor = true;
            break;
        case PX4CustomMode::POSCTL_ORBIT:
            mode.multiRotor = false;
            break;
        default:
            break;
        }

        // Update Fixed Wing
        switch (cMode) {
        case PX4CustomMode::OFFBOARD:
        case PX4CustomMode::SIMPLE:
        case PX4CustomMode::POSCTL_ORBIT:
        case PX4CustomMode::AUTO_FOLLOW_TARGET:
        case PX4CustomMode::AUTO_PRECLAND:
            mode.fixedWing = false;
            break;
        case PX4CustomMode::MANUAL:
        case PX4CustomMode::STABILIZED:
        case PX4CustomMode::ACRO:
        case PX4CustomMode::RATTITUDE:
        case PX4CustomMode::ALTCTL:
        case PX4CustomMode::POSCTL_POSCTL:
        case PX4CustomMode::AUTO_LOITER:
        case PX4CustomMode::AUTO_MISSION:
        case PX4CustomMode::AUTO_RTL:
        case PX4CustomMode::AUTO_LAND:
        case PX4CustomMode::AUTO_READY:
        case PX4CustomMode::AUTO_RTGS:
        case PX4CustomMode::AUTO_TAKEOFF:
            mode.fixedWing = true;
            break;
        default:
            break;
        }

        // Update CanBeSet
        switch (cMode){
        case PX4CustomMode::AUTO_LOITER:
        case PX4CustomMode::AUTO_RTL:
        case PX4CustomMode::AUTO_MISSION:
            mode.canBeSet = true;
            break;
        case PX4CustomMode::OFFBOARD:
        case PX4CustomMode::SIMPLE:
        case PX4CustomMode::POSCTL_ORBIT:
        case PX4CustomMode::AUTO_FOLLOW_TARGET:
        case PX4CustomMode::AUTO_PRECLAND:
        case PX4CustomMode::MANUAL:
        case PX4CustomMode::STABILIZED:
        case PX4CustomMode::ACRO:
        case PX4CustomMode::RATTITUDE:
        case PX4CustomMode::ALTCTL:
        case PX4CustomMode::POSCTL_POSCTL:
        case PX4CustomMode::AUTO_LAND:
        case PX4CustomMode::AUTO_READY:
        case PX4CustomMode::AUTO_RTGS:
        case PX4CustomMode::AUTO_TAKEOFF:
            mode.canBeSet = false;
            break;
        default:
            break;
        }
    }

    _updateFlightModeList(modeList);
}
