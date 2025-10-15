#ifndef BSP_H
#define BSP_H
#include "bspindex.h"
#include <QExplicitlySharedDataPointer>
#include <QHash>
#include <QPointF>
#include <QPolygonF>
#include <QSet>
#include <QTransform>
enum RectIntersectionType
{
    None        = 0x0,
    LeftSide    = 0x1,
    RightSide   = 0x2,
    TopSide     = 0x4,
    BottomSide  = 0x8,
    Contains    = 0x10
};

enum LineIntersectionType
{
    NoIntersection,
    BoundedIntersection,
    UnboundedIntersection,
};
/**
 * @brief Быстрый алгоритм вычисления точки пересечения отрезков для целочисленной системы координат
 * @param intersectionPoint опциональный параметр - точка пересечения
 */
LineIntersectionType intersects(const QLine &one, const QLine &other, QPointF *intersectionPoint = 0);
/**
 * @brief Определяет пересечение прямоугольника и отрезка.
 */
RectIntersectionType lineRectIntersection(const QRect &rect, const QLine &line);

template <class T>
class BspTree
{
    class LeafData : public QSharedData
    {
    public:
        QSet<int> toc;
    };

    struct BspTreeNode
    {
        BspTreeNode() : leftChildIdx(0), rightChildIdx(0) {}
        union {
            struct {
                int leftChildIdx;
                int rightChildIdx;
            };
            LeafData *d = nullptr;
        };
    };

    QRectF              m_rect;
    int                 m_maxDepth = 15;
    QList<BspTreeNode>  t_nodes;
    QList<T>            t_leaves;
    QTransform          mat;
    QTransform          invmat;


public:
    BspTree() { t_nodes.append(BspTreeNode()); }

    void setRect(const QRectF &rect);

    inline QPoint mapPoint(const QPointF &pt) const { qreal mx, my; mat.map(pt.x(), pt.y(), &mx, &my); return QPoint( (uint)mx, (uint)my ); }
    QRectF rect() const { return m_rect; }
    int maxDepth() const { return m_maxDepth; }
    void setMaxDepth(int maxDepth) { m_maxDepth = maxDepth; }

    void insertRect(const QRectF &rect, const T &data);
    void insertPoint(const QPointF &point, const T &data);
    void insertPoints(const QPolygonF &polygon, const T &data);

    void clear()
    {
        t_nodes.clear();
        t_leaves.clear();
        t_nodes.append(BspTreeNode());
    }

    QList<T> itemsAt(const QPointF &pt);
    QList<T> itemsAt(const QRectF &rect);

    uint depthForResolution(qreal size) const
    {
        uint s = (uint)(size * qMin(mat.m11(), mat.m22()));
        uint i;
        for(i = 58; s != 0; s >>= 1, i -= 2) { }
        return i;
    }

    //#ifdef QT_DEBUG
    QList<QRectF> dumpRects() const { return getRects(0, BspIndex(0)); }
    QList<BspIndex> dumpLeaves() const { return getLeaveCells(0, BspIndex(0)); }
    //#endif


private:
    QList<BspIndex> insertRectHelper(const QRect &rect, BspIndex id, uint depth);
    QList<BspIndex> insertPointHelper(const QPoint &point, BspIndex id, uint depth);

    static const QRectF maxRect;

    void insertLeaf(BspIndex id, LeafData *data);
    void spreadLeaf(int nodeIdx, LeafData *data);
    //#ifdef QT_DEBUG
    QList<QRectF> getRects(int nodeIdx, const BspIndex &idx) const;

    QList<BspIndex> getLeaveCells(int nodeIdx, const BspIndex &idx) const
    {
        QList<BspIndex> ret;
        int leftChildIdx = t_nodes.at(nodeIdx).leftChildIdx;
        int rightChildIdx = t_nodes.at(nodeIdx).rightChildIdx;
        if (leftChildIdx > 0)
            ret.append(getLeaveCells(leftChildIdx, idx.leftChild()));
        else if (leftChildIdx < 0) {
            ret.append(idx.leftChild());
        }
        if (rightChildIdx > 0)
            ret.append(getLeaveCells(rightChildIdx, idx.rightChild()));
        else if (rightChildIdx < 0) {
            ret.append(idx.rightChild());
        }
        return ret;
    }
    //#endif
    inline QList<T> getNodeLeaves(int nodeIdx) const
    {
        QList<T> ret;
        if (nodeIdx >= 0)
            return ret;
        const QSet<int> &toc = t_nodes.at(-nodeIdx).d->toc;
        for (int leafIdx : toc)
            ret.append(t_leaves.at(leafIdx));
        return ret;
    }

    QSet<T> gatherCellLeaves(int nodeIdx, const BspIndex &idx) const
    {
        if (nodeIdx < 0)
            return getNodeLeaves(nodeIdx).toSet();
        QSet<T> ret;
        int leftChildIdx = t_nodes.at(nodeIdx).leftChildIdx;
        int rightChildIdx = t_nodes.at(nodeIdx).rightChildIdx;
        if (leftChildIdx > 0)
            ret.unite(gatherCellLeaves(leftChildIdx, idx.leftChild()));
        else if (leftChildIdx < 0) {
            ret.unite(getNodeLeaves(leftChildIdx).toSet());
        }
        if (rightChildIdx > 0)
            ret.unite(gatherCellLeaves(rightChildIdx, idx.rightChild()));
        else if (rightChildIdx < 0) {
            ret.unite(getNodeLeaves(rightChildIdx).toSet());
        }
        return ret;
    }

    QList<T> findItemsAt(const QPoint &pos, int nodeIdx, const BspIndex &idx);
    QSet<T> findItemsInRect(const QRect &rect, int nodeIdx, const BspIndex &idx);

protected:


};

template <class T>
void BspTree<T>::setRect(const QRectF &rect)
{
    m_rect = rect;
    QPolygonF source = QPolygonF(rect);
    source.removeLast();
    QPolygonF destination = maxRect;
    destination.removeLast();
    QTransform::quadToQuad(source, destination, mat);
    invmat = mat.inverted();
}

template <class T>
void BspTree<T>::insertPoint(const QPointF &point, const T &data)
{
    QPoint pt = mapPoint(point);
//    if( !m_rect.contains(pt) ) return;
    int leafIdx = t_leaves.size();
    t_leaves.append(data);
    LeafData *d = new LeafData;
    d->toc.insert(leafIdx);
    QList<BspIndex> idxList = insertPointHelper(pt, 0, m_maxDepth);
    for (BspIndex idx : idxList)
        insertLeaf(idx, d);
}

template <class T>
void BspTree<T>::insertPoints(const QPolygonF &polygon, const T &data)
{
    for (QPointF pt : polygon)
        insertPoint(pt, data);
}


template <class T>
void BspTree<T>::insertLeaf(BspIndex id, LeafData *data)
{
    int nodesCount = t_nodes.size();
    QBitArray path = id.path(id);
    BspTreeNode *c = &t_nodes.first(); // root
    int *idx;
    for (int i = 0; i < path.size(); ++i) {
        if(path[i])
            idx = &c->rightChildIdx;
        else
            idx = &c->leftChildIdx;
        if (*idx == 0) {
            *idx = nodesCount++;
            t_nodes.append(BspTreeNode());
        } else if (*idx < 0) {
            // handle existing leaf
            *idx *= -1; // convert index from leaf to node index
            LeafData *ldata = t_nodes[*idx].d;
            ldata->ref.deref();
            BspTreeNode node;
            node.leftChildIdx = -nodesCount++;
            node.rightChildIdx = -nodesCount++;
            t_nodes[*idx] = node;
            BspTreeNode leafLeft;
            BspTreeNode leafRight;
            leafLeft.d = ldata;
            ldata->ref.ref();
            leafRight.d = ldata;
            ldata->ref.ref();

            QExplicitlySharedDataPointer<LeafData> leaf = QExplicitlySharedDataPointer<LeafData>(ldata);

            if (!leaf->toc.contains(data->toc)) {
                leaf.detach();
                LeafData *temp = leaf.data();
                data->ref.ref();
                temp->toc.unite(data->toc);
                leafLeft.d = temp;
                temp->ref.ref();
                leafRight.d = temp;
                temp->ref.ref();
            }
            t_nodes.append(leafLeft);
            t_nodes.append(leafRight);
        }
        c = &t_nodes[qAbs(*idx)];
    }
    if (c->leftChildIdx != 0 || c->rightChildIdx != 0) {
        // spread leaf down
        //                spreadLeaf();
    } else {
        if (*idx > 0)
            *idx *= -1;
        data->ref.ref();
        c->d = data;
    }
}

template <class T>
void BspTree<T>::spreadLeaf(int nodeIdx, LeafData *data)
{
    BspTreeNode *c = &t_nodes[qAbs(nodeIdx)];

    if (c->leftChildIdx < 0) {
        QSharedDataPointer<LeafData> leaf = QSharedDataPointer<LeafData>(t_nodes[-c->leftChildIdx].d);
        if (!leaf.constData()->toc.contains(data->toc)) {
            LeafData *temp = leaf.data(); // data is detached here
            temp->toc.unite(data->toc);
            t_nodes[-c->leftChildIdx].d = temp;
        }
    } else if (c->leftChildIdx > 0) {
        spreadLeaf(c->leftChildIdx, data);
    } else if (c->leftChildIdx == 0) {
        int s = t_nodes.size();
        BspTreeNode node;
        data->ref.ref();
        node.d = data;
        c->leftChildIdx = -s;
        t_nodes.append(node);
    }
    if (c->rightChildIdx < 0) {
        QSharedDataPointer<LeafData> leaf = QSharedDataPointer<LeafData>(t_nodes[-c->rightChildIdx].d);
        if (!leaf.constData()->toc.contains(data->toc)) {
            LeafData *temp = leaf.data(); // data is detached here
            temp->toc.unite(data->toc);
            t_nodes[-c->rightChildIdx].d = temp;
        }
    } else if (c->rightChildIdx > 0) {
        spreadLeaf(c->rightChildIdx, data);
    } else if (c->rightChildIdx == 0) {
        int s = t_nodes.size();
        BspTreeNode node;
        data->ref.ref();
        node.d = data;
        c->rightChildIdx = -s;
        t_nodes.append(node);
    }
}
//#ifdef QT_DEBUG
template <class T>
QList<QRectF> BspTree<T>::getRects(int nodeIdx, const BspIndex &idx) const
{
    QList<QRectF> ret;
    int leftChildIdx = t_nodes.at(nodeIdx).leftChildIdx;
    int rightChildIdx = t_nodes.at(nodeIdx).rightChildIdx;
    if (leftChildIdx > 0)
        ret.append(getRects(leftChildIdx, idx.leftChild()));
    else if (leftChildIdx < 0) {
        BspIndex leaf = idx.leftChild();
        QRectF rect = invmat.mapRect(QRectF(leaf.rect()));
        ret.append(rect);
    }
    if (rightChildIdx > 0)
        ret.append(getRects(rightChildIdx, idx.rightChild()));
    else if (rightChildIdx < 0) {
        BspIndex leaf = idx.rightChild();
        QRectF rect = invmat.mapRect(QRectF(leaf.rect()));
        ret.append(rect);
    }
    return ret;
}

template <class T>
QList<T> BspTree<T>::findItemsAt(const QPoint &pos, int nodeIdx, const BspIndex &idx)
{
    QList<T> ret;

    BspIndex left = idx.leftChild();
    if (left.containsPoint(pos)) {
        int leftChildIdx = t_nodes.at(nodeIdx).leftChildIdx;
        if (leftChildIdx > 0)
            ret.append(findItemsAt(pos, leftChildIdx, left));
        else if (leftChildIdx < 0)
            ret.append(getNodeLeaves(leftChildIdx));
    }

    BspIndex right = idx.rightChild();
    if (right.containsPoint(pos)) {
        int rightChildIdx = t_nodes.at(nodeIdx).rightChildIdx;
        if (rightChildIdx > 0)
            ret.append(findItemsAt(pos, rightChildIdx, right));
        else if (rightChildIdx < 0)
            ret.append(getNodeLeaves(rightChildIdx));
    }
    return ret;
}

template <class T>
QSet<T> BspTree<T>::findItemsInRect(const QRect &rect, int nodeIdx, const BspIndex &idx)
{
    QSet<T> ret;

    BspIndex left = idx.leftChild();
    QRect leftRect = left.rect();
    int leftChildIdx = t_nodes.at(nodeIdx).leftChildIdx;
    if (leftChildIdx) {
        if (rect.contains(leftRect)) {
            ret.unite(gatherCellLeaves(leftChildIdx, left));
        } else if (rect.intersects(leftRect)) {
            if (leftChildIdx > 0)
                ret.unite(findItemsInRect(rect, leftChildIdx, left));
            else if (leftChildIdx < 0)
                ret.unite(getNodeLeaves(leftChildIdx).toSet());
        }
    }
    BspIndex right = idx.rightChild();
    int rightChildIdx = t_nodes.at(nodeIdx).rightChildIdx;
    QRect rightRect = right.rect();
    if (rightChildIdx) {
        if (rect.contains(rightRect)) {
            ret.unite(gatherCellLeaves(rightChildIdx, right));
        } else if (rect.intersects(rightRect)) {
            if (rightChildIdx > 0)
                ret.unite(findItemsInRect(rect, rightChildIdx, right));
            else if (rightChildIdx < 0)
                ret.unite(getNodeLeaves(rightChildIdx).toSet());
        }
    }
    return ret;
}
//#endif

template <class T>
void BspTree<T>::insertRect(const QRectF &rect, const T &data)
{
    QPoint ul = mapPoint(rect.topLeft());
    QPoint br = mapPoint(rect.bottomRight());
    int leafIdx = t_leaves.size();
    t_leaves.append(data);
    LeafData *d = new LeafData;
    d->toc.insert(leafIdx);
    QList<BspIndex> idxList = insertRectHelper(QRect(ul, br), 0, m_maxDepth);
    for (BspIndex idx : idxList)
        insertLeaf(idx, d);
}

template <class T>
QList<T> BspTree<T>::itemsAt(const QPointF &pt)
{
    QPoint p = mapPoint(pt);
    return findItemsAt(p, 0, 0);
}

template <class T>
QList<T> BspTree<T>::itemsAt(const QRectF &rect)
{
    QPoint ul = mapPoint(rect.topLeft());
    QPoint br = mapPoint(rect.bottomRight());
    QRect r(ul, br);
    return findItemsInRect(r, 0, 0).toList();
}

template <class T>
QList<BspIndex> BspTree<T>::insertRectHelper(const QRect &rect, BspIndex id, uint depth)
{
    QList<BspIndex> ret;
    if (!depth)
        return {id};
    BspIndex left = id.leftChild();
    QRect leftRect = left.rect();
    BspIndex right = id.rightChild();
    QRect rightRect = right.rect();

    if (rect.contains(leftRect, true)) {
        ret.append(left.id());
    } else {
        if (leftRect.intersects(rect)) {
            ret.append(insertRectHelper(rect, left, depth - 1));
        }
    }

    if (rect.contains(rightRect, true)) {
        ret.append(right.id());
    } else {
        if (rightRect.intersects(rect)) {
            ret.append(insertRectHelper(rect, right, depth - 1));
        }
    }
    return ret;
}

template <class T>
QList<BspIndex> BspTree<T>::insertPointHelper(const QPoint &point, BspIndex id, uint depth)
{
    if (!depth)
        return {id};

    QList<BspIndex> ret;
    BspIndex left = id.leftChild();
    QRect leftRect = left.rect();
    BspIndex right = id.rightChild();
    QRect rightRect = right.rect();

    if (leftRect.contains(point))
        ret.append(insertPointHelper(point, left, depth - 1));
    else if (rightRect.contains(point))
        ret.append(insertPointHelper(point, right, depth - 1));

    return ret;
}


template <class T> const QRectF BspTree<T>::maxRect = QRectF(0, 0, 0x1FFFFFFF, 0x1FFFFFFF);

#endif // BSP_H
