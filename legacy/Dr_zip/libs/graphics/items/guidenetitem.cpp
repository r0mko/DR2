#include "guidenetitem.h"
#include "guidesmoother.h"
#include <guidepathdebug.h>
#include <QPainter>

QVariantList GuideNet::findVertices(NetVertex::ElementType type) const
{
    QVariantList ret;
    for (QHash<uint, Edge>::const_iterator ii = links.begin(); ii != links.end(); ++ii) {
        if (ii->vertex.type() == type)
            ret.append(ii->vertex);
    }
    return ret;
}

GuideNet::GuideNet(QGraphicsItem *parent) : GraphicsObject(parent) {}

void GuideNet::componentComplete()
{
    GraphicsObject::componentComplete();
}

void GuideNet::classBegin() {}

QVariantList GuideNet::endVertices() const
{
    QVariantList ret;
    for (auto j : findVertices(NetVertex::EndType)) {
        ret.append(j);
    }
    return ret;
}

QRectF GuideNet::boundingRect() const
{
    return m_boundingRect;
}

void GuideNet::buildPaths(qreal scale)
{


    //        prepareGeometryChange();
}

Guide GuideNet::buildPath(Runner runner, qreal scale)
{
    GuideSmoother sm;
    sm.setPrecision(m_precision);
    sm.setRadius(m_smooth);
    if (runner.point() == QPointF())
        qWarning() << "Runner null point" << runner.position() << runner.currentNodeType();
    Guide p = sm.smoothed(&runner, scale, 0.1 / m_precision);
    outline.addPath(p.outline(10 / scale, Qt::FlatCap));
    return p;
}

//void GuideNet::buildBatches()
//{
//    QHash<uint, QSet<uint>> matrix;
//    QHash<uint, int> hits;
//    for (QHash<uint, Edge>::iterator ii = links.begin(); ii != links.end(); ++ii) {
//        matrix[ii->sourceUid].insert(ii->destUid);
//        hits[ii->sourceUid]++;
//        hits[ii->destUid]++;
//    }
//    for (auto it = matrix.begin(); it != matrix.end(); ++it) {
//        if (hits.value(it.key()) == 1) {
//            Batch b;

//            b.couplers.append(links.value(it.key()).vertex);
//        }

//    }


//}

GPen GuideNet::pen() const
{
    return m_pen;
}

qreal GuideNet::precision() const
{
    return m_precision;
}

qreal GuideNet::smooth() const
{
    return m_smooth;
}

void GuideNet::setPen(GPen pen)
{
    m_pen = pen;
    update();
}

void GuideNet::setPrecision(qreal precision)
{
    m_precision = precision;
    update();

}

void GuideNet::setSmooth(qreal smooth)
{
    m_smooth = smooth;
    update();

}

void GuideNet::setupScene(Scene *scene)
{
    connect(scene, &Scene::viewportScaleChanged, this, &GuideNet::build);
}

QPainterPath GuideNet::shape() const
{
    return outline;
}

Runner GuideNet::findNearestPoint(QPointF pos) const
{
    qreal minDist = 1E38;
    Runner ret;
    for (Guide s : paths) {
        qreal dist;
        Runner r = s.findNearestPos(pos, &dist);
        if (minDist > dist) {
            minDist = dist;
            ret = r;
        }
    }
    return ret;
}

NetSegment GuideNet::segmentAt(QPointF pos) const
{
    return findSegment(pos);
}

void GuideNet::splitAtPoint(QPointF pos)
{
    qreal minDist = 1E38;
    Runner runner;
    //    NetSegment seg;
    uint uid;
    for (auto ii = m_segments.begin(); ii != m_segments.end(); ++ii ) {
        qreal dist;
        Runner r = ii->path().findNearestPos(pos, &dist);
        if (minDist > dist) {
            minDist = dist;
            runner = r;
            uid = ii.key();
        }
    }
    //    if ()
    NetSegment s = m_segments[uid];
    //    qDebug() << "split at segment" << uid << "at position" << runner.position() << "entry pos" << s.entryPos() << "exit pos" << s.exitPos();
    NetSegment detached;
    detached.setEntryPos(runner.position());
    detached.setExitPos(s.exitPos());
    detached.d->uid = s.uid();
    detached.setName(s.name());
    detached.setPath(s.path());
    detached.setOffset(s.offset());
    s.setExitPos( runner.position() );
    emit segmentChanged(s.name(), s.entryPos(), s.exitPos(), s.offset());
    detached.updatePart();
    emit segmentDetached(detached);
    updateParts();


    QList<Edge> detachedLinks = detachBranch(uid, false);
    for (Edge e : detachedLinks) {
        QString type(QMetaEnum::fromType<NetVertex::ElementType>().valueToKey(e.vertex.type()));
        type.replace("Type", "Vertex");
        emit linkRemoved(e.sourceUid, e.destUid, type);
    }
    emit splitComplete(runner, s, detached);
    build(scene()->viewportScale());
}

QString GuideNet::name() const
{
    return m_name;
}

QVariantList GuideNet::startVertices() const
{
    QVariantList ret;
    for (auto j : findVertices(NetVertex::StartType)) {
        ret.append(j);
    }
    return ret;
}

QVariantList GuideNet::joints() const
{
    QVariantList ret;
    for (auto j : findVertices(NetVertex::JointType)) {
        ret.append(j);
    }
    return ret;
}

QVariantList GuideNet::segments() const
{
    QVariantList ret;
    for (NetSegment e : m_segments) {
        ret.append(e);
    }
    return ret;
}

NetSegment GuideNet::segment(uint uid) const
{
    return m_segments.value(uid, NetSegment());
}

bool GuideNet::hasSegment(uint uid) const
{
    return m_segments.contains(uid);
}


void GuideNet::updateSegment(uint uid, qreal entryPos, qreal exitPos, qreal offset)
{
    if (!m_segments.contains(uid)) {
        qDebug() << __func__ << "segment" << uid << "not found in net " << m_name;
        return;
    }
    NetSegment seg = m_segments[uid];

    seg.setExitPos(exitPos);
    seg.setEntryPos(entryPos);
    seg.setOffset(offset);
    seg.updateOffset(scene()->viewportScale());
    //    emit segmentChanged(seg.name(), entryPos, exitPos, offset);
}


void GuideNet::appendSegment(uint uid, QString name, Guide path, qreal entry, qreal exit, qreal offset)
{
    if (m_segments.contains(uid)) {
        qDebug() << __func__ << "segment" << name << "already added";
        return;
    }
    NetSegment seg;
    seg.setPath(path);
    seg.setEntryPos(entry);
    seg.setExitPos(exit);
    seg.setOffset(offset);
    seg.d->uid = uid;
    seg.setName(name);
    pushSegment(seg);
    emit segmentsChanged();
    emit netChanged();
//    qDebug() << "Added segment" << seg.name() << "uid" << seg.uid() << "entry" << seg.entryPos() <<"exit" << seg.exitPos() << "offset" << seg.offset() << "length" << seg.length();

    //    build(scene()->viewportScale());
    //    prepareGeometryChange();
}

void GuideNet::removeSegment(uint uid)
{
    const NetSegment re = m_segments.value(uid);
    qDebug() << "Removing segment" << re.name() << "uid" << re.uid() << "entry" << re.entryPos() <<"exit" << re.exitPos() << "offset" << re.offset() << "length" << re.length();
    m_segments.remove(uid);
    emit netChanged();
    build(scene()->viewportScale());
//    prepareGeometryChange();
}

void GuideNet::addBoundary(NetSegment from, NetSegment to, bool isStart)
{
    uint st = isStart ? 0 : from.uid();
    for (Edge &e : links.values(st)) {
        if (e.destUid == to.uid() && e.vertex.type() == NetVertex::BoundaryType) {
            qWarning() << qPrintable(QString("Boundary %1->%2 at edge #%3 already exists").arg(from.net()->name()).arg(to.net()->name()).arg(to.uid()));
            return;
        }
    }
//    if (links.contains(from.))
    BoundaryVertex b;
    b.d->net = this;
    b.setFrom(from);
    b.setTo(to);
    b.setIsStart(isStart);
    b.setGap(10);
    if (isStart) // foreign case
        links.insert(0, Edge(0, to.uid(), b));
    else
        links.insert(from.uid(), Edge(from.uid(), to.uid(), b));
    emit boundsChanged();
}

void GuideNet::clearLinks()
{
    links.clear();
}



void GuideNet::removeLink(uint from, uint to)
{
    QHash<uint, Edge>::iterator it = links.find(from);
    while (it.key() == from) {
        if (it->destUid == to) {
            qDebug() << "removing link" << it->vertex.type() << "from" << it->sourceUid << "to" << it->destUid;
            it = links.erase(it);
            emit netChanged();
        } else
            ++it;
    }
    build(scene()->viewportScale());
}

void GuideNet::addJoint(uint uidFrom, uint uidTo)
{
    if (!m_segments.contains(uidFrom)) {
        qWarning() << __func__ << this << "source segment not found: uid =" << uidFrom;
        return;
    }
    if (!m_segments.contains(uidTo)) {
        qWarning() << __func__ << this << "destination segment not found: uid =" << uidTo;
        return;
    }
    NetSegment from = m_segments[uidFrom];
    NetSegment to = m_segments[uidTo];

    JointVertex j;
    j.setFrom(from);
    j.setTo(to);

    links.insert(uidFrom, Edge(uidFrom, uidTo, j));
    emit jointsChanged();
    emit netChanged();
    //    build(scene()->viewportScale());
}

void GuideNet::addStart(uint uidTo)
{
    if (!m_segments.contains(uidTo)) {
        qWarning() << __func__ << "unknown segment" << uidTo << m_segments.keys();

        return;
    }
    StartVertex v;
    v.setNextSegment(m_segments[uidTo]);
    links.insert(0, { 0, uidTo, v });
    //    m_entryPoints.append(v);
    //    outs.insert(0, uidTo), v);

    qDebug() << "Added StartVertex on edge" << uidTo;
    emit netChanged();
    build(scene()->viewportScale());
}

void GuideNet::addEnd(uint uidFrom)
{
    if (!m_segments.contains(uidFrom)) {
        qWarning() << __func__ << "unknown segment" << uidFrom;
        return;
    }
    EndVertex v;
    v.setPrevSegment(m_segments[uidFrom]);

    qDebug() << "Added EndVertex on edge" << uidFrom;
    links.insert(uidFrom, Edge(uidFrom, 0, v));
    emit netChanged();
    //    build(scene()->viewportScale());
}

void GuideNet::updateParts()
{
    for (auto it = m_segments.begin(); it != m_segments.end(); ++it) {
        //        qDebug() << "Fix segment" << it.key() << it->name() << it->net()->name();
        it->updatePart();
        emit segmentChanged(it->name(), it->entryPos(), it->exitPos(), it->offset());
    }
}

void GuideNet::clearSegments()
{
    m_segments.clear();
    emit netChanged();
    prepareGeometryChange();
}

void GuideNet::build(qreal scale)
{
    QVector<Runner> entryRunners;
    m_boundingRect = QRectF();
    paths.clear();
    outline = QPainterPath();
    for (auto it = m_segments.begin(); it != m_segments.end(); ++it) {
        it->updatePart();
        it->updateOffset(1 / scale);
        m_boundingRect = m_boundingRect.united(it->result().boundingRect());
        emit jointsChanged();
    }

    GuideSmoother sm;
    sm.setPrecision(m_precision);
    sm.setRadius(m_smooth);
    for (QHash<uint, Edge>::const_iterator ii = links.begin(); ii != links.end(); ++ii) {
        switch (ii->vertex.type()) {
        case NetVertex::JointType: {
            JointVertex v = JointVertex::fromVertex(ii->vertex);
            sm.addCoupler(v.link());
            break;
        }
        case NetVertex::JunctionType: {
            JunctionVertex v = JunctionVertex::fromVertex(ii->vertex);
            for (NetSegment s : v.to()) {
                entryRunners.append(Runner(s.result()));
            }
            break;
        }
        case NetVertex::ConvergenceType: {
            ConvergenceVertex v = ConvergenceVertex::fromVertex(ii->vertex);
            for (NetSegment s : v.from()) {
                entryRunners.append(Runner(s.result()));
            }
            break;
        }
        case NetVertex::StartType: {
            StartVertex v = StartVertex::fromVertex(ii->vertex);
            entryRunners.append(Runner(v.nextSegment().result(), 0));
            break;
        }
        case NetVertex::BoundaryType: {
            BoundaryVertex v = BoundaryVertex::fromVertex(ii->vertex);
            v.update();
            emit boundsChanged();
            sm.addCoupler(v.link());
            if (v.isStart()) {
                entryRunners.append(v.runner(sm.couplers.last()));
            }


            //            //            entryRunners.append(PathRunner(v.to().result(), 0));
            //            //            entryRunners.append());
            //            //            l.d->

            //            //            entryRunners.append(PathRunner(couplers.last().d->to->t, 0));
            //            //            entryRunners.append(PathRunner(v.segment().result()));
            //            break;
        }

        default:
            break;
        }
    }
//    qDebug() << "Creating" << m_name << endl
//             << paths.size() << "paths" << endl
//             << entryRunners.size() << "runners";
    for (Runner runner : entryRunners) {
        Guide p = sm.smoothed(&runner, scale, 0.1 / m_precision);
        outline.addPath(p.outline(10 / scale, Qt::FlatCap));
        paths.append(p);
    }
    prepareGeometryChange();
}

void GuideNet::update()
{
    if (!scene())
        return;
    build(scene()->viewportScale());
    GraphicsObject::update();

}

void GuideNet::setDebugNodes(bool enable)
{
    debugNodes = enable;
    update();
}

void GuideNet::pushSegment(NetSegment segment)
{
    if (m_segments.contains(segment.uid())) {
        NetSegment s = m_segments.value(segment.uid());
        qWarning() << Q_FUNC_INFO << "segment" << segment.name() << "already exists in way" << name()
                   << "\n existing entry:" << s.entryPos() << "exit:" << s.exitPos()
                   << "\n new entry:" << segment.entryPos() << "exit:" << segment.exitPos();
        s.merge(segment);
    } else {
        segment.d->net = this;
        m_segments.insert(segment.uid(), segment);
    }
}

void GuideNet::merge(GuideNet *other)
{
    for (NetSegment s : other->m_segments) {
        NetSegment seg = m_segments[s.uid()];
        seg.merge(s);
        emit segmentChanged(s.name(), s.entryPos(), s.exitPos(), s.offset());
    }
    links.unite(other->links);
    build(scene()->viewportScale());

}

void GuideNet::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}

QVariantList GuideNet::bounds() const
{
    QVariantList ret;
    for (BoundaryVertex v : findVertices<BoundaryVertex>(NetVertex::BoundaryType)) {
        ret.append(v);
//        qDebug() << "Bound" << v.from().name() << "to" << v.to().name() << v.point();
    }
    return ret;
}

QList<GuideNet::Edge> GuideNet::detachBranch(uint edgeUid, bool erase)
{
    QList<GuideNet::Edge> ret;
    if (edgeUid == 0)
        return ret;
    QHash<uint, Edge>::iterator ii;
    if (!m_segments.contains(edgeUid)) {
        qWarning() << Q_FUNC_INFO << "segment" << edgeUid << "not found" << "Net" << name() ;
        //        return ret;
    } else if(erase) {
        NetSegment s = m_segments.take(edgeUid);
        qDebug() << __func__ << "Removing segment" << s.name() << "entry"<<  s.entryPos() << "exit" << s.exitPos();
        emit segmentRemoved(s);
    }

    do {
        ii = links.find(edgeUid);
        if (ii == links.end())
            break;
        if (ii->vertex.type() != NetVertex::BoundaryType) {
            uint next = ii->destUid;
            ret.append(*ii);
            ii = links.erase(ii);
            ret.append(detachBranch(next));
        } else {
            qDebug() << Q_FUNC_INFO << "implement boundary detach";
        }
    } while (ii != links.end());
    return ret;
}

QHash<uint, GuideNet::Edge>::iterator GuideNet::findInEdge(uint uid)
{
    for (QHash<uint, Edge>::iterator it = links.begin(); it != links.end(); ++it) {
        if (it->destUid == uid)
            return it;
    }
}

QPair<QVariantList, QVariantList> GuideNet::extractBranch(uint edgeUid)
{
    QVariantList nodes;
    QVariantList edges;
    if (edgeUid == 0)
        return qMakePair(nodes, edges);
    QList<Edge> edgeList = links.values(edgeUid);
    for (const Edge& edge : edgeList) {
        QVariantMap seg;
        NetSegment s = m_segments.value(edgeUid);

        seg["uid"] = edgeUid;
        seg["startPos"] = s.entryPos();
        seg["endPos"] = s.exitPos();
        seg["offset"] = s.offset();
        seg["id"] = s.name();
        seg["guide"] = QVariant::fromValue(s.path());
        seg["deleted"] = true;
        nodes.append(seg);
        QVariantMap link;
        link["inUid"] = edgeUid;
        link["outUid"] = edge.destUid;
        QMetaEnum e = QMetaEnum::fromType<NetVertex::ElementType>();
        link["type"] = e.valueToKey(edge.vertex.type());
        link["link"] = edge.vertex;
        qDebug() << "adding recursive segments" << edge.destUid << edgeUid;
        edges.append(link);
        auto deep = extractBranch(edge.destUid);
        nodes.append(deep.first);
        edges.append(deep.second);
    }

    return qMakePair(nodes, edges);
}

void GuideNet::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(m_pen);
    for (Guide p : paths) {
        painter->drawPolyline(p.toPolygon());
    }
    if (!debugNodes)
        return;
    for (Guide p : paths) {
        GuideDebug::paintNodes(p, painter, m_pen);
    }
//    if (painter->worldTransform().m11() < 2)
//        return;
    QPen resultPen;
    resultPen.setColor("#ff0");
    resultPen.setWidth(0);
    resultPen.setStyle(Qt::DotLine);
    resultPen.setCosmetic(true);
    resultPen.setCapStyle(Qt::RoundCap);
    QPen partPen = resultPen;
    partPen.setColor("#0ff");
    for (NetSegment s : m_segments) {
        painter->strokePath(s.result().toPath(), resultPen);
        painter->strokePath(s.part().toPath(), partPen);
    }



}


Guide GuideNet::Batch::exec(qreal radius, qreal precision, qreal scale)
{
    GuideSmoother sm;
    sm.setPrecision(precision);
    sm.setRadius(radius);
}


NetSegment GuideNet::findSegment(const QPointF &point, Runner *runner) const
{
    qreal minDist = 1E38;
    Runner ret;
    NetSegment nearestSegment;
    for (NetSegment s : m_segments) {
        qreal dist;
        Runner r = s.path().findNearestPos(point, &dist);
        if (minDist > dist) {
            minDist = dist;
            nearestSegment = s;
            ret = r;
        }
    }
    if (runner) {
        *runner = ret;
    }
    return nearestSegment;
}
