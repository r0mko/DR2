#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QQuickItem>
#include <QTransform>
#include <QAbstractListModel>
class SpatiaLiteDB;

class MapView : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QRectF sourceRect READ sourceRect WRITE setSourceRect NOTIFY sourceRectChanged)
    Q_PROPERTY(uint targetSRID READ targetSRID WRITE setTargetSRID NOTIFY targetSRIDChanged)
    Q_PROPERTY(QTransform mapTransform READ transform NOTIFY mapTransformChanged)

    Q_PROPERTY(SpatiaLiteDB* database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(QQuickItem* scene READ scene NOTIFY sceneChanged)
    Q_PROPERTY(bool viewReady READ viewReady WRITE setViewReady NOTIFY viewReadyChanged)

    QTransform m_transform;
    QRectF m_sourceRect;
    SpatiaLiteDB* m_database;
    uint m_targetSRID;

    QQuickItem* m_scene;
    void setMapTransform(QTransform arg);
    bool m_viewReady = false;

public:
    explicit MapView(QQuickItem *parent = 0);
    QRectF sourceRect() const;
    SpatiaLiteDB *database() const;
    void setDatabase(SpatiaLiteDB* arg);
    uint targetSRID() const;
    Q_INVOKABLE QTransform transform() const;
    QPainterPath blobToPath(const QByteArray& ba) const;
    QQuickItem* scene() const;
    void setViewReady(bool viewReady);
    inline bool viewReady() const { return m_viewReady; }

signals:
    void sourceRectChanged(QRectF arg);
    void databaseChanged(SpatiaLiteDB* arg);
    void targetSRIDChanged(uint arg);
    void sceneChanged(QQuickItem* arg);
    void mapTransformChanged(QTransform arg);
    void viewReadyChanged(bool viewReady);

protected:
    void itemChange(ItemChange change, const ItemChangeData &value);

public slots:
    void getExtent();
    void setSourceRect(QRectF arg);
    void setTargetSRID(uint arg);
    void blobToPath(QVariant blob, QObject *path);
    void blobToGraphModel(QVariant blob, QObject *model);
//    void save( QString filename );
//    void load( QString filename );


};
#endif // MAPVIEW_H
