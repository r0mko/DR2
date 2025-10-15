import QtQuick 2.3
import QtQuick.Controls 1.2
import Qt.labs.settings 1.0
import Dr.QRPC 1.0
import DataReactor.UI 1.0
import QtQuick.Layouts 1.1
import "theme" 1.0
import "widgets"
import "delegates"

ApplicationWindow {
    id: appwindow
    visible: true
    width: 1080
    height: 760
    flags: Qt.Tool


//    Component.onCompleted: scanner.object = sym1

    Dummi {
        id: dum
        width: panel1.width
    }


    SplitView {
        anchors.fill: parent
        anchors.margins: 8
        orientation: Qt.Horizontal
        //        spacing: 8
        ColumnLayout {
            id: panel1
            width: 300
            Layout.fillWidth: false
            Layout.preferredWidth: 300

            ListWidget {
                id: propList
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: scanner.properties
                delegate: PropertyDelegate {
                    id: prop_delegate
                    sourceObj: scanner.object

                    SequentialAnimation {
                        id: heightAnim;
                        property alias to: anim1.to
                        running: false

                        PauseAnimation { duration: propertyIndex * 3 }

                        NumberAnimation {
                            id: anim1
                            target: prop_delegate;
                            property: "height";
                            duration: 350;
                            easing.type: Easing.OutCubic
                        }
                        onStopped: {
                            propList.scrollArea.resizeContent(propList.listItem.width, propList.listItem.height, 0)
                            propList.scrollArea.returnToBounds()
                            //                                        console.log("Finished resizing")
                        }
                    }

                    property bool display: (propertyIndex >= scanner.propertyOffset) || !showSuper1.checked

                    onDisplayChanged: {
                        if (display) {
                            heightAnim.to = defaultSize;
                            heightAnim.restart()
                        } else {
                            heightAnim.to = 0
                            heightAnim.restart()
                        }
                    }
                }
           }

            ThinToolbar {
                Layout.fillWidth: true
                height: 20
                ThinToolButton {
                    id: showSuper1
                    width: 30
                    checkable: true
                    Symbol {
                        anchors.fill: parent
                        font: Symbol.FontAwesome
                        symbol: parent.checked ? FontAwesome.ChevronDown : FontAwesome.ChevronUp
                    }
                }
            }

        }

        ColumnLayout {
            Layout.preferredWidth: 300
            Layout.fillWidth: true
            anchors.margins: 4
            ListWidget {
                id: mview
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: scanner.methods
                delegate: MethodDelegate {

                }

            }
            ThinToolbar {
                Layout.fillWidth: true
                height: 20
                ThinToolButton {
                    id: showSuper2
                    width: 30
                    checkable: true
                    Symbol {
                        anchors.fill: parent
                        font: Symbol.FontAwesome
                        symbol: parent.checked ? FontAwesome.ChevronDown : FontAwesome.ChevronUp
                    }
                }
            }

        }


    }
    MetaObject {
        id: scanner
        object: panel1
    }



    property var model: [sym1, sym1.font, appwindow, 5, 8.24, true,  UI.Entypo, "model", {foo: "bar", baz: true}]
    Symbol {
        id: sym1
        pixelSize: 50
        font: Symbol.FontAwesome
        symbol: FontAwesome.Eject
        visible: false
        ListView.delayRemove: true
        Keys.enabled: true
    }

}
