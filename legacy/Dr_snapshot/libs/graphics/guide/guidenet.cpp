#include "guidenet.h"
#include "guidenetitem.h"
#include "guidesmoother.h"


#include <qpainter.h>

void NetSegment::updatePart()
{
    if (d->entryPos != d->exitPos && !d->path.isEmpty()) {
        d->part = d->path.part(d->entryPos, d->exitPos);
    } else {
        d->part = Guide();
    }
}

void NetSegment::updateOffset(qreal scale)
{
    if (d->part.isEmpty()) {
        d->result.clear();
        return;
    }
    d->result = d->part.offseted(scale * d->offset);
}

qreal NetSegment::entryPos() const
{
    return d->entryPos;
}

qreal NetSegment::exitPos() const
{
    return d->exitPos;
}

qreal NetSegment::offset() const
{
    return d->offset;
}

Guide NetSegment::path() const
{
    return d->path;
}

Guide NetSegment::part() const
{
    return d->part;
}

Guide NetSegment::result() const
{
    return d->result;
}

bool NetSegment::isNull() const
{
    return d->path.isEmpty();
}

qreal NetSegment::length() const
{
    return qAbs(d->exitPos - d->entryPos);
}

GuideNet *NetSegment::net() const
{
    return d->net;
}

uint NetSegment::uid() const
{
    return d->uid;
}

QString NetSegment::name() const
{
    return d->name;
}

void NetSegment::setEntryPos(qreal entryPos)
{
    d->entryPos = entryPos;
    updatePart();
}


void NetSegment::setExitPos(qreal exitPos)
{
    d->exitPos = exitPos;
    updatePart();
}

void NetSegment::setOffset(qreal offset)
{
    d->offset = offset;
}


void NetSegment::setPath(Guide path)
{
    d->path = path;
    updatePart();
}

void NetSegment::setName(QString name)
{
    d->name = name;
}

void NetSegment::merge(NetSegment other)
{
    if (d->uid != other.uid())
        return;
    if (d->entryPos > d->exitPos) {
        d->entryPos = qMax(other.entryPos(), other.exitPos());
        d->exitPos = qMin(other.entryPos(), other.exitPos());
    } else {
        d->exitPos = qMax(other.entryPos(), other.exitPos());
        d->entryPos = qMin(other.entryPos(), other.exitPos());
    }

}

JointVertex::operator QVariant() const { return QVariant::fromValue(*this); }

NetSegment JointVertex::to() const
{
    return dfunc()->to;
}


NetSegment JointVertex::from() const
{
    return dfunc()->from;
}

//VectorNode JointVertex::link() const
//{
////    return dfunc()->link.d->connector.t;
//}

JointVertex JointVertex::fromVertex(NetVertex vertex)
{
    if (vertex.type() == JointType)
        return JointVertex(static_cast<JointVertexData*>(vertex.d.data()));
    return JointVertex (nullptr);
}


void JointVertex::setTo(NetSegment to)
{
    dfunc()->to = to;
}


void JointVertex::setFrom(NetSegment from)
{
    dfunc()->from = from;
}

Guide::Link JointVertex::link()
{
    Guide pFrom = dfunc()->from.result();
    Guide pTo = dfunc()->to.result();
    GuideIterator from = pFrom.last();
    GuideIterator to = pTo.first();
    VectorNode v(pFrom.tailPoint(), pTo.headPoint());
    if (v.length() > .1) {
        //        qDebug() << Q_FUNC_INFO << "link length" << v.length()
        //                 << "\n src:" << dfunc()->from.name() << dfunc()->from.net()->name() << "exit:" << dfunc()->from.exitPos() << "L:" << dfunc()->from.result().length()
        //                 << "\n dst:" << dfunc()->to.name() << dfunc()->to.net()->name() << "entry:" << dfunc()->to.entryPos()<< "L:" << dfunc()->to.result().length()
        //                    ;
        return Guide::Link (from, to, v);
    } else {
        return Guide::Link (from, to);
    }
}

QPointF JointVertex::point() const
{
    if (dfunc()->to.result().isEmpty()) {
        dfunc()->to.updatePart();
        dfunc()->to.updateOffset(1);
    }
    if (dfunc()->from.result().isEmpty()) {
        dfunc()->from.updatePart();
        dfunc()->from.updateOffset(1);
    }
    return (dfunc()->to.result().headPoint() + dfunc()->from.result().tailPoint()) / 2;
}

//void JointVertex::setLink(VectorNode link)
//{

//}

//void JointVertex::plug()
//{

//}

//void JointVertex::unplug()
//{
//    link = Guide::Link();
//}








NetSegment BoundaryVertex::from() const
{
    return dfunc()->from;
}

NetSegment BoundaryVertex::to() const
{
    return dfunc()->to;
}

void BoundaryVertex::setFrom(NetSegment from)
{
    dfunc()->from = from;
}


void BoundaryVertex::setTo(NetSegment to)
{
    dfunc()->to = to;
}


bool BoundaryVertex::isStart() const
{
    return dfunc()->isStart;
}

void BoundaryVertex::setIsStart(bool isStart)
{
    dfunc()->isStart = isStart;
}

qreal BoundaryVertex::gap() const
{
    return dfunc()->gap;
}

void BoundaryVertex::setGap(qreal gap)
{
    dfunc()->gap = gap;
}

Guide::Link BoundaryVertex::link()
{
    GuideIterator from = dfunc()->from.result().last();
    GuideIterator to = dfunc()->to.result().first();
    //             << "to" << dfunc()->to.name() << dfunc()->to.net()->name();
    return Guide::Link (from, to, InterlinkNode (dfunc()->to.result().headPoint(), dfunc()->from.result().tailPoint()));
    //    qDebug() << "new to.prev" << to.prev()->type()
}

Runner BoundaryVertex::runner(const Guide::Link &linker) const
{
    return Runner(linker.connector(), dfunc()->point);
}

void BoundaryVertex::update()
{
    dfunc()->updatePoint();

}


BoundaryVertex BoundaryVertex::fromVertex(NetVertex vertex)
{
    if (vertex.type() == BoundaryType)
        return BoundaryVertex(static_cast<BoundaryVertexData*>(vertex.d.data()));
    return BoundaryVertex (nullptr);
}


NetSegment StartVertex::nextSegment() const
{
    return dfunc()->nextSegment;
}


NetSegment StartVertex::prevSegment() const
{
    return dfunc()->prevSegment;
}

Runner StartVertex::runner() const
{
    return Runner(dfunc()->nextSegment.result().head());
}


void StartVertex::setNextSegment(NetSegment nextSegment)
{
    dfunc()->nextSegment = nextSegment;
}


void StartVertex::setPrevSegment(NetSegment prevSegment)
{
    dfunc()->prevSegment = prevSegment;
}

StartVertex StartVertex::fromVertex(NetVertex vertex)
{
    if (vertex.type() == StartType)
        return StartVertex(static_cast<StartVertexData*>(vertex.d.data()));
    return StartVertex (nullptr);
}


NetSegment EndVertex::nextSegment() const
{
    return dfunc()->nextSegment;
}

NetSegment EndVertex::prevSegment() const
{
    return dfunc()->prevSegment;
}

void EndVertex::setNextSegment(NetSegment nextSegment)
{
    dfunc()->nextSegment = nextSegment;
}

void EndVertex::setPrevSegment(NetSegment prevSegment)
{
    dfunc()->prevSegment = prevSegment;
}

EndVertex EndVertex::fromVertex(NetVertex vertex)
{
    if (vertex.type() == EndType)
        return EndVertex(static_cast<EndVertexData*>(vertex.d.data()));
    return EndVertex (nullptr);
}

JunctionVertex::JunctionVertex() : NetVertex(new JunctionVertexData, JunctionType) {}

QList<NetSegment> JunctionVertex::to() const
{
    return dfunc()->to;
    //    QVariantList ret;
    //    for (auto i : dfunc()->to) {
    //        ret.append(i);
    //    }
    //    return ret;
}

NetSegment JunctionVertex::from() const
{
    return dfunc()->from;
}

void JunctionVertex::switchTo(int index)
{
}

void JunctionVertex::setFrom(NetSegment from)
{
    dfunc()->from = from;
}

void JunctionVertex::appendOutSegment(NetSegment seg)
{
    dfunc()->to.append(seg);
}

JunctionVertex JunctionVertex::fromVertex(NetVertex vertex)
{
    if (vertex.type() == JunctionType)
        return JunctionVertex(static_cast<JunctionVertexData*>(vertex.d.data()));
    return JunctionVertex (nullptr);
}


ConvergenceVertex ConvergenceVertex::fromVertex(NetVertex vertex)
{
    if (vertex.type() == ConvergenceType)
        return ConvergenceVertex(static_cast<ConvergenceVertexData*>(vertex.d.data()));
    return ConvergenceVertex (nullptr);
}






//QRectF GuideNet::GuideNetStroke::boundingRect() const
//{
//    return net->m_boundingRect;
//}

//QPainterPath GuideNet::GuideNetStroke::shape() const
//{
//    return net->outline;
//}

//void GuideNet::GuideNetStroke::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
//{
////    net->updateSegments(painter->worldTransform().m11());
////    net->buildPath();
//    painter->strokePath(net->path, net->m_pen);
//}

//void GuideNet::GuideNetStroke::updateGeometry()
//{
//    prepareGeometryChange();
//}


NetSegment ConvergenceVertex::to() const { return dfunc()->to; }

QList<NetSegment> ConvergenceVertex::from() const
{
    return dfunc()->from;
    //    QVariantList ret;
    //    for (auto i : dfunc()->from) {
    //        ret.append(i);
    //    }
    //    return ret;
}

void ConvergenceVertex::setTo(NetSegment to)
{
    dfunc()->to = to;
}

void ConvergenceVertex::appendInSegment(NetSegment seg)
{
    dfunc()->from.append(seg);
}


uint qHash(const NetVertex &key, uint seed)
{
    return qHash((quintptr)key.d.data(), seed);
}


QPointF BoundaryVertex::point() const
{
    return dfunc()->point;
}

uint qHash(const BoundaryVertex &key, uint seed)
{
    return qHash((quintptr)key.dfunc(), seed);
}


void BoundaryVertex::BoundaryVertexData::updatePoint()
{
    qreal bs = bisectorFactor(&from.part().lastVNode(), &to.part().firstVNode());
//    if (isStart) {
//        if (to.part().isEmpty()) {
//            if (!from.part().isEmpty())
//                point = from.part().lastVNode().normal() * from.offset();
//            return;
//        }
//        to.result().trimStart(bs * from.offset());
//    } else {
//        if (from.part().isEmpty()) {
//            if (!to.part().isEmpty())
//                point = to.part().firstVNode().normal() * to.offset();
//            return;
//        }
//        from.result().trimEnd(bs * to.offset());
//    }
    point = (from.part().tailPoint() + to.part().headPoint()) / 2
            + to.offset() * bs * -from.part().lastVNode().unitVector()
            + from.offset() * bs * to.part().firstVNode().unitVector();
}
