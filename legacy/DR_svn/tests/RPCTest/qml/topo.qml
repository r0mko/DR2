import QtQuick 2.3
import QtQuick.Controls 1.2
import Qt.labs.settings 1.0
import DataReactor.RPC 1.0
import DataReactor.UI 1.0
import QtQuick.Layouts 1.1
import "widgets"
import "delegates"

ApplicationWindow {
    id: appwindow
    visible: true
    width: 500
    height: 800
    flags: Qt.Tool

    ColumnLayout {
        id: panel1
        width: 300
        anchors.fill: parent

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


    MetaObject {
        id: scanner
        object: appwindow
    }

}
