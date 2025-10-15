import QtQuick 2.3
import QtQuick.Window 2.2
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import 'Targets' 1.0

Window {
    id: mainWindow
    width: 800
    height: 800
    title: 'Window title'
    readonly property bool test: true
    property bool test2: true
    property var w: mainWindow
    Component.onCompleted: {
        show()
        console.log(JSON.stringify(myTargets))
    }

    property var myTargets: [
        {
            "name":"propertyEditor",
            "delegates": [
                {
                    "component": UniTargets.propertyEditor
                }

            ]
        }

    ]

    SplitView{
        anchors.fill: parent
        smooth: true
        antialiasing: true
        Rectangle{
            smooth: true
            antialiasing: true
            width: 400
            height: parent.height
            color: '#333'
            Text{
                id: item
                width: 300
                height: 100
                text: 'Brown fox thingie'
                color: '#fff'
                font.bold: true
                font.family: 'Arial'
            }
        }
//Loader{
//                anchors.fill: parent
        UniDelegate {
            target: UniTargets.propertyEditor
            value: item
        }
//}

    }

}
