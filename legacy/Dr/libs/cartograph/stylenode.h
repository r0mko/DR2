#ifndef STYLENODE_H
#define STYLENODE_H

#include "defs.h"
#include <qsgnode.h>
#define DECLARE_DATA(Class) inline Class ## Data *_df() const { return static_cast<Class ## Data*>(d.data()); }
class QSGGeometryNode;
class QSGMaterial;
class QSGGeometry;
class QSGNode;
class GeoLayer;
class GeomBuffer;
class StyleNode;
/**
 * @brief общие данные стиля
 */
class StyleNodeData : public QSharedData
{
public:
    QList<QVariant>    children;
    bool               deleted = false;
    ~StyleNodeData();
};

class StyleNode
{
    Q_GADGET
    Q_PROPERTY(bool deleted READ deleted WRITE setDeleted)

public:
    StyleNode() : d(new StyleNodeData) {}
    virtual ~StyleNode() {}

    void init(const VertexBuffer &geometry);

    QSGNode *updateNode(QSGNode *node, const VertexBuffer &geometry);
    void appendChild(const QVariant &child);
    Q_INVOKABLE void insertChild(const QVariant &child, int index);
    void moveChildren(int start, int end, int newStart);
    void removeChildren(int start, int end);
    inline bool isEmpty() const { return d->children.isEmpty(); }

    QSGMaterial *material() const { return m_material; }
    QSGGeometry *geometry() const { return m_geometry; }
    QSGNode *node() const;

    inline bool deleted() const { return d->deleted; }
    void setDeleted(bool deleted) ;
    inline operator QVariant() const { return QVariant::fromValue(*this); }
    static StyleNode *fromVariant(const QVariant &variant);

    inline bool operator ==(const StyleNode &other) const { return d == other.d; }
protected:
    StyleNode(StyleNodeData *ptr) : d(ptr) {}
    virtual void initBuffer(const VertexBuffer &) {}
    virtual QSGNode *updateStyleNode(QSGNode *n, const VertexBuffer &) { qDebug() << "Dummy node"; return n; }
    virtual const VertexBuffer &updateBuffer(const VertexBuffer &geometry, bool = false) { return geometry; }

    inline void setNode(QSGGeometryNode *node) { m_node = node; }
    inline void setMaterial(QSGMaterial *material) { m_material = material; }
    inline void setGeometry(QSGGeometry *geometry) { m_geometry = geometry; }

    QSGGeometryNode *getGeometryNode(QSGNode *node);
    QSGOpacityNode *getOpacityNode(QSGNode *node);
    template <class T> T *getMaterial(QSGGeometryNode *n);

    QExplicitlySharedDataPointer<StyleNodeData> d;

private:
    QSGNode*            m_node = nullptr;
    QSGMaterial*        m_material = nullptr;
    QSGGeometry*        m_geometry = nullptr;

};

Q_DECLARE_METATYPE(StyleNode)
template <class T> T *StyleNode::getMaterial(QSGGeometryNode *n)
{
    T *m = static_cast<T*>(n->material());
    if (!m) {
        if (!m_material)
            m_material = new T;
        n->setMaterial(m_material);
    } else {
        if (m_material && m_material != m) {
            qWarning() << "Material was changed!";
        }
        m_material = m;
    }
    return static_cast<T*>(m_material);
}


#endif // STYLENODE_H
