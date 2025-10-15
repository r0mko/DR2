#ifndef GEOMBUFFER_H
#define GEOMBUFFER_H
#include "defs.h"
#include "style.h"
#include "wkt.h"

#include <QFuture>
#include <QJsonObject>
#include <QObject>
#include <QQmlParserStatus>
#include <QTransform>
#include <QFutureWatcher>
#include "stylenode.h"
class GeoLayer;
class SpatialQuery;
class QSGNode;
class QSGGeometry;
class QSGGeometryNode;
class GeomBuffer : public QObject, public QQmlParserStatus, public VertexBuffer
{
    Q_OBJECT
    Q_PROPERTY(GeoLayer* layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(QString clause READ clause WRITE setClause NOTIFY clauseChanged)
    Q_PROPERTY(bool highlight READ highlight WRITE setHighlight NOTIFY highlightChanged)

    Q_PROPERTY(int entityCount READ entityCount NOTIFY entityCountChanged)
    Q_PROPERTY(int completedEntityCount READ completedEntityCount NOTIFY completedEntityCountChanged)

    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(Style style READ style WRITE setStyle NOTIFY styleNodeChanged)
    Q_PROPERTY(SpatialQuery* queryData READ queryData NOTIFY queryDataChanged)

    Q_INTERFACES(QQmlParserStatus)

public:

    enum Status {
        Invalid = 0,
        Empty = 1,
        Queued = 2,
        Fetching = 3,
        Processing = 4,
        Ready = 5
    };
    Q_ENUM(Status)
    enum GL_GeometryFlag {
        GLPoints        = 0x1,
        GLLines         = 0x2,
        GLTriangles     = 0x3,
        DrawingModeFlag = 0x7,
        ColoredVertices = 0x8,
        TexturedVertices = 0x9
    };
    Q_ENUM(GL_GeometryFlag)
    Q_DECLARE_FLAGS(GeometryFlags, GL_GeometryFlag)

    GeomBuffer(QObject *parent = 0) : QObject(parent) {}
    ~GeomBuffer();

    // QQmlParserStatus interface
    void classBegin() {}
    void componentComplete();

    GeoLayer *layer() const;
    QString filterClause() const;
    QString clause() const;
    inline void lock() { bufferLock.lock(); }
    inline void unlock() { bufferLock.unlock(); }

    bool highlight() const;
    void setHighlight(bool highlight);

    int entityCount() const;
    bool isValid() const;
    inline bool ready() const { return m_ready; }
    inline Status status() const { return m_status; }

    void setLayer(GeoLayer* layer);
    void setClause(QString clause);

    QSGNode *updateNode(QSGNode *last);

    Q_INVOKABLE QTransform combinedTransform(bool respect_triangulator = false) const;
    Q_INVOKABLE QByteArray getCache() const;
    Q_INVOKABLE bool loadCache(const QByteArray &blob);

    int completedEntityCount() const;

    Style style() const;
    void setStyle(Style style);

    void reset();
    QSGGeometryNode *getHightlightNode();

    SpatialQuery* queryData() const
    {
        return ds;
    }

public slots:
    void fetchGeometry(QStringList attributeFields = QStringList());
    QVariantList buildGraph() const;

private slots:
    void processReply();
    void reportBufferReady();
    void reportProgress(int count);
    void onLayerValidChanged();
    void reportBufferProgress(int c);

signals:
    void filterClauseChanged(QString arg);
    void entityCountChanged();
    void styleChanged();
    void isValidChanged();
    void clauseChanged(QString clause);
    void readyChanged(bool ready);
    void stylesChanged(QVariantList m_style);
    void statusChanged(Status status);
    void highlightChanged(bool highlight);
    void layerChanged(GeoLayer* layer);
    void completedEntityCountChanged(int completedEntityCount);
    void styleNodesChanged();
    void styleNodeChanged(StyleNode style);

    void queryDataChanged(SpatialQuery* queryData);

private:
    QString                 m_clause;               ///>
    GeoLayer*               m_layer = nullptr;      ///>
    bool                    m_ready = false;        ///> первичный вершинный буфер готов, можно создавать геометрию GL
    Status                  m_status = Invalid;     ///>
    Style                   m_style;                ///>
    SpatialQuery*           ds = nullptr;           ///>
    bool                    complete = false;
    QMutex                  bufferLock;
    QFutureWatcher<void>    job;
    QFuture<void>           future;
    bool                    m_highlight = false;
    QSGGeometryNode *       highlightNode = nullptr;
//    QSGNode *               groot = nullptr;
    QSGOpacityNode *        container = nullptr;
    int                     m_completedEntityCount = 0;

    void parseStyles();
    void setStatus(Status status);
    void setReady(bool ready);

    void applyDefaultStyle();
    QSGNode *createNode();
    QSGGeometryNode *getPointGeometry(QColor color, qreal size);
    QSGGeometryNode *getLinearGeometry(QColor color, qreal width);
    QSGGeometryNode *getFillGeometry(QColor color);

};



#endif // GEOMBUFFER_H
