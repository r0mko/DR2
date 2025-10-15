import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Window {
    id: root
    modality: Qt.ApplicationModal
    //    flags: Qt.ToolTip
    //    color: 'transparent'
    color: '#e0666666'
    //    width: childrenRect.width
    //    height: childrenRect.height
    width: 320
    height: 200
    property string type
    //    Window.activeFocusItem: ttt










    //    Rectangle {
    //        id: cont
    //        width: 320
    //        height: 200
    //        opacity: 0.9
    //        radius: 10
    //        border.color: '#555'
    //        border.width: 2
    ////        Loader {
    ////            id: loader
    ////            source: 'filter_' + root.type + '.qml'
    ////// load filter based on filter setting for the field
    ////// string | none | dateRange
    ////        }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 5
        Label {
            text: 'filter by text'
        }
        TextField {
            id: ttt
            Layout.fillWidth: true
            placeholderText: 'filter'
        }
    }

    //        Button {
    //            anchors.top: parent.top
    //            anchors.right: parent.right
    //            anchors.margins: 5
    //            onClicked: root.close()
    //            height: 20
    //            width: height
    //            iconSource: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAACTwAAAk8B95E4kAAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAB9SURBVDiNrdMxDsQwCATAUd6RZ/ndrtL5KSm4xpFyJwVOcixR2LALrEFEQEPHHhEyw44Dbd41nAiMjGSCx4w9J1afD5GR/IAv60+OL5Ispg4oElQljrK6P/pM9amULsXdrJ7XWlgSMfuqzHeN8togeWGU15bpts5HBn5a5w+UaViixuw2kAAAAABJRU5ErkJggg7ef1635f4c172d8ac1b3318806e154b6"
    //        }
    //    }
}

