import QtQuick 2.0
import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import "Delegates"
Target {
    targetName: 'propertyEditorElement'
    Delegate{
        component: IntDelegate {}
        typeName: 'int'
    }

    Delegate{
        component: StringDelegate {}
        typeName: 'QString'
    }
    Delegate{
        component: BoolDelegate {}
        typeName: 'bool'
    }
    Delegate{
        component: DoubleDelegate {}
        typeName: 'double'
    }
    Delegate{
        component: ColorDelegate {}
        typeName: 'QColor'
    }
    Delegate{
        component: DelegatePropertyEditor {}
        typeName: 'QObject*'
    }
    Delegate{
        component: Text{ text: 'QRectF ' + modelData.value }
        typeName: 'QRectF'
    }
    Delegate{
        component: Text{ text: 'QPointF ' + modelData.value }
        typeName: 'QPointF'
    }
    Delegate{
        component: Text{ text: 'anchorLine ' + modelData.value }
        typeName: 'QQuickAnchorLine'
    }
    Delegate{
        component: Text{ text: 'list ' + modelData.value }
        typeName: 'QQmlListProperty<QQuickState>'
    }
    Delegate{
        component: Text{ text: 'list ' + modelData.value }
        typeName: 'QQmlListProperty<QQuickTransition>'
    }
    Delegate{
        component: Text{ text: 'list ' + modelData.value }
        typeName: 'QQmlListProperty<QQuickTransform>'
    }
    Delegate{
        component: Text{ text: 'list ' + modelData.value }
        typeName: 'QQmlListProperty<QObject>'
    }
    Delegate{
        component: Text{ text: 'list ' + modelData.value }
        typeName: 'QQmlListProperty<QQuickItem>'
    }

}
