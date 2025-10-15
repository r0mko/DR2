import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

Item {
    id: root
    width: childrenRect.width
    height: childrenRect.height
    property alias labelVisible: label.visible
    property alias labelText: label.text
    property alias maximumDate: calendar.maximumDate
    property alias minimumDate: calendar.minimumDate
    property alias date: calendar.selectedDate
    ColumnLayout {
        Label {
            id: label
            Layout.fillWidth: true
            text: 'конкретный день'
        }
        Calendar {
            id: calendar
            Layout.fillWidth: true
            Layout.fillHeight: true

            style: CalendarStyle {
                navigationBar: Item{
                    width: control.width
                    height: childrenRect.height + 10
                    RowLayout {
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        width: parent.width
                        Item{width: 10;Layout.fillHeight: true}
                        ComboBox {
                            id: months
                            implicitWidth: 100
                            model: [
                                'Январь',
                                'Февраль',
                                'Март',
                                'Апрель',
                                'Май',
                                'Июнь',
                                'Июль',
                                'Август',
                                'Сентябрь',
                                'Октябрь',
                                'Ноябрь',
                                'Декабрь'
                            ]
                            Layout.fillHeight: true
                            currentIndex: control.visibleMonth
                            onCurrentIndexChanged: {
//                                console.log( 'current index changed', currentIndex )
                                control.visibleMonth = currentIndex
                                currentIndex = Qt.binding( function(){ return control.visibleMonth} )
                            }
                        }
                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                        SpinBox {
                            minimumValue: 1901
                            maximumValue: 2050
                            Layout.fillHeight: true
                            value: control.visibleYear//Qt.formatDate( control.selectedDate, 'yyyy' )
                            onValueChanged: control.visibleYear = value
                        }
                        Item{width: 10;Layout.fillHeight: true}
                    }
                }
            }
        }
    }
}
