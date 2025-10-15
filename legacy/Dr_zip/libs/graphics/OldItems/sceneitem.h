#ifndef SCENEITEM_H
#define SCENEITEM_H

#include <QObject>
#include "qqml.h"
#include "qgraphicsitem.h"

#include <QPolygonF>
#include <QPainterPath>
#include <QVector2D>
#include "cursor.h"
#include "graphicsitem.h"

#define PROTO_ITEM(Class) \
    QT_BEGIN_NAMESPACE \
    inline Class *item() const { return reinterpret_cast<Class *>(SceneItem::item()); } \
    QT_END_NAMESPACE

#define gItem item()
class SceneItem;
class Scene;

class SceneItem : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_FLAGS(SceneItemFlags)

    Q_PROPERTY(QRectF boudingRect READ boundingRect NOTIFY boundingRectChanged FINAL)
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged FINAL)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged FINAL)
    Q_PROPERTY(QSizeF size READ size WRITE setSize NOTIFY sizeChanged FINAL)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged FINAL)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged FINAL)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged FINAL)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged FINAL)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QPointF transformOriginPoint READ transformOriginPoint WRITE setTransformOriginPoint NOTIFY transformOriginPointChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY zChanged FINAL)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(bool isSelected READ isSelected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(bool isHovered READ isHovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool isPressed READ isPressed NOTIFY pressedChanged)
    Q_PROPERTY(bool dragging READ dragging NOTIFY draggingChanged)
    Q_PROPERTY(QVariant data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(bool clip READ clip WRITE setClip NOTIFY clipChanged FINAL)
    Q_PROPERTY(QQmlListProperty<SceneItem> childItems READ qmlChildItems NOTIFY childItemsChanged)
    Q_PROPERTY(SceneItem* parentItem READ parentItem WRITE setParentItem NOTIFY parentItemChanged)
    Q_PROPERTY(Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(SceneItemFlags flags READ flags WRITE setFlags NOTIFY flagsChanged)
    Q_PROPERTY(qreal gridStep READ gridStep WRITE setGridStep NOTIFY gridStepChanged)
    Q_PROPERTY(Cursor* cursor READ cursor WRITE setCursor NOTIFY cursorChanged)


    Q_CLASSINFO("DefaultProperty", "childItems")

public:
    enum SceneItemFlag {
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
        //        ItemIsPanel = 0x4000,
        ItemSendsScenePositionChanges = 0x10000,
        ItemStopsClickFocusPropagation = 0x20000,
        ItemStopsFocusHandling  =   0x40000,
        // QuickGraphicsItem specific flags
        ItemPositionSnapsToGrid     = 0x100000,
        ItemRectSnapsToGrid         = 0x200000,
        ItemIgnoresTransformations  = 0x400000,
        ItemPixelAligned            = 0x800000,
        NoSelectionOutline  =        0x1000000,
        QuickGraphicsFlagsMask  = 0xfff00000
    };
    Q_DECLARE_FLAGS(SceneItemFlags, SceneItemFlag)

    enum DataKey {
        UserData = 0,
        QMLParent = 1,
        PaintData = 2,
        ModelData = 3
    };

    SceneItem(SceneItem *parent = 0);
    virtual ~SceneItem();

    QRectF rect() const;
    QRectF boundingRect() const;

    QPointF position() const;
    qreal x() const;
    qreal y() const;

    QSizeF size() const;
    qreal width() const;
    qreal height() const;

    qreal rotation() const;
    qreal scale() const;
    QPointF transformOriginPoint() const;

    qreal opacity() const;
    bool visible() const;
    qreal z() const;

    bool hoverEnabled() const;
    Qt::MouseButtons acceptedButtons() const;

    bool isSelected() const;
    bool isHovered() const;
    bool isPressed() const;
    bool dragging() const;

    QVariant data() const;

    bool clip() const;

    SceneItem *parentItem() const;

    SceneItemFlags flags() const;
    QGraphicsItem *item() const;

    Scene *scene() const;

    qreal gridStep() const;

    void setRect(QRectF arg);

    void setPosition(QPointF arg);
    void setX(qreal arg);
    void setY(qreal arg);

    void setSize(QSizeF arg);
    void setWidth(qreal arg);
    void setHeight(qreal arg);

    void setRotation(qreal arg);
    void setScale(qreal arg);
    void setTransformOriginPoint(QPointF arg);

    void setOpacity(qreal arg);
    void setVisible(bool arg);
    void setZ(qreal arg);

    void setHoverEnabled(bool arg);
    void setAcceptedButtons(Qt::MouseButtons arg);

    void setSelected(bool arg);

    void setData(QVariant arg);

    void setClip(bool arg);

    void setParentItem(SceneItem* arg);

    void setGridStep(qreal arg);
//    void paint(QPainter *painter);

    Q_INVOKABLE QPointF mapFromItem(const SceneItem * item, const QPointF & point) const;
    Q_INVOKABLE QPolygonF mapFromItem(const SceneItem * item, const QRectF & rect) const;
    Q_INVOKABLE QPolygonF mapFromItem(const SceneItem * item, const QPolygonF & polygon) const;
    Q_INVOKABLE QPainterPath mapFromItem(const SceneItem * item, const QPainterPath & path) const;
    Q_INVOKABLE QPolygonF mapFromItem(const SceneItem * item, qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE QPointF mapFromItem(const SceneItem * item, qreal x, qreal y) const;

    Q_INVOKABLE QPointF mapFromParent(const QPointF & point) const;
    Q_INVOKABLE QPolygonF mapFromParent(const QRectF & rect) const;
    Q_INVOKABLE QPolygonF mapFromParent(const QPolygonF & polygon) const;
    Q_INVOKABLE QPainterPath mapFromParent(const QPainterPath & path) const;
    Q_INVOKABLE QPolygonF mapFromParent(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE QPointF mapFromParent(qreal x, qreal y) const;

    Q_INVOKABLE QPointF mapFromScene(const QPointF & point) const;
    Q_INVOKABLE QPolygonF mapFromScene(const QRectF & rect) const;
    Q_INVOKABLE QPolygonF mapFromScene(const QPolygonF & polygon) const;
    Q_INVOKABLE QPainterPath mapFromScene(const QPainterPath & path) const;
    Q_INVOKABLE QPolygonF mapFromScene(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE QPointF mapFromScene(qreal x, qreal y) const;

    Q_INVOKABLE QRectF mapRectFromItem(const SceneItem * item, const QRectF & rect) const;
    Q_INVOKABLE QRectF mapRectFromItem(const SceneItem * item, qreal x, qreal y, qreal w, qreal h) const;

    Q_INVOKABLE QRectF mapRectFromParent(const QRectF & rect) const;
    Q_INVOKABLE QRectF mapRectFromParent(qreal x, qreal y, qreal w, qreal h) const;

    Q_INVOKABLE QRectF mapRectFromScene(const QRectF & rect) const;
    Q_INVOKABLE QRectF mapRectFromScene(qreal x, qreal y, qreal w, qreal h) const;

    Q_INVOKABLE QRectF mapRectToItem(const SceneItem * item, const QRectF & rect) const;
    Q_INVOKABLE QRectF mapRectToItem(const SceneItem * item, qreal x, qreal y, qreal w, qreal h) const;

    Q_INVOKABLE QRectF mapRectToParent(const QRectF & rect) const;
    Q_INVOKABLE QRectF mapRectToParent(qreal x, qreal y, qreal w, qreal h) const;

    Q_INVOKABLE QRectF mapRectToScene(const QRectF & rect) const;
    Q_INVOKABLE QRectF mapRectToScene(qreal x, qreal y, qreal w, qreal h) const;

    Q_INVOKABLE QPointF mapToItem(const SceneItem * item, const QPointF & point) const;
    Q_INVOKABLE QPolygonF mapToItem(const SceneItem * item, const QRectF & rect) const;
    Q_INVOKABLE QPolygonF mapToItem(const SceneItem * item, const QPolygonF & polygon) const;
    Q_INVOKABLE QPainterPath mapToItem(const SceneItem * item, const QPainterPath & path) const;
    Q_INVOKABLE QPolygonF mapToItem(const SceneItem * item, qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE QPointF mapToItem(const SceneItem * item, qreal x, qreal y) const;

    Q_INVOKABLE QPointF mapToParent(const QPointF & point) const;
    Q_INVOKABLE QPolygonF mapToParent(const QRectF & rect) const;
    Q_INVOKABLE QPolygonF mapToParent(const QPolygonF & polygon) const;
    Q_INVOKABLE QPainterPath mapToParent(const QPainterPath & path) const;
    Q_INVOKABLE QPolygonF mapToParent(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE QPointF mapToParent(qreal x, qreal y) const;

    Q_INVOKABLE QPointF mapToScene(const QPointF & point) const;
    Q_INVOKABLE QPolygonF mapToScene(const QRectF & rect) const;
    Q_INVOKABLE QPolygonF mapToScene(const QPolygonF & polygon) const;
    Q_INVOKABLE QPainterPath mapToScene(const QPainterPath & path) const;
    Q_INVOKABLE QPolygonF mapToScene(qreal x, qreal y, qreal w, qreal h) const;
    Q_INVOKABLE QPointF mapToScene(qreal x, qreal y) const;

    Cursor* cursor() const;

public slots:
    void moveBy(qreal dx, qreal dy);
    void moveBy(QVector2D vec);
    void moveBy(QPointF pt);

    void update(const QRectF &rect = QRectF());

    void setFlags(SceneItemFlags arg);
    void setScene(Scene *arg);
    void setCursor(Cursor *arg);

signals:
    void rectChanged(QRectF arg);
    void positionChanged();
    void xChanged(qreal arg);
    void yChanged(qreal arg);
    void sizeChanged(QSizeF arg);
    void widthChanged(qreal arg);
    void heightChanged(qreal arg);
    void rotationChanged();
    void opacityChanged();
    void transformOriginPointChanged();
    void visibleChanged();
    void hoverEnabledChanged();
    void acceptedButtonsChanged(Qt::MouseButtons arg);
    void selectedChanged();
    void hoveredChanged(bool arg);
    void hoverPositionChanged(QPointF pos);
    void pressedChanged(bool arg);
    void draggingChanged(bool arg);

    void dataChanged(QVariant arg);
    void zChanged(qreal arg);
    void clipChanged(bool arg);
    void childItemsChanged();
    void boundingRectChanged(QRectF arg);
    void scaleChanged();
    void parentItemChanged(SceneItem* arg);

    void childItemAdded();
    void transformChanged();
    void enabledChanged();
    void parentChanged();
    void sceneChanged();
    void cursorChanged();
    void toolTipChanged();
    void flagsChanged();
    void zValueChanged();
    void scenePositionChanged();

    void flagsChanged(SceneItemFlags arg);

    void gridStepChanged(qreal arg);

    void cursorChanged(Cursor *arg);

    void pressed(int buttons, QPointF pos);
    void moved(int buttons, QPointF pos);
    void released(QPointF pos);

protected:
    SceneItem(QuickGraphicsItem *item, SceneItem *parent = 0);
    inline const QRectF *rectPtr() const { return &m_rect; }

    void componentComplete();
    void classBegin();
    virtual void initScene() {}

private:
    friend class QuickGraphicsItem;
    void checkDeleted();
    void destroyHelper();

    QuickGraphicsItem* m_item;

    bool m_hovered:1;
    bool m_pressed:1;
    bool m_dragging:1;
    bool isDestroying:1;

    inline void setHovered(bool arg);
    inline void setPressed(bool arg);
    inline void setDragging(bool arg);

    QRectF m_rect;
    SceneItemFlags m_flags;
    QQmlListProperty<SceneItem> qmlChildItems();
    static void qmlChildItemsAppend(QQmlListProperty<SceneItem> *list, SceneItem *object);
    static int qmlChildItemsCount(QQmlListProperty<SceneItem> *list);
    static SceneItem *qmlChildItemsAt(QQmlListProperty<SceneItem> *list, int index);
    qreal m_gridStep;
    Cursor* m_cursor;
};


void SceneItem::setHovered(bool arg) {
    if (m_hovered == arg)
        return;
    m_hovered = arg;
    emit hoveredChanged(arg);
}

void SceneItem::setPressed(bool arg)
{
    if (m_pressed == arg)
        return;
    m_pressed = arg;
    emit pressedChanged(arg);
}

void SceneItem::setDragging(bool arg)
{
    if (m_dragging == arg)
        return;

    m_dragging = arg;
    emit draggingChanged(arg);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(SceneItem::SceneItemFlags)

#endif // SCENEITEM_H
