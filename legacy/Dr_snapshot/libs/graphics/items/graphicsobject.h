#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H


#include "visualinstance.h"
#include <QObject>
#include <QQmlParserStatus>
#include <eventwrappers.h>


class GraphicsObject : public QGraphicsObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QRectF boundingRect READ boundingRect NOTIFY boundingRectChanged)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedMouseButtons WRITE setAcceptedMouseButtons)
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(bool hovered READ isHovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool pressed READ isPressed NOTIFY pressedChanged)
    Q_PROPERTY(Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(VisualInstanceHandle::VisualInstanceFlags flags READ flags WRITE setFlags NOTIFY flagsChanged)
    Q_PROPERTY(QQmlListProperty<QObject> children READ qmlChildren NOTIFY childrenChanged)

    Q_INTERFACES(QQmlParserStatus)
    Q_CLASSINFO("DefaultProperty", "children")

public:
    explicit GraphicsObject(QGraphicsItem *parent = 0);

    Scene *scene() const;
    void setScene(Scene* scene);

    VisualInstanceHandle::VisualInstanceFlags flags() const;
    void setFlags(VisualInstanceHandle::VisualInstanceFlags flags);
    Q_INVOKABLE bool bind(VisualInstance::GraphicsItemChange change, QJSValue callback);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    virtual void setupScene(Scene *scene) { Q_UNUSED(scene); }

    bool isHovered() const;
    bool isPressed() const;
    bool hoverEnabled() const;
    void setHoverEnabled(bool hoverEnabled);

    QTransform currentViewTransform() const;

protected:
    void preparePainter(QPainter *painter) const;

signals:
    void flagsChanged();
    void sceneChanged();
    void boundingRectChanged();
    void hoveredChanged();
    void pressedChanged();
    void hoverEnabledChanged();
    void selectedChanged();
    void childrenChanged();

    void mousePress(GraphicsSceneMouseEvent mouse);
    void mouseRelease(GraphicsSceneMouseEvent mouse);
    void mouseMove(GraphicsSceneMouseEvent mouse);
    void mouseClicked(GraphicsSceneMouseEvent mouse);

    void hoverEnter(GraphicsSceneHoverEvent hover);
    void hoverLeave(GraphicsSceneHoverEvent hover);
    void hoverMove(GraphicsSceneHoverEvent hover);

public slots:
    void update(const QRectF &rect = QRectF());
    void grabMouse();
    void ungrabMouse();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void classBegin();
    void componentComplete();



protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QQmlListProperty<QObject> qmlChildren();
    template<class T>
    QJSValue handleItemChange(QQmlEngine *engine, QJSValue thisObject, QJSValue callback, T arg)
    {
        return callback.callWithInstance(thisObject, { engine->toScriptValue(arg) });
    }
    static void qmlAppendChild(QQmlListProperty<QObject> *list, QObject *child);
    static int qmlChildCount(QQmlListProperty<QObject> *list);
    static QObject *qmlChildAt(QQmlListProperty<QObject> *list, int index);
    void appendChild(QObject *object);
    int removeChild(QObject *object);
    QList<QObject*>         m_children;
    bool                    m_hovered : 1;
    bool                    m_pressed : 1;
};

#endif // GRAPHICSOBJECT_H
