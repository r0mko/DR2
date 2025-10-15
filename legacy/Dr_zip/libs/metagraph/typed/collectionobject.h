#ifndef MODELWRAPPEROBJECT_H
#define MODELWRAPPEROBJECT_H
#include <dynamicobjectextension.h>

class GraphModel;
//class Instance;
class Element;

class ElementAgentBase: public QObject // all elements
{
    Q_OBJECT

public:
    ElementAgentBase( QObject* parent= nullptr): QObject( parent ) {}
    virtual ~ElementAgentBase() {}
};

class ElementAgent : public DynamicObjectExtension<ElementAgentBase>
{
    Element *d = nullptr;

public:
    ElementAgent(QObject *parent = 0);
    ElementAgent(const QMetaObject *metaObject, Element *node, QObject *parent = 0);
    ~ElementAgent();

protected:
    void setProperty(int index, QVariant value);
    QVariant getProperty(int index) const;
    QVariant callMethod(int index, QVariantList args);

    // DynamicObjectExtension interface
public:
};

Q_DECLARE_METATYPE(ElementAgent*)

#endif // MODELWRAPPEROBJECT_H
