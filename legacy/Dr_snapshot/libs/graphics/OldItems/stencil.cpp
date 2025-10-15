#include "stencil.h"
#include <QMetaProperty>
#include <QColor>
#include <QFont>
#include <QDebug>

QQmlListProperty<QObject> Stencil::qmlData() {
    return QQmlListProperty<QObject>(this, 0,
                                     &Stencil::qmlDataAppend,
                                     &Stencil::qmlDataCount,
                                     &Stencil::qmlDataAt,
                                     QQmlListProperty<QObject>::ClearFunction());
}

void Stencil::qmlDataAppend(QQmlListProperty<QObject> *list, QObject *object)
{
    Stencil *o = qobject_cast<Stencil*>(list->object);
    o->m_data.append(object);
    object->setParent(o);
    emit o->dataChanged();
}

int Stencil::qmlDataCount(QQmlListProperty<QObject> *list)
{
    Stencil *o = qobject_cast<Stencil*>(list->object);
    return o->m_data.size();
}

QObject *Stencil::qmlDataAt(QQmlListProperty<QObject> *list, int index)
{
    Stencil *o = qobject_cast<Stencil*>(list->object);
    return o->m_data.at(index);
}

Stencil::Stencil(QObject *parent) :
    GraphicsPath(parent)
{
}

void Stencil::classBegin()
{
}



void Stencil::componentComplete()
{
    blockSignals(true);
    QList<QVariant> oldValues;
    const QMetaObject *mo = metaObject();
    int slotIdx = mo->indexOfSlot("updateShape()");
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
        QMetaProperty p = mo->property(i);
        oldValues.append(p.read(this));
        BindData d;
        d.propertyIndex = i;
        d.notifySignalIndex = p.notifySignalIndex();
        p.write(this, d.storeToType(p.type()));
    }
    blockSignals(false);
    emit draw();
    blockSignals(true);
    auto it = bindings.cbegin();
    while (it != bindings.cend()) {
        int sigId = it.key();
        QMetaObject::connect(this, sigId, this, slotIdx);
        ++it;
    }
    int c = 0;
    blockSignals(false);
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
        QMetaProperty p = mo->property(i);
//        qDebug() << "Restoring value" << p.name() << oldValues.at(c);
        p.write(this, oldValues.at(c++));
    }
    emit pathChanged();

}

void Stencil::moveTo(QPointF pos)
{
    //    qreal x = pos.x();
    //    qreal y = rpath().elementCount();
    addPointBinding(pos);
    //    bindings[(int)pos.y()].pathElements.append(rpath().elementCount());
    //    bindings[(int)pos.y()].propertyIndex = (int)x;
    rpath().moveTo(pos);
}

void Stencil::addCircle(QPointF center, qreal radius)
{
    BindData pt = BindData::fromValue(center);
    BindData r = BindData::fromValue(radius);
    QVector<int> props;
    props.append(pt.propertyIndex);
    props.append(r.propertyIndex);
    addBinding(props, pt.notifySignalIndex, updateCircle);
    addBinding(props, r.notifySignalIndex, updateCircle);
    QRectF rect(0, 0, radius * 2, radius * 2);
    rect.moveCenter(center);
    rpath().arcMoveTo(rect, 0);
    rpath().arcTo(rect, 0, 360);

}

void Stencil::lineTo(QPointF pos)
{
    //    qreal x = pos.x();
    //    qreal y = rpath().elementCount();
    //    bindings[(int)pos.y()].pathElements.append(rpath().elementCount());
    //    bindings[(int)pos.y()].propertyIndex = (int)x;

    addPointBinding(pos);
    //    rpath().lineTo(x, y);
    rpath().lineTo(pos);
}

void Stencil::cubicTo(QPointF cp1, QPointF cp2, QPointF pos)
{
    addPointBinding(cp1);
    addPointBinding(cp2);
    addPointBinding(pos);
    //    qreal y = rpath().elementCount();
    //    qreal cp1x = cp1.x();
    //    qreal cp2x = cp2.x();
    //    qreal x = pos.x();
    //    bindings[(int)cp1.y()].pathElements.append(rpath().elementCount());
    //    bindings[(int)cp1.y()].propertyIndex = (int)cp1x;
    //    bindings[(int)cp2.y()].pathElements.append(rpath().elementCount() + 1);
    //    bindings[(int)cp2.y()].propertyIndex = (int)cp2x;
    //    bindings[(int)pos.y()].pathElements.append(rpath().elementCount() + 2);
    //    bindings[(int)pos.y()].propertyIndex = (int)x;
    //    rpath().cubicTo(cp1x, y, cp2x, y, x, y);
    rpath().cubicTo(cp1, cp2, pos);
}

void Stencil::updateShape()
{
//    qDebug() << "notifier" << senderSignalIndex() << metaObject()->method(senderSignalIndex()).methodSignature();
    for (const Binding &b:bindings.value(senderSignalIndex())){
        QVariantList args;
        for (int idx:b.properties)
            args.append(metaObject()->property(idx).read(this));
//        qDebug() << "Calling update fn args" << args << b.elementIndex;
        b.fn(this, args, b.elementIndex);
    }
}

void Stencil::addBinding(const QVector<int> &props, int notifySignalIndex, Stencil::UpdateFn function)
{
    Binding b;
    b.elementIndex = path().elementCount();
    b.fn = function;
    b.properties = props;
    bindings[notifySignalIndex].append(b);
}

//void Stencil::addBinding(const QVector<int> &&props, int notifySignalIndex, UpdateFn function)
//{
//    Binding b;
//    b.elementIndex = path().elementCount();
//    b.fn = function;
//    b.properties = props;
//    bindings[notifySignalIndex].append(b);
//}

void Stencil::addPointBinding(const QPointF &p1)
{
    BindData d = BindData::fromValue(p1);
    QVector<int> props;
    props << d.propertyIndex;
//    addBinding({d.propertyIndex}, d.notifySignalIndex, updatePoint);
    addBinding(props, d.notifySignalIndex, updatePoint);
}

void Stencil::updatePoint(Stencil *obj, QVariantList args, int element)
{
    Q_ASSERT(args.size() == 1);
    QPointF p = args.first().toPointF();
    obj->setElementPosition(element, p.x(), p.y());
}

void Stencil::updateCircle(Stencil *obj, QVariantList args, int element)
{
    Q_ASSERT(args.size() == 2);
    QPointF p = args.at(0).toPointF();
    qreal r = args.at(1).toDouble();
    QSignalBlocker _b(obj);
//    qDebug() << "update circle at" << p << "radius" << r;
    obj->setElementPosition(element + 0, p + QPointF(r * 1, r * 0));
    obj->setElementPosition(element + 1, p + QPointF(r * 1, r * -0.552285));
    obj->setElementPosition(element + 2, p + QPointF(r * 0.552285, r * -1));
    obj->setElementPosition(element + 3, p + QPointF(r * 0, r * -1));
    obj->setElementPosition(element + 4, p + QPointF(r * -0.552285, r * -1));
    obj->setElementPosition(element + 5, p + QPointF(r * -1, r * -0.552285));
    obj->setElementPosition(element + 6, p + QPointF(r * -1, r * 0));
    obj->setElementPosition(element + 7, p + QPointF(r * -1, r * 0.552285));
    obj->setElementPosition(element + 8, p + QPointF(r * -0.552285, r * 1));
    obj->setElementPosition(element + 9, p + QPointF(r * 0, r * 1));
    obj->setElementPosition(element + 10, p + QPointF(r * 0.552285, r * 1));
    obj->setElementPosition(element + 11, p + QPointF(r * 1, r * 0.552285));
    obj->setElementPosition(element + 12, p + QPointF(r * 1, r * 0));

}

QVariant Stencil::prop(int index) const
{
    return metaObject()->property(index).read(this);
}

QVariant Stencil::BindData::storeToType(QVariant::Type type)
{
    switch (type) {
    case QVariant::PointF:
        return QVariant(QPointF(propertyIndex, notifySignalIndex));
    case QVariant::Double:
        return QVariant((double)((propertyIndex << 10) | notifySignalIndex));
    case QVariant::Color: {
        int a = propertyIndex & 0xff;
        int r = (propertyIndex & 0xff00) >> 8;
        int g = notifySignalIndex & 0xff;
        int b = (notifySignalIndex & 0xff00) >> 8;
        return QVariant(QColor(r,g,b,a));
    }
    case QVariant::Font: {
        QFont f;
        f.setPixelSize(propertyIndex);
        f.setStretch(notifySignalIndex);
        return QVariant(f);
    }
    case QVariant::RectF: {
        QRectF r;
        r.setX(propertyIndex);
        r.setY(notifySignalIndex);
        return QVariant(r);
    }
    default:
        qDebug() << "Unable to store binding data in value type" << QMetaType::typeName(type);
        break;
    }
    return QVariant();
}

Stencil::BindData Stencil::BindData::fromVariant(const QVariant &value)
{
    switch (value.type()) {
    case QVariant::PointF:
        return fromValue(value.value<QPointF>());
    case QVariant::Double:
        return fromValue(value.value<double>());
    case QVariant::Color:
        return fromValue(value.value<QColor>());
    case QVariant::Font:
        return fromValue(value.value<QFont>());
    case QVariant::RectF:
        return fromValue(value.value<QRectF>());
    default:
        qWarning() << "Unable to load binding data from value type" << value.typeName();
        break;
    }
    return BindData();
}

Stencil::BindData Stencil::BindData::fromValue(const QPointF &point)
{
    BindData ret;
    ret.propertyIndex = point.x();
    ret.notifySignalIndex = point.y();
    return ret;
}

Stencil::BindData Stencil::BindData::fromValue(double real)
{
    BindData ret;
    ret.propertyIndex = (int)real >> 10;
    ret.notifySignalIndex = (int)real & 0x3ff;
    return ret;
}

Stencil::BindData Stencil::BindData::fromValue(const QColor &color)
{
    BindData ret;
    ret.propertyIndex = color.alpha() + (color.red() << 8);
    ret.notifySignalIndex = color.green() + (color.blue() << 8);
    return ret;
}

Stencil::BindData Stencil::BindData::fromValue(const QRectF &rect)
{
    BindData ret;
    ret.propertyIndex = rect.x();
    ret.notifySignalIndex = rect.y();
    return ret;
}

Stencil::BindData Stencil::BindData::fromValue(const QFont &font)
{
    BindData ret;
    ret.propertyIndex = font.pixelSize();
    ret.notifySignalIndex = font.stretch();
    return ret;
}
