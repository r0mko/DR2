#include "guideview.h"

#include <QPainter>
#include <guidepathdebug.h>
#include <guidesmoother.h>

GuideView::GuideView(QGraphicsItem *parent) :
    GraphicsObject(parent)
{

}


Segment GuideView::segment(int id) const
{
    return m_segments.value(id);

}

int GuideView::count() const
{
    return m_segments.size();
}

void GuideView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    qreal s = painter->worldTransform().m11();
    QPen posPen = m_posPen;
    QPen negPen = m_negPen;
    // 1 / (viewportScale > 4 ? viewportScale : 2 * Math.sqrt(viewportScale))
    if (s < 4.0) {
        qreal w = 0.5 * qSqrt(s);
        posPen.setWidthF(qMax(m_posPen.width() * w, 1.0));
        negPen.setWidthF(qMax(m_negPen.width() * w, 1.0));
    }

    for (Segment seg : m_segments) {
        painter->strokePath(seg.d->posResult.toPath(), posPen);
        if (m_doubleWire)
            painter->strokePath(seg.d->negResult.toPath(), negPen);
        //        painter->strokePath(seg.d->result.toPath(), seg.d->pen);
    }

    if (!m_debugNodes)
        return;
    for (Segment s : m_segments) {
        GuideDebug::paintNodes(s.d->posResult, painter, posPen);
        if (m_doubleWire)
            GuideDebug::paintNodes(s.d->negResult, painter, negPen);

    }

    m_shapeReady = false;
}

QVariantList GuideView::segments() const
{
    QVariantList lst;
    for (auto it = m_segments.cbegin(); it != m_segments.cend(); ++it) {
        lst.append(*it);
    }
    return lst;

    //        return m_segments;
}

bool GuideView::hasSegment(uint uid) const
{
    return m_segments.contains(uid);
}

Segment GuideView::addSegment(uint uid, Guide path, qreal offset)
{
    Segment seg;
    seg.setSource(path);
    seg.setView(this);
    seg.d->uid = uid;
    seg.setOffset(offset);
    m_segments.insert(uid, seg);
    emit countChanged();
    emit segmentsChanged();
    updateGeometry();
    //    qDebug() << "Added path" << uid << " length" << path.length() << "count" << m_segments.size() << "bounding rect" << bndRect;
    update();
    return seg;
}

qreal GuideView::radius() const
{
    return m_radius;
}

void GuideView::setRadius(qreal radius)
{
    if (m_radius == radius)
        return;

    m_radius = radius;
    emit radiusChanged(radius);
    updateGeometry();
}

QRectF GuideView::boundingRect() const
{
    return bndRect;
}

QPainterPath GuideView::shape() const
{
    return outline;
}

qreal GuideView::scalingFactor() const
{
    return m_scale;
}

void GuideView::setScalingFactor(qreal factor)
{
    if (m_scale == factor)
        return;

    m_scale = factor;
    emit offsetScaleChanged(factor);
    updateGeometry();
}

bool GuideView::isShapeReady() const
{
    return m_shapeReady;
}

GPen GuideView::negPen() const
{
    return m_negPen;
}

GPen GuideView::posPen() const
{
    return m_posPen;
}

qreal GuideView::spacing() const
{
    return m_spacing;
}

qreal GuideView::precision() const
{
    return m_precision;
}

bool GuideView::doubleWire() const
{
    return m_doubleWire;
}

void GuideView::removeSegment(uint id)
{
    if (!m_segments.contains(id))
        return;
    Segment s = m_segments.take(id);
    s.clearLinks();
    clearLinks(id);
    //    for (Segment seg : m_segments) {
    //        if (seg.d->pre)

    //    }
    emit segmentsChanged();
    updateGeometry();
}

void GuideView::addLink(uint from, uint to, int type)
{
    qDebug() << "Add internal link from" << from << "to" << to << (GuideNode::NodeType)type;

    if (!m_segments.contains(from)) {
        qWarning() << __func__ << "from" << from << "not found in" << objectName();
        return;
    }
    if (!m_segments.contains(to)) {
        qWarning() << __func__ << "to" << to << "not found in" << objectName();
        return;
    }
    Segment sFrom = m_segments.value(from);
    Segment sTo = m_segments.value(to);

    createLink(sFrom, sTo, (GuideNode::NodeType)type);
}

void GuideView::createLink(Segment from, Segment to, GuideNode::NodeType type)
{
    if (!from.d->nextSegment) {
        from.d->nextSegment = to.d.data();
        from.d->nextLinkType = type;
    } else {
        qDebug() << "Segment" << from.uid() << "already has next segment" << from.d->nextSegment->uid;
    }
    if (!to.d->prevSegment) {
        to.d->prevSegment = from.d.data();
        to.d->prevLinkType = type;
    } else {
        qDebug() << "Segment" << to.uid() << "already has prev segment" << to.d->prevSegment->uid;
    }
        qDebug() << __func__ << "creating link from" << from.uid() << "to" << to.uid() << "type" << type;
}

void GuideView::addOutBoundary(uint localSegmentUid, GuideView *nextSection, uint remoteSegmentUid)
{
    //    qDebug() << "Add external link from" << localSegmentUid << "to remote" << nextSection->objectName().append(":%1").arg(remoteSegmentUid);
    if (!m_segments.contains(localSegmentUid)) {
        qWarning() << __func__ << objectName() << "outgoing segment uid" << localSegmentUid << "to" << nextSection->objectName() << "seg" << remoteSegmentUid << "not found!";
        return;
    }
    if (!nextSection->hasSegment(remoteSegmentUid)) {
        qWarning() << __func__ << objectName() << "receiving remote segment uid" << remoteSegmentUid << "from local" << localSegmentUid << "not found in" << nextSection->objectName();
        return;
    }
    Segment prevSeg = m_segments.value(localSegmentUid);
    Segment nextSeg = nextSection->segment(remoteSegmentUid);
    createLink(prevSeg, nextSeg, GuideNode::InterlinkType);
}

void GuideView::addInBoundary(uint localSegmentUid, GuideView *prevSection, uint remoteSegmentUid)
{
    if (!m_segments.contains(localSegmentUid)) {
        qWarning() << __func__ << objectName() << "receiving segment uid" << localSegmentUid << "from" << prevSection->objectName() << "seg" << remoteSegmentUid << "not found!";
        return;
    }
    if (!prevSection->hasSegment(remoteSegmentUid)) {
        qWarning() << __func__ << objectName() << "outgoing remote segment uid" << remoteSegmentUid << "to local" << localSegmentUid << "not found in" << prevSection->objectName();
        return;
    }
    //    qDebug() << "Add external link from remote" << prevSection->objectName().append(":%1").arg(remoteSegmentUid) << "to" << localSegmentUid;
    Segment prevSeg = prevSection->segment(remoteSegmentUid);
    Segment nextSeg = m_segments.value(localSegmentUid);
    createLink(prevSeg, nextSeg, GuideNode::InterlinkType);
}

void GuideView::setDebugMode(bool debug)
{
    m_debugNodes = debug;
    update();
}

void GuideView::clearLinks(uint id)
{
    for (Segment seg : m_segments) {
        if (seg.d->nextSegment && seg.d->nextSegment->uid == id) {
            seg.d->nextSegment = nullptr;
            seg.d->nextLinkType = GuideNode::InvalidNode;
            qDebug() << "Removed reference 'next' to" << id << "from" << seg.uid() << m_segments.value(seg.uid()).d->nextSegment;
        }
        if (seg.d->prevSegment && seg.d->prevSegment->uid == id) {
            seg.d->prevSegment = nullptr;
            seg.d->prevLinkType = GuideNode::InvalidNode;
            qDebug() << "Removed reference 'prev' to" << id << "from" << seg.uid()<< m_segments.value(seg.uid()).d->prevSegment;
        }
    }
}

void GuideView::setNegPen(GPen negPen)
{
    if (m_negPen == negPen)
        return;

    m_negPen = negPen;
    emit negPenChanged(negPen);
}

void GuideView::setPosPen(GPen posPen)
{
    //    qDebug() << "new pospen" << posPen.operator QPen().color().name();
    if (m_posPen == posPen)
        return;
    //    qDebug() << "change pospen" << m_posPen.operator QPen().color().name();

    m_posPen = posPen;
    emit posPenChanged(posPen);
}

void GuideView::setSpacing(qreal spacing)
{
    if (m_spacing == spacing)
        return;

    m_spacing = spacing;
    emit spacingChanged(spacing);
}

void GuideView::setPrecision(qreal precision)
{
    if (m_precision == precision)
        return;

    m_precision = precision;
    emit precisionChanged(precision);
    updateGeometry();
}

void GuideView::setDoubleWire(bool doubleWire)
{
    if (m_doubleWire == doubleWire)
        return;

    m_doubleWire = doubleWire;
    emit doubleWireChanged(doubleWire);
}

Runner GuideView::findNearestPoint(QPointF pos) const
{
    qreal minDist = 1E38;
    Runner ret;
    Segment s;
    for (Segment seg : m_segments) {
        qreal dist;
        Runner r = seg.d->offsetted.findNearestPos(pos, &dist);
        if (minDist > dist) {
            minDist = dist;
            ret = r;
            s = seg;
        }
    }
    return ret;
}

FindPointReply GuideView::  findAtPoint(QPointF pos) const
{
    qreal minDist = 1E38;
    FindPointReply reply;
    Runner runner;
    Segment s;
    for (Segment seg : m_segments) {
        qreal dist;
        Runner r = seg.d->source.findNearestPos(pos, &dist);
        if (minDist > dist) {
            minDist = dist;
            runner = r;
            s = seg;
        }
    }
    reply.m_sourceRunner = runner;
    reply.m_segment = s;
    reply.m_posResultRunner= s.d->posResult.findNearestPos(pos);
    if (!s.d->negResult.isEmpty())
        reply.m_negResultRunner= s.d->negResult.findNearestPos(pos);
    reply.m_distance = minDist;
    return reply;

    //        return runner;
}

void GuideView::updateGeometry()
{
    QRectF rect;
    outline = QPainterPath();
    for (auto it = m_segments.begin(); it != m_segments.end(); ++it) {
        it->updateOffset(m_scale);
        rect = rect.united(it->d->offsetted.boundingRect());
    }
    bndRect = rect;
    for (auto it = m_segments.begin(); it != m_segments.end(); ++it) {
        it->updateOutput(m_spacing, m_scale, m_precision, m_radius);
        outline.addPath(it->d->posResult.outline(8 / scene()->viewportScale(), Qt::SquareCap));
        if (m_doubleWire)
            outline.addPath(it->d->negResult.outline(8 / scene()->viewportScale(), Qt::SquareCap));
    }
    prepareGeometryChange();
    emit shapeReady();
    m_shapeReady = true;
    for (const std::pair<uint, BoundItem> &kv : m_boundItems) {
        const BoundItem &i = kv.second;
        Guide g;
        switch (i.guideType) {
        case Segment::SourceGuide:
            g = i.segment.source();
            break;
        case Segment::OffsettedGuide:
            g = i.segment.offsetted();
            break;
        case Segment::PosResultGuide:
            g = i.segment.posResult();
            break;
        case Segment::NegResultGuide:
            g = i.segment.negResult();
            break;
        default:
            break;
        }
        Runner r(g, i.pos);
        r.toNearestVector();

        QTransform tr;

//        tr.translate(r.point().x(), r.point().y());
        if (i.followAngle)
            tr.rotate(r.angle());
        if (i.fixedScale) {
            tr.translate(i.xOffset * m_scale, i.yOffset * m_scale);
            tr.scale(m_scale, m_scale);
        } else
            tr.translate(i.xOffset, i.yOffset);
//            i.item->setRotation(r.angle());
        i.item->setTransform(i.baseTransform * tr);
        i.item->setPos(r.point());
        emit boundItemChanged(kv.first, r.point(), r.angle(), m_scale);
//        i.item->settr
        i.item->update();
    }

    update();
}

int GuideView::bindItem(VisualInstanceHandle item, Segment segment, int type, qreal position, qreal xOffset, qreal yOffset, bool fixedScale, bool followAngle)
{
    BoundItem i;
    i.item = item.item();
    i.baseTransform = item.item()->transform();
    i.baseTransform.scale(item.scale(), item.scale());
    i.segment = segment;
    i.guideType = type;
    i.pos = position;
    i.followAngle = followAngle;
    i.xOffset = xOffset;
    i.yOffset = yOffset;
    i.fixedScale = fixedScale;
    m_boundItems.insert(std::pair<uint, BoundItem>(m_boundItems.size(), i));
    updateGeometry();
    return m_boundItems.size() - 1;

}

void GuideView::unbindItem(int bindingId)
{
//    qDebug() << "Removing binding" << bindingId;
    m_boundItems.erase(bindingId);
    updateGeometry();
}

Segment::Segment() : d(new SegmentData) {}

qreal Segment::offset() const
{
    return d->offset;
}

Guide Segment::posResult() const
{
    return d->posResult;
}

Guide Segment::negResult() const
{
    return d->negResult;
}


Guide Segment::source() const
{
    return d->source;
}

GuideView *Segment::view() const
{
    return d->view;
}

void Segment::updateOffset(qreal scale)
{
    if (d->offset > 0)
        d->offsetted = d->source.offseted(d->offset * scale);
    else
        d->offsetted = d->source.clone();
}

void Segment::updateOutput(qreal spacing, qreal scale, qreal precision, qreal radius)
{
    if (radius == 0.0) {
        d->posResult = d->offsetted.clone();
    } else {
        GuideSmoother sm;
        sm.setPrecision(precision);
        sm.setRadius(radius);
        Runner start;

        if (d->prevSegment) {
            Segment prev(d->prevSegment);
            QPointF pt = prev.d->offsetted.tailPoint();
            start = sm.addCoupler(d->offsetted.linkPrev(prev.d->offsetted, d->prevLinkType), pt);
            //        qDebug() << d->uid << "has prev, link" << d->prevLinkType << "start point" << pt;
        } else {
            start = d->offsetted.headRunner();
            //        qDebug() << d->uid << "has no prev, start point" << start.point();
            //        qDebug() << "No prev: start point" << start.point() << "tail point" << d->offsetted.tailPoint();
        }

        if (d->nextSegment) {
            Segment next(d->nextSegment);
            sm.addCoupler(d->offsetted.linkNext(next.d->offsetted, d->nextLinkType));
            //        qDebug() << d->uid << "has next, link" << d->nextLinkType;
        }

        d->posResult = sm.smoothed(&start, 1 / scale, 1 / precision);
    }
    if (spacing > 0) {
        d->negResult = d->posResult.offseted(spacing * scale);
    }
}

void Segment::setView(GuideView *view)
{
    //    if (view) {
    //        if (d->view)
    //            qDebug() << "Setting view" << view->objectName() << "current" << d->view->objectName()<< d->view << d->uid;
    //        else
    //            qDebug() << "Setting view" << view->objectName() << d->view << d->uid;
    //    }
    d->view = view;
}

void Segment::clearLinks()
{
    if (d->prevSegment) {
        if (d->prevSegment->nextSegment == d.data()) {
            qDebug() << "Removed reference to" << d->uid << "from" << d->prevSegment->uid;
            d->prevSegment->nextLinkType = GuideNode::InvalidNode;
            d->prevSegment->nextSegment = nullptr;
        }
        d->prevSegment = nullptr;
        d->prevLinkType = GuideNode::InvalidNode;
    }
    if (d->nextSegment) {
        if (d->nextSegment->prevSegment == d.data()) {
            qDebug() << "Removed reference to" << d->uid << "from" << d->nextSegment->uid;
            d->nextSegment->prevLinkType = GuideNode::InvalidNode;
            d->nextSegment->prevSegment = nullptr;
        }
        d->nextSegment = nullptr;
        d->nextLinkType = GuideNode::InvalidNode;
    }
}

uint Segment::uid() const
{
    return d->uid;
}


void Segment::setOffset(qreal offset)
{
    d->offset = offset;
    //    if (d->view)
    //        d->view->updateStroke();
}


void Segment::setSource(Guide guide)
{
    d->source = guide;
}

Guide Segment::offsetted() const
{
    return d->offsetted;
}
