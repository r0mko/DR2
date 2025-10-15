#include "groove.h"
#include "qqml.h"
#include <QQmlEngine>
#include <QAbstractListModel>
Groove::Groove(QObject *parent) :
    QObject(parent),
    v(new VectorPath),
    ownsPath(true)
{
}

Groove::Groove(const VectorPath &vp, QObject *parent) :
    QObject(parent),
    v(new VectorPath(vp)),
    ownsPath(true)
{}
Groove::Groove(VectorPath *vp, QObject *parent) :
    QObject(parent),
    v(vp),
    ownsPath(false)
{
}

Groove::~Groove()
{
    if (ownsPath) {
        delete v;
    }
}

qreal Groove::length() const
{
    return v->length();
}

bool Groove::isNull() const
{
    return v->isNull();
}

QPolygonF Groove::polygon() const
{
    return v->toPolygon();
}

QPointF Groove::pointAtLength(qreal length)
{
    return v->pointAtLength(length);
}

QPointF Groove::nearestPoint(QPointF point)
{
    qreal len;
    return v->nearestPoint(point, &len);
}

qreal Groove::nearestPos(QPointF point) const
{
    qreal len;
    v->nearestPoint(point, &len);

    //    qDebug() << "nrst pos" << point << len;
    return len;
}

GrooveRunner *Groove::setRunner(qreal pos) const
{
    VectorPath::Runner r = v->setRunner(pos);
    GrooveRunner *gr = new GrooveRunner(const_cast<Groove*>(this), r);
//    QQmlEngine::setObjectOwnership(gr, QQmlEngine::CppOwnership);
    return gr;
}

void Groove::setVectorPath(const VectorPath &vp)
{
    if (v && ownsPath)
        delete v;
    v = new VectorPath(vp);
    ownsPath = true;
    emit lengthChanged();
    emit polygonChanged();
    emit polygonDataChanged();
}

QVariantList Groove::partData(qreal inPos, qreal outPos)
{
    VectorPath p = v->getPart(inPos, outPos);
    return toVaritantList(p);
}

Groove *Groove::split(qreal pos)
{
    VectorPath rest = v->split(pos);
    emit polygonChanged();
    qDebug() << "Splitted vp" << rest.length() << v->length();
    return new Groove(rest, this);
}

QVariantList Groove::toVaritantList(const VectorPath &vectorPath) const
{
    QVariantList ret;
    for (const VectorPath::VNode &n:vectorPath.nodes) {
        QVariantMap node;
        node["x"] = n.point.x();
        node["y"] = n.point.y();
        node["length"] = n.sideLength;
        node["vx"] = n.unitVector.x();
        node["vy"] = n.unitVector.y();
        node["data"] = n.data;
        ret.append(node);
    }
    return ret;
}

QObject *Groove::nodeModel() const
{
    return m_nodeModel;
}

QVariantList Groove::polygonData() const
{
    return toVaritantList(*v);
}


void Groove::setPolygon(QPolygonF arg)
{
    v->clear();
    v->addPolygon(arg);
    emit polygonChanged();
}

void Groove::addPolygon(QPolygonF polygon, QVariant &data)
{
    v->addPolygon(polygon, data);
    setLength(v->length());
}

void Groove::setPolygonData(QVariantList arg)
{
    v->clear();
    for (const QVariant &var:arg) {
        QVariantMap n = var.toMap();
        if (n.isEmpty()) {
            qWarning() << "Groove JSON parser: Wrong format!";
            qWarning() << "Groove JSON parser: JS object dump:" << var;
            break;
        }
        VectorPath::VNode node;
        node.point = QPointF(n["x"].toReal(), n["y"].toReal());
        node.unitVector = QVector2D(n["vx"].toFloat(), n["vy"].toFloat());
        node.sideLength = n["length"].toDouble();
        node.data = n["data"];
        v->appendNode(node);
    }
}

void Groove::setNodeModel(QObject* arg)
{
    if (m_nodeModel == arg)
        return;
    m_nodeModel = arg;
    QAbstractListModel *mdl = qobject_cast<QAbstractListModel*>(m_nodeModel);
    if (mdl) {
        v->clear();
        int x = mdl->roleNames().key("x");
        int y = mdl->roleNames().key("y");
        int vx = mdl->roleNames().key("vx");
        int vy = mdl->roleNames().key("vy");
        int length = mdl->roleNames().key("length");
        int data= mdl->roleNames().key("data");
        for (int i = 0; i < mdl->rowCount(); ++i) {
            VectorPath::VNode node;
            QModelIndex idx = mdl->index(i);
            node.point = QPointF(idx.data(x).toReal(), idx.data(y).toReal());
            node.unitVector = QVector2D(idx.data(vx).toFloat(), idx.data(vy).toFloat());
            node.sideLength = idx.data(length).toReal();
            node.data = idx.data(data);
            v->appendNode(node);
            qDebug() << i << idx.data(data);
        }
        emit polygonChanged();
    }
    emit nodeModelChanged();
}


void Groove::setLength(qreal arg)
{
    if (v->length() == arg)
        return;
    v->m_length = arg;
    emit lengthChanged();
}


GrooveRunner::GrooveRunner(Groove *groove, const VectorPath::Runner &r) :
    QObject(groove),
    m_guide(groove),
    r(r)
{
    connect(groove, SIGNAL(lengthChanged()), this, SLOT(refresh()));
    QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);
}

GrooveRunner::~GrooveRunner()
{
//    qDebug() << "runner destroyed";
}

Groove *GrooveRunner::guide() const
{
    return m_guide;
}

void GrooveRunner::setGuide(Groove *arg)
{
    if (m_guide == arg || !arg)
        return;
    m_guide = arg;
    r = m_guide->vectorPath().setRunner(r.position());
    emit grooveChanged();
}

qreal GrooveRunner::position() const
{
    return r.position();
}

bool GrooveRunner::isNull() const
{
    return r.isNull();
}

bool GrooveRunner::atEnd() const
{
    return r.atEnd();
}

bool GrooveRunner::atStart() const
{
    return r.atStart();
}

bool GrooveRunner::atNode() const
{
    return r.atEnd();
}

bool GrooveRunner::atFirstEdge() const
{
    return !r.hasPrev();
}

bool GrooveRunner::atLastEdge() const
{
    return !r.hasNext();
}

QVariant GrooveRunner::data() const
{
    return r.data();
}

qreal GrooveRunner::lengthBehind() const
{
    return r.lengthBehind();
}

qreal GrooveRunner::lengthAhead() const
{
    return r.lengthAhead();
}

QPointF GrooveRunner::point() const
{
    return r.point();
}

qreal GrooveRunner::angle() const
{
    return r.angle();
}

QVector2D GrooveRunner::normal() const
{
    return r.normal();
}

void GrooveRunner::setPosition(qreal arg)
{
    if (r.position() == arg)
        return;
    if (!m_guide) {
        r.m_position = arg;
        return;
    }
    r = m_guide->vectorPath().setRunner(arg);
//    if (r.isNull()) {
//    } else {
//        qreal l = r.moveBy(arg - r.position());
//        emit moved(l);
//    }
    notifyChanges();
}

void GrooveRunner::setData(QVariant arg)
{
    if (r.data() == arg)
        return;
    r.data() = arg;
    emit dataChanged();
}

QPointF GrooveRunner::smoothedPoint(qreal radius) const
{
    return r.getSmoothedPoint(radius);
}

QVariantMap GrooveRunner::getWirePoint(qreal radius, qreal offset) const
{
    qreal left = qMin(r.lengthBehind(), 0.01);
    qreal right = qMin(r.lengthAhead(), 0.01);
    VectorPath::Runner rl(r);
    qreal k = 1;
    if (left > 0) {
        rl.moveBy(-left);
        k = -1;
    } else if (right > 0) {
        rl.moveBy(right);
    }
    QPointF p1 = r.getSmoothedPoint(radius);
    QPointF p2 = rl.getSmoothedPoint(radius);
    QVector2D l(p2 - r.getSmoothedPoint(radius));
    l.normalize();
    l *= k;

    QVector2D norm(-l.y(), l.x());
    QVariantMap ret;
    ret["source"] = r.point();
    ret["point"] = p1 + (norm * offset).toPointF();
    ret["angle"] = qAtan2(l.y(), l.x()) * 180 / M_PI;
    return ret;


}

QVariant GrooveRunner::prevData() const
{
    return r.prevData();
}

QVariant GrooveRunner::nextData() const
{
    return r.nextData();
}

void GrooveRunner::moveBy(qreal length)
{
    if (length == 0)
        return;
    emit moved(r.moveBy(length));
    emit positionChanged();
    emit pointChanged();
    notifyChanges();
}

void GrooveRunner::refresh()
{
    r = m_guide->vectorPath().setRunner(r.position());

    //    if (r.position() > m_groove->length()) {
    //        setPosition(m_groove->length());
    //    }
    notifyChanges();
}

void GrooveRunner::notifyChanges()
{
    static bool atNode;
    static bool atStart;
    static bool atEnd;
    static bool atFirstEdge;
    static bool atLastEdge;
    static qreal pos;
    static QVariant *dataPtr;
    if (atNode != r.atNode()) {
        atNode = r.atNode();
        emit atNodeChanged();
    }
    if (atStart != r.atStart()) {
        atStart = r.atStart();
        emit atStartChanged();
    }
    if (atEnd != r.atEnd()) {
        atEnd = r.atEnd();
        emit atEndChanged();
    }
    if (pos != r.position()) {
        pos = r.position();
        emit positionChanged();
        emit lengthAheadChanged();
        emit lengthBehindChanged();
        emit pointChanged();
    }
    if (dataPtr != r.dataPtr()) {
        dataPtr = r.dataPtr();
        emit dataChanged();
        emit angleChanged();
        emit normalChanged();
        if (atLastEdge == r.hasNext()) {
            atLastEdge = !r.hasNext();
            emit atLastEdgeChanged();
        }
        if (atFirstEdge == r.hasPrev()) {
            atFirstEdge = !r.hasPrev();
            emit atFirstEdgeChanged();
        }
    }

}



GuideSplitter::GuideSplitter()
    : m_left(nullptr),
      m_right(nullptr)
{

}

Groove *GuideSplitter::left() const
{
    return m_left;
}

Groove *GuideSplitter::right() const
{
    return m_right;
}

bool GuideSplitter::atLeftEnd() const
{
    return splitter.atLeftEnd();
}

bool GuideSplitter::atRightEnd() const
{
    return splitter.atLeftEnd();
}

qreal GuideSplitter::nearestPos(QPointF pos) const
{
    if (!m_left || !m_right)
        return 0;
    m_left->vectorPath().checkLength();
    m_right->vectorPath().checkLength();
    qreal lpos = m_left->nearestPos(pos);
    qreal rpos = m_right->nearestPos(pos);
    qreal dl = m_left->length() - lpos;
    qreal dr = rpos;
    if (dr <= 0)
        return -dl;
    if (dl <= 0)
        return dr;
//    qDebug() << "nearest left" << dl << "right" << dr;
    if (dl > dr)
        return dr;
    else
        return -dl;
}

qreal GuideSplitter::x() const
{
    return splitter.position().x();
}

qreal GuideSplitter::y() const
{
    return splitter.position().y();
}

QPointF GuideSplitter::position() const
{
    return splitter.position();
}

void GuideSplitter::setSplitter(Groove *other, qreal pos)
{
    if (m_left)
        m_left->deleteLater();
    if (m_right)
        m_right->deleteLater();
    m_left = new Groove(this);
    m_right = new Groove(this);
    splitter.splitPath(other->vectorPath(), pos, m_left->vectorPathPtr(), m_right->vectorPathPtr());
    QQmlEngine::setObjectOwnership(m_left, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_right, QQmlEngine::CppOwnership);
//    QQmlEngine::setObjectOwnership(m_left, QQmlEngine::JavaScriptOwnership);
//    QQmlEngine::setObjectOwnership(m_right, QQmlEngine::JavaScriptOwnership);
    emit leftChanged(m_left);
    emit rightChanged(m_right);
    emit positionChanged();
    emit xChanged();
    emit yChanged();
}

void GuideSplitter::shiftLeft(qreal dist)
{
    //    qreal oldLen = m_left->length();
    //    int oldSize = m_left->vectorPath().nodes.size();
    qreal rem = splitter.shiftLeft(dist);
    if (rem == dist) {
        qDebug() << "No move left";
        return;
    }
    if (rem > 0) {
        qDebug() << "Cannot shift left anymore" << rem;
        if (splitter.atLeftEnd())
            emit atLeftEnd();
    }
    emit m_left->polygonChanged();
    emit m_right->polygonChanged();
    emit m_left->lengthChanged();
    emit m_right->lengthChanged();
    emit positionChanged();
    emit xChanged();
    emit yChanged();
}

void GuideSplitter::shiftRight(qreal dist)
{
    qreal rem = splitter.shiftRight(dist);
    if (rem == dist) {
        qDebug() << "No move right";
        return;
    }
    if (rem > 0) {
        qDebug() << "Cannot shift right anymore" << rem;
        if (splitter.atRightEnd())
            emit atRightEnd();
    }
    emit m_left->polygonChanged();
    emit m_right->polygonChanged();
    emit m_left->lengthChanged();
    emit m_right->lengthChanged();
    emit positionChanged();
    emit xChanged();
    emit yChanged();
}

void GuideSplitter::shift(qreal dist)
{
    if (dist < 0)
        shiftLeft(-dist);
    if (dist > 0)
        shiftRight(dist);
}

void GuideSplitter::setLeft(Groove *arg)
{
    if (m_left == arg)
        return;
    if (m_left)
        m_left->deleteLater();
    m_left = arg;
    splitter.setLeft(m_left->vectorPathPtr());
    emit leftChanged(arg);
    emit positionChanged();
    emit xChanged();
    emit yChanged();

}

void GuideSplitter::setRight(Groove *arg)
{
    if (m_right == arg)
        return;
    if (m_right)
        m_right->deleteLater();
    m_right = arg;
    splitter.setRight(m_right->vectorPathPtr());
    emit rightChanged(arg);
    emit positionChanged();
    emit xChanged();
    emit yChanged();

}


