import QtQuick 2.5
import QtQuick.Layouts 1.2
import Dr.MetaGraph 1.0
import QtQuick.Dialogs 1.2
import Dr.UI 1.0
import QtQml.Models 2.2

ItemSelectionModel {
    id: _sel
    property string selectionTitle
    
    function decrementRow(modifiers) {
        var prevIdx
        var h = graphModel.getHandle(currentIndex)
        var prev = h.prevSibling()
        if (prev.isValid) {
            while (isExpanded(prev.modelIndex)) {
                prev = prev.childNodes[prev.childNodes.length - 1]
            }
        } else {
            prev = h.parent
            if (!prev.isValid)
                return
        }
        __listView.currentIndex--
        __currentRow = __listView.currentIndex
        __listView.positionViewAtIndex(__listView.currentIndex, ListView.Contain)
        selectElement(prev.modelIndex, modifiers)
    }
    
    function incrementRow(modifiers) {
        var nextIdx
        if (isExpanded(currentIndex))
            nextIdx = graphModel.childIndex(0, currentIndex)
        else {
            var h = graphModel.getHandle(currentIndex)
            do {
                var nxt = h.nextSibling()
                if (nxt.isValid) {
                    nextIdx = nxt.modelIndex
                    break
                } else {
                    h = h.parent
                    if (!h.isValid)
                        return
                }
            } while(true)
        }
        __listView.currentIndex++
        __currentRow = __listView.currentIndex
        __listView.positionViewAtIndex(__listView.currentIndex, ListView.Contain)
        selectElement(nextIdx, modifiers)
    }
    
    function selectElement(idx, modifiers) {
        if (modifiers & Qt.ControlModifier) {
            console.log("append to selection", selectedRows().length)
            select(idx, ItemSelectionModel.Toggle)
            setCurrentIndex(idx, ItemSelectionModel.Current | ItemSelectionModel.Toggle)
        } else {
            setCurrentIndex(idx, ItemSelectionModel.Current)
            select(idx, ItemSelectionModel.ClearAndSelect)
        }
    }
    
    function selectByMouse(idx, mouse, item) {
        setCurrentIndex(idx, ItemSelectionModel.NoUpdate)
        var treePos = tree_view.mapFromItem(item, mouse.x, mouse.y)
        var pressedRow = __listView.indexAt(0, treePos.y + __listView.contentY)
        __listView.forceActiveFocus()
        __currentRow = pressedRow
        if (mouse.button === Qt.LeftButton) {
            selectElement(idx, mouse.modifiers)
            __listView.currentIndex = __currentRow
        }
        buildTitle()
    }
    
    function buildTitle() {
        var l = selectedIndexes.length
        if (isSelected(currentIndex) && l > 1) {
            var suffix
            var rest = l % 10
            if (l > 15 || l <= 10) {
                switch (rest) {
                case 1:
                    suffix = "ел"
                    break
                case 2:
                case 3:
                case 4:
                    suffix = "ла"
                    break
                default:
                    suffix = "лов"
                }
            } else {
                suffix = "лов"
            }
            selectionTitle = "%1 уз%2".arg(l).arg(suffix)
        } else {
            if (!isSelected(currentIndex))
                selectionTitle = "%1".arg(model.getNodeId(currentIndex))
            else
                selectionTitle = "%1".arg(model.getNodeId(selectedIndexes[0]))
        }
    }
}
