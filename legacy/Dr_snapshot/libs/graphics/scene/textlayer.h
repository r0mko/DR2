#ifndef TEXTLAYER_H
#define TEXTLAYER_H

#include <QQuickPaintedItem>
#include <QTimer>
#include <guide.h>
#include <qrawfont.h>
#include <tools/bsp.h>

class QAbstractItemModel;
class TextLayer : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QTransform viewTransform READ viewTransform WRITE setViewTransform NOTIFY viewTransformChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(int depth READ depth WRITE setDepth NOTIFY depthChanged)
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool outline READ outline WRITE setOutline NOTIFY outlineChanged)
    Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor NOTIFY outlineColorChanged)

    struct GuideLabel {
        QVector< quint32 >  glyphIndexList;
        QVector< QPointF >  advances;
        qreal               labelWidth = 0;
        Guide           path;
        QString             text;
        int                 priority;
        inline bool compare(const GuideLabel &right) const {
            return text == right.text && path == right.path;
        }
    };

    struct LablesData {
        QHash< quint32, QPainterPath >  glyphPathCache;
        QRawFont                        rawFont{QRawFont::fromFont(QFont("Arial"))};
        QList<GuideLabel>               labels;
        qreal                           ascent;
        qreal                           descent;
        qreal                           maxCharWidth;
    };

    struct LabelPosition {
        bool success{false};
        qreal pos{0};
        bool reversed{false};
        QPolygonF mesh;
    };


    QFont                           m_font{QFont("Arial")};
    QColor                          m_color{Qt::black};
    QTransform                      m_viewTransform;
    QRectF                          m_viewportRect;
    qreal                           m_spacing{50};
    bool                            m_outline{false};
    QColor                          m_outlineColor{Qt::white};
    QMap< int, LablesData >         m_labels;
    BspTree<GuideLabel>             m_bsp;
    QTimer                          m_delayPaint;

    inline bool checkPoint(const QPointF &p);
    void paintLabel(QPainter* painter, const GuideLabel &s, qreal invScaleX, qreal invScaleY, const LablesData &labelData);
    LabelPosition findSuitableLabelPosition(Runner & peekRunner, const GuideLabel &s, qreal invScaleX, qreal invScaleY, const LablesData &labelData);
    void bspReset();
    void fontUpdate();



public:
    TextLayer(QQuickItem *parent = 0);

    void paint(QPainter *painter);


    QFont font() const;
    QColor color() const;
    QTransform viewTransform() const;
    QRectF viewportRect() const;
    int depth() const;
    qreal spacing() const;
    int count() const;
    bool outline() const;
    QColor outlineColor() const;

signals:
    void fontChanged(QFont font);
    void colorChanged(QColor color);
    void viewTransformChanged(QTransform viewTransform);
    void viewportRectChanged(QRectF viewportRect);
    void depthChanged(int depth);
    void spacingChanged(qreal spacing);
    void countChanged(int count);
    void outlineChanged(bool outline);
    void outlineColorChanged(QColor outlineColor);
    void vanished();
    void emerged();

public slots:
    void appendLabel(const QPolygonF& path, const QString &text, int priority);
    void appendLabel(const Guide& path, const QString &text, int priority);
    void clear();
    void repaint();



    void setFont(QFont font);
    void setColor(QColor color);
    void setViewTransform(QTransform viewTransform);
    void setViewportRect(QRectF viewportRect);
    void setDepth(int depth);
    void setSpacing(qreal spacing);
    void setOutline(bool outline);
    void setOutlineColor(QColor outlineColor);
};


bool TextLayer::checkPoint(const QPointF &p) {
    auto i = m_bsp.itemsAt(p);
    if( i.isEmpty() ) return true;
    return false;
}




#endif // TEXTLAYER_H
