#ifndef POLYGONITEM_H
#define POLYGONITEM_H
#include "visualinstance.h"

#include <QAbstractListModel>
#include "shapeinstance.h"

class PolygonFactory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractListModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QPolygonF polygon READ polygon WRITE setPolygon NOTIFY polygonChanged)
    Q_PROPERTY(QString pointRole READ pointRole WRITE setPointRole NOTIFY pointRoleChanged)
    Q_PROPERTY(QString xRole READ xRole WRITE setXRole NOTIFY xRoleChanged)
    Q_PROPERTY(QString yRole READ yRole WRITE setYRole NOTIFY yRoleChanged)

    QAbstractListModel* m_model = nullptr;
    QPolygonF m_polygon;
    QString m_pointRole;
    QString m_xRole;
    QString m_yRole;

    int i_p = -1;
    int i_x = -1;
    int i_y = -1;

public:
    PolygonFactory (QObject *parent = nullptr);
    QAbstractListModel* model() const;
    QPolygonF polygon() const;
    QString pointRole() const;
    QString xRole() const;
    QString yRole() const;

    void setModel(QAbstractListModel* model);
    void setPolygon(QPolygonF polygon);
    void setPointRole(QString pointRole);
    void setXRole(QString xRole);
    void setYRole(QString yRole);

public slots:
    void buildPolygon();

private slots:
    void updatePolygon(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    bool bindRoles();
signals:
    void modelChanged(QAbstractListModel* model);
    void polygonChanged(QPolygonF polygon);
    void pointRoleChanged(QString pointRole);
    void xRoleChanged(QString xRole);
    void yRoleChanged(QString yRole);
};

class PolygonItem : public ShapeInstance
{
    GR_INSTANCE(PolygonItem)
    QPolygonF polygon;
    QRectF m_boundingRect;
public:
    using ShapeInstance::ShapeInstance;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
    QPainterPath shape() const;
    QRectF boundingRect() const;
    int type() const;

};


class PolygonItemHandle : public ShapeInstanceHandle
{
    Q_GADGET
    Q_PROPERTY(QPolygonF polygon READ polygon WRITE setPolygon)
    GR_HANDLE(PolygonItem)

public:
    using ShapeInstanceHandle::ShapeInstanceHandle;

    QPolygonF polygon() const;
    void setPolygon(QPolygonF polygon);

};


DECLARE_ITEM(PolygonItem)
#endif // POLYGONITEM_H

