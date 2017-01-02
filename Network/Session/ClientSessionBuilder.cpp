#include <iscore/serialization/DataStreamVisitor.hpp>
#include <QDataStream>
#include <QIODevice>
#include <sys/types.h>

#include "ClientSession.hpp"
#include "ClientSessionBuilder.hpp"
#include <Network/Communication/NetworkMessage.hpp>
#include <Network/Communication/NetworkSocket.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/model/Identifier.hpp>
#include <core/presenter/Presenter.hpp>
#include <core/presenter/DocumentManager.hpp>
#include <core/document/Document.hpp>
#include <core/command/CommandStackSerialization.hpp>
#include <Network/Client/LocalClient.hpp>
#include <Network/Client/RemoteClient.hpp>
#include <Network/Document/DocumentPlugin.hpp>
#include <Network/Document/ClientPolicy.hpp>
#include <iscore/plugins/documentdelegate/DocumentDelegateFactory.hpp>
#include <iscore/application/ApplicationContext.hpp>
namespace Network
{
ClientSessionBuilder::ClientSessionBuilder(
        const iscore::GUIApplicationContext& ctx,
        QString ip,
        int port):
    m_context{ctx}
{
    m_mastersocket = new NetworkSocket(ip, port, nullptr);
    connect(m_mastersocket, &NetworkSocket::messageReceived,
            this, &ClientSessionBuilder::on_messageReceived);
}

void ClientSessionBuilder::initiateConnection()
{
    // Todo only call this if the socket is ready.
    NetworkMessage askId;
    askId.address = "/session/askNewId";
    {
        QDataStream s{&askId.data, QIODevice::WriteOnly};
        s << m_clientName;
    }

    m_mastersocket->sendMessage(askId);
}

ClientSession*ClientSessionBuilder::builtSession() const
{
    return m_session;
}

QByteArray ClientSessionBuilder::documentData() const
{
    return m_documentData;
}

const std::vector<iscore::CommandData>& ClientSessionBuilder::commandStackData() const
{
    return m_commandStack;
}

void ClientSessionBuilder::on_messageReceived(const NetworkMessage& m)
{
    if(m.address == "/session/idOffer")
    {
        m_sessionId.setVal(m.sessionId); // The session offered
        m_masterId.setVal(m.clientId); // Message is from the master
        QDataStream s(m.data);
        int32_t id;
        s >> id; // The offered client id
        m_clientId = Id<Client>(id);

        NetworkMessage join;
        join.address = "/session/join";
        join.clientId = m_clientId.val();
        join.sessionId = m.sessionId;

        m_mastersocket->sendMessage(join);
    }
    else if(m.address == "/session/document")
    {
        auto remoteClient = new RemoteClient(m_mastersocket, m_masterId);
        remoteClient->setName("RemoteMaster");
        m_session = new ClientSession(remoteClient,
                                      new LocalClient(m_clientId),
                                      m_sessionId,
                                      nullptr);
        m_session->localClient().setName(m_clientName);

        // We start building our document.
        DataStreamWriter writer{m.data};
        writer.m_stream >> m_documentData;

        // The SessionBuilder should have a saved document and saved command list.
        // However there is a difference with what happens when there is a crash :
        // Here the document is sent as it is in its current state. The CommandList only serves
        // in case somebody does undo, so that the computer who joined later can still
        // undo, too.

        iscore::Document* doc = m_context.documents.loadDocument(
                       m_context,
                       m_documentData,
                       *m_context.interfaces<iscore::DocumentDelegateList>().begin()); // TODO id instead

        if(!doc)
        {
            qDebug() << "Invalid document received";
            delete m_session;
            m_session = nullptr;

            emit sessionFailed();
            return;
        }

        iscore::loadCommandStack(
                    m_context.components,
                    writer,
                    doc->commandStack(),
                    [] (auto) { }); // No redo.

        auto& ctx = doc->context();
        auto& np = ctx.plugin<NetworkDocumentPlugin>();
        np.setPolicy(new ClientNetworkPolicy{m_session, ctx});

        emit sessionReady();
    }
}
}