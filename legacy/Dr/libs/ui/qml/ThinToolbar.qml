import QtQuick 2.3

Rectangle {
    implicitHeight: 20
    radius: 2
    border.width: 1
    border.color: "#bbb"
    default property alias data: row.children
    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#ffffff"
        }
        GradientStop {
            position: 0.44;
            color: "#f4f4f4";
        }
        GradientStop {
            position: 0.56;
            color: "#eaeaea";
        }
        GradientStop {
            position: 1.00;
            color: "#efefef";
        }
    }
    Row {
        id: row
        anchors.fill: parent
        spacing: -1
    }
}
