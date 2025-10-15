import QtQuick 2.3
import QtQuick.Controls 1.2
import DataReactor 1.0

Object {
    property alias name: baseState.name
    property Action action
    property alias changes: baseState.changes

    StateGroup {
        id: _states

        State {
            id: baseState
            when: action.checked
        }
    }
}
