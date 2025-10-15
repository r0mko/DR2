import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import Dr.UI 1.0

Rectangle {
    id: root
    clip: true
    Layout.fillWidth: true

    default property Component content
    property alias title: title_label.text
    property real contentHeight: 300
    property alias active: loader_panel.active
    color: "#ddd"

    Behavior on implicitHeight { NumberAnimation { id: panel_animation; duration: 250; easing.type: Easing.OutQuint } }
    Component.onCompleted: state = "expanded"
    states: [
        State {
            name: "expanded"
            PropertyChanges {
                target: root
                implicitHeight: contentHeight + c_header.height
            }
            PropertyChanges {
                target: b_expand_collapse
                iconRotation: 0
                onClicked: root.state = "collapsed"
            }
        },
        State {
            name: "collapsed"
            PropertyChanges {
                target: root
                implicitHeight: c_header.height
            }
            PropertyChanges {
                target: b_expand_collapse
                iconRotation: 180
                onClicked: root.state = "expanded"
            }
        },
        State {
            name: "detached"
            when: panel_win.visible
            PropertyChanges {
                target: root
                visible: false
            }
            PropertyChanges {
                target: loader_panel
                active: false
                sourceComponent: null
            }
            PropertyChanges {
                target: panel_win
                visible: true
                content: root.content
            }
        }
    ]
    Rectangle {
        id: c_header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 18
        //        anchors.fill: parent
        color: aux.style.sidebarPanel.data.backgroundColor
        //        property int buttonSize: aux.style.sidebarPanel.data.buttonSize

        RowLayout {
            anchors.fill: parent
            anchors.margins: 1
            spacing: 1
            //            layoutDirection: Qt.RightToLeft

            VectorIconButton {
                id: b_menu
                iconVPadding: 3
                iconHPadding: 3
                width: 16
                height: 16
                iconColor: palette.light
                iconData: aux.icons.menu.data
                onClicked: { console.log("TODO: panel context menu")}
            }

            Label {
                id: title_label
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.verticalCenter: parent.verticalCenter
                //                anchors.centerIn: parent
                font.pixelSize: 12
                //                font.bold: true
                font.weight: Font.Light
                color: palette.light
                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        if( root.state === "expanded" )
                            root.state = "collapsed"
                        else
                            root.state = "expanded"
                    }
                }
            }
            VectorIconButton {
                id: b_detach
                iconVPadding: 1
                iconHPadding: 1
                width: 16
                height: 16
                iconData: aux.icons.detach.data
                iconColor: palette.light
                onClicked: {
//                    panel_win.width = root.width
//                    panel_win.height = loader_panel.height
                    panel_win.panelState = root.state
                    root.state = "detached"
                }
            }
            VectorIconButton {
                id: b_expand_collapse
                checkable: true
                iconVPadding: 2
                iconHPadding: 2
                iconColor: palette.light
                width: 16
                height: 16
                iconData: aux.icons.chevron_up.data
                Behavior on iconRotation { NumberAnimation { duration: panel_animation.duration; easing: panel_animation.easing } }
            }
        }
    }
    Rectangle {
        id: underline
        anchors.fill: loader_panel
        anchors.bottomMargin: parent.anchors.bottomMargin - 2
        color: "transparent"
        border.color: "#888"
    }


    Loader {
        id: loader_panel
        sourceComponent: content
        anchors.top: c_header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 3
        clip: true
    }

    Window {
        id: panel_win
        flags: Qt.Tool
        property alias content: loader_win.sourceComponent
        property string panelState
        onClosing: root.state = panelState
        color: root.color
        title: root.title
        Loader {
            id: loader_win
            active: panel_win.visible
            anchors.fill: parent
            anchors.bottomMargin: 1
        }
    }
    SystemPalette { id: palette }
}

