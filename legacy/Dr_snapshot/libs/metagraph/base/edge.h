#ifndef EDGE_H
#define EDGE_H
#define checkPtr(X) if (!d) return X
#include "node.h"
#include "abstractedge.h"
#include "hashstring.h"
#include <QAbstractListModel>
#include <QJSValue>
#include <QSet>
#include <QDataStream>


class Edge : public AbstractEdge
{
    static uint count;
    const quint32 uid;
    QVariant m_data;
    QSet<HashString> m_tags;
public:
    Edge() : uid(count++) {}
    friend class EdgeModel;
    friend class GraphModel;
    ~Edge();

    inline QVariant data() const { return m_data; }
    void setData(const QVariant &data);
    inline quint32 getUid() const { return uid; }

    QString tag() const;
    QStringList tags() const;
    inline bool hasRawTag(uint rawTag) const { return m_tags.contains(rawTag); }
    bool hasTag(const QString &tag) const;
    bool hasAllTags(const QStringList &tags) const;
    bool hasAnyTag(const QStringList &tags) const;
    void setTag(const QString &tag);
    void setTags(const QStringList &tags);
    void addTag(const QString &tag);

    int typeId() const;
    void setUid(const quint32 &value);
};

//template <typename T> T getFromStream(QDataStream & str) {
//   T value;
//   str >> value;
//   return value;
//}

//inline QDataStream & operator<<(QDataStream & str, const Edge::EdgeTypes & type) {
//    str << (quint8)type;
//    return str;
//}
//inline QDataStream & operator>>(QDataStream & str, Edge::EdgeTypes & type) {
//    type = static_cast<Edge::EdgeTypes>(getFromStream<quint8>(str));
//    return str;
//}

class EdgeHandle
{
    Q_GADGET
    Q_PROPERTY(QVariant data READ data WRITE setData)
    Q_PROPERTY(QString tag READ tag CONSTANT)
    Q_PROPERTY(QStringList tags READ tags CONSTANT)
    Q_PROPERTY(QJSValue startNode READ startNode)
    Q_PROPERTY(QJSValue endNode READ endNode)
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(uint uid READ uid CONSTANT)

    Edge *d = nullptr;
    GraphModel *m_model = nullptr;

public:
    EdgeHandle() {}
    EdgeHandle(GraphModel *model) : m_model(model) {}
    EdgeHandle(Edge *edge, GraphModel *model) : d(edge), m_model(model) {}
    QVariant data() const;
    QString tag() const;
    QStringList tags() const;
//    QSet<Edge::EdgeTypes> types() const {return d->types(); }
    QJSValue startNode() const;
    QJSValue endNode() const;
    QVariant startNodeHandle() const;
    QVariant endNodeHandle() const;
    inline bool isValid() const { return d != nullptr; }
    inline void setModel(GraphModel *model) { m_model = model; }
    GraphModel *model() { return m_model; }

    Q_INVOKABLE void destroy();
    Q_INVOKABLE bool hasTag(const QString &tag) const;
    Q_INVOKABLE void addTag(const QString &tag);
    Q_INVOKABLE void setTags(const QStringList &tags);

    Q_INVOKABLE QVariant getData() const;
    Q_INVOKABLE void setData(QVariant data);

    uint uid() const;

private:
    void notifyModel() const;
};
typedef QList<EdgeHandle> EdgeHandleList;

Q_DECLARE_METATYPE(Edge)
Q_DECLARE_METATYPE(EdgeHandle)
Q_DECLARE_METATYPE(EdgeHandleList)
Q_DECLARE_METATYPE(Edge*)

#endif // EDGE_H
