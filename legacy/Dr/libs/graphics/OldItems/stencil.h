#ifndef STENCIL_H
#define STENCIL_H

#include <QObject>
#include <QPainterPath>
#include <QPicture>
#include "graphicspath.h"
#include <qqml.h>
#include <QHash>


class Stencil : public GraphicsPath, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<QObject> data READ qmlData NOTIFY dataChanged)

    QQmlListProperty<QObject> qmlData();

    static void qmlDataAppend(QQmlListProperty<QObject> *list, QObject *object);
    static int qmlDataCount(QQmlListProperty<QObject> *list);
    static QObject *qmlDataAt(QQmlListProperty<QObject> *list, int index);

    Q_CLASSINFO("DefaultProperty", "data")

public:

    typedef void(*UpdateFn)(Stencil*,QVariantList,int);

    explicit Stencil(QObject *parent = 0);
    void classBegin();
    void componentComplete();

public slots:
    void moveTo(QPointF pos);
    void lineTo(QPointF pos);
    void cubicTo(QPointF cp1, QPointF cp2, QPointF pos);
    void addCircle(QPointF center, qreal radius);

signals:
    void draw();
    void dataChanged();

private slots:
    void updateShape();

private:
    QList<QObject*> m_data;

    struct Binding {
        QVector<int> properties;
        int elementIndex;
        UpdateFn fn;
    };

    struct BindData {
        int propertyIndex;
        int notifySignalIndex;

        QVariant storeToType(QVariant::Type type);

        static BindData fromVariant(const QVariant &value);

        static BindData fromValue(const QPointF &point);
        static BindData fromValue(double real);
        static BindData fromValue(const QColor &color);
        static BindData fromValue(const QRectF &rect);
        static BindData fromValue(const QFont &font);

        BindData() : propertyIndex(-1) , notifySignalIndex(-1) {}
    };

    QHash<int, QList<Binding>> bindings;

    void addBinding(const QVector<int> &props, int notifySignalIndex, UpdateFn function);
//    void addBinding(const QVector<int> &&props, int notifySignalIndex, UpdateFn function);
    void addPointBinding(const QPointF &p1);

    static void updatePoint(Stencil *obj, QVariantList args, int element);
    static void updateCircle(Stencil *obj, QVariantList args, int element);
    QVariant prop(int index) const;
};


#endif // SHAPEELEMENT_H
