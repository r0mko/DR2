#include "geolayer.h"
#include "geoview.h"

#include <QPainter>

QStringList GeoView::styleTypes = QStringList();

GeoView::GeoView(QQuickItem *parent) :
    QQuickItem(parent)
{
}

int GeoView::targetSRID() const
{
    return m_targetSRID;
}

QRectF GeoView::viewExtent() const
{
    return m_viewExtent;
}

bool GeoView::isValid() const
{
    return m_isValid;
}

QTransform GeoView::viewTransform() const
{
    return m_viewTransform;
}

void GeoView::componentComplete()
{
    QQuickItem::componentComplete();
    if(!m_isValid)
        initView();
}

void GeoView::setTargetSRID(int targetSRID)
{
    if (m_targetSRID == targetSRID)
        return;
    m_targetSRID = targetSRID;
    emit targetSRIDChanged(targetSRID);
    initView();
}

QVariantList GeoView::getStyleProperties(QString styleType) const
{
    QVariantList ret;
    int tid = QMetaType::type(styleType.toLocal8Bit());
    if (tid == QMetaType::UnknownType) {
        qWarning() << "Unknown style" << styleType;
        return ret;
    }
    const QMetaObject *mo = QMetaType::metaObjectForType(tid);
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
        QVariantMap p;
        QMetaProperty mp = mo->property(i);
        p["type"] = QString(mp.typeName());
        p["name"] = QString(mp.name());
        ret.append(p);
    }
    return ret;
}

void GeoView::setContentWidth(qreal contentWidth)
{
    if (m_contentWidth == contentWidth)
        return;

    m_contentWidth = contentWidth;
    emit contentWidthChanged(contentWidth);
    if (m_contentHeight > 0)
        adjustTransform();
}

void GeoView::setContentHeight(qreal contentHeight)
{
    if (m_contentHeight == contentHeight)
        return;

    m_contentHeight = contentHeight;
    emit contentHeightChanged(contentHeight);
    if (m_contentWidth > 0)
        adjustTransform();
}

void GeoView::setViewExtent(QRectF viewExtent)
{
    if (m_viewExtent == viewExtent)
        return;

    m_viewExtent = viewExtent;
    emit viewExtentChanged(viewExtent);
    adjustTransform();
}

void GeoView::adjustTransform()
{
    if (m_viewExtent.isEmpty())
        return;
    QTransform newTr;
    QPolygonF src = QPolygonF() << m_viewExtent.bottomLeft() << m_viewExtent.bottomRight() << m_viewExtent.topRight() << m_viewExtent.topLeft();

    QRectF targetRect = boundingRect();
    if (m_contentHeight > 0 && m_contentWidth> 0)
        targetRect.setSize(QSizeF(m_contentWidth, m_contentHeight));
    QSizeF dstSize = m_viewExtent.size().scaled(targetRect.size(), Qt::KeepAspectRatio);
    QRectF dstRect = boundingRect();
    dstRect.setSize(dstSize);
    QPolygonF dst(dstRect);
    dst.removeLast();
//    const QRectF r = boundingRect();
//    dst << r.topLeft() << r.topRight() << r.bottomRight() << r.bottomLeft();
    if (!QTransform::quadToQuad(src, dst, newTr)) {
        qWarning() << "Illegal geometry!" << src << dst;
        return;
    }
    if (m_viewTransform == newTr)
        return;
    m_viewTransform = newTr;
//    qDebug() << "View layer count:" << m_layers.size()
//             << "\nscale: x =" << newTr.m11() << ", y =" << newTr.m22()
//             << "\ntranslate: dx =" << newTr.m31() << ", dy =" << newTr.m32();
    emit viewTransformChanged();
}

void GeoView::fitLayers()
{
    QRectF extent;
    for (GeoLayer *layer : m_layers) {
        if (layer->isValid()) {
            extent = extent.united(layer->extent());
            disconnect(layer, &GeoLayer::isValidChanged, this, &GeoView::fitLayers);
        } else {
            connect(layer, &GeoLayer::isValidChanged, this, &GeoView::fitLayers);
        }
    }
    setViewExtent(extent);
}

void GeoView::addLayer(GeoLayer *layer)
{
    if (m_layers.contains(layer))
        return;
    m_layers.append(layer);
    layer->setParentItem(this);
    layer->setView(this);
    connect(layer, &GeoLayer::extentChanged, this, &GeoView::fitLayers);
    fitLayers();
}

void GeoView::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    switch (change) {
    case QQuickItem::ItemChildAddedChange: {
        GeoLayer *layer = qobject_cast<GeoLayer*>(value.item);
        if (layer) {
            addLayer(layer);
        }
        break;
    }
        /*
    case QQuickItem::ItemParentHasChanged:{
        QQuickItem* o = parentItem();
        while( o != nullptr ){
            // TODO: !!! dont forget about scene classname if it changes
            // FIXME: try to fix it somehow

            if(o->inherits("Scene")) {
                m_scene = o;
                qDebug() << "Found scene" << o;
                break;
            }
            o = o->parentItem();
        }
        break;
    }
*/
    default:
        break;
    }
    QQuickItem::itemChange(change, value);
}

void GeoView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    qDebug() << "GeoView geometry changed" << newGeometry << oldGeometry;
    if (!newGeometry.isEmpty()) {
        adjustTransform();
        update();
    }

    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void GeoView::setValid(bool isValid)
{
    if (m_isValid == isValid)
        return;
    m_isValid = isValid;
    emit isValidChanged();
}

void GeoView::initView()
{
    if (m_targetSRID < 0 || m_layers.isEmpty())
        return;
    fitLayers();
    setValid(true);
}

