#pragma once

#include "UnitTest.h"

class BatteryFactGroupTest : public UnitTest
{
    Q_OBJECT

private slots:
    void _estimateCurve_data();
    void _estimateCurve();
    void _batteryStatusPassThrough();
    void _batteryStatusEstimated();
    void _batteryStatusNoVoltage();
};
