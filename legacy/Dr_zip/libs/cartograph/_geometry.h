#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <QFile>
#include <QObject>
#include <QPainterPath>
#include <QDataStream>
#include <QPointF>
#include <QPolygonF>
#include <QDebug>
#include <QMetaType>
#include <QUrl>
#include "style.h"

QDebug operator<<(QDebug dbg, const GeomType &c);


class SpatiaLiteDB;
class Style;
class QSGNode;
class MapLayer;

class Geometry : public QObject
{
    Q_OBJECT

    Q_ENUMS(GeomType)
    Q_PROPERTY(MapLayer* layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(bool hidden READ hidden WRITE setHidden NOTIFY hiddenChanged)
    Q_PROPERTY(QString layerName READ layerName WRITE setLayerName NOTIFY layerNameChanged)
    Q_PROPERTY(QString where READ where WRITE setWhere NOTIFY whereChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(uint sourceSRID READ sourceSRID NOTIFY sourceSRIDChanged)
    Q_PROPERTY(Style* style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(GeomType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(SpatiaLiteDB* database READ database NOTIFY databaseChanged)

    friend QDataStream &operator <<(QDataStream &stream,const Geometry &s);
    friend QDataStream &operator >>(QDataStream &stream, Geometry &s);
public:
    enum GeomType {
        Unknown,
        Point,
        Polyline,
        Polygon
    };
    explicit Geometry(QObject *parent = 0) : QObject(parent) {}
    virtual ~Geometry() { qDeleteAll(m_geometry); }
    void readWkb(QDataStream &wkb);
    QList<GeoEntity*> data() const { return m_geometry; }
    uint sourceSRID() const;

    void setBoundingRect(QRectF arg);

    QPainterPath getPath(const QTransform &transform, bool clearData = false) const;

    void buildBuffers(const QTransform &transform);

    inline bool complete() const { return m_complete; }

    QSGNode *node() const;
    uint targetSRID() const;
    SpatiaLiteDB *database() const;

    inline MapLayer* layer() const { return m_layer; }
    inline QString layerName() const { return m_layerName; }
    inline QString where() const { return m_where; }
    inline QString query() const { return m_query; }
    inline Style* style() const { return m_style; }

    void setLayer(MapLayer* arg);
    void setLayerName(QString arg);
    void setWhere(QString arg);
    void setQuery(QString arg);
    void setStyle(Style* arg);
    void setType(GeomType arg);

    bool inScene() const { return m_inScene; }
    bool hidden() const { return m_hidden; }

    inline GeomType type() const { return m_type; }
    inline bool buffersComplete() const { return m_buffersComplete;}

public slots:
    void setComplete(bool arg);
    void setHidden(bool arg);
    void parseBlob(const QByteArray &ba);

signals:
    void queryChanged(QString arg);
    void layerNameChanged(QString arg);
    void sourceSRIDChanged();
    void completed();
    void databaseChanged();
    void styleChanged(Style* arg);
    void whereChanged(QString arg);
    void layerChanged(MapLayer* arg);
    void buffersReady();
    void hiddenChanged();

    void typeChanged(GeomType arg);

private slots:
    void tryFetch();
    void updateQuery();

private:
    QString m_layerName;
    QString m_where;
    QString m_query;

    QList<GeoEntity*> m_geometry;
    QRectF m_boundingRect;

    bool m_buffersComplete = false;
    bool m_complete = false;
    bool m_hidden = false;

    mutable bool m_inScene = false;

    Style* m_style = nullptr;
    MapLayer* m_layer = nullptr;
    mutable QSGNode *m_node = nullptr;
    GeomType m_type = Unknown;
};

Q_DECLARE_METATYPE(Geometry *)

#endif // GEOMETRY_H
