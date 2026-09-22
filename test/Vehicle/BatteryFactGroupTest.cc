#include "BatteryFactGroupTest.h"

#include <QtTest/QTest>

#include "BatteryFactGroupListModel.h"
#include "MAVLinkLib.h"

namespace {
constexpr double kPercentTolerance = 1.0;
constexpr double kNoVoltage = -1.0;

mavlink_message_t _batteryStatusMessage(uint8_t id, double packVoltage, int8_t batteryRemaining)
{
    mavlink_battery_status_t batteryStatus{};
    batteryStatus.id = id;
    batteryStatus.battery_function = MAV_BATTERY_FUNCTION_ALL;
    batteryStatus.type = MAV_BATTERY_TYPE_LION;
    batteryStatus.temperature = INT16_MAX;
    batteryStatus.current_battery = -1;
    batteryStatus.current_consumed = -1;
    batteryStatus.energy_consumed = -1;
    batteryStatus.battery_remaining = batteryRemaining;
    batteryStatus.charge_state = MAV_BATTERY_CHARGE_STATE_OK;

    constexpr int cellCount = 6;
    for (int i = 0; i < 10; i++) {
        if (packVoltage > 0.0 && i < cellCount) {
            batteryStatus.voltages[i] = static_cast<uint16_t>(qRound(packVoltage / cellCount * 1000.0));
        } else {
            batteryStatus.voltages[i] = UINT16_MAX;
        }
    }

    mavlink_message_t message{};
    (void) mavlink_msg_battery_status_encode(1, 1, &message, &batteryStatus);
    return message;
}
}  // namespace

void BatteryFactGroupTest::_estimateCurve_data()
{
    QTest::addColumn<double>("packVoltage");
    QTest::addColumn<double>("throttlePct");
    QTest::addColumn<double>("expectedPercent");

    QTest::newRow("rest-full-25V") << 25.0 << 0.0 << 99.9;
    QTest::newRow("rest-24V") << 24.0 << 0.0 << 78.7;
    QTest::newRow("rest-23.68V") << 23.68 << 0.0 << 72.0;
    QTest::newRow("rest-22V") << 22.0 << 0.0 << 40.3;
    QTest::newRow("rest-empty-17V") << 17.0 << 0.0 << 2.4;
    QTest::newRow("rest-clamp-high") << 30.0 << 0.0 << 100.0;
    QTest::newRow("rest-clamp-low") << 10.0 << 0.0 << 0.0;
    QTest::newRow("rest-nan-throttle") << 22.0 << qQNaN() << 40.3;
    QTest::newRow("blend-22V-quarter-hover") << 22.0 << 25.0 << 52.3;
    QTest::newRow("flight-22V-hover") << 22.0 << 50.0 << 64.3;
    QTest::newRow("flight-22V-above-hover") << 22.0 << 100.0 << 64.3;
    QTest::newRow("flight-24V-hover") << 24.0 << 50.0 << 91.4;
    QTest::newRow("flight-20V-hover") << 20.0 << 50.0 << 37.8;
}

void BatteryFactGroupTest::_estimateCurve()
{
    QFETCH(double, packVoltage);
    QFETCH(double, throttlePct);
    QFETCH(double, expectedPercent);

    const double actual = BatteryFactGroup::estimatePercentRemaining(packVoltage, throttlePct);
    QVERIFY2(qAbs(actual - expectedPercent) < kPercentTolerance,
             qPrintable(QStringLiteral("expected %1 got %2").arg(expectedPercent).arg(actual)));

    QVERIFY(qIsNaN(BatteryFactGroup::estimatePercentRemaining(qQNaN(), throttlePct)));
}

void BatteryFactGroupTest::_batteryStatusPassThrough()
{
    BatteryFactGroup battery(0);
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 24.0, 57));

    QCOMPARE(battery.percentRemaining()->rawValue().toDouble(), 57.0);
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), false);
    QVERIFY(qAbs(battery.voltage()->rawValue().toDouble() - 24.0) < 0.01);
}

void BatteryFactGroupTest::_batteryStatusEstimated()
{
    BatteryFactGroup battery(0);
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 24.0, -1));

    // No vehicle means no throttle, so the resting curve applies
    const double percent = battery.percentRemaining()->rawValue().toDouble();
    QVERIFY2(qAbs(percent - 78.7) < kPercentTolerance, qPrintable(QStringLiteral("got %1").arg(percent)));
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), true);

    // A later real percentage replaces the estimate and clears the flag
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 24.0, 42));
    QCOMPARE(battery.percentRemaining()->rawValue().toDouble(), 42.0);
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), false);
}

void BatteryFactGroupTest::_batteryStatusNoVoltage()
{
    BatteryFactGroup battery(0);
    battery.handleMessage(nullptr, _batteryStatusMessage(0, kNoVoltage, -1));

    QVERIFY(qIsNaN(battery.voltage()->rawValue().toDouble()));
    QVERIFY(qIsNaN(battery.percentRemaining()->rawValue().toDouble()));
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), false);
}

UT_REGISTER_TEST(BatteryFactGroupTest, TestLabel::Unit, TestLabel::Vehicle)
