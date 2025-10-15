#include "mapview.h"
#include "maplayer.h"
#include "spatialitedb.h"
#include "style.h"
//#include "../../../libs/Graphics/scene.h"


QTransform MapView::transform() const
{
    return m_transform;
}

QPainterPath MapView::blobToPath(const QByteArray &ba) const
{
    if (ba.isEmpty())
        return QPainterPath();
    QScopedPointer<Geometry> g(new Geometry);
    g->parseBlob(ba);
    return g->getPath( m_transform );
}

QQuickItem *MapView::scene() const
{
    return m_scene;
}

void MapView::setViewReady(bool viewReady)
{
    if (m_viewReady == viewReady)
        return;

    m_viewReady = viewReady;
    emit viewReadyChanged(viewReady);
}

void MapView::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    switch (change) {
    case QQuickItem::ItemChildAddedChange: {
        MapLayer *layer = qobject_cast<MapLayer*>(value.item);
        if (layer)
            layer->setView(this);
        break;
    }
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
    default:
        break;
    }
    QQuickItem::itemChange(change, value);
}

void MapView::setMapTransform(QTransform arg)
{
    if (m_transform == arg)
        return;

    m_transform = arg;
    emit mapTransformChanged(arg);
}

MapView::MapView(QQuickItem *parent) :
    QQuickItem(parent),
    m_database(nullptr)
{

}

QRectF MapView::sourceRect() const
{
    return m_sourceRect;
}

SpatiaLiteDB *MapView::database() const
{
    return m_database;
}

void MapView::setDatabase(SpatiaLiteDB *arg)
{
    qDebug() <<" set db" << arg;
    if (m_database == arg)
        return;
    if (m_database)
        m_database->disconnect(0, this, 0);
    m_database = arg;
    emit databaseChanged(arg);
    //    qDebug() << "Setting db" << arg << arg->isOpen();
    connect(m_database, SIGNAL(openChanged(bool)), this, SLOT(getExtent()));
    if (m_database->isOpen())
        getExtent();
}

uint MapView::targetSRID() const
{
    return m_targetSRID;
}

void MapView::getExtent()
{
    setSourceRect(m_database->extent(m_targetSRID));
    qDebug() << "got source rect" << m_sourceRect << m_transform;
    setViewReady(true);
}

void MapView::setSourceRect(QRectF arg)
{
    if (m_sourceRect == arg)
        return;

    m_sourceRect = arg;
    QSizeF targetSize = QSizeF(width(), height());
    QSizeF sourceSize = m_sourceRect.size();
    qreal s = qMin(targetSize.width() / sourceSize.width(), targetSize.height() / sourceSize.height());
    m_transform.reset();
    m_transform.scale(s, -s);
    m_transform.translate(-m_sourceRect.x(), -m_sourceRect.y() - m_sourceRect.height());
    //    qDebug() << "Scaling factor" << s << "\nTranspose" << m_sourceRect.topLeft() << "\nTransform" << m_transform;
    emit sourceRectChanged(arg);
    emit setMapTransform(m_transform);
}

void MapView::setTargetSRID(uint arg)
{
    if (m_targetSRID == arg)
        return;

    m_targetSRID = arg;

    emit targetSRIDChanged(arg);
    if (m_database && m_database->isOpen())
        getExtent();
}


void MapView::blobToPath(QVariant blob, QObject *path)
{
    QByteArray ba = blob.toByteArray();
    if (ba.isEmpty())
        return;

    QScopedPointer<Geometry> g(new Geometry);
    g->parseBlob(ba);

    const QMetaObject *mo = path->metaObject();
    //    qDebug() << "path" << mo->className() << "curves size" << g->getPath(m_transform) << qstrcmp(mo->className(), "GraphicsPath");
    if (!qstrcmp(mo->className(), "GraphicsPath")) {
        mo->invokeMethod(path, "setPath", Q_ARG(QPainterPath, g->getPath(m_transform)));
    }
}

void MapView::blobToGraphModel(QVariant blob, QObject *model)
{
    QByteArray ba = blob.toByteArray();
    if (ba.isEmpty())
        return;

    QScopedPointer<Geometry> g(new Geometry);
    g->parseBlob(ba);

    const QMetaObject *mo = model->metaObject();
    while (mo) {
        qDebug() << "class name" << mo->className();
        mo = mo->superClass();
    }
}

//void MapView::save(QString filename)
//{
//    QFile f( filename );
//    f.open( QIODevice::Truncate | QIODevice::WriteOnly );
//    QDataStream ds( &f );
//    m_database->save( ds );
//    ds << m_transform;
//    ds << m_sourceRect;
//    ds << m_targetSRID;
//    ds << children().size();
//    for( int i = 0; i < children().size(); ++i ){
//        MapLayer* ml = qobject_cast<MapLayer*>( children().at( i ) );
//        if( ml != nullptr ) {
//            ds << QByteArray ("MapLayer*");
//            ml->save( ds );
//        }else{
//            qDebug() << "!!!!!";
//            // TODO: save other objects aswell
////            QObject* x = qobject_cast< QObject* >( children().at( i ) );
////            qDebug() << x->metaObject()->className();
////            ds << QByteArray( x->metaObject()->className() );
////            QVariantMap var;
////            for( int j = 0; j < x->metaObject()->propertyCount(); ++j){
////                if( x->metaObject()->property( j ).isStored( x ) ){
////                    var.insert( QByteArray( x->metaObject()->property( j ).name() ), x->metaObject()->property( j ).read( x ) );
////                }
////            }
////            ds << var;
////            qDebug() << var;
//        }
//    }
//    f.close();
//}

//void MapView::load(QString filename)
//{
//    qDebug() << qmlEngine( this );

//    QFile f( filename );
//    f.open( QIODevice::ReadOnly );
//    QDataStream ds( &f );

//    SpatiaLiteDB* db = new SpatiaLiteDB(this);
//    db->load( ds );
//    setDatabase( db );

//    ds >> m_transform;

//    QRectF sr;
//    ds >> sr;
//    setSourceRect( sr );

//    uint tsrid;
//    ds >> tsrid;
//    setTargetSRID(tsrid);


//    int size;
//    ds >> size;
//    for( int i = 0; i < size; ++i ){
//        // load children
//        QByteArray cn;
//        ds >> cn;

//        if( cn == QByteArray( "MapLayer*" ) ){
//            MapLayer* ml = new MapLayer( this );
//            ml->setParentItem( this );
//            ml->setView( this );
//            ml->load( ds );
//        }
//    }
//    f.close();
//    emit viewReady();
//}
