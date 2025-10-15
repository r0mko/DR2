import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2

import Dr.Graphics 1.0
import Dr.UI 1.0
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0

Window {
    id: diupWindow
    width: 480
    height: 480
    visible: false
    title: 'Управление секционным изолятором'
    color: '#eee'
    property BoundaryObject boundary: null
    property var prevSection
    property var nextSection
    property bool isTram: false
    property bool isTroll: false
    property string linkTitle
    property string prevTitle
    property string nextTitle

    onPrevSectionChanged: {
        if (prevSection.data.type === "TramSection") {
            isTroll = false
            isTram = true
            prevTitle = prevSection.data.posCableId
            nextTitle = nextSection.data.posCableId
            linkTitle = "%1, %2".arg(prevTitle).arg(nextTitle)

        } else if (prevSection.data.type === "TrolleySection") {
            isTroll = true
            isTram = false
            prevTitle = "%1/%2".arg(prevSection.data.posCableId).arg(prevSection.data.negCableId)
            nextTitle = "%1/%2".arg(nextSection.data.posCableId).arg(nextSection.data.negCableId)
            linkTitle = "%1, %2".arg(prevTitle).arg(nextTitle)
        } else {
            isTroll = false
            isTram = false
        }
    }
    onNextSectionChanged: {
        if (prevSection.data.type === "TramSection") {
            isTroll = false
            isTram = true
            prevTitle = prevSection.data.posCableId
            nextTitle = nextSection.data.posCableId
            linkTitle = "%1, %2".arg(prevTitle).arg(nextTitle)

        } else if (prevSection.data.type === "TrolleySection") {
            isTroll = true
            isTram = false
            prevTitle = "%1/%2".arg(prevSection.data.posCableId).arg(prevSection.data.negCableId)
            nextTitle = "%1/%2".arg(nextSection.data.posCableId).arg(nextSection.data.negCableId)
            linkTitle = "%1, %2".arg(prevTitle).arg(nextTitle)
        } else {
            isTroll = false
            isTram = false
        }
    }
    property int picPadding: 3

    onBoundaryChanged: boundaryNote.text = boundary.note

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        CaptionLabel {
            Layout.fillWidth: true
            text: "Соединение по КС каб. № %1".arg(linkTitle)
            level: 4
        }

        Label {
            text: 'Поле информационного сообщения'
        }
        TextArea {
            id: boundaryNote
            Layout.fillWidth: true
            Layout.minimumHeight: 60
            onTextChanged: diupWindow.boundary.note = text
        }
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            rows: 5
            columns: 3
            
            Label {
                Layout.fillWidth: true

                Layout.minimumHeight: 40
                text: 'Изоляторы'
            }
            Label {
                Layout.minimumHeight: 40
                Layout.maximumWidth:  92 + 2 * picPadding
                wrapMode: Text.WordWrap
                text: 'Режим питания\nот %1 к %2'.arg(prevTitle).arg(nextTitle)
            }
            Label {
                Layout.minimumHeight: 40
                Layout.maximumWidth:  92 + 2 * picPadding

                wrapMode: Text.WordWrap
                text: 'Режим питания\nот %1 к %2'.arg(nextTitle).arg(prevTitle)
            }
            Button {

                Layout.fillWidth: true
                text: 'Замкнуть по "+"'
                visible: isTroll || isTram
                enabled: diupWindow.boundary.posLink !== BoundaryObject.ClosedBreaker
                onClicked: {
                    diupWindow.boundary.posLink = BoundaryObject.ClosedBreaker
                    diupWindow.boundary.update()
                }
            }
            TinyButton {
                radius: 3
                implicitWidth: 92 + 2 * picPadding
                implicitHeight: 33 + 2 * picPadding
                flat: false
                visible: isTroll
                Image {
                    anchors.fill: parent
                    anchors.margins: 0
                    source: '../a2b-.png'
                }
                onClicked: {
                    diupWindow.boundary.negFeedPass = BoundaryObject.Out2In
                    diupWindow.boundary.posFeedPass = BoundaryObject.NoPass
                    diupWindow.boundary.update()
                }
            }
            TinyButton {
                radius: 3
                flat: false
                implicitWidth: 92 + 2 * picPadding
                implicitHeight: 33 + 2 * picPadding
                visible: isTroll
                Image {
                    anchors.fill: parent
                    anchors.margins: picPadding
                    source: '../b2a-.png'
                }
                onClicked: {
                    diupWindow.boundary.negFeedPass = BoundaryObject.In2Out
                    diupWindow.boundary.posFeedPass = BoundaryObject.NoPass
                    diupWindow.boundary.update()
                }
            }
            Button {
                Layout.fillWidth: true
                visible: isTroll
                text: 'Замкнуть по "—"'
                enabled: diupWindow.boundary.negLink !== BoundaryObject.ClosedBreaker
                onClicked: {
                    diupWindow.boundary.negLink = BoundaryObject.ClosedBreaker
                    diupWindow.boundary.update()
                }
            }

            TinyButton {
                radius: 3
                flat: false
                implicitWidth: 92 + 2 * picPadding
                implicitHeight: 33 + 2 * picPadding
                visible: isTroll || isTram
                Image {
                    anchors.fill: parent
                    anchors.margins: picPadding
                    source: '../a2b+.png'
                }
                onClicked: {
                    diupWindow.boundary.posFeedPass = BoundaryObject.Out2In
                    diupWindow.boundary.negFeedPass = BoundaryObject.NoPass
                    diupWindow.boundary.update()
                }
            }

            TinyButton {
                radius: 3
                flat: false
                implicitWidth: 92 + 2 * picPadding
                implicitHeight: 33 + 2 * picPadding
                visible: isTroll || isTram
                Image {
                    anchors.fill: parent
                    anchors.margins: picPadding
                    source: '../b2a+.png'
                }
                onClicked: {
                    diupWindow.boundary.posFeedPass = BoundaryObject.In2Out
                    diupWindow.boundary.negFeedPass = BoundaryObject.NoPass
                    diupWindow.boundary.update()
                }
            }
            Button {
                Layout.fillWidth: true
                text: 'Разомкнуть "+"'
                visible: isTroll || isTram
                enabled: diupWindow.boundary.posLink !== BoundaryObject.OpenBreaker
                onClicked: {
                    diupWindow.boundary.posLink = BoundaryObject.OpenBreaker
                    diupWindow.boundary.update()
                }
            }

            TinyButton {
                radius: 3
                flat: false
                implicitWidth: 92 + 2 * picPadding
                implicitHeight: 33 + 2 * picPadding
                visible: isTroll
                Image {
                    anchors.fill: parent
                    anchors.margins: picPadding
                    source: '../a2b1.png'
                }
                onClicked: {
                    diupWindow.boundary.posFeedPass = BoundaryObject.Out2In
                    diupWindow.boundary.negFeedPass = BoundaryObject.Out2In
                    diupWindow.boundary.update()
                }
            }

            TinyButton {
                radius: 3
                flat: false
                implicitWidth: 92 + 2 * picPadding
                implicitHeight: 33 + 2 * picPadding
                visible: isTroll
                Image {
                    anchors.fill: parent
                    anchors.margins: picPadding
                    source: '../b2a1.png'
                }
                onClicked: {
                    diupWindow.boundary.posFeedPass = BoundaryObject.In2Out
                    diupWindow.boundary.negFeedPass = BoundaryObject.In2Out
                    diupWindow.boundary.update()
                }
            }
            Button {
                Layout.fillWidth: true
                text: 'Разомкнуть "—"'
                visible: isTroll
                enabled: diupWindow.boundary.negLink !== BoundaryObject.OpenBreaker
                onClicked: {
                    diupWindow.boundary.negLink = BoundaryObject.OpenBreaker
                    diupWindow.boundary.update()
                }
            }

            TinyButton {
                radius: 3
                flat: false
                implicitWidth: 92 + 2 * picPadding
                implicitHeight: 33 + 2 * picPadding
                visible: isTroll
                Image {
                    anchors.fill: parent
                    anchors.margins: picPadding
                    source: '../a2b2.png'
                }
                onClicked: {
                    diupWindow.boundary.posOperation = BoundaryObject.OperationInProgress
                    diupWindow.boundary.negOperation = BoundaryObject.OperationInProgress
                    diupWindow.boundary.update()
                }
            }

            TinyButton {
                radius: 3
                flat: false
                implicitWidth: 92 + 2 * picPadding
                implicitHeight: 33 + 2 * picPadding
                visible: isTroll
                Image {
                    anchors.fill: parent
                    anchors.margins: picPadding
                    source: '../b2a2.png'
                }
                onClicked: {
                    diupWindow.boundary.posOperation = BoundaryObject.OperationCompleted
                    diupWindow.boundary.negOperation = BoundaryObject.OperationCompleted
                    diupWindow.boundary.update()
                }
            }
            Item { Layout.fillHeight: true }
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.minimumHeight: 50
            Item { Layout.fillWidth: true }
            Button {
                text: 'Снять'
                onClicked: {
                    diupWindow.boundary.posFeedPass = BoundaryObject.NoPass
                    diupWindow.boundary.negFeedPass = BoundaryObject.NoPass
                    diupWindow.boundary.update()
                }
            }
            Button {
                text: 'Выход'
                onClicked: diupWindow.visible = false
            }
        }
    }
}
