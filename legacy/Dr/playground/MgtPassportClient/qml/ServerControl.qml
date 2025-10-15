import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import Test 1.0
Tab {
    id: root
    title: 'Управление сервером'

    RowLayout {
        anchors.fill: parent
        spacing: 0
        Rectangle {
            width: 300
            color: '#00000000'
            Layout.fillHeight: true
            //            ScrollView {
            //                anchors.fill: parent
            //Rectangle {
            //    color: '#40ff6666'
            //    anchors.fill: parent
            //    width: parent.width
            //    height: 800
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                GroupBox {

                    Layout.fillWidth: true
                    title: 'Обновление справочников'
                    ColumnLayout {
                        width: parent.width
                        BigButton {
                            text: 'Загрузить справочник\nОМКУМ_Р1 из .dbf файла...'
                            //                    onClicked: service.refreshServerSettings()
                            Layout.fillWidth: true
                            fontSize: uiStyle.fontSizeNormal //14
                            margin: 5
                        }
                        BigButton {
                            text: 'Загрузить справочник\nОМКУМ_Р2 из .dbf файла...'
                            //                    onClicked: service.refreshServerSettings()
                            Layout.fillWidth: true
                            fontSize: uiStyle.fontSizeNormal //14
                            margin: 5
                        }
                    }
                }
                GroupBox {

                    Layout.fillWidth: true
                    title: 'Управление сервером'
                    ColumnLayout {
                        width: parent.width
                        //                        BigButton {
                        //                            text: 'Обновить настройки\nисточников данных'
                        //                            onClicked: service.refreshServerSettings()
                        //                            Layout.fillWidth: true
                        //                            fontSize: uiStyle.fontSizeNormal //14
                        //                            margin: 5
                        //                        }
                        BigButton {
                            text: 'Остановить сервер'
                            onClicked: service.stopServer()
                            Layout.fillWidth: true
                            fontSize: uiStyle.fontSizeNormal //14
                            margin: 5
                        }
                        BigButton {
                            text: 'Тест печати'
                            //                    onClicked: printView.showPrintDialog()
                            Layout.fillWidth: true
                            fontSize: uiStyle.fontSizeNormal //14
                            margin: 5
                        }
                    }
                }
                /**
                GroupBox {
                    Layout.fillWidth: true
                    title: 'Настройки соединения'
                    ColumnLayout {
                        width: parent.width
                        Label {
                            font.pixelSize: uiStyle.fontSizeNormal //16
                            text: 'Адрес сервера:'
                        }
                        TextField {
                            Layout.fillWidth: true
                            text: '127.0.0.1'
                        }
                        Label {
                            font.pixelSize: uiStyle.fontSizeNormal //16
                            text: 'Порт:'
                        }
                        SpinBox {
                            Layout.fillWidth: true
                            value: 33667
                            maximumValue: 65535
                            minimumValue: 1
                        }
                        BigButton {
                            text: 'Применить настройки'
    //                        onClicked: printView.showPrintDialog()
                            Layout.fillWidth: true
                            fontSize: uiStyle.fontSizeNormal //14
                            margin: 5
                        }
                    }
                }/**/
                GroupBox {

                    Layout.fillWidth: true
                    title: 'Настройки резервирования'
                    ColumnLayout {
                        width: parent.width
                        Label {
                            font.pixelSize: uiStyle.fontSizeNormal //16
                            text: 'Интервал в часах:'
                        }
                        SpinBox {
                            Layout.fillWidth: true
                            value: 24
                            suffix: ' ч.'
                            maximumValue: 24 * 7
                            minimumValue: 1
                        }
                        Label {
                            font.pixelSize: uiStyle.fontSizeNormal //16
                            text: 'количество резервных копий:'
                        }
                        SpinBox {
                            Layout.fillWidth: true
                            value: 20
                            maximumValue: 1000
                            minimumValue: 0
                        }
                        BigButton {
                            text: 'Применить настройки'
                            //                        onClicked: printView.showPrintDialog()
                            Layout.fillWidth: true
                            fontSize: uiStyle.fontSizeNormal //14
                            margin: 5
                        }
                    }
                }
                GroupBox {
                    Layout.fillWidth: true
                    title: 'Восстановление данных'
                    ColumnLayout {
                        width: parent.width
                        BigButton {
                            text: 'Восстановить базу данных\nиз резервной копии...'
                            Layout.fillWidth: true
                            fontSize: uiStyle.fontSizeNormal //14
                            margin: 5

                        }
                    }
                }
                GroupBox {
                    Layout.fillWidth: true
                    title: 'Расчёт'
                    ColumnLayout {
                        width: parent.width
                        BigButton {
                            text: 'Расчёт'
                            Layout.fillWidth: true
                            fontSize: uiStyle.fontSizeNormal //14
                            margin: 5
                            onClicked: www.show()

                            Calculator {
                                id: www
                            }
                        }
                    }
                }
                Item {
                    Layout.fillHeight: true
                }
                //}
                //            }

            }

        }
        Rectangle {
            width: 1
            Layout.fillHeight: true
            color: '#444'
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: 'transparent'


            /**/

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                Rectangle {
                    height: 36
                    color: 'transparent'
                    Layout.fillWidth: true
                    Text {
                        anchors.fill: parent
                        text: 'Управление пользователями'
                        font.pixelSize: uiStyle.fontSizeMed //20
                    }
                }
                Table{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    __dataModel: usersModel
                }
            }

/**/

        }
    }
}
