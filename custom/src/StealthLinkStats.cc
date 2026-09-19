#include "StealthLinkStats.h"
#include "LinkInterface.h"
#include "MAVLinkProtocol.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

#include <QtCore/QDateTime>

namespace {
constexpr int kPingIntervalMs = 1000;
constexpr int kMissedRepliesBeforeInvalid = 3;
}

StealthLinkStats::StealthLinkStats(QObject *parent)
    : QObject(parent)
{
    _pingTimer.setInterval(kPingIntervalMs);
    (void) connect(&_pingTimer, &QTimer::timeout, this, &StealthLinkStats::_sendPing);
}

void StealthLinkStats::_sendPing()
{
    Vehicle *vehicle = MultiVehicleManager::instance()->activeVehicle();
    if (!vehicle) {
        if (_latencyValid) {
            _latencyValid = false;
            emit latencyChanged();
        }
        return;
    }

    SharedLinkInterfacePtr sharedLink = vehicle->vehicleLinkManager()->primaryLink().lock();
    if (!sharedLink) {
        return;
    }

    if (_lastRequestUsec != 0 && ++_missedReplies >= kMissedRepliesBeforeInvalid && _latencyValid) {
        _latencyValid = false;
        emit latencyChanged();
    }

    _lastRequestUsec = QDateTime::currentMSecsSinceEpoch() * 1000;
    _seq++;

    mavlink_message_t msg;
    mavlink_msg_ping_pack_chan(static_cast<uint8_t>(MAVLinkProtocol::instance()->getSystemId()),
                               static_cast<uint8_t>(MAVLinkProtocol::getComponentId()),
                               sharedLink->mavlinkChannel(),
                               &msg,
                               static_cast<uint64_t>(_lastRequestUsec),
                               _seq,
                               0,   // target_system 0 + target_component 0 marks a ping request
                               0);
    (void) vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
}

void StealthLinkStats::handleMessage(const mavlink_message_t &message)
{
    if (message.msgid != MAVLINK_MSG_ID_PING) {
        return;
    }

    mavlink_ping_t ping;
    mavlink_msg_ping_decode(&message, &ping);

    // Responses carry our system id as the target; requests from the vehicle carry 0/0.
    if (ping.target_system != MAVLinkProtocol::instance()->getSystemId() || ping.seq != _seq) {
        return;
    }

    const qint64 nowUsec = QDateTime::currentMSecsSinceEpoch() * 1000;
    _latencyMs = static_cast<int>((nowUsec - static_cast<qint64>(ping.time_usec)) / 1000);
    _latencyValid = true;
    _missedReplies = 0;
    emit latencyChanged();
}
