#ifndef GEOLAYER_H
#define GEOLAYER_H

#include <QQuickItem>
#include <qtransform.h>
#include "wkt.h"
#include "instantiatorcollection.h"

class SpatiaLiteDB;
class GeoLayer;
class GeoView;
class StyleNode;
class SpatialQuery;
class GeomBuffer;


class GeoLayer : public QQuickItem
{
    Q_OBJECT

    Q_ENUMS(GeometryType)

    Q_PROPERTY(SpatiaLiteDB* dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
    Q_PROPERTY(QString table READ table WRITE setTable NOTIFY tableChanged)
    Q_PROPERTY(GeoView* view READ view WRITE setView NOTIFY viewChanged)

    Q_PROPERTY(GeometryType geometryType READ geometryType NOTIFY geometryTypeChanged)
    Q_PROPERTY(GeometryKind geometryKind READ geometryKind NOTIFY geometryKindChanged)
    Q_PROPERTY(QString geometryColumn READ geometryColumn NOTIFY geometryColumnChanged)
    Q_PROPERTY(int srid READ srid NOTIFY sridChanged)
    Q_PROPERTY(QRectF extent READ extent NOTIFY extentChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(int entityCount READ entityCount NOTIFY entityCountChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(QQmlListProperty<GeomBuffer> geometryBuffers READ qmlGeometryBuffers NOTIFY geometryBuffersChanged)

public:
    friend class SpatiaLiteDB;
    friend class GeomBuffer;
    static const QRectF maxRect;

    enum GeometryKind {
        UnknownGeometry = -1,
        CompoundObjects = 0,
        PointEntity = 1,
        LinearEntity = 2,
        PolygonEntity = 3
    };
    Q_ENUM(GeometryKind)

    enum GeometryType {
        Invalid= -1,
        GeometryEntity = 0,
        Point = 1,
        LineString = 2,
        Polygon = 3,
        MultiPoint = 4,
        MultiLineString = 5,
        MultiPolygon = 6,
        GeometryCollection = 7,
        CircularString = 8,
        CompoundCurve = 9,
        CurvePolygon = 10,
        MultiCurve = 11,
        MultiSurface = 12,
        Curve = 13,
        Surface = 14,
        PolyhedralSurface = 15,
        TIN = 16,
        Triangle = 17,
    };
    Q_ENUM(GeometryType)

    GeoLayer(QQuickItem *parent = 0);
    ~GeoLayer();

    inline GeometryType geometryType() const { return m_geometryType; }
    inline GeometryKind geometryKind() const { return m_entityKind; }
    inline SpatiaLiteDB* dataSource() const { return m_dataSource; }
    int srid() const;
    QString query() const;
    QString geometryColumn() const;
    QString table() const;
    QString classifyColumn() const;
    GeoView *view() const;
    QTransform geomBufferTransform() const;
    QRectF extent() const;
    bool isValid() const;
    int entityCount() const;

    void setDataSource(SpatiaLiteDB* arg);
    void setTable(QString table);
    void setView(GeoView *view);

    void componentComplete();
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);
    qreal progress() const;

public slots:
    void insertBuffer(int index, GeomBuffer *geom);
    void removeBuffer(int index);
    void removeBuffer(GeomBuffer *geom);
    void appendBuffer(GeomBuffer *geom);

signals:
    void sridChanged(int arg);
    void transformChanged();
    void extentChanged();
    void viewChanged();
    void geometryTypeChanged();
    void tableChanged();
    void isValidChanged();
    void dataSourceChanged();
    void geometryColumnChanged();
    void geometryKindChanged(GeometryKind geometryKind);
    void entityCountChanged(int entityCount);
    void progressChanged();
    void geometryBuffersChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData &v);

private slots:
    void initLayer();

private:
    void setExtent(QRectF arg);
    void setValid(bool isValid);
    void setGeometryColumn(QString geometryColumn);
    void setGeometryType(GeometryType type);
//    void setSrid(int arg);

    SpatiaLiteDB*           m_dataSource = nullptr;
    GeometryType            m_geometryType = Invalid;
    GeometryKind              m_entityKind = UnknownGeometry;

    QString                 m_table;
    QString                 m_geometryColumn;
//    int                     m_srid = -1;            ///< Исходная СК
    bool                    m_isValid = false;

    QTransform              m_transform;            ///< Преобразование \a из СК триангулятора (maxRect) \a в СК targetSRID
    QRectF                  m_extent;               ///< Экстент в координатах проекции targetSRID
    GeoView*                m_view = nullptr;
    QVector<GeomBuffer*>    m_buffers;

    int                     m_fetchedCount = 0;
    QSGNode *               highlight = nullptr;

    QQmlListProperty<GeomBuffer> qmlGeometryBuffers();

    static int qmlGeometryBuffersCount(QQmlListProperty<GeomBuffer> *list);
    static GeomBuffer *qmlGeometryBuffersAt(QQmlListProperty<GeomBuffer> *list, int index);
    QQmlListProperty<GeomBuffer> m_geometryBuffers;
};
Q_DECLARE_METATYPE(QTransform)
#endif // GEOLAYER_H
