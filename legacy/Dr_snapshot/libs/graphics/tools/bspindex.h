#ifndef BSPINDEX_H
#define BSPINDEX_H
#include <QtGlobal>
#include <QPointF>
#include <QRectF>
#include <QBitArray>

class BspIndex
{
    quint64 d;

    inline bool isDepthOdd() const { return d & 0x1; }

    inline void decX() { d -= 0x800000000; }
    inline void incX() { d += 0x800000000; }
    inline void incXY() { d += 0x800000040; }

    inline void decY() { d -= 0x40; }
    inline void incY() { d += 0x40; }


public:
    inline BspIndex() : d(0) {}
    inline BspIndex(quint64 id) : d(id) {}
    inline BspIndex(const BspIndex &other) : d(other.d) {}
    inline BspIndex(uint keyX, uint keyY, uint depth) {
        d = ((quint64)(keyX & 0x1FFFFFFF) << 35) | ((quint64)(keyY & 0x1FFFFFFF) << 6) | (depth & 0x3F);
    }

    inline quint64 id() const;
    inline void setId(quint64 id);
    inline BspIndex parent() const;
    inline QPoint point() const;
    inline QRect rect() const;
    inline void setPoint(const QPoint &pt);
    inline uint keyY() const { return (d >> 6) & 0x1FFFFFFF; }
    inline uint keyX() const { return (d >> 35) & 0x1FFFFFFF; }
    inline void setKeyX(uint x) { d &= 0x7FFFFFFFF; d |= (quint64)(x & 0x1FFFFFFF) << 35; }
    inline void setKeyY(uint y);
    inline void setKeys(uint x, uint y);
    static inline QPoint posForKeys(uint keyX, uint keyY, uint depth);

    inline uint depth() const { return d & 0x3F; }
    inline void setDepth(uint depth);
    inline BspIndex leftChild() const;
    inline BspIndex rightChild() const;
    inline BspIndex sibling() const;
    inline bool isRightChild() const;
    inline BspIndex left() const;
    inline BspIndex right() const;
    inline BspIndex upper() const;
    inline BspIndex bottom() const;

    inline void moveLeft() { decX(); }
    inline void moveRight() { decX(); }
    inline void moveUp() { decY(); }
    inline void moveDown() { incY(); }

    inline bool containsPoint(QPoint p);
    inline bool isParent(const BspIndex &other);
    inline bool isSibling(const BspIndex &other);
    inline BspIndex &operator =( const BspIndex &other );
    inline BspIndex &operator =(quint64 id);
    inline operator quint64() const { return d; }

    QBitArray path(BspIndex id) const
    {
        QBitArray path;
        path.resize(depth());
        int i = path.size() - 1;
        do {
            if (id.isRightChild())
                path.setBit(i);
            id = id.parent();
            --i;
        } while(id.depth());
        return path;
    }

}; // BspIndex














quint64 BspIndex::id() const { return d; }
void BspIndex::setId(quint64 id) { d = id; }

BspIndex BspIndex::parent() const {
    BspIndex ret;
    ret.d = d;
    uint d = depth();
    if (isDepthOdd())
        ret.setKeyX(keyX() >> 1);
    else
        ret.setKeyY(keyY() >> 1);
    ret.setDepth(d - 1);
    return ret;
}

QPoint BspIndex::point() const {
    uint d = depth();
    int b = d >> 1;
    int xbits = 29 - (b + (d & 1));
    int ybits = 29 - b;
    return QPoint(keyX() << xbits, keyY() << ybits);
}

QRect BspIndex::rect() const {
    BspIndex br(d);
    br.incXY();
    QPoint pUL = point();
    QPoint pBR = br.point();
    pBR.rx() -= 1;
    pBR.ry() -= 1;
    return QRect(pUL, pBR);
}


void BspIndex::setPoint(const QPoint &pt) {
    uint d = depth();
    int b = d >> 1;
    int xbits = 29 - (b + (d & 1));
    int ybits = 29 - (b + 1);
    setKeyX(pt.x() >> xbits);
    setKeyY(pt.y() >> ybits);
}

void BspIndex::setKeyY(uint y) {
    d &= 0xFFFFFFF80000003F;
    d |= (quint64)(y & 0x1FFFFFFF) << 6;
}


void BspIndex::setKeys(uint x, uint y) {
    d &= 0x3F;
    d |= ((quint64)(x & 0x1FFFFFFF) << 35) | ((quint64)(y & 0x1FFFFFFF) << 6);
}


QPoint BspIndex::posForKeys(uint keyX, uint keyY, uint depth) {
    int b = depth >> 1;
    int xbits = 29 - (b + (depth & 1));
    int ybits = 29 - b;
    return QPoint(keyX << xbits, keyY << ybits);
}

void BspIndex::setDepth(uint depth) {
    d &= 0xFFFFFFFFFFFFFFC0;
    d |= depth & 0x3F;
}


BspIndex BspIndex::leftChild() const {
    BspIndex ret(d);
    uint d0 = depth();
    if (isDepthOdd()) {
        ret.setKeyY(keyY() << 1);
    } else {
        ret.setKeyX(keyX() << 1);
    }
    ret.setDepth(d0 + 1);
    return ret;
}

BspIndex BspIndex::rightChild() const {
    BspIndex ret(d);
    uint d0 = depth();
    if (isDepthOdd())
        ret.setKeyY((keyY() << 1) | 0x1);
    else
        ret.setKeyX((keyX() << 1) | 0x1);
    ret.setDepth(d0 + 1);
    return ret;
}

BspIndex BspIndex::sibling() const {
    BspIndex ret(d);
    if (isDepthOdd())
        ret.setKeyX(keyX() ^ 0x1);
    else
        ret.setKeyY(keyY() ^ 0x1);
    return ret;
}

bool BspIndex::isRightChild() const {
    if (isDepthOdd())
        return keyX() & 0x01;
    else
        return keyY() & 0x01;
}

BspIndex BspIndex::left() const {
    BspIndex ret(d);
    ret.decX();
    return ret;
}

BspIndex BspIndex::right() const {
    BspIndex ret(d);
    ret.incX();
    return ret;
}

BspIndex BspIndex::upper() const {
    BspIndex ret(d);
    ret.decY();
    return ret;
}

BspIndex BspIndex::bottom() const {
    BspIndex ret(d);
    ret.incY();
    return ret;
}

bool BspIndex::containsPoint(QPoint p) {
    uint d = depth();
    int b = d >> 1;
    int xbits = 29 - (b + (d & 1));
    int ybits = 29 - b;
    p.rx() >>= xbits;
    p.ry() >>= ybits;
    QPoint pt = point();
    pt.rx() >>= xbits;
    pt.ry() >>= ybits;
    return (pt == p);
}

bool BspIndex::isParent(const BspIndex &other) {
    if (isDepthOdd()) {
        return (other.keyY() == keyY() && other.keyX() == (keyX() >> 1));
    } else {
        return (other.keyY() == (keyY() >> 1) && other.keyX() == keyX());
    }
}

bool BspIndex::isSibling(const BspIndex &other) {
    quint64 x = d ^ other.d; return (x == 0x40 || x == 0x800000000);
}

BspIndex &BspIndex::operator =(const BspIndex &other) {
    d = other.d;
    return *this;
}

BspIndex &BspIndex::operator =(quint64 id) {
    this->d = id;
    return *this;
}


#endif
