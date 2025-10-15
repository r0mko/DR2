import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
UniDelegate {
    anchors.fill: parent
//    PropertyDelegateCommon{ // not writable integer
//        visible: !isWritable
//        color: Theme.palette.lightSeq[ 8 ]
//    }
    SpinBox{ // writable integer
        id: sb
        height: parent.height
//        visible: isWritable
        enabled: isWritable

        clip: true
        smooth: true
        antialiasing: true

        font.family: main_font.regular.name
        value: modelData.value

        minimumValue: ( typeof extraData === 'undefined' ) ? -9999999999999 : ( extraData.minValue === undefined ) ? -9999999999999 : extraData.minValue
        maximumValue: ( typeof extraData === 'undefined' ) ? 9999999999999 : ( extraData.maxValue === undefined ) ? 9999999999999 : extraData.maxValue
        stepSize: ( typeof extraData === 'undefined' ) ? 0.01 : ( extraData.stepSize === undefined ) ? 0.01 : extraData.stepSize
        onValueChanged: modelData.value = sb.value
        style: SpinBoxStyle{
            renderType: Text.QtRendering
        }
    }
}
