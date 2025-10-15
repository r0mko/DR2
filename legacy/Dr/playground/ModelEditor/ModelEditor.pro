TEMPLATE = app

QT += qml quick widgets qml-private core-private core
#QT += qml quick widgets
CONFIG += c++11

#RESOURCES += qml.qrc
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

SOURCES += \
    main.cpp \
    handle.cpp

HEADERS += \
    handle.h

DISTFILES += \
    editor/ui.mgf \
    editor/rightpanel/typeeditor/BoolColumnDelegate.qml \
    editor/rightpanel/typeeditor/ClassCompleter.qml \
    editor/rightpanel/typeeditor/ColumnDelegate.qml \
    editor/rightpanel/typeeditor/TypeEditor.qml \
    editor/rightpanel/RP_DataNode.qml \
    editor/rightpanel/RP_Instance.qml \
    editor/rightpanel/RP_MetaNode.qml \
    editor/rightpanel/RP_Node.qml \
    editor/rightpanel/RP_Blob.qml \
    editor/rightpanel/RP_Type.qml \
    editor/rightpanel/DefaultDelegateWrapper.qml \
    editor/treeview/GraphTreeView.qml \
    editor/treeview/NodeIcon.qml \
    editor/treeview/TV_Blob.qml \
    editor/treeview/TV_Collection.qml \
    editor/treeview/TV_DataNode.qml \
    editor/treeview/TV_Instance.qml \
    editor/treeview/TV_MetaNode.qml \
    editor/treeview/TV_Node.qml \
    editor/treeview/TV_Type.qml \
    editor/AuxModel.qml \
    editor/main.qml \
    editor/rightpanel/ObjectEditor.qml \
    editor/rightpanel/RP_Collection.qml \
    editor/rightpanel/RP_Element.qml \
    editor/treeview/TV_Element.qml \
    editor/rightpanel/RP_SqlBridgeNode.qml \
    editor/treeview/GraphTreeSelectionModel.qml \
    editor/treeview/TreeNodeDelegate.qml \
    editor/rightpanel/RP_Multi.qml \
    editor/treeview/TV_PointNode.qml \
    editor/rightpanel/RP_PointNode.qml

copydata.commands = $(COPY_DIR) $$PWD/editor $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)

QMAKE_EXTRA_TARGETS += first copydata
DIRNAME = ROOTPATH=\\\"$$OUT_PWD\\\"
DEFINES += $$DIRNAME
