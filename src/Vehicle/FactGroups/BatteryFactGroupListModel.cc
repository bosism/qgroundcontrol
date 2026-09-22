#include "BatteryFactGroupListModel.h"

#include <algorithm>
#include <cmath>

#include "MAVLinkLib.h"
#include "Vehicle.h"

namespace {
// Voltage-only state-of-charge model for the 6S Li-ion 10 Ah pack, fitted from flight logs with the
// ArduPilot BattEstimate sigmoid: soc = c1 * (1 - 1 / (1 + (v / c2)^c4)^c3), v = per-cell volts.
constexpr double kCellCount = 6.0;
constexpr double kHoverCurrentAmps = 25.0;    // measured hover current; the in-flight curve applies fully here
constexpr double kHoverThrottlePct = 22.0;    // measured hover throttle output, used when current is unreported
constexpr double kMinValidPackVoltage = 6.0;  // below this the reading is a telemetry glitch, not a battery

struct SocCurve
{
    double c1;
    double c2;
    double c3;
    double c4;
};

constexpr SocCurve kRestingCurve{281.177, 3.8162, 0.3222, 12.14};   // at rest (throttle 0)
constexpr SocCurve kInFlightCurve{325.292, 3.2571, 0.1516, 10.01};  // hovering at ~50 A

double socFromCellVoltage(const SocCurve& curve, double cellVoltage)
{
    if (cellVoltage <= 0.0) {
        return 0.0;
    }
    const double soc = curve.c1 * (1.0 - 1.0 / std::pow(1.0 + std::pow(cellVoltage / curve.c2, curve.c4), curve.c3));
    return std::clamp(soc, 0.0, 100.0);
}
}  // namespace

BatteryFactGroupListModel::BatteryFactGroupListModel(QObject* parent)
    : FactGroupListModel("battery", parent)
{

}

bool BatteryFactGroupListModel::_shouldHandleMessage(const mavlink_message_t &message, QList<uint32_t> &ids) const
{
    ids.clear();

    switch (message.msgid) {
    case MAVLINK_MSG_ID_HIGH_LATENCY:
    case MAVLINK_MSG_ID_HIGH_LATENCY2:
        ids.append(0); // High latency messages do not have a battery id
        return true;
    case MAVLINK_MSG_ID_BATTERY_STATUS:
    {
        mavlink_battery_status_t batteryStatus{};
        mavlink_msg_battery_status_decode(&message, &batteryStatus);
        ids.append(batteryStatus.id);
        return true;
    }
    default:
        return false; // Not a message we care about
    }
}

FactGroupWithId *BatteryFactGroupListModel::_createFactGroupWithId(uint32_t id)
{
    return new BatteryFactGroup(id, this);
}

BatteryFactGroup::BatteryFactGroup(uint32_t batteryId, QObject *parent)
    : FactGroupWithId(1000, QStringLiteral(":/json/Vehicle/BatteryFact.json"), parent)
{
    _addFact(&_batteryFunctionFact);
    _addFact(&_batteryTypeFact);
    _addFact(&_voltageFact);
    _addFact(&_currentFact);
    _addFact(&_mahConsumedFact);
    _addFact(&_temperatureFact);
    _addFact(&_percentRemainingFact);
    _addFact(&_percentRemainingEstimatedFact);
    _addFact(&_timeRemainingFact);
    _addFact(&_timeRemainingStrFact);
    _addFact(&_chargeStateFact);
    _addFact(&_instantPowerFact);

    _idFact.setRawValue(batteryId);
    _batteryFunctionFact.setRawValue(MAV_BATTERY_FUNCTION_UNKNOWN);
    _batteryTypeFact.setRawValue(MAV_BATTERY_TYPE_UNKNOWN);
    _voltageFact.setRawValue(qQNaN());
    _currentFact.setRawValue(qQNaN());
    _mahConsumedFact.setRawValue(qQNaN());
    _temperatureFact.setRawValue(qQNaN());
    _percentRemainingFact.setRawValue(qQNaN());
    _percentRemainingEstimatedFact.setRawValue(false);
    _timeRemainingFact.setRawValue(qQNaN());
    _chargeStateFact.setRawValue(MAV_BATTERY_CHARGE_STATE_UNDEFINED);
    _instantPowerFact.setRawValue(qQNaN());

    (void) connect(&_timeRemainingFact, &Fact::rawValueChanged, this, &BatteryFactGroup::_timeRemainingChanged);
}

double BatteryFactGroup::_inFlightWeight(double currentAmps, double throttlePct)
{
    if (!qIsNaN(currentAmps)) {
        return std::clamp(currentAmps / kHoverCurrentAmps, 0.0, 1.0);
    }
    const double throttle = qIsNaN(throttlePct) ? 0.0 : throttlePct;
    return std::clamp(throttle / kHoverThrottlePct, 0.0, 1.0);
}

double BatteryFactGroup::estimatePercentRemaining(double packVoltage, double currentAmps, double throttlePct)
{
    if (qIsNaN(packVoltage) || packVoltage < kMinValidPackVoltage) {
        return qQNaN();
    }
    const double cellVoltage = packVoltage / kCellCount;
    const double inFlightWeight = _inFlightWeight(currentAmps, throttlePct);
    return ((1.0 - inFlightWeight) * socFromCellVoltage(kRestingCurve, cellVoltage)) +
           (inFlightWeight * socFromCellVoltage(kInFlightCurve, cellVoltage));
}

void BatteryFactGroup::handleMessage(Vehicle *vehicle, const mavlink_message_t &message)
{
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HIGH_LATENCY:
        _handleHighLatency(vehicle, message);
        break;
    case MAVLINK_MSG_ID_HIGH_LATENCY2:
        _handleHighLatency2(vehicle, message);
        break;
    case MAVLINK_MSG_ID_BATTERY_STATUS:
        _handleBatteryStatus(vehicle, message);
        break;
    default:
        break;
    }
}

void BatteryFactGroup::_handleHighLatency(Vehicle * /*vehicle*/, const mavlink_message_t &message)
{
    mavlink_high_latency_t highLatency{};
    mavlink_msg_high_latency_decode(&message, &highLatency);

    percentRemaining()->setRawValue((highLatency.battery_remaining == UINT8_MAX) ? qQNaN() : highLatency.battery_remaining);
    percentRemainingEstimated()->setRawValue(false);

    _setTelemetryAvailable(true);
}

void BatteryFactGroup::_handleHighLatency2(Vehicle * /*vehicle*/, const mavlink_message_t &message)
{
    mavlink_high_latency2_t highLatency2{};
    mavlink_msg_high_latency2_decode(&message, &highLatency2);

    percentRemaining()->setRawValue((highLatency2.battery == -1) ? qQNaN() : highLatency2.battery);
    percentRemainingEstimated()->setRawValue(false);

    _setTelemetryAvailable(true);
}

void BatteryFactGroup::_handleBatteryStatus(Vehicle *vehicle, const mavlink_message_t &message)
{
    mavlink_battery_status_t batteryStatus{};
    mavlink_msg_battery_status_decode(&message, &batteryStatus);

    if (batteryStatus.id != id()->rawValue().toUInt()) {
        // Disregard battery status messages which are not targeted at this battery id
        return;
    }

    double totalVoltage = qQNaN();
    for (int i = 0; i < 10; i++) {
        const double cellVoltage = ((batteryStatus.voltages[i] == UINT16_MAX)) ? qQNaN() : (static_cast<double>(batteryStatus.voltages[i]) / 1000.0);
        if (qIsNaN(cellVoltage)) {
            break;
        }
        if (i == 0) {
            totalVoltage = cellVoltage;
        } else {
            totalVoltage += cellVoltage;
        }
    }

    for (int i = 0; i < 4; i++) {
        const double cellVoltage = ((batteryStatus.voltages_ext[i] == 0)) ? qQNaN() : (static_cast<double>(batteryStatus.voltages_ext[i]) / 1000.0);
        if (qIsNaN(cellVoltage)) {
            break;
        }
        totalVoltage += cellVoltage;
    }

    function()->setRawValue(batteryStatus.battery_function);
    type()->setRawValue(batteryStatus.type);
    temperature()->setRawValue((batteryStatus.temperature == INT16_MAX) ? qQNaN() : (static_cast<double>(batteryStatus.temperature) / 100.0));
    voltage()->setRawValue(totalVoltage);
    current()->setRawValue((batteryStatus.current_battery == -1) ? qQNaN() : (static_cast<double>(batteryStatus.current_battery) / 100.0));
    mahConsumed()->setRawValue((batteryStatus.current_consumed == -1) ? qQNaN() : batteryStatus.current_consumed);
    if (batteryStatus.battery_remaining == -1) {
        const double throttlePct = vehicle ? vehicle->throttlePct()->rawValue().toDouble() : 0.0;
        const double currentAmps = current()->rawValue().toDouble();
        double estimate = estimatePercentRemaining(totalVoltage, currentAmps, throttlePct);
        if (qIsNaN(estimate)) {
            _lastLoadedEstimate = qQNaN();
        } else if (_inFlightWeight(currentAmps, throttlePct) > 0.0) {
            _lastLoadedEstimate = estimate;
            _settleTimer.start();
        } else if (!qIsNaN(_lastLoadedEstimate) && _settleTimer.isValid() && _settleTimer.elapsed() < _settleTimeMs) {
            estimate = _lastLoadedEstimate;
        }
        percentRemaining()->setRawValue(estimate);
        percentRemainingEstimated()->setRawValue(!qIsNaN(estimate));
    } else {
        percentRemaining()->setRawValue(batteryStatus.battery_remaining);
        percentRemainingEstimated()->setRawValue(false);
    }
    timeRemaining()->setRawValue((batteryStatus.time_remaining == 0) ? qQNaN() : batteryStatus.time_remaining);
    chargeState()->setRawValue(batteryStatus.charge_state);
    instantPower()->setRawValue(totalVoltage * current()->rawValue().toDouble());

    _setTelemetryAvailable(true);
}

void BatteryFactGroup::_timeRemainingChanged(const QVariant &value)
{
    if (qIsNaN(value.toDouble())) {
        _timeRemainingStrFact.setRawValue("––:––:––");
    } else {
        const int totalSeconds = value.toInt();
        const int hours = totalSeconds / 3600;
        const int minutes = (totalSeconds % 3600) / 60;
        const int seconds = totalSeconds % 60;

        _timeRemainingStrFact.setRawValue(QString::asprintf("%02dH:%02dM:%02dS", hours, minutes, seconds));
    }
}
