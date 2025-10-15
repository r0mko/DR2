import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

TextField {
    id: root
    property alias date: c.date


    text: Qt.formatDate( c.date, 'dd.MM.yyyy' )
    implicitWidth: 300
    inputMask: '00.00.0000'
    placeholderText: 'дд.мм.гггг'
    onAccepted: c.date = Date.fromLocaleString( Qt.locale(), text, 'dd.MM.yyyy' )
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
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    cWindow.x = root.mapToItem( null, 0, 0 ).x
                    cWindow.y = root.mapToItem( null, 0, 0 ).y
                    cWindow.show()
                }
            }
        }
    }
    Window {
        id: cWindow
        width: c.width
        height: c.height
        CalendarComponent {
            id: c
            labelVisible: false
            onDateChanged: parent.text = Qt.formatDate( c.date, 'dd.MM.yyyy г.' )
        }
    }
}
