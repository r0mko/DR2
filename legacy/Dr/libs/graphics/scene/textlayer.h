#ifndef TEXTLAYER_H
#define TEXTLAYER_H

#include <QQuickPaintedItem>
#include <guidepath.h>
#include <qrawfont.h>
class QAbstractItemModel;
class TextLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString polygonRole READ polygonRole WRITE setPolygonRole NOTIFY polygonRoleChanged)
    Q_PROPERTY(QString textRole READ textRole WRITE setTextRole NOTIFY textRoleChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(qreal minimumScale READ minimumScale WRITE setMinimumScale NOTIFY minimumScaleChanged)
    Q_PROPERTY(QTransform viewTransform READ viewTransform WRITE setViewTransform NOTIFY viewTransformChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)


    struct GuideLabel {
        QVector< quint32 >  glyphIndexList;
        QVector< QPointF >  advances;
        qreal               labelWidth = 0;
        GuidePath           path;
        QString             text;
    };

    QList<GuideLabel>   labels;
    QString             m_polygonRole;
    int                 i_polygonRole = -1;
    QString             m_textRole;
    int                 i_textRole = -1;
    QFont               m_font;
    QColor              m_color;
    QAbstractItemModel* m_model = nullptr;
    QRawFont m_rawFont { QRawFont::fromFont(QFont("Roboto", 12)) };
    QHash< quint32, QPainterPath > m_glyphPathCache;
    void readModel();
    void addItem(const QModelIndex &idx);
    bool checkModel();

    void paintLabel(QPainter* painter, const QTransform& tr, const QRectF &viewportRect, GuideLabel &s);
    qreal m_minimumScale = 0.2;
    QTransform m_viewTransform;
    QRectF m_viewportRect;

public:
    TextLayer(QQuickItem *parent = 0);

    QString polygonRole() const;

    QString textRole() const;

    QFont font() const;

    QColor color() const;

    QAbstractItemModel* model() const;

    void setPolygonRole(QString polygonRole);
    void setTextRole(QString textRole);
    void setFont(QFont font);
    void setColor(QColor color);
    void setModel(QAbstractItemModel* model);

    // QQuickPaintedItem interface
    void setMinimumScale(qreal minimumScale);

    void setViewTransform(QTransform viewTransform);

    void setViewportRect(QRectF viewportRect);


signals:
    void polygonRoleChanged(QString polygonRole);
    void textRoleChanged(QString textRole);
    void fontChanged(QFont font);
    void colorChanged(QColor color);
    void modelChanged(QAbstractItemModel* model);
    void minimumScaleChanged(qreal minimumScale);
    void viewTransformChanged(QTransform viewTransform);
    void viewportRectChanged(QRectF viewportRect);

private slots:
    void onAddItem(const QModelIndex &parent, int first, int last);
public:
void paint(QPainter *painter);
qreal minimumScale() const;
QTransform viewTransform() const;
QRectF viewportRect() const;
};

#endif // TEXTLAYER_H
