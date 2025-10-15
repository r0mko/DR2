TEMPLATE = app

QT += qml qml_private quick widgets
CONFIG += c++11 x86
CONFIG -= x86_64
SOURCES += main.cpp

#RESOURCES += qml.qrc
#INCLUDEPATH += /System/Library/Frameworks/Carbon.framework/Headers
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
#include(deployment.pri)

HEADERS +=

OTHER_FILES += \
    qml/main.qml \
    qml/test.qml \
    qml/topo.qml \
    qml/testrpc/PeerDelegate.qml \
    qml/testrpc/PeerList.qml \
    qml/testrpc/PeerListItem.qml \
    qml/testrpc/PuppetDelegate.qml \
    qml/testrpc/TestObject.qml \
    qml/theme/Theme.qml \
    qml/widgets/AddPeerForm.qml \
    qml/widgets/Dial.qml \
    qml/widgets/FormLine.qml \
    qml/widgets/GlossyButton.qml \
    qml/widgets/InfoPanel.qml \
    qml/widgets/ListWidget.qml \
    qml/widgets/LogMsgForm.qml \
    qml/widgets/LogViewer.qml \
    qml/widgets/ThinToolbar.qml \
    qml/widgets/ThinToolButton.qml \
    qml/theme/qmldir \
    qml/PropertyDelegate.qml \
    qml/Dummi.qml \
    qml/MethodDelegate.qml \
    qml/delegates/ItemDelegate.qml \
    qml/delegates/PropertyDelegate.qml \
    qml/delegates/MethodDelegate.qml


RESOURCES += \
    rc.qrc
