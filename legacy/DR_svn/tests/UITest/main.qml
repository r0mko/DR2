import QtQuick 2.3
import QtQuick.Window 2.2
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Window {
    id: window
    visible: true
    width: 120
    height: 800

    property int substeps: 3
    property var palette: Theme.palette.midSeq

    Component.onCompleted: {
        window.show()
        console.log("initialized")
        for (var i = 0; i < palette.length; ++i) {
            list.col1 = palette[i]
            list.col2 = palette[(i + 1) % palette.length]
            for (var j = 0; j < substeps; ++j) {
                list.col2.a = j / substeps
                var col3 = Qt.tint(list.col1, list.col2)
                list.append({
                                colorValue: col3.toString()
                            })
            }
        }
    }
    onClosing: {
        var p = []
        for (var i = 0; i < list.count; ++i) {
            p.push(list.get(i).colorValue)
        }
        console.log(p, p.length)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: list
            clip: true
            delegate: cellcomp
        }
    }

    ListModel {
        property color col1
        property color col2

        id: list
    }

    Component {
        id: cellcomp
        Rectangle {
            width: parent.width
            height: 20
            color: colorValue
            Text {
                anchors.centerIn: parent
                text: parent.color
                //                text: JSON.stringify(modelData)
            }
        }
    }
}
