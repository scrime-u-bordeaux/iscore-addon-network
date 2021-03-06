cmake_minimum_required(VERSION 3.0)
project(score_addon_network LANGUAGES CXX)

score_common_setup()

# Packages
find_package(Qt5 5.3 REQUIRED COMPONENTS Core Widgets Network WebSockets Xml Test)

# Files & main target
set(HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Client/Client.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Client/LocalClient.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Client/RemoteClient.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/MessageMapper.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/MessageValidator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/NetworkMessage.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/NetworkServer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/NetworkSocket.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/ClientPolicy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/DocumentPlugin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/MasterPolicy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Timekeeper.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/NTP.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/PlaceholderPolicy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/BasicPruner.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/DateExpression.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/SyncMode.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/Context.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/FreeScenarioPolicy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/SharedScenarioPolicy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/SharedNonCompensatedExpressions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/SharedCompensatedExpressions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/MixedScenarioPolicy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/MasterPolicy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/SlavePolicy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/AddClientToGroup.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/ChangeGroup.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/CreateGroup.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/DistributedScenarioCommandFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/RemoveClientFromGroup.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/RemoveGroup.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/AddCustomMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Group.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/NetworkActions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupManager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupMetadataWidget.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/GroupPanelDelegate.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/GroupPanelFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupHeaderItem.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupListWidget.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupTableCheckbox.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupTableWidget.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupWidget.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/SessionHeaderItem.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/IpDialog.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/IpWidget.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/NetworkApplicationPlugin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/PlayerPlugin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/ClientSessionBuilder.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/PlayerSessionBuilder.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/ClientSession.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/MasterSession.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/RemoteClientBuilder.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/Session.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/commands/ClientNameChangedCommand.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/commands/ClientPortChangedCommand.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/commands/MasterPortChangedCommand.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/NetworkSettings.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/NetworkSettingsModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/NetworkSettingsPresenter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/NetworkSettingsView.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/score_addon_network.hpp"

    )
set(SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Client/ClientSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupManagerSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupMetadataSerialization.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/MessageMapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/MessageValidator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/NetworkMessage.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/NetworkServer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Communication/NetworkSocket.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/ClientPolicy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/DocumentPlugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/DocumentPluginSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/PlaceholderPolicy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/MasterPolicy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/NTP.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/BasicPruner.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/DateExpression.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/FreeScenarioPolicy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/SharedScenarioPolicy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/MixedScenarioPolicy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/MasterPolicy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Document/Execution/SlavePolicy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/AddClientToGroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/ChangeGroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/CreateGroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/DistributedScenarioCommandFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/RemoveClientFromGroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Commands/AddCustomMetadata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Group.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupMetadata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupMetadataWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/GroupSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/GroupPanelDelegate.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/GroupPanelFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupHeaderItem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupListWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupTableCheckbox.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupTableWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/GroupWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Group/Panel/Widgets/SessionHeaderItem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/IpDialog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/IpWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/NetworkApplicationPlugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/PlayerPlugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/ClientSessionBuilder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/PlayerSessionBuilder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/ClientSession.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/MasterSession.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/RemoteClientBuilder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Session/Session.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/NetworkSettingsModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/NetworkSettingsPresenter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Settings/NetworkSettingsView.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/score_addon_network.cpp"
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})
score_generate_command_list_file(${PROJECT_NAME} "${HDRS}")
# target_sources(ossia PRIVATE $<TARGET_OBJECTS:Servus>)
# get_target_property(SERVUS_LINK_LIBRARIES Servus INTERFACE_LINK_LIBRARIES)
# get_target_property(SERVUS_COMPILE_DEFINITIONS Servus INTERFACE_COMPILE_DEFINITIONS)
# target_link_libraries(${PROJECT_NAME} PUBLIC ${SERVUS_LINK_LIBRARIES})
# target_include_directories(${PROJECT_NAME} PUBLIC ${OSSIA_3RDPARTY_FOLDER}/Servus)
# target_compile_definitions(${PROJECT_NAME} PUBLIC ${SERVUS_COMPILE_DEFINITIONS})

#target_include_directories(${PROJECT_NAME} PRIVATE "${OSSIA_3RDPARY_DIRECTORY}/libzmq/include")
target_link_libraries(${PROJECT_NAME} PUBLIC
                                    score_lib_base
                                    score_plugin_scenario
                                    score_lib_process
                                    score_plugin_engine
                                    Qt5::Core
                                    Qt5::Widgets
                                    Qt5::Network
                                    Qt5::WebSockets
#                                    libzmq-static
                                    )


setup_score_plugin(${PROJECT_NAME})
