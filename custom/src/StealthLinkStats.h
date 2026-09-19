#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtQmlIntegration/QtQmlIntegration>

#include "QGCMAVLink.h"

/// Measures MAVLink round-trip latency to the active vehicle by sending PING
/// requests once a second and timing the vehicle's PING response.
/// Exposed to QML as the StealthLink singleton.
class StealthLinkStats : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int  latencyMs   READ latencyMs  NOTIFY latencyChanged)
    Q_PROPERTY(bool latencyValid READ latencyValid NOTIFY latencyChanged)

public:
    explicit StealthLinkStats(QObject *parent = nullptr);

    /// Starts pinging. Called once the application (vehicle manager, links) is initialised.
    void start() { _pingTimer.start(); }

    int latencyMs() const { return _latencyMs; }
    bool latencyValid() const { return _latencyValid; }

    /// Called from the core plugin's mavlinkMessage hook for every received message.
    void handleMessage(const mavlink_message_t &message);

signals:
    void latencyChanged();

private slots:
    void _sendPing();

private:
    QTimer  _pingTimer;
    quint32 _seq = 0;
    qint64  _lastRequestUsec = 0;
    int     _latencyMs = 0;
    bool    _latencyValid = false;
    int     _missedReplies = 0;
};
