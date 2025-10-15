import QtQuick 2.5
import QtQuick.Controls 1.4
import Dr.Xvfb 1.0

Rectangle {
    anchors.fill: parent
    color: '#000'
    property alias fb: fbo
    /**
TabView {
    focus: true
    Tab {

        title: "QQuickImageProvider"
        Rectangle {
            id: imageRect
//            width: parent.width /2
//            height: parent.height
            anchors.fill: parent
            color: "black"
            Image {
                id: ximg
                property int currentFrameNumber: 0
                source: "image://XvfbImage/" + currentFrameNumber
                cache: false
                anchors.fill: parent

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        ximg.currentFrameNumber++
                    }
                }

                Connections {
                    target: processRunner
                    onStarted: {
                        console.log("repaint")
                        ximg.currentFrameNumber++
                    }
                }

                Keys.onPressed: {
                    X11Event.keyPress(event.key, event.modifiers, 1)
                    console.log( 'keys pressed' )
            //                    ximg.currentFrameNumber++
                }

                Keys.onReleased: {
                    X11Event.keyPress(event.key, event.modifiers, 0)
            //                    ximg.currentFrameNumber++
                }


            }
        }
    }
    Tab {
        title: "QQuickPaintedItem"
        Rectangle {
            id: itemRect
            width: parent.width /2
            height: parent.height
            color: "black"
            XvfbItem {
                id: xitem
                anchors.fill: parent
                MouseArea {
                    id: xitemMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.AllButtons
                    cursorShape: Qt.BlankCursor
                    onClicked: {
                        xitem.focus = true
                        xitem.forceActiveFocus()
                    }
                    onPositionChanged: {
                        X11Event.mouseMove(mouse.x, mouse.y, xitem.width, xitem.height)
                    }
                    onPressed: {
                        X11Event.mouseClick(mouse.buttons, 1)
                    }
                    onReleased: {
                        X11Event.mouseClick(mouse.buttons, 0)
                    }
                    onWheel: {
                        if (wheel.angleDelta.y > 0) {
                            X11Event.mouseClick(4, 1)
                            X11Event.mouseClick(4, 0)
                        } else {
                            X11Event.mouseClick(5, 1)
                            X11Event.mouseClick(5, 0)
                        }
                    }
                }

                Keys.onPressed: {
                    X11Event.keyPress(event.key, event.modifiers, 1);
                    event.accepted = true;
                }

                Keys.onReleased: {
                    X11Event.keyPress(event.key, event.modifiers, 0);
                    event.accepted = true;
                }
            }
        }
    }
/**/
    //    Tab {
    //        title: "QQuickFramebufferObject"

    XvfbRenderer {
        id: fbo
        anchors.fill: parent
        visible: false
        Component.onCompleted: {
            update()
            visible = true
        }
        MouseArea {
            id: fboMouse
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.AllButtons
            cursorShape: Qt.BlankCursor
            onClicked: {
                fbo.focus = true
                fbo.forceActiveFocus()
            }
            onPositionChanged: {
                X11Event.mouseMove(mouse.x, mouse.y, fbo.width, fbo.height)
            }
            onPressed: {
                X11Event.mouseClick(mouse.buttons, 1)
            }
            onReleased: {
                X11Event.mouseClick(mouse.buttons, 0)
            }
            onWheel: {
                if (wheel.angleDelta.y > 0) {
                    X11Event.mouseClick(4, 1)
                    X11Event.mouseClick(4, 0)
                } else {
                    X11Event.mouseClick(5, 1)
                    X11Event.mouseClick(5, 0)
                }
            }
        }

        Keys.onPressed: {
            X11Event.keyPress(event.key, event.modifiers, 1);
            event.accepted = true;
        }

        Keys.onReleased: {
            X11Event.keyPress(event.key, event.modifiers, 0);
            event.accepted = true;
        }
    }

    //    }
    /**/
}



