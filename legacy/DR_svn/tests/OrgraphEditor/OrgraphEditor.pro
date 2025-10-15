TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.

OTHER_FILES += \
    GraphView/NodeDelegate.qml \
    orgraph.qml \
    Node.qml \
    ListView/NodeDelegate.qml \
    overhead_net.osm.xml \
    ways.xml \
    GraphView/EdgeDelegate.qml \
    Editor/ToolBase.qml \
    Editor/NodeEditorTool.qml \
    Editor/Drawboard.qml \
    Editor/EdgeEditorTool.qml \
    Editor/NodesView.qml \
    Editor/MouseHandler.qml
