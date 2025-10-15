#ifndef GIHANDLE_H
#define GIHANDLE_H

#include <QObject>
#include "scene.h"
#include <QGraphicsItem>
#include <QPersistentModelIndex>

#include <QGraphicsSceneEvent>
#include <QQmlEngine>
#define SELF_PTR_TYPEID qMetaTypeId<std::add_pointer<std::remove_cv<std::remove_pointer<decltype(this)>::type>::type>::type>()
#define SELF_TYPEID qMetaTypeId<std::remove_cv<std::remove_pointer<decltype(this)>::type>::type>()
#define SELF_TYPE QByteArray(QMetaType::typeName(SELF_TYPEID))
#define chkItem(X) if (!isValid()) return X
#define COLON :
#define GR_INSTANCE(Class)                                                              \
    friend class Class ## Handle;                                                       \
    public:                                                                             \
    inline int type() const;

#define GR_HANDLE(X)                                                                    \
    inline X *item() const { return static_cast<X*>(VisualInstanceHandle::item()); }    \
    friend QDebug operator <<(QDebug, const X ## Handle &);                             \
    protected:                                                                          \
    virtual VisualInstance* createItem ( QGraphicsItem* parent = nullptr ) const        \
    {                                                                                   \
        VisualInstance *i = new X(parent);                                              \
        return i;                                                                       \
    }                                                                                   \
    friend class X;



#define DECLARE_ITEM(Class)                                                             \
    Q_DECLARE_METATYPE(Class*)                                                          \
    Q_DECLARE_METATYPE(Class ## Handle)                                                 \
    int Class :: type() const { return SELF_PTR_TYPEID; }                        \
    template<>                                                                          \
    struct HandleType<Class>                                                            \
    {                                                                                   \
        static inline int handleMetaType() { return qMetaTypeId<Class ## Handle>(); }   \
        typedef Class ## Handle type;                                                   \
    };                                                                                  \
    inline Class ## Handle getHandle(Class *item) {                                     \
        return Class ## Handle(item);                                                   \
    }                                                                                   \
    inline QDebug operator <<(QDebug dbg, const Class ## Handle &handle)                \
    {                                                                                   \
        dbg.nospace() << #Class << ":";                                                 \
        if (!handle.isValid()) {                                                        \
            dbg << "invalid";                                                           \
        } else {                                                                        \
            dbg << handle.item();                                                       \
        }                                                                               \
        return dbg;                                                                     \
    }                                                                                   \


class VisualInstance: public QGraphicsItem
{
public:
    QPersistentModelIndex   m_index;

    enum DataKey {
        // Item change
        ItemPositionChangeHandler = QGraphicsItem::ItemPositionChange,
        ItemMatrixChangeHandler = QGraphicsItem::ItemMatrixChange,
        ItemVisibleChangeHandler = QGraphicsItem::ItemVisibleChange,
        ItemEnabledChangeHandler = QGraphicsItem::ItemEnabledChange,
        ItemSelectedChangeHandler = QGraphicsItem::ItemSelectedChange,
        ItemParentChangeHandler = QGraphicsItem::ItemParentChange,
        ItemChildAddedChangeHandler = QGraphicsItem::ItemChildAddedChange,
        ItemChildRemovedChangeHandler = QGraphicsItem::ItemChildRemovedChange,
        ItemTransformChangeHandler = QGraphicsItem::ItemTransformChange,
        ItemPositionHasChangedHandler = QGraphicsItem::ItemPositionHasChanged,
        ItemTransformHasChangedHandler = QGraphicsItem::ItemTransformHasChanged,
        ItemSceneChangeHandler = QGraphicsItem::ItemSceneChange,
        ItemVisibleHasChangedHandler = QGraphicsItem::ItemVisibleHasChanged,
        ItemEnabledHasChangedHandler = QGraphicsItem::ItemEnabledHasChanged,
        ItemSelectedHasChangedHandler = QGraphicsItem::ItemSelectedHasChanged,
        ItemParentHasChangedHandler = QGraphicsItem::ItemParentHasChanged,
        ItemSceneHasChangedHandler = QGraphicsItem::ItemSceneHasChanged,
        ItemCursorChangeHandler = QGraphicsItem::ItemCursorChange,
        ItemCursorHasChangedHandler = QGraphicsItem::ItemCursorHasChanged,
        ItemToolTipChangeHandler = QGraphicsItem::ItemToolTipChange,
        ItemToolTipHasChangedHandler = QGraphicsItem::ItemToolTipHasChanged,
        ItemFlagsChangeHandler = QGraphicsItem::ItemFlagsChange,
        ItemFlagsHaveChangedHandler = QGraphicsItem::ItemFlagsHaveChanged,
        ItemZValueChangeHandler = QGraphicsItem::ItemZValueChange,
        ItemZValueHasChangedHandler = QGraphicsItem::ItemZValueHasChanged,
        ItemOpacityChangeHandler = QGraphicsItem::ItemOpacityChange,
        ItemOpacityHasChangedHandler = QGraphicsItem::ItemOpacityHasChanged,
        ItemScenePositionHasChangedHandler = QGraphicsItem::ItemScenePositionHasChanged,
        ItemRotationChangeHandler = QGraphicsItem::ItemRotationChange,
        ItemRotationHasChangedHandler = QGraphicsItem::ItemRotationHasChanged,
        ItemScaleChangeHandler = QGraphicsItem::ItemScaleChange,
        ItemScaleHasChangedHandler = QGraphicsItem::ItemScaleHasChanged,
        ItemTransformOriginPointChangeHandler = QGraphicsItem::ItemTransformOriginPointChange,
        ItemTransformOriginPointHasChangedHandler = QGraphicsItem::ItemTransformOriginPointHasChanged,

        // Item events
        ContextMenuEventHandler,

        DragEnterEventHandler,
        DragLeaveEventHandler,
        DragMoveEventHandler,
        DropEventHandler,

        FocusInEventHandler,
        FocusOutEventHandler,

        HoverEnterEventHandler,
        HoverMoveEventHandler,
        HoverLeaveEventHandler,

        KeyPressEventHandler,
        KeyReleaseEventHandler,

        MousePressEventHandler,
        MouseMoveEventHandler,
        MouseReleaseEventHandler,
        MouseDoubleClickEventHandler,

        WheelEventHandler,

        InputMethodEventHandler,

        // basic data
        ModelIndexKey,
        QmlEngineKey,
        CachedHandleKey,
        ExtraFlagsKey,
        SceneDataId

    };

    VisualInstance(QGraphicsItem * parent = nullptr) : QGraphicsItem( parent ) {}
    virtual ~VisualInstance() {}
    inline int type() const;

    QRectF boundingRect() const { return QRectF(); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) { Q_UNUSED(painter); Q_UNUSED(option); }
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    QVariant getHandle();
protected:
    QTransform currentViewTransform() const;
    void preparePainter(QPainter *painter) const;
    QRectF fixRect(const QRectF &rect) const;
    QPainterPath fixPath(const QPainterPath &path) const;

private:
    void handleMouseEvent(QGraphicsSceneMouseEvent *event, DataKey eventType);
    void handleMouseHoverEvent(QGraphicsSceneHoverEvent *event, DataKey eventType);
    template<class T>
    QJSValue handleItemChange(QQmlEngine *engine, QJSValue thisObject, QJSValue callback, T arg)
    {
        return callback.callWithInstance(thisObject, { engine->toScriptValue(arg) });
    }

};


class VisualInstanceHandle
{
    Q_GADGET

    Q_ENUMS(GraphicsItemChange)
    Q_ENUMS(VisualInstanceFlag)
    Q_FLAGS(VisualInstanceFlags)

    Q_PROPERTY(QRectF boundingRect READ boundingRect FINAL)
    Q_PROPERTY(QPointF position READ position WRITE setPosition FINAL)
    Q_PROPERTY(qreal x READ x WRITE setX FINAL)
    Q_PROPERTY(qreal y READ y WRITE setY FINAL)
    Q_PROPERTY(qreal z READ z WRITE setZ FINAL)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation )
    Q_PROPERTY(qreal scale READ scale WRITE setScale )
    Q_PROPERTY(QPointF transformOriginPoint READ transformOriginPoint WRITE setTransformOriginPoint )
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity )

    Q_PROPERTY(bool visible READ visible WRITE setVisible )
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled )
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected )
    Q_PROPERTY(bool valid READ isValid)

    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons )
    Q_PROPERTY(QVariantList childItems READ childItems)
    Q_PROPERTY(VisualInstanceHandle parentItem READ parentItem WRITE setParentItem)

    Q_PROPERTY(int flags READ flags WRITE setFlags )

    // event handlers
    Q_PROPERTY(QJSValue onMousePress READ onMousePress WRITE setOnMousePress)
    Q_PROPERTY(QJSValue onMouseRelease READ onMouseRelease WRITE setOnMouseRelease)
    Q_PROPERTY(QJSValue onMouseMove READ onMouseMove WRITE setOnMouseMoved)

    Q_PROPERTY(QJSValue onHoverEnter READ onHoverEnter WRITE setOnHoverEnter)
    Q_PROPERTY(QJSValue onHoverLeave READ onHoverLeave WRITE setOnHoverLeave)
    Q_PROPERTY(QJSValue onHoverMove READ onHoverMove WRITE setOnHoverMove)
    Q_PROPERTY(int cacheMode READ cacheMode WRITE setCacheMode)

    Q_PROPERTY(QPersistentModelIndex modelIndex READ modelIndex)

    //    Q_PROPERTY(QRectF rect READ rect WRITE setRect FINAL)
    //    Q_PROPERTY(QSizeF size READ size WRITE setSize FINAL)
    //    Q_PROPERTY(qreal width READ width WRITE setWidth FINAL)
    //    Q_PROPERTY(qreal height READ height WRITE setHeight FINAL)
    //    Q_PROPERTY(Scene* scene READ scene WRITE setScene )
    //    Q_PROPERTY(bool hovered READ isHovered )
    //    Q_PROPERTY(bool pressed READ isPressed )
    //    Q_PROPERTY(bool dragging READ dragging )
    //    Q_PROPERTY(QVariant data READ data WRITE setData )
    //    Q_PROPERTY(bool clip READ clip WRITE setClip FINAL)
public:
    enum VisualInstanceFlag {
        ItemIsMovable = 0x1,
        ItemIsSelectable = 0x2,
        ItemIsFocusable = 0x4,
        ItemClipsToShape = 0x8,
        ItemClipsChildrenToShape = 0x10,
        ItemIgnoresParentOpacity = 0x40,
        ItemDoesntPropagateOpacityToChildren = 0x80,
        ItemStacksBehindParent = 0x100,
        ItemUsesExtendedStyleOption = 0x200,
        ItemHasNoContents = 0x400,
        ItemSendsGeometryChanges = 0x800,
        ItemAcceptsInputMethod = 0x1000,
        ItemNegativeZStacksBehindParent = 0x2000,
        ItemSendsScenePositionChanges = 0x10000,
        ItemStopsClickFocusPropagation = 0x20000,
        ItemStopsFocusHandling  =   0x40000,
        // Dr.Graphics VisualInstance specific flags
        ItemPositionSnapsToGrid     = 0x100000,
        ItemRectSnapsToGrid         = 0x200000,
        ItemIgnoresTransformations  = 0x400000,
        ItemPixelAligned            = 0x800000,
        NoSelectionOutline  =        0x1000000,
        DrGraphicsFlagsMask  = 0xfff00000
    };
    Q_DECLARE_FLAGS(VisualInstanceFlags, VisualInstanceFlag)
    Q_FLAG(VisualInstanceFlags)

    enum GraphicsItemChange {
        ItemPositionChange = VisualInstance::ItemPositionChangeHandler,
        ItemMatrixChange = VisualInstance::ItemMatrixChangeHandler,
        ItemVisibleChange = VisualInstance::ItemVisibleChangeHandler,
        ItemEnabledChange = VisualInstance::ItemEnabledChangeHandler,
        ItemSelectedChange = VisualInstance::ItemSelectedChangeHandler,
        ItemParentChange = VisualInstance::ItemParentChangeHandler,
        ItemChildAddedChange = VisualInstance::ItemChildAddedChangeHandler,
        ItemChildRemovedChange = VisualInstance::ItemChildRemovedChangeHandler,
        ItemTransformChange = VisualInstance::ItemTransformChangeHandler,
        ItemPositionHasChanged = VisualInstance::ItemPositionHasChangedHandler,
        ItemTransformHasChanged = VisualInstance::ItemTransformHasChangedHandler,
        ItemSceneChange = VisualInstance::ItemSceneChangeHandler,
        ItemVisibleHasChanged = VisualInstance::ItemVisibleHasChangedHandler,
        ItemEnabledHasChanged = VisualInstance::ItemEnabledHasChangedHandler,
        ItemSelectedHasChanged = VisualInstance::ItemSelectedHasChangedHandler,
        ItemParentHasChanged = VisualInstance::ItemParentHasChangedHandler,
        ItemSceneHasChanged = VisualInstance::ItemSceneHasChangedHandler,
        ItemCursorChange = VisualInstance::ItemCursorChangeHandler,
        ItemCursorHasChanged = VisualInstance::ItemCursorHasChangedHandler,
        ItemToolTipChange = VisualInstance::ItemToolTipChangeHandler,
        ItemToolTipHasChanged = VisualInstance::ItemToolTipHasChangedHandler,
        ItemFlagsChange = VisualInstance::ItemFlagsChangeHandler,
        ItemFlagsHaveChanged = VisualInstance::ItemFlagsHaveChangedHandler,
        ItemZValueChange = VisualInstance::ItemZValueChangeHandler,
        ItemZValueHasChanged = VisualInstance::ItemZValueHasChangedHandler,
        ItemOpacityChange = VisualInstance::ItemOpacityChangeHandler,
        ItemOpacityHasChanged = VisualInstance::ItemOpacityHasChangedHandler,
        ItemScenePositionHasChanged = VisualInstance::ItemScenePositionHasChangedHandler,
        ItemRotationChange = VisualInstance::ItemRotationChangeHandler,
        ItemRotationHasChanged = VisualInstance::ItemRotationHasChangedHandler,
        ItemScaleChange = VisualInstance::ItemScaleChangeHandler,
        ItemScaleHasChanged = VisualInstance::ItemScaleHasChangedHandler,
        ItemTransformOriginPointChange = VisualInstance::ItemTransformOriginPointChangeHandler,
        ItemTransformOriginPointHasChanged = VisualInstance::ItemTransformOriginPointHasChangedHandler
    };
    Q_ENUM(GraphicsItemChange)


public:
    VisualInstanceHandle(VisualInstance *item = nullptr);
    virtual ~VisualInstanceHandle();

    VisualInstance* item() const;
    void setItem(VisualInstance* item);

    QRectF boundingRect() const;
    bool isValid() const;

    QPointF position() const;
    void setPosition(QPointF position);

    qreal x() const;
    void setX(qreal x);

    qreal y() const;
    void setY(qreal y);

    qreal rotation() const;
    void setRotation(qreal rotation);

    qreal scale() const;
    void setScale(qreal scale);

    QPointF transformOriginPoint() const;
    void setTransformOriginPoint(QPointF transformOriginPoint);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    bool visible() const;
    void setVisible(bool visible);

    qreal z() const;
    void setZ(qreal z);

    bool hoverEnabled() const;
    void setHoverEnabled(bool hoverEnabled);

    bool isSelected() const;
    void setSelected(bool selected);

    QVariantList childItems() const;

    VisualInstanceHandle parentItem() const;
    void setParentItem(VisualInstanceHandle parentItem);

    Qt::MouseButtons acceptedButtons() const;
    void setAcceptedButtons(Qt::MouseButtons acceptedButtons);

    int flags() const;
    void setFlags(int flags);

    Q_INVOKABLE void update() const;
    Q_INVOKABLE void destroy();

    Q_INVOKABLE bool bind(GraphicsItemChange change, QJSValue callback);

    Q_INVOKABLE inline QPointF mapFromItem(const VisualInstanceHandle &item, const QPointF & point) const;
    Q_INVOKABLE inline QPolygonF mapFromItem(const VisualInstanceHandle &item, const QRectF & rect) const;
    Q_INVOKABLE inline QPolygonF mapFromItem(const VisualInstanceHandle &item, const QPolygonF & polygon) const;
    Q_INVOKABLE inline QPainterPath mapFromItem(const VisualInstanceHandle &item, const QPainterPath & path) const;
    Q_INVOKABLE inline QPolygonF mapFromItem(const VisualInstanceHandle &item, qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QPointF mapFromItem(const VisualInstanceHandle &item, qreal x, qreal y) const;
    Q_INVOKABLE inline QPointF mapFromParent(const QPointF & point) const;
    Q_INVOKABLE inline QPolygonF mapFromParent(const QRectF & rect) const;
    Q_INVOKABLE inline QPolygonF mapFromParent(const QPolygonF & polygon) const;
    Q_INVOKABLE inline QPainterPath mapFromParent(const QPainterPath & path) const;
    Q_INVOKABLE inline QPolygonF mapFromParent(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QPointF mapFromParent(qreal x, qreal y) const;
    Q_INVOKABLE inline QPointF mapFromScene(const QPointF & point) const;
    Q_INVOKABLE inline QPolygonF mapFromScene(const QRectF & rect) const;
    Q_INVOKABLE inline QPolygonF mapFromScene(const QPolygonF & polygon) const;
    Q_INVOKABLE inline QPainterPath mapFromScene(const QPainterPath & path) const;
    Q_INVOKABLE inline QPolygonF mapFromScene(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QPointF mapFromScene(qreal x, qreal y) const;
    Q_INVOKABLE inline QRectF mapRectFromItem(const VisualInstanceHandle &item, const QRectF & rect) const;
    Q_INVOKABLE inline QRectF mapRectFromItem(const VisualInstanceHandle &item, qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QRectF mapRectFromParent(const QRectF & rect) const;
    Q_INVOKABLE inline QRectF mapRectFromParent(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QRectF mapRectFromScene(const QRectF & rect) const;
    Q_INVOKABLE inline QRectF mapRectFromScene(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QRectF mapRectToItem(const VisualInstanceHandle &item, const QRectF & rect) const;
    Q_INVOKABLE inline QRectF mapRectToItem(const VisualInstanceHandle &item, qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QRectF mapRectToParent(const QRectF & rect) const;
    Q_INVOKABLE inline QRectF mapRectToParent(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QRectF mapRectToScene(const QRectF & rect) const;
    Q_INVOKABLE inline QRectF mapRectToScene(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QPointF mapToItem(const VisualInstanceHandle &item, const QPointF & point) const;
    Q_INVOKABLE inline QPolygonF mapToItem(const VisualInstanceHandle &item, const QRectF & rect) const;
    Q_INVOKABLE inline QPolygonF mapToItem(const VisualInstanceHandle &item, const QPolygonF & polygon) const;
    Q_INVOKABLE inline QPainterPath mapToItem(const VisualInstanceHandle &item, const QPainterPath & path) const;
    Q_INVOKABLE inline QPolygonF mapToItem(const VisualInstanceHandle &item, qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QPointF mapToItem(const VisualInstanceHandle &item, qreal x, qreal y) const;
    Q_INVOKABLE inline QPointF mapToParent(const QPointF & point) const;
    Q_INVOKABLE inline QPolygonF mapToParent(const QRectF & rect) const;
    Q_INVOKABLE inline QPolygonF mapToParent(const QPolygonF & polygon) const;
    Q_INVOKABLE inline QPainterPath mapToParent(const QPainterPath & path) const;
    Q_INVOKABLE inline QPolygonF mapToParent(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QPointF mapToParent(qreal x, qreal y) const;
    Q_INVOKABLE inline QPointF mapToScene(const QPointF & point) const;
    Q_INVOKABLE inline QPolygonF mapToScene(const QRectF & rect) const;
    Q_INVOKABLE inline QPolygonF mapToScene(const QPolygonF & polygon) const;
    Q_INVOKABLE inline QPainterPath mapToScene(const QPainterPath & path) const;
    Q_INVOKABLE inline QPolygonF mapToScene(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE inline QPointF mapToScene(qreal x, qreal y) const;

    QJSValue onMousePress() const;
    void setOnMousePress(QJSValue arg);

    QJSValue onMouseRelease() const;
    void setOnMouseRelease(QJSValue arg);

    QJSValue onMouseMove() const;
    void setOnMouseMoved(QJSValue arg);

    QJSValue onHoverEnter() const;
    void setOnHoverEnter(QJSValue arg);

    QJSValue onHoverLeave() const;
    void setOnHoverLeave(QJSValue arg);

    QJSValue onHoverMove() const;
    void setOnHoverMove(QJSValue arg);

    QPersistentModelIndex modelIndex() const
    {
        return item()->m_index;
    }


    inline int cacheMode() const
    {
        return item()->cacheMode();
    }
    inline void setCacheMode(int cacheMode)
    {
        m_item->setCacheMode((QGraphicsItem::CacheMode)cacheMode);
    }

protected:
    //    virtual VisualInstance* createItem ( QGraphicsItem* parent = nullptr ) const;
    virtual VisualInstance* createItem ( QGraphicsItem* parent = nullptr ) const;


private:
    mutable VisualInstance* m_item{nullptr};

};



template<class T>
struct HandleType
{
    typedef void type;
    static inline int handleMetaType() { return 0; }
};

QPointF VisualInstanceHandle::mapFromItem(const VisualInstanceHandle &item, const QPointF &point) const
{
    chkItem(QPointF());
    return m_item->mapFromItem(item.m_item, point);
}

QPolygonF VisualInstanceHandle::mapFromItem(const VisualInstanceHandle &item, const QRectF &rect) const
{
    chkItem(QPolygonF());
    return m_item->mapFromItem(item.m_item, rect);
}

QPolygonF VisualInstanceHandle::mapFromItem(const VisualInstanceHandle &item, const QPolygonF &polygon) const
{
    chkItem(QPolygonF());
    return m_item->mapFromItem(item.m_item, polygon);
}

QPainterPath VisualInstanceHandle::mapFromItem(const VisualInstanceHandle &item, const QPainterPath &path) const
{
    chkItem(QPainterPath());
    return m_item->mapFromItem(item.m_item, path);
}

QPolygonF VisualInstanceHandle::mapFromItem(const VisualInstanceHandle &item, qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QPolygonF());
    return m_item->mapFromItem(item.m_item, x, y, w, h);
}

QPointF VisualInstanceHandle::mapFromItem(const VisualInstanceHandle &item, qreal x, qreal y) const
{
    chkItem(QPointF());
    return m_item->mapFromItem(item.m_item, x, y);
}

QPointF VisualInstanceHandle::mapFromParent(const QPointF &point) const
{
    chkItem(QPointF());
    return m_item->mapFromParent(point);
}

QPolygonF VisualInstanceHandle::mapFromParent(const QRectF &rect) const
{
    chkItem(QPolygonF());
    return m_item->mapFromParent(rect);
}

QPolygonF VisualInstanceHandle::mapFromParent(const QPolygonF &polygon) const
{
    chkItem(QPolygonF());
    return m_item->mapFromParent(polygon);
}

QPainterPath VisualInstanceHandle::mapFromParent(const QPainterPath &path) const
{
    chkItem(QPainterPath());
    return m_item->mapFromParent(path);
}

QPolygonF VisualInstanceHandle::mapFromParent(qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QPolygonF());
    return m_item->mapFromParent(x, y, w, h);
}

QPointF VisualInstanceHandle::mapFromParent(qreal x, qreal y) const
{
    chkItem(QPointF());
    return m_item->mapFromParent(x, y);
}

QPointF VisualInstanceHandle::mapFromScene(const QPointF &point) const
{
    chkItem(QPointF());
    return m_item->mapFromScene(point);
}

QPolygonF VisualInstanceHandle::mapFromScene(const QRectF &rect) const
{
    chkItem(QPolygonF());
    return m_item->mapFromScene(rect);
}

QPolygonF VisualInstanceHandle::mapFromScene(const QPolygonF &polygon) const
{
    chkItem(QPolygonF());
    return m_item->mapFromScene(polygon);
}

QPainterPath VisualInstanceHandle::mapFromScene(const QPainterPath &path) const
{
    chkItem(QPainterPath());
    return m_item->mapFromScene(path);
}

QPolygonF VisualInstanceHandle::mapFromScene(qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QPolygonF());
    return m_item->mapFromScene(x, y, w, h);
}

QPointF VisualInstanceHandle::mapFromScene(qreal x, qreal y) const
{
    chkItem(QPointF());
    return m_item->mapFromScene(x, y);
}

QRectF VisualInstanceHandle::mapRectFromItem(const VisualInstanceHandle &item, const QRectF &rect) const
{
    chkItem(QRectF());
    return m_item->mapRectFromItem(item.m_item, rect);
}

QRectF VisualInstanceHandle::mapRectFromItem(const VisualInstanceHandle &item, qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QRectF());
    return m_item->mapRectFromItem(item.m_item, x, y, w, h);
}

QRectF VisualInstanceHandle::mapRectFromParent(const QRectF &rect) const
{
    chkItem(QRectF());
    return m_item->mapRectFromParent(rect);
}

QRectF VisualInstanceHandle::mapRectFromParent(qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QRectF());
    return m_item->mapRectFromParent(x, y, w, h);
}

QRectF VisualInstanceHandle::mapRectFromScene(const QRectF &rect) const
{
    chkItem(QRectF());
    return m_item->mapRectFromScene(rect);
}

QRectF VisualInstanceHandle::mapRectFromScene(qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QRectF());
    return m_item->mapRectFromScene(x, y, w, h);
}

QRectF VisualInstanceHandle::mapRectToItem(const VisualInstanceHandle &item, const QRectF &rect) const
{
    chkItem(QRectF());
    return m_item->mapRectToItem(item.m_item, rect);
}

QRectF VisualInstanceHandle::mapRectToItem(const VisualInstanceHandle &item, qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QRectF());
    return m_item->mapRectToItem(item.m_item, x, y, w, h);
}

QRectF VisualInstanceHandle::mapRectToParent(const QRectF &rect) const
{
    chkItem(QRectF());
    return m_item->mapRectToParent(rect);
}

QRectF VisualInstanceHandle::mapRectToParent(qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QRectF());
    return m_item->mapRectToParent(x, y, w, h);
}

QRectF VisualInstanceHandle::mapRectToScene(const QRectF &rect) const
{
    chkItem(QRectF());
    return m_item->mapRectToScene(rect);
}

QRectF VisualInstanceHandle::mapRectToScene(qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QRectF());
    return m_item->mapRectToScene(x, y, w, h);
}

QPointF VisualInstanceHandle::mapToItem(const VisualInstanceHandle &item, const QPointF &point) const
{
    chkItem(QPointF());
    return m_item->mapToItem(item.m_item, point);
}

QPolygonF VisualInstanceHandle::mapToItem(const VisualInstanceHandle &item, const QRectF &rect) const
{
    chkItem(QPolygonF());
    return m_item->mapToItem(item.m_item, rect);
}

QPolygonF VisualInstanceHandle::mapToItem(const VisualInstanceHandle &item, const QPolygonF &polygon) const
{
    chkItem(QPolygonF());
    return m_item->mapToItem(item.m_item, polygon);
}

QPainterPath VisualInstanceHandle::mapToItem(const VisualInstanceHandle &item, const QPainterPath &path) const
{
    chkItem(QPainterPath());
    return m_item->mapToItem(item.m_item, path);
}

QPolygonF VisualInstanceHandle::mapToItem(const VisualInstanceHandle &item, qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QPolygonF());
    return m_item->mapToItem(item.m_item, x, y, w, h);
}

QPointF VisualInstanceHandle::mapToItem(const VisualInstanceHandle &item, qreal x, qreal y) const
{
    chkItem(QPointF());
    return m_item->mapToItem(item.m_item, x, y);
}

QPointF VisualInstanceHandle::mapToParent(const QPointF &point) const
{
    chkItem(QPointF());
    return m_item->mapToParent(point);
}

QPolygonF VisualInstanceHandle::mapToParent(const QRectF &rect) const
{
    chkItem(QPolygonF());
    return m_item->mapToParent(rect);
}

QPolygonF VisualInstanceHandle::mapToParent(const QPolygonF &polygon) const
{
    chkItem(QPolygonF());
    return m_item->mapToParent(polygon);
}

QPainterPath VisualInstanceHandle::mapToParent(const QPainterPath &path) const
{
    chkItem(QPainterPath());
    return m_item->mapToParent(path);
}

QPolygonF VisualInstanceHandle::mapToParent(qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QPolygonF());
    return m_item->mapToParent(x, y, w, h);
}

QPointF VisualInstanceHandle::mapToParent(qreal x, qreal y) const
{
    chkItem(QPointF());
    return m_item->mapToParent(x, y);
}

QPointF VisualInstanceHandle::mapToScene(const QPointF &point) const
{
    chkItem(QPointF());
    return m_item->mapToScene(point);
}

QPolygonF VisualInstanceHandle::mapToScene(const QRectF &rect) const
{
    chkItem(QPolygonF());
    return m_item->mapToScene(rect);
}

QPolygonF VisualInstanceHandle::mapToScene(const QPolygonF &polygon) const
{
    chkItem(QPolygonF());
    return m_item->mapToScene(polygon);
}

QPainterPath VisualInstanceHandle::mapToScene(const QPainterPath &path) const
{
    chkItem(QPainterPath());
    return m_item->mapToScene(path);
}

QPolygonF VisualInstanceHandle::mapToScene(qreal x, qreal y, qreal w, qreal h) const
{
    chkItem(QPolygonF());
    return m_item->mapToScene(x, y, w, h);
}

QPointF VisualInstanceHandle::mapToScene(qreal x, qreal y) const
{
    chkItem(QPointF());
    return m_item->mapToScene(x, y);
}


DECLARE_ITEM(VisualInstance)
#endif // GIHANDLE_H
