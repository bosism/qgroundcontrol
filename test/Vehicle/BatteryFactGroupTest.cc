#include "BatteryFactGroupTest.h"

#include <QtTest/QTest>

#include "BatteryFactGroupListModel.h"
#include "MAVLinkLib.h"

namespace {
constexpr double kPercentTolerance = 1.0;
constexpr double kNoVoltage = -1.0;

mavlink_message_t _batteryStatusMessage(uint8_t id, double packVoltage, int8_t batteryRemaining,
                                        double currentAmps = qQNaN())
{
    mavlink_battery_status_t batteryStatus{};
    batteryStatus.id = id;
    batteryStatus.battery_function = MAV_BATTERY_FUNCTION_ALL;
    batteryStatus.type = MAV_BATTERY_TYPE_LION;
    batteryStatus.temperature = INT16_MAX;
    batteryStatus.current_battery = qIsNaN(currentAmps) ? -1 : static_cast<int16_t>(qRound(currentAmps * 100.0));
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
    QTest::addColumn<double>("currentAmps");
    QTest::addColumn<double>("throttlePct");
    QTest::addColumn<double>("expectedPercent");

    const double noCurrent = qQNaN();
    QTest::newRow("rest-full-25V") << 25.0 << noCurrent << 0.0 << 99.9;
    QTest::newRow("rest-24V") << 24.0 << noCurrent << 0.0 << 78.7;
    QTest::newRow("rest-23.68V") << 23.68 << noCurrent << 0.0 << 72.0;
    QTest::newRow("rest-22V") << 22.0 << noCurrent << 0.0 << 40.3;
    QTest::newRow("rest-empty-17V") << 17.0 << noCurrent << 0.0 << 2.4;
    QTest::newRow("rest-clamp-high") << 30.0 << noCurrent << 0.0 << 100.0;
    QTest::newRow("rest-clamp-low") << 10.0 << noCurrent << 0.0 << 0.0;
    QTest::newRow("rest-zero-current") << 22.0 << 0.0 << 0.0 << 40.3;
    QTest::newRow("rest-nan-throttle") << 22.0 << noCurrent << qQNaN() << 40.3;
    QTest::newRow("current-22V-half-hover") << 22.0 << 12.5 << 0.0 << 52.3;
    QTest::newRow("current-22V-hover") << 22.0 << 25.0 << 0.0 << 64.3;
    QTest::newRow("current-22V-above-hover") << 22.0 << 50.0 << 0.0 << 64.3;
    QTest::newRow("current-wins-over-throttle") << 22.0 << 25.0 << 100.0 << 64.3;
    QTest::newRow("current-24V-hover") << 24.0 << 25.0 << 0.0 << 91.4;
    QTest::newRow("current-20V-hover") << 20.0 << 25.0 << 0.0 << 37.8;
    QTest::newRow("throttle-22V-half-hover") << 22.0 << noCurrent << 11.0 << 52.3;
    QTest::newRow("throttle-22V-hover") << 22.0 << noCurrent << 22.0 << 64.3;
    QTest::newRow("throttle-22V-above-hover") << 22.0 << noCurrent << 100.0 << 64.3;
}

void BatteryFactGroupTest::_estimateCurve()
{
    QFETCH(double, packVoltage);
    QFETCH(double, currentAmps);
    QFETCH(double, throttlePct);
    QFETCH(double, expectedPercent);

    const double actual = BatteryFactGroup::estimatePercentRemaining(packVoltage, currentAmps, throttlePct);
    QVERIFY2(qAbs(actual - expectedPercent) < kPercentTolerance,
             qPrintable(QStringLiteral("expected %1 got %2").arg(expectedPercent).arg(actual)));

    QVERIFY(qIsNaN(BatteryFactGroup::estimatePercentRemaining(qQNaN(), currentAmps, throttlePct)));
    QVERIFY(qIsNaN(BatteryFactGroup::estimatePercentRemaining(5.0, currentAmps, throttlePct)));
    QVERIFY(qIsNaN(BatteryFactGroup::estimatePercentRemaining(0.0, currentAmps, throttlePct)));
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

    // No current and no vehicle means no load, so the resting curve applies
    double percent = battery.percentRemaining()->rawValue().toDouble();
    QVERIFY2(qAbs(percent - 78.7) < kPercentTolerance, qPrintable(QStringLiteral("got %1").arg(percent)));
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), true);

    // Reported hover current selects the in-flight curve
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 22.0, -1, 25.0));
    percent = battery.percentRemaining()->rawValue().toDouble();
    QVERIFY2(qAbs(percent - 64.3) < kPercentTolerance, qPrintable(QStringLiteral("got %1").arg(percent)));
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), true);

    // A later real percentage replaces the estimate and clears the flag
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 24.0, 42));
    QCOMPARE(battery.percentRemaining()->rawValue().toDouble(), 42.0);
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), false);
}

void BatteryFactGroupTest::_batteryStatusSettleHold()
{
    BatteryFactGroup battery(0);
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 22.0, -1, 25.0));
    const double loaded = battery.percentRemaining()->rawValue().toDouble();
    QVERIFY2(qAbs(loaded - 64.3) < kPercentTolerance, qPrintable(QStringLiteral("got %1").arg(loaded)));

    // Load drops to zero: the loaded estimate is held while the pack voltage recovers
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 23.0, -1, 0.0));
    QCOMPARE(battery.percentRemaining()->rawValue().toDouble(), loaded);
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), true);

    // Once the settle window has passed the resting curve takes over
    battery._settleTimeMs = 0;
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 23.0, -1, 0.0));
    const double rested = battery.percentRemaining()->rawValue().toDouble();
    QVERIFY2(qAbs(rested - 58.3) < kPercentTolerance, qPrintable(QStringLiteral("got %1").arg(rested)));
}

void BatteryFactGroupTest::_batteryStatusNoVoltage()
{
    BatteryFactGroup battery(0);
    battery.handleMessage(nullptr, _batteryStatusMessage(0, kNoVoltage, -1));

    QVERIFY(qIsNaN(battery.voltage()->rawValue().toDouble()));
    QVERIFY(qIsNaN(battery.percentRemaining()->rawValue().toDouble()));
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), false);
}

void BatteryFactGroupTest::_batteryStatusDropout()
{
    BatteryFactGroup battery(0);
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 22.0, -1, 25.0));
    QVERIFY(!qIsNaN(battery.percentRemaining()->rawValue().toDouble()));

    // A telemetry glitch reporting a near-zero pack voltage must not read as an empty battery
    battery.handleMessage(nullptr, _batteryStatusMessage(0, 0.5, -1, 0.0));
    QVERIFY(qIsNaN(battery.percentRemaining()->rawValue().toDouble()));
    QCOMPARE(battery.percentRemainingEstimated()->rawValue().toBool(), false);
}

UT_REGISTER_TEST(BatteryFactGroupTest, TestLabel::Unit, TestLabel::Vehicle)
