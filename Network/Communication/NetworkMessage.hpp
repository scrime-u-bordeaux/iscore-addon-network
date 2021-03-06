#pragma once
#include <QByteArray>
#include <QString>
#include <QMetaType>
#include <score/model/Identifier.hpp>
#include <wobjectdefs.h>

class QDataStream;
namespace Network
{
class Client;
class Session;

struct NetworkMessage
{
        friend QDataStream& operator<<(QDataStream& s, const NetworkMessage& m);
        friend QDataStream& operator>>(QDataStream& s, NetworkMessage& m);

        NetworkMessage() = default;
        NetworkMessage(Session& s, QByteArray&& addr, QByteArray&& data);

        QByteArray address;
        Id<Session> sessionId;
        Id<Client> clientId;
        QByteArray data;
};
}

Q_DECLARE_METATYPE(Network::NetworkMessage)
W_REGISTER_ARGTYPE(Network::NetworkMessage)
