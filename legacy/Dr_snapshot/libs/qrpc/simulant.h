#ifndef SIMULANT_H
#define SIMULANT_H

#include <QObject>

class Simulant : public QObject
{
public:
    explicit Simulant(QObject *parent = 0);
    static void _staticMetacall(QObject *object, QMetaObject::Call call, int id, void **arguments);
    virtual int qt_metacall(QMetaObject::Call call, int id, void **arguments);
    const QMetaObject *metaObject() const;
    void setMetaobject(const QMetaObject *mo);
    void setProto(QObject *object);
private:
    QMetaObject m_metaobject;
};

#endif // SIMULANT_H
