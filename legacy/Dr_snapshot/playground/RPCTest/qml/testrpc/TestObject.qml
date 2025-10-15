import QtQuick 2.3

QtObject {
    property var varProperty
    property int intProperty
    property string textProperty
    property int x: cursor.x
    property int y: cursor.y
    signal someSig01
    signal someSig02(QtObject argument)
    onSomeSig01: console.log("sig01 emitted")
    function slot1() {
        console.log("slot 1 invoked")
    }

    function slot2(arg1, arg2) {
        console.log("slot 2 invoked", arg1, arg2)
        return JSON.stringify([arg1, arg2])
    }


}
