#ifndef GUIDENET_H
#define GUIDENET_H

#include "guide.h"

#include <QGraphicsItem>
#include <QObject>
#include <QQmlListProperty>
#include <QQmlParserStatus>
#include <gpen.h>
#include <graphicsobject.h>
#include <scene.h>
#define DECLARE_DATA(Class) inline Class ## Data *dfunc() const { return static_cast<Class ## Data*>(d.data()); }
#define DECLARE_NETVERTEX(Class) DECLARE_DATA(Class) friend uint qHash(const Class &, uint);

class GuideNet;

//class Way

class NetSegment
{
    Q_GADGET
    Q_PROPERTY(qreal entryPos READ entryPos WRITE setEntryPos)
    Q_PROPERTY(qreal exitPos READ exitPos WRITE setExitPos)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)
    Q_PROPERTY(Guide path READ path WRITE setPath)
    Q_PROPERTY(Guide result READ result)
    Q_PROPERTY(Guide part READ part)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(uint uid READ uid)
    Q_PROPERTY(qreal length READ length)
    Q_PROPERTY(bool isNull READ isNull)
    Q_PROPERTY(GuideNet* net READ net CONSTANT)

    class Data : public QSharedData
    {
    public:
        GuideNet *          net = nullptr;
        qreal               entryPos = 0;
        qreal               exitPos = 0;
        qreal               offset = 0;
        uint                uid = 0;
        QString             name;
        Guide           path;
        Guide           result;
        Guide           part;
    };

    friend class GuideNet;

    QExplicitlySharedDataPointer<Data> d;

public:
    NetSegment(GuideNet *net = 0) : d(new Data) { d->net = net; }

    qreal entryPos() const;
    qreal exitPos() const;
    qreal offset() const;
    Guide path() const;
    Guide part() const;
    Guide result() const;
    bool isNull() const;
    qreal length() const;
    GuideNet* net() const;
    uint uid() const;
    QString name() const;

    void updatePart();
    void updateOffset(qreal scale);
    void setEntryPos(qreal entryPos);
    void setExitPos(qreal exitPos);
    void setOffset(qreal offset);
    void setPath(Guide path);
    void setName(QString name);

    void merge(NetSegment other);

    inline operator QVariant() const { return QVariant::fromValue(*this); }
    inline bool operator == (const NetSegment &other) const { return other.d == d; }
    inline bool operator != (const NetSegment &other) const { return other.d != d;  }
};

class NetVertex
{
    Q_GADGET
    Q_ENUMS(ElementType)
    Q_PROPERTY(int type READ type CONSTANT)
    Q_PROPERTY(GuideNet* net READ net)

    friend class GuideNet;
    friend class JointVertex;
    friend class StartVertex;
    friend class BoundaryVertex;
    friend class EndVertex;
    friend class JunctionVertex;
    friend class ConvergenceVertex;
    friend uint qHash(const NetVertex &, uint);

public:
    enum ElementType {
        InvalidType,
        BoundaryType,
        StartType,
        EndType,
        JunctionType,
        ConvergenceType,
        SeparatorType,
        JointType
    };
    Q_ENUM(ElementType)
    NetVertex() : d(new NetVertexData) {}
    virtual ~NetVertex() {}

    int type() const { return d->type; }
    inline GuideNet* net() const { return d->net; }

    inline operator QVariant() const { return QVariant::fromValue(*this); }
    bool operator==(const NetVertex &other) const { return d == other.d; }
    bool operator!=(const NetVertex &other) const { return d != other.d; }

protected:
    class NetVertexData : public QSharedData
    {
    public:
        virtual ~NetVertexData(){}
        int         type = InvalidType;
        GuideNet*   net = nullptr;
    };
    NetVertex(NetVertexData *other, ElementType type) : d(other) { d->type = type; }
    NetVertex(NetVertexData *other) : d(other) { }
    QExplicitlySharedDataPointer<NetVertexData> d;
};

uint qHash(const NetVertex &key, uint seed);

class BoundaryVertex : public NetVertex
{
    Q_GADGET
    Q_PROPERTY(NetSegment to READ to WRITE setTo)
    Q_PROPERTY(NetSegment from READ from WRITE setFrom)
    Q_PROPERTY(bool isStart READ isStart WRITE setIsStart)
    Q_PROPERTY(qreal gap READ gap WRITE setGap)
    Q_PROPERTY(QPointF point READ point)

    class BoundaryVertexData : public NetVertexData
    {
    public:
        NetSegment to;
        NetSegment from;
        QPointF point;
        qreal gap = 0;
        bool isStart = false;

        void updatePoint();

    };

    DECLARE_NETVERTEX(BoundaryVertex)
    friend class GuideNet;



public:
    BoundaryVertex() : NetVertex(new BoundaryVertexData, BoundaryType) {}
    static BoundaryVertex fromVertex(NetVertex vertex);

    NetSegment from() const;
    void setFrom(NetSegment from);

    NetSegment to() const;
    void setTo(NetSegment to);

    bool isStart() const;
    void setIsStart(bool isStart);

    qreal gap() const;
    void setGap(qreal gap);

    QPointF point() const;

    Guide::Link link();
    Runner runner(const Guide::Link &linker) const;

    inline operator QVariant() const { return QVariant::fromValue(*this); }

    void update();

public slots:


protected:
    using NetVertex::NetVertex;

    // NetVertex interface
};
uint qHash(const BoundaryVertex &key, uint seed);

class StartVertex : public NetVertex
{
    Q_GADGET
    Q_PROPERTY(NetSegment nextSegment READ nextSegment WRITE setNextSegment)
    Q_PROPERTY(NetSegment prevSegment READ prevSegment WRITE setPrevSegment)

    class StartVertexData : public NetVertexData
    {
    public:
        NetSegment nextSegment;
        NetSegment prevSegment;
    };
    DECLARE_DATA(StartVertex)
    friend class GuideNet;

public:
    StartVertex() : NetVertex(new StartVertexData, StartType) {}

    NetSegment nextSegment() const;
    NetSegment prevSegment() const;
    Runner runner() const;

    void setNextSegment(NetSegment nextSegment);
    void setPrevSegment(NetSegment prevSegment);
    static StartVertex fromVertex(NetVertex vertex);
    inline operator QVariant() const { return QVariant::fromValue(*this); }
protected:
    using NetVertex::NetVertex;

};

class EndVertex : public NetVertex
{
    Q_GADGET
    Q_PROPERTY(NetSegment nextSegment READ nextSegment WRITE setNextSegment)
    Q_PROPERTY(NetSegment prevSegment READ prevSegment WRITE setPrevSegment)
//    Q_PROPERTY(QPointF point READ point)
    class EndVertexData : public NetVertexData
    {
    public:
        NetSegment nextSegment;
        NetSegment prevSegment;
    };
    DECLARE_DATA(EndVertex)
    friend class GuideNet;

public:
    EndVertex() : NetVertex(new EndVertexData, EndType) {}

    NetSegment nextSegment() const;
    NetSegment prevSegment() const;

    void setNextSegment(NetSegment nextSegment);
    void setPrevSegment(NetSegment prevSegment);
    inline operator QVariant() const { return QVariant::fromValue(*this); }
    static EndVertex fromVertex(NetVertex vertex);
protected:
    using NetVertex::NetVertex;
};

class JointVertex : public NetVertex
{
    Q_GADGET
    Q_PROPERTY(NetSegment to READ to WRITE setTo)
    Q_PROPERTY(NetSegment from READ from WRITE setFrom)
//    Q_PROPERTY(VectorNode link READ link)
    Q_PROPERTY(QPointF point READ point)


    class JointVertexData : public NetVertexData
    {
    public:
        NetSegment      to;
        NetSegment      from;
    };

    DECLARE_DATA(JointVertex)
    friend class GuideNet;


public:
    JointVertex() : NetVertex(new JointVertexData, JointType) {}
    NetSegment to() const;
    NetSegment from() const;
//    VectorNode link() const;

    void setTo(NetSegment to);
    void setFrom(NetSegment from);
    Guide::Link link();

    virtual QPointF point() const;


    static JointVertex fromVertex(NetVertex vertex);
    operator QVariant() const;
protected:
    using NetVertex::NetVertex;
};

class JunctionVertex : public NetVertex
{
    Q_GADGET

    Q_PROPERTY(QList<NetSegment> to READ to)
    Q_PROPERTY(NetSegment from READ from WRITE setFrom)

    class JunctionVertexData : public NetVertexData
    {
    public:
        QList<NetSegment> to;
        NetSegment from;
        Guide::Link link;
    };

    DECLARE_DATA(JunctionVertex)
    friend class GuideNet;

public:
    JunctionVertex();

    QList<NetSegment> to() const;
    NetSegment from() const;
    void switchTo(int index);

    Q_INVOKABLE void setFrom(NetSegment from);
    Q_INVOKABLE void appendOutSegment(NetSegment seg);

    static JunctionVertex fromVertex(NetVertex vertex);
    inline operator QVariant() const { return QVariant::fromValue(*this); }

protected:
    using NetVertex::NetVertex;
};

class ConvergenceVertex : public NetVertex
{
    Q_GADGET
    Q_PROPERTY(NetSegment to READ to WRITE setTo)
    Q_PROPERTY(QList<NetSegment> from READ from )
    class ConvergenceVertexData : public NetVertexData
    {
    public:
        NetSegment to;
        QList<NetSegment> from;
    };
    DECLARE_DATA(ConvergenceVertex)
    friend class GuideNet;

public:
    ConvergenceVertex() : NetVertex(new ConvergenceVertexData, ConvergenceType) {}
    NetSegment to() const;
    QList<NetSegment> from() const;
    Q_INVOKABLE void setTo(NetSegment to);
    Q_INVOKABLE void appendInSegment(NetSegment seg);

    static ConvergenceVertex fromVertex(NetVertex vertex);
    inline operator QVariant() const { return QVariant::fromValue(*this); }
protected:
    using NetVertex::NetVertex;
};

Q_DECLARE_METATYPE(NetVertex)
Q_DECLARE_METATYPE(NetSegment)
Q_DECLARE_METATYPE(JointVertex)
Q_DECLARE_METATYPE(ConvergenceVertex)
Q_DECLARE_METATYPE(JunctionVertex)
Q_DECLARE_METATYPE(StartVertex)
Q_DECLARE_METATYPE(EndVertex)


#endif // GUIDENET_H
