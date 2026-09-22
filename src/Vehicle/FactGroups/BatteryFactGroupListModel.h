#pragma once

#include <QtCore/QElapsedTimer>

#include "FactGroupListModel.h"

class BatteryFactGroupListModel : public FactGroupListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit BatteryFactGroupListModel(QObject* parent = nullptr);

protected:
    // Overrides from FactGroupListModel
    bool _shouldHandleMessage(const mavlink_message_t &message, QList<uint32_t> &ids) const final;
    FactGroupWithId *_createFactGroupWithId(uint32_t id) final;
};

class BatteryFactGroup : public FactGroupWithId
{
    Q_OBJECT
    Q_PROPERTY(Fact *function           READ function           CONSTANT)
    Q_PROPERTY(Fact *type               READ type               CONSTANT)
    Q_PROPERTY(Fact *temperature        READ temperature        CONSTANT)
    Q_PROPERTY(Fact *voltage            READ voltage            CONSTANT)
    Q_PROPERTY(Fact *current            READ current            CONSTANT)
    Q_PROPERTY(Fact *mahConsumed        READ mahConsumed        CONSTANT)
    Q_PROPERTY(Fact *percentRemaining   READ percentRemaining   CONSTANT)
    Q_PROPERTY(Fact *percentRemainingEstimated READ percentRemainingEstimated CONSTANT)
    Q_PROPERTY(Fact *timeRemaining      READ timeRemaining      CONSTANT)
    Q_PROPERTY(Fact *timeRemainingStr   READ timeRemainingStr   CONSTANT)
    Q_PROPERTY(Fact *chargeState        READ chargeState        CONSTANT)
    Q_PROPERTY(Fact *instantPower       READ instantPower       CONSTANT)

public:
    explicit BatteryFactGroup(uint32_t batteryId, QObject *parent = nullptr);

    /// Voltage-only state of charge for the hardcoded 6S Li-ion pack. Blends the resting and
    /// in-flight curves by load: reported current if available (NaN = not reported), else throttle.
    /// Returns NaN when packVoltage is NaN or implausibly low.
    static double estimatePercentRemaining(double packVoltage, double currentAmps, double throttlePct);

    Fact *function() { return &_batteryFunctionFact; }
    Fact *type() { return &_batteryTypeFact; }
    Fact *voltage() { return &_voltageFact; }
    Fact *percentRemaining() { return &_percentRemainingFact; }
    Fact *percentRemainingEstimated() { return &_percentRemainingEstimatedFact; }
    Fact *mahConsumed() { return &_mahConsumedFact; }
    Fact *current() { return &_currentFact; }
    Fact *temperature() { return &_temperatureFact; }
    Fact *instantPower() { return &_instantPowerFact; }
    Fact *timeRemaining() { return &_timeRemainingFact; }
    Fact *timeRemainingStr() { return &_timeRemainingStrFact; }
    Fact *chargeState() { return &_chargeStateFact; }

    // Overrides from FactGroup
    void handleMessage(Vehicle *vehicle, const mavlink_message_t &message) final;

private slots:
    void _timeRemainingChanged(const QVariant &value);

private:
    friend class BatteryFactGroupTest;

    static double _inFlightWeight(double currentAmps, double throttlePct);

    void _handleHighLatency(Vehicle *vehicle, const mavlink_message_t &message);
    void _handleHighLatency2(Vehicle *vehicle, const mavlink_message_t &message);
    void _handleBatteryStatus(Vehicle *vehicle, const mavlink_message_t &message);

    Fact _batteryFunctionFact = Fact(0, QStringLiteral("batteryFunction"), FactMetaData::valueTypeUint8);
    Fact _batteryTypeFact = Fact(0, QStringLiteral("batteryType"), FactMetaData::valueTypeUint8);
    Fact _voltageFact = Fact(0, QStringLiteral("voltage"), FactMetaData::valueTypeDouble);
    Fact _currentFact = Fact(0, QStringLiteral("current"), FactMetaData::valueTypeDouble);
    Fact _mahConsumedFact = Fact(0, QStringLiteral("mahConsumed"), FactMetaData::valueTypeDouble);
    Fact _temperatureFact = Fact(0, QStringLiteral("temperature"), FactMetaData::valueTypeDouble);
    Fact _percentRemainingFact = Fact(0, QStringLiteral("percentRemaining"), FactMetaData::valueTypeDouble);
    Fact _percentRemainingEstimatedFact =
        Fact(0, QStringLiteral("percentRemainingEstimated"), FactMetaData::valueTypeBool);
    Fact _timeRemainingFact = Fact(0, QStringLiteral("timeRemaining"), FactMetaData::valueTypeDouble);
    Fact _timeRemainingStrFact = Fact(0, QStringLiteral("timeRemainingStr"), FactMetaData::valueTypeString);
    Fact _chargeStateFact = Fact(0, QStringLiteral("chargeState"), FactMetaData::valueTypeUint8);
    Fact _instantPowerFact = Fact(0, QStringLiteral("instantPower"), FactMetaData::valueTypeDouble);

    // Pack voltage recovers for several seconds after the load drops, so the last loaded estimate is held
    // for _settleTimeMs before the resting curve takes over.
    double _lastLoadedEstimate = qQNaN();
    QElapsedTimer _settleTimer;
    int _settleTimeMs = 15000;
};
