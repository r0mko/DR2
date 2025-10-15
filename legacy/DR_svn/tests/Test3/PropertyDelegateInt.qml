import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

UniDelegate {
    anchors.fill: parent
    PropertyDelegateCommon{ // not writable integer
        visible: !isWritable && !isEnumType
        color: Theme.palette.lightSeq[ 7 ]
    }
    SpinBox{ // writable integer
        id: sbox
        height: parent.height
        visible: isWritable && !isEnumType
        enabled: isWritable
        clip: true
        smooth: true
        antialiasing: true
        font.family: main_font.regular.name
        value: modelData.value


        minimumValue: ( typeof extraData === 'undefined' ) ? -9999999999999 : ( extraData.minValue === undefined ) ? -9999999999999 : extraData.minValue
        maximumValue: ( typeof extraData === 'undefined' ) ? 9999999999999 : ( extraData.maxValue === undefined ) ? 9999999999999 : extraData.maxValue
        stepSize: ( typeof extraData === 'undefined' ) ? 1 : ( extraData.stepSize === undefined ) ? 1 : extraData.stepSize

        onValueChanged: modelData.value = sbox.value
        style: SpinBoxStyle{
            renderType: Text.QtRendering
        }
    }
    PropertyDelegateCommon{ // not writable enum
        visible: !isWritable && isEnumType
        color: Theme.palette.lightSeq[ 7 ]
        text: enumerator !== null ? String( modelData.enumerator.key( modelData.value ) ) : String( modelData.value )
    }
    ComboBox{ // writable enum
        width: 200
        height: parent.height
        visible: isWritable && isEnumType
        enabled: isWritable
        model: enumerator !== null ? modelData.enumerator.keys : [ modelData.value ]
        currentIndex: enumerator !== null ? modelData.enumerator.values.indexOf( modelData.value ) : 0
        onCurrentIndexChanged: if( enumerator !== null ) modelData.value = modelData.enumerator.value( currentIndex )

        style:ComboBoxStyle{
            renderType: Text.QtRendering
        }

    }
}
