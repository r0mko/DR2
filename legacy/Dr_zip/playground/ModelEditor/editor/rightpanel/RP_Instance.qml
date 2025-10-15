import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Dr.UI 1.0
//import Dr.MetaGraph 1.0

RP_Node {

    CaptionLabel {
        Layout.fillWidth: true
        text: 'Instance specific stuff ' + objectData.typeName
        level: 3
    }

    Button {
        text: 'test'
        onClicked: {
            f_PointNode.test()
        }
    }
    Button {
        text: 'test2'
        onClicked: {
            f_PointNode.test2()
        }
    }
}

