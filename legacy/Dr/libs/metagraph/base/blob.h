#ifndef BLOB_H
#define BLOB_H

#include "node.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlPropertyMap>
/**
 * @brief Узел для хранения двоичных данных. Может использоваться как база для всех типов узлов с информацией, используя \c QByteArray
 * в качестве постоянного хранилища, добавив методы и переменные для оперативной работы.
 */
class Blob : public Node
{
    METAGRAPH_NODE(Blob)
public:
    using Node::Node;
    QByteArray rawdata; ///< Двоичные данные
    QJsonObject metadata;
    uint type;
    bool exportToFile(QUrl filename) const;
    virtual int typeId() const;
    QVariant data() const;
    void setData(const QVariant &value);
};
/**
 * @brief Ссылка на узел \c Blob
 */
class BlobHandle : public NodeHandle
{
    NODE_FN(Blob)
    Q_GADGET
    Q_PROPERTY(QByteArray data READ data WRITE setData)
    Q_PROPERTY(QJsonObject metadata READ metadata WRITE setMetadata)
    Q_PROPERTY(QString type READ type WRITE setType)
    Q_PROPERTY(QQmlPropertyMap* metadataHandle READ metadataHandle)
    Q_PROPERTY(int size READ size)
    static int id_metadataHandle;

public:
    using NodeHandle::NodeHandle;
    QByteArray data() const;
    void setData(QByteArray data);
    QJsonObject metadata() const;
    QString type() const;

    void setMetadata(QJsonObject metadata);
    void setType(QString type);

    int size() const;
    operator QByteArray() const;
    QQmlPropertyMap* metadataHandle() const;
};
/**
 * @brief Фабрика узлов типа Blob
 */
class BlobFactory : public NodeFactory
{
    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QJsonObject metadata READ metadata WRITE setMetadata NOTIFY metadataChanged)

    static QHash<uint, QString> typeHash2String;

    QByteArray m_data;
    QString m_type;
    QJsonObject m_metadata;

    friend class BlobHandle;
public:
    using NodeFactory::NodeFactory;
    QByteArray data() const { return m_data; }
    QString type() const { return m_type; }
    QJsonObject metadata() const { return m_metadata; }
    static inline QString resolveBlobType(uint typeHash) { return typeHash2String.value(typeHash); }
    static uint typeHash(const QString &blobType);

    void setType(QString type);
    void setMetadata(QJsonObject metadata);

    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;

public slots:
    void setData(QByteArray data);

signals:
    void dataChanged();
    void typeChanged();
    void metadataChanged();
protected:
    void setupNode(Node *node);
};

DECLARE_NODE(Blob)
#endif // BLOB_H
