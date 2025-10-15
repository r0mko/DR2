import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

Item {
    id: root
    anchors.fill: parent
    visible: false

    Action { // login action
        id: loginAction
        onTriggered: {
            service.login( loginTextField.text, passwordTextField.text )
            loginTextField.text = ''
            loginTextField.focus = true
            passwordTextField.text = ''
        }
    }


    /**/// background
    Item { // background
        anchors.fill: parent
        Image {
            id: background

            anchors.fill: parent
            fillMode: Image.Tile
            source: 'qrc:/NSTexturedFullScreenBackgroundColor.png'
        }
        HueSaturation {
            id: bghue
            anchors.fill: parent
            source: background
            lightness: +0.45
            saturation: -0.5
        }
        ZoomBlur {
            anchors.fill: parent
            cached: true
            length: 2
            verticalOffset: 10
            samples: 32
            source: bghue
        }
        RadialGradient {
            id: shade
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0; color: "transparent" }
                GradientStop { position: 1; color: "#80000000" }
            }
        }
    }
    /**/

    /**/// main layout
    ColumnLayout {
        id: layout
        anchors.centerIn: parent

        Rectangle { // logo
            width: 200*1.8
            height: 170*1.8
            color: 'transparent'
            Image {
                id: logoImage
                anchors.fill: parent
                source: 'qrc:/logo-mosgortrans.png'
                fillMode: Image.PreserveAspectFit
                visible: false
            }
            HueSaturation {
                anchors.fill: parent
                source: logoImage
                hue: +0.03
                saturation: -0.3
            }
        }
        TextField { // login field
            id: loginTextField
            focus: true
            activeFocusOnTab: true
            activeFocusOnPress: true
            implicitWidth: 200*1.8
            implicitHeight: 20*1.8
            font.pixelSize: uiStyle.fontSizeBig// 25
            placeholderText: 'логин'

            onEditingFinished: passwordTextField.focus = true
            onFocusChanged: if( !focus ) loginTextFieldSideTooltip.hide()

            Keys.onReleased: {
                if( event.key === Qt.Key_Shift ) {
                    event.accepted = false
                    return;
                }

                var x = text[ text.length - 1 ]
                var lowerCase = /[a-zа-я]/
                var upperCase = /[A-ZА-Я]/
                var russian = /[А-Яа-я]/
                var shiftPressed = event.modifiers & Qt.ShiftModifier

                if( !shiftPressed && upperCase.test( x ) && russian.test( x ) ) {
                    loginTextFieldSideTooltip.show( 'Caps lock и Русская раскладка' )
                }
                else if( !shiftPressed && upperCase.test( x ) ) {
                    loginTextFieldSideTooltip.show( 'Caps lock' )
                }
                else if( russian.test( x ) ) {
                    loginTextFieldSideTooltip.show( 'Русская раскладка' )
                }
                else {
                    loginTextFieldSideTooltip.hide()
                }
                event.accepted = true
            }
            SideToolTip {
                id: loginTextFieldSideTooltip
            }
        }
        TextField { // password field
            id: passwordTextField
            activeFocusOnTab: true
            activeFocusOnPress: true

            implicitWidth: 200*1.8
            implicitHeight: 20*1.8
            font.pixelSize: uiStyle.fontSizeBig// 25
            placeholderText: 'пароль'
            echoMode: TextInput.Password

            onFocusChanged: if( !focus ) passwordTextFieldSideTooltip.hide()

            Keys.onReturnPressed: loginAction.trigger( this )
            Keys.onReleased: {
                if( event.key === Qt.Key_Shift ) {
                    event.accepted = false
                    return;
                }

                var x = text[ text.length - 1 ]
                var lowerCase = /[a-zа-я]/
                var upperCase = /[A-ZА-Я]/
                var russian = /[А-Яа-я]/
                var shiftPressed = event.modifiers & Qt.ShiftModifier

                if( !shiftPressed && upperCase.test( x ) && russian.test( x ) ) {
                    passwordTextFieldSideTooltip.show( 'Caps lock и Русская раскладка' )
                }
                else if( !shiftPressed && upperCase.test( x ) ) {
                    passwordTextFieldSideTooltip.show( 'Caps lock' )
                }
                else if( russian.test( x ) ) {
                    passwordTextFieldSideTooltip.show( 'Русская раскладка' )
                }
                else {
                    passwordTextFieldSideTooltip.hide()
                }
                event.accepted = true
            }
            SideToolTip {
                id: passwordTextFieldSideTooltip
            }
        }
//        Button { // login button
//            activeFocusOnTab: true
//            implicitWidth: 200*1.8

//            implicitHeight: 20*1.8

//            height: 20*1.8
//            text: 'Войти'
//            onClicked: loginAction.trigger( this )
//            style: ButtonStyle {
//                label: Text {
//                    anchors.fill: parent
//                    text: control.text
//                    verticalAlignment: Text.AlignVCenter
//                    horizontalAlignment: Text.AlignHCenter
//                    font.pixelSize: uiStyle.fontSizeBig//24
//                    //font.bold: true

//                }
//            }
//        }
        Button {
            activeFocusOnTab: true
            implicitWidth: 200*1.8

            implicitHeight: 20*1.8

            height: 20*1.8
            text: 'Войти'
            onClicked: service.login('tox','123')
            style: ButtonStyle {
                label: Text {
                    anchors.fill: parent
                    text: control.text
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: uiStyle.fontSizeBig//24
                    //font.bold: true

                }
            }

        }
    }
    /**/




    /**/// bottom texts /**/
    Text { // date time
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 20
        font.pixelSize: uiStyle.fontSizeSmall// 12
        height: 20
        renderType: Text.NativeRendering
        color: '#ddd'
        style: Text.Raised
        styleColor: '#333'
        Timer {
            interval: 100
            repeat: true
            onTriggered: parent.text = Qt.formatDateTime( new Date(), 'dd MMMM yyyy г. hh:mm:ss' )
            Component.onCompleted: start()
        }
    }
    Text { // copyright
        anchors.bottom: parent.bottom
        width: parent.width
        height: 20
        renderType: Text.NativeRendering
        //font.family: 'Times New Roman'
        font.pixelSize: uiStyle.fontSizeSmall//12
        text: 'Power Grid Engineering © 2015'
        color: '#ddd'
        horizontalAlignment: Text.AlignHCenter
        style: Text.Raised
        styleColor: '#333'
    }
    Text { // connection text
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 25
        font.pixelSize: uiStyle.fontSizeSmall//12
        height: 20
        renderType: Text.NativeRendering
        text: service.isConnected ? 'связь с сервером установлена' : 'нет связи с сервером'
        color: '#ddd'
        style: Text.Raised
        styleColor: '#333'
    }
    Rectangle { // connection indicator
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.bottomMargin: 5
        width: 15
        height: 15
        radius: 2
        opacity: 0.5
        color: service.isConnected ? '#0f0' : '#f00'
        Behavior on color { ColorAnimation { duration: 100 } }
    }
    /**/


}

