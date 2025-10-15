import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

BaseEditor {
    validator: DoubleValidator {
        id: validator
        locale: "C"
    }
//    text: Number( modelValue ).toPrecision( 3 )
}

