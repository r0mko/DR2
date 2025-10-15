import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
FormGenericDelegate {
    id: root
    property alias labelText: l.text
    property var value: c.date
    property alias l: l
    property alias v: v
    //    property var value
    Label {
        id: l
        Layout.minimumWidth: __labelWidth
        font.pixelSize: __fontSize
    }
    RowLayout {
        Layout.minimumWidth: __widgetWidth
        Layout.fillWidth: true
        CheckBox {
            id: nullData
            checked: true
            onCheckedChanged: {
                if( nullData.checked )
                    root.value = Qt.binding( function(){ return c.date } )
                else
                    root.value = undefined
            }
        }

        TextField {
            visible: nullData.checked

            id: v
            Layout.fillWidth: true
            inputMethodHints: Qt.ImhDate
            inputMask: '00.00.0000'
            text: Qt.formatDate( root.value, 'dd.MM.yyyy' )
            Rectangle {
                anchors.right: im.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                height: parent.height
                color: '#888'
            }
            Item {
                id: im
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: parent.height
                Image {
                    anchors.centerIn: parent
                    opacity: 0.5
                    source: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQBAMAAADt3eJSAAAAA3NCSVQICAjb4U/gAAAACXBIWXMAAAbsAAAG7AEedTg1AAAAGXRFWHRTb2Z0d2FyZQB3d3cuaW5rc2NhcGUub3Jnm+48GgAAAC1QTFRF////AAAAAAAAAAAEAAADAAACAAACAgACAgACAgACAQABAQADAQACAQACAQACLfu+0QAAAA50Uk5TAA8oQGB6f4CKj7/A3PokJ2IeAAAAR0lEQVQIW2NgYHEAQQYG1axlIBjEcLJ6OwjOYXgAkeJjeMAABsgM9ud8D+oKgAzmYvYCcwNkBlwKj2KY1EUIQ5ah9x0Y3AAA6eEotqwoVwgAAAAASUVORK5CYII8f0ac36c431d50f11ec7542dbec591cb'
                }
                MouseArea {
                    enabled: nullData
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        cWindow.show()
                    }
                }
                Window {
                    id: cWindow
                    width: c.width
                    height: c.height
                    CalendarComponent {
                        id: c
                        labelVisible: false
                        onDateChanged: v.text = Qt.formatDate( c.date, 'dd.MM.yyyy' )

                    }
                }
            }
        }
        onVisibleChanged: if( !visible )
                          {
                              if( cWindow.visible )
                                  cWindow.close()


                          }
    }
}
