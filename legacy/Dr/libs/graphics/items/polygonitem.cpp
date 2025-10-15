#include "polygonitem.h"
#include "shapeinstance.h"
#include "shapeinstance.h"
#include "visualinstance.h"

#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <guidepath.h>

void PolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option)
    //        qreal s = scene()->viewportScale();
    switch (shapeType) {
    case ::ShapeInstanceHandle::Polygonal:
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawPolygon(polygon);
        break;
    case ::ShapeInstanceHandle::Linear:
        painter->setPen(pen);
        painter->drawPolyline(polygon);
        break;
    default:
        break;
    }

    qreal s = currentViewTransform().m11();

//    if (s < 8)
        return;

    QPen dotPen;
    dotPen.setWidthF(pen.width() * 3.5);
    dotPen.setCosmetic(true);
    dotPen.setCapStyle(Qt::RoundCap);
    dotPen.setColor(pen.color());
    painter->setPen(dotPen);
    painter->drawPoints(polygon);
    //    painter->drawLines(lines);
    QPen centerPen;
    centerPen.setCosmetic(true);
    centerPen.setColor(QColor(0,0,0));
    centerPen.setCapStyle(Qt::RoundCap);
    centerPen.setWidthF(pen.width() * 2);
//    outlinePen.setWidthF(pen()->width() * 1.5);
    painter->setPen(centerPen);
    painter->drawPoints(polygon);
    QFont f;
    f.setPointSizeF(10);
    painter->setFont(f);
    int i = 0;

    painter->setPen(Qt::white);
    painter->save();
    painter->scale(1/s, 1/s);
    painter->translate(-4, 0);
    for (QPointF pt:polygon) {
        painter->drawText(pt * s, QString::number(i++));
    }
    painter->restore();
}

QPainterPath PolygonItem::shape() const
{
    if(shapeType == ShapeInstanceHandle::Linear) {
        GuidePath p(polygon);
        return p.outline(10 / currentViewTransform().m11(), Qt::RoundCap);
    }
    QPainterPath s;
    s.addPolygon(polygon);
    return s;
}

QRectF PolygonItem::boundingRect() const
{
    return m_boundingRect;
}

int PolygonItem::type() const
{
    return SELF_PTR_TYPEID;
}

QPolygonF PolygonItemHandle::polygon() const
{
    return item()->polygon;
}

void PolygonItemHandle::setPolygon(QPolygonF polygon)
{
    item()->polygon = polygon;
    item()->m_boundingRect = polygon.boundingRect();
    item()->prepareGeometryChange();
    item()->update();
}


PolygonFactory::PolygonFactory(QObject *parent) : QObject(parent) {}

QAbstractListModel *PolygonFactory::model() const
{
    return m_model;
}

QPolygonF PolygonFactory::polygon() const
{
    return m_polygon;
}

QString PolygonFactory::pointRole() const
{
    return m_pointRole;
}

QString PolygonFactory::xRole() const
{
    return m_xRole;
}

QString PolygonFactory::yRole() const
{
    return m_yRole;
}

void PolygonFactory::setModel(QAbstractListModel *model)
{
    if (m_model == model)
        return;

    m_model = model;

    emit modelChanged(model);
    connect(model, &QAbstractListModel::dataChanged, this, &PolygonFactory::updatePolygon);
    connect(model, &QAbstractListModel::rowsInserted, this, &PolygonFactory::bindRoles);
    if (model->rowCount(QModelIndex()) > 0) {
        if (bindRoles())
            buildPolygon();
    }

}

void PolygonFactory::setPolygon(QPolygonF polygon)
{
    if (m_polygon == polygon)
        return;

    m_polygon = polygon;
    emit polygonChanged(polygon);
    //    if (bindRoles())
    //        buildPolygon();
}

void PolygonFactory::setPointRole(QString pointRole)
{
    if (m_pointRole == pointRole)
        return;

    m_pointRole = pointRole;
    if (bindRoles())
        buildPolygon();
    emit pointRoleChanged(pointRole);
}

void PolygonFactory::setXRole(QString xRole)
{
    if (m_xRole == xRole)
        return;

    m_xRole = xRole;
    if (bindRoles())
        buildPolygon();
    emit xRoleChanged(xRole);
}

void PolygonFactory::setYRole(QString yRole)
{
    if (m_yRole == yRole)
        return;

    m_yRole = yRole;
    if (bindRoles())
        buildPolygon();
    emit yRoleChanged(yRole);
}

void PolygonFactory::buildPolygon()
{
//    qDebug() << "Building polygon" << m_model->rowCount(QModelIndex()) << m_polygon;
    if (!m_model)
        return;
    if (m_pointRole.isEmpty() && (m_xRole.isEmpty() || m_yRole.isEmpty())) {
        qWarning() << "Point role or both x and y roles must be provided";
        return;
    }
    if (i_x < 0 && i_y < 0 && i_p < 0) {
        qWarning() << "Unbound roles!";
        return;
    }
    ////    if (i_x < 0)
    ////    bindRoles();
    //    if (i_x < 0) {
    //        qWarning() << "role" << m_xRole << "does not exist!";
    //        return;
    //    }
    //    if (i_y < 0) {
    //        qWarning() << "role" << m_yRole << "does not exist!";
    //        return;
    //    }

    //    if (m_pointRole.isEmpty()) {
    //        i_x = roles.key(m_xRole.toLocal8Bit(), -1);
    //        i_y = roles.key(m_yRole.toLocal8Bit(), -1);
    //    } else {
    //        i_p = roles.key(m_pointRole.toLocal8Bit(), -1);
    //        if (i_p < 0) {
    //            qWarning() << "role" << m_pointRole << "does not exist!";
    //            return;
    //        }
    //    }
    int size = m_model->rowCount(QModelIndex());
    m_polygon.resize(size);
    for (int i = 0; i < size; ++i) {
        QPointF pt;
        QModelIndex index = m_model->index(i, 0, QModelIndex());
        if (!index.isValid()) {
            qWarning() << "Invalid index at pos" << i;
            continue;
        }
        if (i_p < 0) {
            bool ok;
            pt.rx() = index.data(i_x).toDouble(&ok);
            pt.ry() = index.data(i_y).toDouble(&ok);
            //            qDebug() << "got coords" << pt.x() << pt.y();
            if (ok)
                m_polygon[i] = pt;
            else
                qWarning() << "Error at point" << i << index.data(i_x) << index.data(i_y);
        } else {

            pt = index.data(i_p).toPointF();
            m_polygon[i] = pt;
        }
    }
    emit polygonChanged(m_polygon);
}

void PolygonFactory::updatePolygon(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!roles.isEmpty() && !roles.contains(i_p) && !roles.contains(i_y) && !roles.contains(i_x))
        return;
    if (bottomRight.row() >= m_polygon.size())
        m_polygon.resize(bottomRight.row() + 1);
    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        QModelIndex idx = m_model->index(i, 0, QModelIndex());
        //        qDebug() << "Updating polygon" << roles;
        QPointF p;
        if (roles.contains(i_p))
            m_polygon[i] = idx.data(i_p).toPointF();
        if (roles.contains(i_x))
            m_polygon[i].setX(idx.data(i_x).toReal());
        if (roles.contains(i_y))
            m_polygon[i].setY(idx.data(i_y).toReal());
    }
    emit polygonChanged(m_polygon);
}

bool PolygonFactory::bindRoles()
{
    if (!m_model)
        return false;
    bool res = true;
    QHash<int, QByteArray> roles = m_model->roleNames();
    if (m_pointRole.isEmpty()) {
        i_x = roles.key(m_xRole.toLocal8Bit(), -1);
        i_y = roles.key(m_yRole.toLocal8Bit(), -1);
        if (i_x < 0) {
            qWarning() << "role" << m_xRole << "does not exist!";
            res = false;
        }
        if (i_y < 0) {
            qWarning() << "role" << m_yRole << "does not exist!";
            res = false;
        }
    } else {
        i_p = roles.key(m_pointRole.toLocal8Bit(), -1);
        if (i_p < 0) {
            qWarning() << "role" << m_pointRole << "does not exist!";
            res = false;
        }
    }
    return res;
}

