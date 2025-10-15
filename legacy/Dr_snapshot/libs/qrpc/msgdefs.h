#ifndef MSGDEFS_H
#define MSGDEFS_H
#include "datastream.h"
#include "message.h"

enum RPCMessageType
{
    RPCIntro = 0x10,
    PeerRejected = 0x16,
    MetaTypeList = 0x11,
    ServiceRegistered = 0x12,
    ServiceUnregistered = 0x13,
    ObjectRegistered = 0x14,
    ObjectUnregistered = 0x15,
    ReadProperty = 0x20,
    ReadPropertyReply = 0x21,
    WriteProperty = 0x22,
    ResetProperty = 0x23,
    SignalConnected = 0x24,
    SignalDisconnected = 0x25,
    NotifierConnected = 0x26,
    InvokeMetaMethod = 0x28,
    InvokeMetaMethodReply = 0x29,
    EmitSignal = 0x2A,
    EmitNotifier = 0x2B,
    QueryObject = 0x2C,
    QueryObjectReply = 0x2D,
    ListObjects = 0x2D,
    ListObjectsReply = 0x2F
};


class MESSAGE_CLASS(RPCIntro)
{
    QString appName;
    QString appVersion;
    QString nodeName;
    QList<QString> services;
    STREAM_FN(appName, appVersion, nodeName, services)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(PeerRejected)
{
    QString messageText;
    int reasonCode;
    STREAM_FN(messageText, reasonCode)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(MetaTypeList)
{
    QMap<QByteArray, QByteArray> metaObjects;
    QHash<int, QByteArray> types;
    STREAM_FN(metaObjects, types)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(ServiceRegistered)
{
    QString serviceName;
    QByteArray className;
    STREAM_FN(serviceName, className)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(ServiceUnregistered)
{
    QString serviceName;
    STREAM_FN(serviceName)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(ObjectRegistered)
{
    QString objectName;
    QString className;
    bool isGadget;
    //    QList<QByteArray> metaObjects;
    //    quint64 objPtr;
    STREAM_FN(objectName, className, isGadget)
};
END_MESSAGE_DEF


struct RemoteObjectInfo {
    QString objectName;
    QString className;
    bool isGadget;
    QList<QString> properties;
    QList<QString> methods;
};
Q_DECLARE_METATYPE(RemoteObjectInfo)

inline QDataStream &operator <<(QDataStream &out, const RemoteObjectInfo &v) {
    out << v.objectName
        << v.className
        << v.isGadget
        << v.properties
        << v.methods;
    return out;
}

inline QDataStream &operator >>(QDataStream &in, RemoteObjectInfo &v) {
    in  >> v.objectName
        >> v.className
        >> v.isGadget
        >> v.properties
        >> v.methods;
    return in;
}


class MESSAGE_CLASS(ListObjects)
{
    quint64 transactionId;
    bool excludeQObjects;
    bool excludeGadgets;
    //    quint64 objPtr;
    STREAM_FN(transactionId, excludeQObjects, excludeGadgets)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(ListObjectsReply)
{
    quint64 transactionId;
    QVariantList objectsInfo; // QList<RemoteObjectInfo>

    STREAM_FN(transactionId, objectsInfo)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(ObjectUnregistered)
{
    QString objectName;
    STREAM_FN(objectName)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(ReadProperty)
{
    quint64 objPtr;
    int id;
    quint64 transactionId;
    quint64 arguments;
    STREAM_FN(objPtr, id, transactionId, arguments)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(ReadPropertyReply)
{
    quint64 transactionId;
    quint64 arguments;
    QVariantStreamer value;
    STREAM_FN(transactionId, arguments, value)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(WriteProperty)
{
    quint64 objPtr;
    int propertyIndex;
    QVariantStreamer value;
    STREAM_FN(objPtr, propertyIndex, value)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(SignalConnected)
{
    quint64 remotePtr;
    int signalIdx;
    quint64 puppetPtr;
    STREAM_FN(remotePtr, signalIdx, puppetPtr)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(NotifierConnected)
{
    quint64 remotePtr;
    int signalIdx;
    int propertyIndex;
    quint64 puppetPtr;
    STREAM_FN(remotePtr, signalIdx, propertyIndex, puppetPtr)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(InvokeMetaMethod)
{
    quint64 remotePtr;
    int methodIndex;
    quint64 transactionId;
    quint64 argumentsPtr;
    QList<QVariant> arguments;
    STREAM_FN(remotePtr, methodIndex, transactionId, argumentsPtr, arguments)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(InvokeMetaMethodReply)
{
    quint64 transactionId;
    quint64 argumentsPtr;
    QVariant value;
    STREAM_FN(transactionId, argumentsPtr, value)
};
END_MESSAGE_DEF


class MESSAGE_CLASS(SignalDisconnected)
{
    quint64 remotePtr;
    int signalIdx;
    quint64 puppetPtr;
    STREAM_FN(remotePtr, signalIdx, puppetPtr)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(EmitSignal)
{
    quint64 puppetPtr;
    int signalIdx;
    QVariantList arguments;
    STREAM_FN(puppetPtr, signalIdx, arguments)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(EmitNotifier)
{
    quint64 puppetPtr;
    int signalIdx;
    int propertyIndex;
    QVariantStreamer propertyValue;

    STREAM_FN(puppetPtr, signalIdx, propertyIndex, propertyValue)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(QueryObject)
{
    QString objectName;
    int flags;
    quint64 transactionId;
    STREAM_FN(objectName, flags, transactionId)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(QueryObjectReply)
{
    QList<QByteArray> metaObjects;
    QString objectName;
    quint64 objPtr;
    quint64 transactionId;
    STREAM_FN(objectName, metaObjects, objPtr, transactionId)
};
END_MESSAGE_DEF

#endif // MSGDEFS_H
