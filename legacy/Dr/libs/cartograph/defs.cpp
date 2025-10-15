#include "defs.h"


uint qHash(const QPointF &key, uint seed)
{
    uint h1 = qHash((quint64)key.x(), seed);
    uint h2 = qHash((quint64)key.y(), seed);
    return ((h1 << 16) | (h1 >> 16)) ^ h2 ^ seed;
}

void copyPolygon(const QPolygonF &pol, QPainterPath &path)
{
    for(QPointF p:pol) {
        Q_ASSERT(!p.isNull());
        if(p == pol.first())
            path.moveTo(p);
        else
            path.lineTo(p);
    }
}

QPolygonF reversedPolygon(const QPolygonF &pol)
{
    QPolygonF ret;
    for (int i = pol.size() - 1; i >= 0; --i)
        ret.append(pol.at(i));
    return ret;
}

qreal geoDist(const QPointF &p1, const QPointF &p2)
{
    qreal x1 = deg2rad(p1.x());
    qreal y1 = deg2rad(p1.y());

    qreal x2 = deg2rad(p2.x());
    qreal y2 = deg2rad(p2.y());

    qreal sinDx = sin(x2 - x1);
    qreal sinDy = sin(y2 - y1);
    return 2 * asin(sqrt(sinDx * sinDx + cos(x1) * cos(x2) * sinDy * sinDy));
}

QPainterPath buildPath(QList<QPolygonF> parts)
{
    QPainterPath ret;

    if (parts.isEmpty())
        return ret;

    while (!parts.isEmpty()) {
        QPolygonF joined = parts.takeFirst();
        if (!joined.isClosed()) {
            int counter = parts.size();
            do {
                auto i = parts.begin();
                while (i != parts.end() && !joined.isClosed()) {
                    QPolygonF p = *i;
                    if (joined.last() == p.first()) {
                        joined.removeLast();
                        joined += p;
                        i = parts.erase(i);
                        continue;
                    } else if (joined.last() == p.last()) {
                        joined.removeLast();
                        joined += reversedPolygon(p);
                        i = parts.erase(i);
                        continue;
                    } else if (joined.first() == p.last()) {
                        p.removeLast();
                        joined = p + joined;
                        i = parts.erase(i);
                        continue;
                    } else if (joined.first() == p.first()) {
                        QPolygonF head = reversedPolygon(p);
                        head.removeLast();
                        joined = head + joined;
                        i = parts.erase(i);
                        continue;
                    }
                    ++i;
                }
            } while (!parts.isEmpty() && counter-- && !joined.isClosed());

            // force merge non-connected polylines
            //            int tries = parts.size() * 2;
            //            enum class Mode {
            //                BB = 11,
            //                BE = 12,
            //                EB = 21,
            //                EE = 22
            //            } mode;
            //            while (!parts.isEmpty() && tries--) {

            //                qreal dmin = HUGE;
            //                int index = -1;
            //                auto i = parts.begin();
            //                int c = 0;
            //                while (i != parts.end()) {
            //                    QPolygonF pol = *i;
            //                    qreal dBB = geoDist(joined.first(), pol.first());
            //                    qreal dBE = geoDist(joined.first(), pol.last());
            //                    qreal dEB = geoDist(joined.last(), pol.first());
            //                    qreal dEE = geoDist(joined.last(), pol.last());
            //                    if (dmin > dBB) {
            //                        dmin = dBB;
            //                        index = c;
            //                        mode = Mode::BB;
            //                    }

            //                    if (dmin > dBE) {
            //                        dmin = dBE;
            //                        index = c;
            //                        mode = Mode::BE;
            //                    }

            //                    if (dmin > dEB) {
            //                        dmin = dEB;
            //                        index = c;
            //                        mode = Mode::EB;
            //                    }

            //                    if (dmin > dEE) {
            //                        dmin = dEE;
            //                        index = c;
            //                        mode = Mode::EE;
            //                    }
            //                    ++c;
            //                    ++i;
            //                }
            //                if (parts.isEmpty())
            //                    break;
            //                QPolygonF pol;
            //                if (index >= 0)
            //                    pol = parts.takeAt(index);
            //                switch (mode) {
            //                case Mode::BB:
            //                    joined = reversedPolygon(pol) + joined;
            //                    break;
            //                case Mode::BE:
            //                    joined = pol + joined;
            //                    break;
            //                case Mode::EB:
            //                    joined += pol;
            //                    break;
            //                case Mode::EE:
            //                    joined += reversedPolygon(pol);
            //                    break;
            //                default:
            //                    break;
            //                }

            //            }
        }
        copyPolygon(joined, ret);
    }
    ret.closeSubpath();
    return ret;
}

QPainterPath multipolygon(QList<QPolygonF> outer, QList<QPolygonF> inner)
{
    QPainterPath out;
    if (outer.isEmpty())
        return out;
    if (outer.size() > 1)
        out = buildPath(outer);
    else
        copyPolygon(outer.first(), out);
    out.closeSubpath();
    QPainterPath in = buildPath(inner);
    if (in.isEmpty())
        return out;
    return out - in;
}


quint32 VertexBuffer::addVertex(const QPointF &pt, bool fill)
{
    auto it = point2index.find(pt);
    QVector<quint32> &indices = fill ? m_fillIndices : m_lineIndices;
    quint32 idx;
    if (it == point2index.end()) {
        idx = m_vertices.size();
        point2index.insert(pt, idx);
        m_vertices.append(pt);
        indices.append(idx);
    } else {
        idx = it.value();
        indices.append(idx);
    }
    return idx;
}

void VertexBuffer::addTriangle(const QPointF &p1, const QPointF &p2, const QPointF &p3)
{
    addVertex(p1, true);
    addVertex(p2, true);
    addVertex(p3, true);
}

void VertexBuffer::addLine(const QPointF &p1, const QPointF &p2)
{
    addVertex(p1, false);
    addVertex(p2, false);
}

void VertexBuffer::mergeFill(const VertexBuffer &buffer)
{
    for (uint idx:buffer.m_fillIndices)
        addVertex(buffer.m_vertices.at(idx), true);
}

void VertexBuffer::addPolyLine(const QPolygonF &pol)
{
    for(int i = 0; i < pol.size() - 1; ++i) {
        addLine( pol.at(i), pol.at(i + 1) );
    }
}

void VertexBuffer::addPoints(const QPolygonF &pol)
{
    m_vertices.append(pol);
}



void VertexBuffer::addPolyLines(const QList<QPolygonF> &pols)
{
    for (QPolygonF pol:pols)
        addPolyLine(pol);
}

void VertexBuffer::clear()
{
    point2index.clear();
    m_vertices.clear();
    m_fillIndices.clear();
    m_lineIndices.clear();
}

QSGGeometry *VertexBuffer::createPointGeometry() const
{
    QSGGeometry *g;
    if (!hasFill()) {
        g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0, GL_UNSIGNED_INT);
        g->setDrawingMode(GL_POINTS);
        return g;
    }
    g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount());
    g->setDrawingMode(GL_POINTS);
    for (int i = 0; i < vertexCount(); ++i)
        g->vertexDataAsPoint2D()[i] = point2d(i);
    return g;
}

QSGGeometry *VertexBuffer::createLineGeometry() const
{
    QSGGeometry *g;
    if (!hasLine()) {
        g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0, GL_UNSIGNED_INT);
        g->setDrawingMode(GL_LINES);
        return g;
    }
    g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount(), lineIndexCount(), GL_UNSIGNED_INT);
    g->setDrawingMode(GL_LINES);
    commitLine(g);
    return g;
}

QSGGeometry *VertexBuffer::createFillGeometry() const
{
    QSGGeometry *g;
    if (!hasFill()) {
        g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0, GL_UNSIGNED_INT);
        g->setDrawingMode(GL_TRIANGLES);
        return g;
    }
    g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount(), fillIndexCount(), GL_UNSIGNED_INT);
    g->setDrawingMode(GL_TRIANGLES);
    commitFill(g);
    return g;
}

QSGGeometry *VertexBuffer::createOutlineGeometry() const
{
    if (!hasFill())
        return nullptr;
    auto idx = generateOutlineIndices();
    QSGGeometry *g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount(), lineIndexCount(), GL_UNSIGNED_INT);
    g->setDrawingMode(GL_LINES);
    commitIndexBuffer(g, idx);
    return g;
}

bool VertexBuffer::updatePointGeometry(QSGGeometry *g, bool force) const
{
    if (!g)
        return false;
    if (g->drawingMode() != GL_POINTS) {
        qWarning() << g << "is not point geometry";
        return false;
    }
    if (g->vertexCount() != vertexCount()){
        g->allocate(vertexCount());
        force = true;
    };
    if (force)
        for (int i = 0; i < vertexCount(); ++i)
            g->vertexDataAsPoint2D()[i] = point2d(i);
    return force;
}

bool VertexBuffer::updateLineGeometry(QSGGeometry *g, bool force) const
{

    if (!g)
        return false;
    if (g->drawingMode() != GL_LINES) {
        qWarning() << g << "is not linear geometry";
        return false;
    }
    if (g->vertexCount() != vertexCount() || g->indexCount() != lineIndexCount()){
        g->allocate(vertexCount(), lineIndexCount   ());
        force = true;
    };
    //    qDebug() << "Updating line geometry" << force;
    if (force)
        commitLine(g);
    return force;
}

bool VertexBuffer::updateFillGeometry(QSGGeometry *g, bool force) const
{
    if (!g)
        return false;
    if (g->drawingMode() != GL_TRIANGLES) {
        qWarning() << g << "is not fill geometry";
        return false;
    }
    if (g->vertexCount() != vertexCount() || g->indexCount() != fillIndexCount()){
        g->allocate(vertexCount(), fillIndexCount());
        force = true;
    };
    if (force)
        commitFill(g);
    return force;
}



QVector<uint> VertexBuffer::generateOutlineIndices() const
{
    QHash<quint64, int> edges;
    QVector<uint>::const_iterator it = m_fillIndices.cbegin();
    while (it != m_fillIndices.cend()) {
        uint p1, p2, p3;
        p1 = *it++;
        p2 = *it++;
        p3 = *it++;
        quint64 h1 = ((quint64)p1 << 32) + p2;
        quint64 h2 = ((quint64)p2 << 32) + p3;
        quint64 h3 = ((quint64)p3 << 32) + p1;
        quint64 rh1 = (h1 >> 32) | (h1 << 32);
        quint64 rh2 = (h2 >> 32) | (h2 << 32);
        quint64 rh3 = (h3 >> 32) | (h3 << 32);

        if (edges.contains(h1))
            edges[h1]++;
        else if (edges.contains(rh1))
            edges[rh1]++;
        else
            edges.insert(h1, 0);

        if (edges.contains(h2))
            edges[h2]++;
        else if (edges.contains(rh2))
            edges[rh2]++;
        else
            edges.insert(h2, 0);
        if (edges.contains(h3))
            edges[h3]++;
        else if (edges.contains(rh3))
            edges[rh3]++;
        else edges.insert(h3, 0);
    }
    QHash<quint64, int>::iterator it2 = edges.begin();
    QVector<uint> ret;
    while (it2 != edges.end()) {
        if (it2.value() > 0)
            it2 = edges.erase(it2);
        else {
            uint i1 = (it2.key() >> 32);
            ret.append(i1);
            uint i2 = (it2.key() & 0xffffffff);
            ret.append(i2);
            ++it2;
        }
    }

    return ret;
}

QList<QPolygonF> VertexBuffer::generatePolygons() const
{
    QList<QPolygonF> ret;
    QHash<uint, QSet<uint>> adjacencyHash;
    QVector<uint> vhits;
    vhits.fill(0, m_vertices.size());
    if (m_lineIndices.size() % 2)
        qWarning() << "odd line index count" << m_lineIndices.size();

    for (int n = 0; n < m_lineIndices.size(); n += 2) {
        uint i0 = m_lineIndices.at(n);                            // generate vertex hitlist and siblings matrix
        uint i1 = m_lineIndices.at(n + 1);                        // hits > 2 - line junction (graph node)
        adjacencyHash[i0].insert(i1);                                  // hits = 2 - transit node
        adjacencyHash[i1].insert(i0);                                  // hits = 1 - dead end
        vhits[i0]++;                                              // hits = 0 - blind node
        vhits[i1]++;
    }
    QSet<uint> nodeIndexes;
    auto ii = vhits.cbegin();
    while (ii != vhits.cend()) {
        ii = std::find_if(ii, vhits.cend(), [](const uint &v) { return v > 2; } );
        if (ii == vhits.cend())
            break;
        int ind = std::distance(vhits.cbegin(), ii);
        nodeIndexes.insert(ind);
        ii++;
    }
    qDebug() << "nodes count:" << nodeIndexes.size();
    int c = 0;
    QList<QPointF> graphNodes;

    while (!nodeIndexes.isEmpty()) {

        uint node_i = *nodeIndexes.begin();

        //        int maxhit = vhits[i];
        qDebug() << "idx" << node_i << "nodes count:" << adjacencyHash.size();

        while (vhits[node_i]) {
//            qDebug() << "iteration" << c++ << "edge #" << vhits[node_i];
            uint i = node_i;
            QPolygonF pol;
            pol.append(m_vertices.at(i));
            forever {
                QSet<uint> &current_sibs = adjacencyHash[i];
                if (current_sibs.isEmpty()) {
                    adjacencyHash.remove(i);
                    if (vhits[i]) {
                        qWarning() << "Error vhits at" << i << vhits[i];
                        vhits[i] = 0;
                    }
                    break;
                }
                uint next_i = *current_sibs.begin();             ///< next sibling index
                vhits[i]--;
                vhits[next_i]--;
                current_sibs.remove(next_i);
                adjacencyHash[next_i].remove(i);
                pol.append(m_vertices.at(next_i));
                if (!vhits[i]) {
                    adjacencyHash.remove(i); // remove empty sibling list
                }
                if (!vhits[next_i]) {
                    adjacencyHash.remove(next_i); // remove empty sibling list
//                    break;
                }
                if (nodeIndexes.contains(next_i))
                    break;
                i = next_i;
            };
            if (pol.size() > 1) {
                ret.append(pol);
                qDebug() << "Added polygon size" << pol.size() << adjacencyHash.size();
            } else {
                qDebug() << "small polygon" << vhits[i] << "nodes count" << adjacencyHash.size();
                break;
            }
        }
        graphNodes.append(m_vertices.at(node_i));
        nodeIndexes.remove(node_i);
    }
    //    it = std::max_element(vhits.begin(), vhits.end());
    //    qDebug() << *it;
    qDebug() << "Nodes count" << graphNodes.size() << "remaining adjtable size" << adjacencyHash.size() << vhits;
    return ret;

}

void VertexBuffer::initTable()
{
    point2index.clear();
    for (int i = 0; i < vertexCount(); ++i) {
        point2index.insert(m_vertices.at(i), i);
    }
}

void VertexBuffer::commitLine(QSGGeometry *g) const
{
    for (int i = 0; i < vertexCount(); ++i)
        g->vertexDataAsPoint2D()[i] = point2d(i);
    for (int i = 0; i < lineIndexCount(); ++i )
        g->indexDataAsUInt()[i] = lineIndex(i);
}

void VertexBuffer::commitFill(QSGGeometry *g) const
{
    for (int i = 0; i < vertexCount(); ++i)
        g->vertexDataAsPoint2D()[i] = point2d(i);
    for (int i = 0; i < fillIndexCount(); ++i)
        g->indexDataAsUInt()[i] = fillIndex(i);
}

void VertexBuffer::commitIndexBuffer(QSGGeometry *g, const QVector<uint> &idx) const
{
    for (int i = 0; i < vertexCount(); ++i)
        g->vertexDataAsPoint2D()[i] = point2d(i);
    for (int i = 0; i < idx.size(); ++i )
        g->indexDataAsUInt()[i] = idx.at(i);
}

QPolygonF mergePolygons(const QPolygonF &one, const QPolygonF &other)
{
    if (one.isEmpty())
        return other;
    if (other.isEmpty())
        return one;
    QPolygonF joined = one;
    if (joined.last() == other.first()) {
        joined.removeLast();
        joined += other;
    } else if (joined.last() == other.last()) {
        joined.removeLast();
        joined += reversedPolygon(other);
    } else if (joined.first() == other.last()) {
        joined.removeFirst();
        joined = other + joined;
    } else if (joined.first() == other.first()) {
        QPolygonF head = reversedPolygon(other);
        head.removeLast();
        joined = head + joined;
    }
    return joined;
}

qreal normalizeAngle(qreal angle)
{
    qreal m = fmod(angle, (2 * M_PI));
    if (m < 0)
        m = M_PI * 2 + m;
    return m;
}


qreal segmentAngle(qreal startAngle, qreal endAngle)
{
    startAngle = normalizeAngle(startAngle);
    endAngle = normalizeAngle(endAngle);
    qreal d = startAngle - endAngle;
    if (d < 0)
        d = 2 * M_PI + d;
    return d;
}
