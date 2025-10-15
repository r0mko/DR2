#include "guideoffsetter.h"


QPointF Wavefront::resultOrigin() const
{
    return m_resultOrigin;
}

Guide Wavefront::toGuide() const
{
    Guide r;
    if (m_result.isEmpty())
        return r;
    r.appendPoint(m_resultOrigin + m_result.first().vnode.normal() * m_offset);
    for (WFNode n : m_result) {
        r.appendNode(n.vnode);
    }
    return r;
}

QPolygonF Wavefront::toPolygon() const
{
    QPolygonF ret;
    QPointF pt(m_resultOrigin + m_result.first().vnode.normal() * m_offset);
    for (WFNode n : m_result) {
        pt += n.vnode.vector();
        ret.append(pt);
    }
    return ret;
}

qreal Wavefront::offset() const
{
    return m_offset;
}

void Wavefront::setOffset(qreal offset)
{
    m_offset = offset;
}

void Wavefront::shrinkAll(float offset)
{
    for (WFNode & n : m_result) {
        n.shrink(offset);
    }

}

Wavefront::Wavefront(Guide path)
{
    setSource(path);
}

void Wavefront::setSource(Guide path)
{
    if (path.isEmpty())
        return;
    //    m_sourcePath = path;
    m_sourceOrigin = path.headPoint();
    for (GuideIterator i_vec = path.first(); i_vec != path.tail(); ++i_vec) {
        GuideIterator i_prevVec = i_vec - 1;
        WFNode w;
        w.vnode = *i_vec.vnode();
        if (!i_prevVec->isVector()) {
            m_source.append(w);
            continue;
        }
        QVector2D ubs = unitBisector(i_prevVec.vnode(), i_vec.vnode());
        w.leftDr = QVector2D::dotProduct(ubs, QVector2D(i_vec.vnode()->unitVector()));
        m_source.last().rightDr = QVector2D::dotProduct(ubs, -QVector2D(i_prevVec.vnode()->unitVector()));
        m_source.append(w);
    }
//    qDebug() << "Max offsets:";
//    for (auto ii = maxOffsets.begin(); ii != maxOffsets.end(); ++ii) {
//        qDebug() << " node" << std::distance(m_source.begin(), QLinkedList<WFNode>::iterator(ii.value()))
//                 << "offset" << ii.key();
//    }
    //    update();
}

QPointF Wavefront::sourceOrigin() const
{
    return m_sourceOrigin;
}

void Wavefront::update(bool)
{
    m_resultOrigin = m_sourceOrigin;
    m_result = m_source;
//    m_result.reserve(m_result.size() * 5);
    m_result.setSharable(false);
    QPointF pt = m_resultOrigin;

    QMap<float, QLinkedList<WFNode>::iterator> maxOffsets;

    for (QLinkedList<WFNode>::iterator it = m_result.begin(); it != m_result.end(); ++it) {
        it->initialMaxOffset = it->maxOffset();
        maxOffsets.insert(it->initialMaxOffset, it);
    }

    float currentOffset = 0;
    while (fabsf(currentOffset) < fabsf(m_offset)) {
        QMap<float, QLinkedList<WFNode>::iterator>::iterator it;
        if (m_offset > 0)
            it = maxOffsets.lowerBound(currentOffset);
        else
            it = maxOffsets.upperBound(currentOffset);

//        if ()
        float offsetDelta = m_offset - currentOffset;
        if (it == maxOffsets.end() || fabsf(it.key()) > fabsf(m_offset)) {
            // shrink all nodes and return
            for (WFNode & n : m_result) {
                n.shrink(offsetDelta);
            }
            return;
        } else {
            float maxDelta = it.key() - currentOffset;
            QLinkedList<WFNode>::iterator ii(it.value());
            QLinkedList<WFNode>::iterator iin;
            QLinkedList<WFNode>::iterator iip;

            iin = ii + 1;
            iip = ii - 1;
//            while (iin->deleted && ++iin != m_result.end()) {};
            if (ii == m_result.begin()) {
//                m_resultOrigin -= currentOffset * ii->vnode.normal();
                m_resultOrigin += ii->vnode.vector();
                ii = m_result.erase(ii);
                ii->leftDr = 0; // set first bisector to normal
//                m_resultOrigin += maxDelta * ii->vnode.normal();
                shrinkAll(maxDelta);
                it = maxOffsets.erase(it);
                float lastMax = ii->initialMaxOffset;
                if (maxOffsets.remove(lastMax) < 1) {
                    auto nrst_l = maxOffsets.lowerBound(lastMax);
                    auto nrst_u = maxOffsets.upperBound(lastMax);
//                    qWarning() << "offset" << lastMax << "not found in map; nearest" << nrst_l.key() << "and" << nrst_u.key() << maxDelta;
                }
                currentOffset += maxDelta;
                ii->initialMaxOffset = ii->maxOffset() + currentOffset;
                maxOffsets.insert(ii->initialMaxOffset, ii);
//                qDebug() << "inserted 1st max" << ii->initialMaxOffset << "current" << currentOffset << "target" << m_offset << "delta" << maxDelta;
                continue;
            }
//            while (iip->deleted && --iip != m_result.begin()) {};
            if (iin == m_result.end()) {
                ii = --m_result.erase(ii);
                ii->rightDr = 0; // set last bisector to normal
                shrinkAll(maxDelta);
                it = maxOffsets.erase(it);
                float lastMax = ii->initialMaxOffset;
                if (maxOffsets.remove(lastMax) < 1) {
                    auto nrst_l = maxOffsets.lowerBound(lastMax);
                    auto nrst_u = maxOffsets.upperBound(lastMax);
//                    qWarning() << "offset" << lastMax << "not found in map; nearest" << nrst_l.key() << "and" << nrst_u.key() << maxDelta;
                }
                currentOffset += maxDelta;
                ii->initialMaxOffset = ii->maxOffset() + currentOffset;
                maxOffsets.insert(ii->initialMaxOffset, ii);
                continue;
            }
//            qDebug() << "removed next maxoffset" << maxOffsets.remove(iin->initialMaxOffset);
//            qDebug() << "removed prev maxoffset" << maxOffsets.remove(iip->initialMaxOffset);
            shrinkAll(maxDelta);
//            for (WFNode & n : m_result) {
//                n.shrink(maxDelta);
//            }
            QVector2D ubs = unitBisector(&iip->vnode, &iin->vnode);
            //           pt += ii->vnode.vector();
            iin->leftDr = QVector2D::dotProduct(ubs, QVector2D(iin->vnode.unitVector()));
            iip->rightDr = QVector2D::dotProduct(ubs, -QVector2D(iip->vnode.unitVector()));
//            ii->deleted = true;
//            = --m_result.erase(ii);
            it = maxOffsets.erase(it);
            float lastMaxP = iip->initialMaxOffset;
            if (maxOffsets.remove(lastMaxP) < 1) {
                auto nrst_l = maxOffsets.lowerBound(lastMaxP);
                auto nrst_u = maxOffsets.upperBound(lastMaxP);
                qWarning() << "prev offset" << lastMaxP << "not found in map; nearest" << nrst_l.key() << "and" << nrst_u.key() << maxDelta;
            }
            float lastMaxN = iin->initialMaxOffset;
            if (maxOffsets.remove(lastMaxN) < 1) {
                auto nrst_l = maxOffsets.lowerBound(lastMaxN);
                auto nrst_u = maxOffsets.upperBound(lastMaxN);
                qWarning() << "next offset" << lastMaxN << "not found in map; nearest" << nrst_l.key() << "and" << nrst_u.key() << maxDelta;
            }

            ii = m_result.erase(ii);
            currentOffset += maxDelta;
            iip->initialMaxOffset = iip->maxOffset() + currentOffset;
            ii->initialMaxOffset = ii->maxOffset() + currentOffset;
            maxOffsets.insert(iip->initialMaxOffset, iip);
            maxOffsets.insert(ii->initialMaxOffset, ii);
        }
    }
}


void Wavefront::WFNode::shrink(float offset)
{
//    vnode.lref() -= decayRate() * (offset - currentOffset);
//    if (vnode.length() < 0) {
//        qWarning() << "incorrect shrink";
//    }
//    currentOffset = offset;
    vnode.lref() -= decayRate() * offset;
//    if (vnode.length() < 0) {
//        qWarning() << "incorrect shrink";
//    }
}

void Wavefront::WFNode::expand(float offset)
{
//    vnode.lref() += decayRate() * (offset - currentOffset);
//    currentOffset = offset;
    vnode.lref() += decayRate() * offset;

}




