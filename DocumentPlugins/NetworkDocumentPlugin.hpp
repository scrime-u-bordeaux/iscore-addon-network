#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <QObject>
#include <vector>

#include <iscore/plugins/documentdelegate/plugin/ElementPluginModel.hpp>

#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <core/document/Document.hpp>

class DataStream;
class JSONObject;
class QWidget;
struct VisitorVariant;

namespace iscore
{
    class Document;
}

namespace Network
{
class Session;
class GroupManager;
class GroupMetadata;
class NetworkPolicyInterface : public QObject
{
    public:
        using QObject::QObject;
        virtual Session* session() const = 0;
};

class NetworkDocumentPlugin final :
        public iscore::SerializableDocumentPlugin
{
        Q_OBJECT

        ISCORE_SERIALIZE_FRIENDS(NetworkDocumentPlugin, DataStream)
        ISCORE_SERIALIZE_FRIENDS(NetworkDocumentPlugin, JSONObject)
    public:
        NetworkDocumentPlugin(NetworkPolicyInterface* policy, iscore::Document& doc);

        // Loading has to be in two steps since the plugin policy is different from the client
        // and server.
        NetworkDocumentPlugin(const VisitorVariant& loader, iscore::Document& doc);

        void setPolicy(NetworkPolicyInterface*);

        GroupManager* groupManager() const
        { return m_groups; }

        NetworkPolicyInterface* policy() const
        { return m_policy; }

    signals:
        void sessionChanged();

    private:
        std::vector<iscore::ElementPluginModelType> elementPlugins() const override;

        iscore::ElementPluginModel* makeElementPlugin(
                const QObject* element,
                iscore::ElementPluginModelType,
                QObject* parent) override;

        iscore::ElementPluginModel* loadElementPlugin(
                const QObject* element,
                const VisitorVariant&,
                QObject* parent) override;

        iscore::ElementPluginModel* cloneElementPlugin(
                const QObject* element,
                iscore::ElementPluginModel*,
                QObject* parent) override;

        virtual QWidget *makeElementPluginWidget(
                const iscore::ElementPluginModel*, QWidget* widg) const override;

        void serialize_impl(const VisitorVariant&) const override;
        ConcreteFactoryKey concreteFactoryKey() const override;

        void setupGroupPlugin(GroupMetadata* grp);

        NetworkPolicyInterface* m_policy{};
        GroupManager* m_groups{};

};

class DocumentPluginFactory :
        public iscore::DocumentPluginFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("58c9e19a-fde3-47d0-a121-35853fec667d")

    public:
        iscore::DocumentPlugin* load(
                const VisitorVariant& var,
                iscore::DocumentContext& doc,
                QObject* parent) override
        {
            return new NetworkDocumentPlugin{var, doc.document}; // TODO Smell
        }
};
}
