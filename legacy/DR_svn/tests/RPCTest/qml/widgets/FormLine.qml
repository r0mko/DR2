import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

RowLayout {
    property alias labelWidth: lbl_txt.width
    property alias label: lbl_txt.text
    property color labelColor: "#000000"
    property color textColor: "#000000"
    property alias text: textField1.text


    Label {
        id: lbl_txt
        width: 48
        Layout.minimumWidth: 48
        font {
            family: "HelveticaNeueCyr"
            weight: Font.Normal
            pointSize: 12
        }
        text: "Label"
        color: labelColor
    }

    TextField {
        id: textField1

        font {
            family: "HelveticaNeueCyr"
            weight: Font.Light
            pointSize: 12
        }
        Layout.fillWidth: true
        style: TextFieldStyle {
            textColor: "black"
            background: Rectangle {
                radius: 4
                color: control.activeFocus ? "white" : "transparent"
                border {
                    width: 1
                    color: control.activeFocus ? "#80000000" : control.hovered ? "#8000aaff" : "#30000000"
                }
            }

        }
    }



}
