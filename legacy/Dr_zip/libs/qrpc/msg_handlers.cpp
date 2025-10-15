#include "engine.h"
#include "netmessenger.h"
#include "private/qmetaobjectbuilder_p.h"
#include "rpcpeer.h"
#include <QMetaObject>
#include "puppet.h"
#include "QMetaObject"
#include "rpcattached.h"
#include "qmlpeer.h"
#include "private/qmetatype_p.h"
#include "datastream.h"

void RPCEngine::h_MetatypeList(Peer *peer, Message *message)
{
    GET_MESSAGE(MetaTypeList, message, b);
    RPCEngine *e = RPCEngine::engine();
    RPCPeer *p = e->m_peers.value(peer, nullptr);
    if (p)
        QMetaObject::invokeMethod(e, "setupTypeSystem", Qt::QueuedConnection, Q_ARG(Peer*, peer), Q_ARG(BlobMap, b->metaObjects), Q_ARG(TypeIdHash, b->types));
    else
        QMetaObject::invokeMethod(e, "waitForInitialize", Qt::QueuedConnection, Q_ARG(Peer*, peer), Q_ARG(BlobMap, b->metaObjects), Q_ARG(TypeIdHash, b->types));

}

void RPCEngine::h_RPCIntro(Peer *peer, Message *message)
{
    GET_MESSAGE(RPCIntro, message, intro);
    auto body = intro.take();
    QMetaObject::invokeMethod(RPCEngine::engine(), "constructPeer", Qt::QueuedConnection, Q_ARG(Peer*, peer), Q_ARG(RPCIntroMessage::Body*, body));
}

void RPCEngine::h_PeerRejected(Peer *peer, Message *message)
{
    GET_MESSAGE(PeerRejected, message, msg);
    //    RPCPeer *p = e->m_peers.value(peer, nullptr);
    //    if (p) {

    //    }


}

void RPCEngine::h_ObjectRegistered(Peer *peer, Message *message)
{
    GET_MESSAGE(ObjectRegistered, message, msg);
    RPCEngine *e = RPCEngine::engine();
    RPCPeer *p = e->m_peers.value(peer, nullptr);
    Q_ASSERT(p != nullptr);
    if (msg->isGadget)
        engine()->gadgetRegistered(p, msg->objectName, msg->className);
    else
        engine()->objectRegistered(p, msg->objectName, msg->className);
}

void RPCEngine::h_ObjectUnregistered(Peer *peer, Message *message)
{
    GET_MESSAGE(ObjectUnregistered, message, msg);
    RPCEngine *e = RPCEngine::engine();
    RPCPeer *p = e->m_peers.value(peer, nullptr);
    engine()->objectUnRegistered(p, msg->objectName);
    Q_ASSERT(p != nullptr);
    p->removePuppet(msg->objectName);
}

void RPCEngine::h_ReadProperty(Peer *peer, Message *message)
{
    GET_MESSAGE(ReadProperty, message, msg);
    //    RPCEngine *e = E_PTR(peer->ptr());
    QObject *object = reinterpret_cast<QObject*>(msg->objPtr);
    QMetaProperty p = object->metaObject()->property(msg->id);
    MSG_BODY(ReadPropertyReply) re;
    re.transactionId = msg->transactionId;
    re.arguments = msg->arguments;
    re.value = QVariantStreamer(p.read(object), RPCEngine::engine()->peers().value(peer));
    //    qDebug() << "property" << object->metaObject()->property(msg->id).name() << "of object" << object << ":" << re.value;
    peer->sendMessage(new ReadPropertyReplyMessage(re));
}

void RPCEngine::h_ReadPropertyReply(Peer *peer, Message *message)
{
    Q_UNUSED(peer);
    GET_MESSAGE(ReadPropertyReply, message, msg);
    //    RPCEngine *e = E_PTR(peer->ptr());
    void **a = (void**)msg->arguments;
//    QMetaType mt();
    QVariant value = msg->value.reconstruct();
//    qDebug() << "got property reply" << value;
    QMetaType::construct(value.userType(), a[0], value.data());
//    a[0] = value.data();
//    } else {
//    }
    Transaction *trn = reinterpret_cast<Transaction *>(msg->transactionId);
    trn->dataReady.wakeAll();
    //    qDebug() << "Reply for transaction" << (void*)msg->transactionId << "arrived in" << QDateTime::currentMSecsSinceEpoch() - trn->timestamp << "ms. Value:" << msg->value;
}

void RPCEngine::h_WriteProperty(Peer *peer, Message *message)
{
    Q_UNUSED(peer);
    GET_MESSAGE(WriteProperty, message, msg);
    emit RPCEngine::engine()->writeProperty(msg->objPtr, msg->propertyIndex, msg->value);
}

void RPCEngine::h_SignalConnected(Peer *peer, Message *message)
{
    GET_MESSAGE(SignalConnected, message, msg);
    RPCEngine *e = RPCEngine::engine();
    RPCPeer *p = e->m_peers.value(peer);
    Q_ASSERT(p != nullptr);
    QObject *sender = reinterpret_cast<QObject*>(msg->remotePtr);
    p->forwardSignal(sender, msg->signalIdx, msg->puppetPtr);
}

void RPCEngine::h_SignalDisconnected(Peer *peer, Message *message)
{
    GET_MESSAGE(SignalConnected, message, msg);
    RPCEngine *e = RPCEngine::engine();
    RPCPeer *p = e->m_peers.value(peer);
    Q_ASSERT(p != nullptr); // TODO: crash here!
    QObject *sender = reinterpret_cast<QObject*>(msg->remotePtr);
    p->removeSignalForwarding(sender, msg->signalIdx, msg->puppetPtr);
}

void RPCEngine::h_NotifierConnected(Peer *peer, Message *message)
{
    GET_MESSAGE(NotifierConnected, message, msg);
    RPCEngine *e = RPCEngine::engine();
    RPCPeer *p = e->m_peers.value(peer);
    Q_ASSERT(p != nullptr);
    QObject *sender = reinterpret_cast<QObject*>(msg->remotePtr);
    p->forwardSignal(sender, msg->signalIdx, msg->puppetPtr, msg->propertyIndex);
}

void RPCEngine::h_EmitSignal(Peer *peer, Message *message)
{
    Q_UNUSED(peer)
    GET_MESSAGE(EmitSignal, message, msg);
    emit RPCEngine::engine()->emitPuppetSignal(msg->puppetPtr, msg->signalIdx, msg->arguments);
}

void RPCEngine::h_EmitNotifier(Peer *peer, Message *message)
{
    Q_UNUSED(peer)
    GET_MESSAGE(EmitNotifier, message, msg);
    emit RPCEngine::engine()->emitPuppetNotifier(msg->puppetPtr, msg->signalIdx, msg->propertyIndex, msg->propertyValue.reconstruct());
}

void RPCEngine::h_InvokeMetaMethod(Peer *peer, Message *message)
{
    GET_MESSAGE(InvokeMetaMethod, message, msg);
    QObject *obj = reinterpret_cast<QObject*>(msg->remotePtr);
    QVector<void*> args;
    args.resize(1);
    for (QVariant arg:msg->arguments) {
        args.append(arg.data());
    }
    QMetaMethod m = obj->metaObject()->method(msg->methodIndex);
    QVariant ret;
    if (m.returnType() != QMetaType::Void) {
        ret = QVariant(m.returnType(), args.data()[0]);
    }
    //    qDebug() << "Invoke method" << m.methodSignature();
    switch (m.parameterCount()) {
    case 0:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()));
        break;
    case 1:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData())
                 );
        break;
    case 2:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData())
                 );
        break;
    case 3:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData()),
                 QGenericArgument(m.parameterTypes().at(2).constData(), msg->arguments.at(2).constData())
                 );
        break;
    case 4:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData()),
                 QGenericArgument(m.parameterTypes().at(2).constData(), msg->arguments.at(2).constData()),
                 QGenericArgument(m.parameterTypes().at(3).constData(), msg->arguments.at(3).constData())
                 );
        break;
    case 5:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData()),
                 QGenericArgument(m.parameterTypes().at(2).constData(), msg->arguments.at(2).constData()),
                 QGenericArgument(m.parameterTypes().at(3).constData(), msg->arguments.at(3).constData()),
                 QGenericArgument(m.parameterTypes().at(4).constData(), msg->arguments.at(4).constData())
                 );
        break;
    case 6:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData()),
                 QGenericArgument(m.parameterTypes().at(2).constData(), msg->arguments.at(2).constData()),
                 QGenericArgument(m.parameterTypes().at(3).constData(), msg->arguments.at(3).constData()),
                 QGenericArgument(m.parameterTypes().at(4).constData(), msg->arguments.at(4).constData()),
                 QGenericArgument(m.parameterTypes().at(5).constData(), msg->arguments.at(5).constData())
                 );
        break;
    case 7:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData()),
                 QGenericArgument(m.parameterTypes().at(2).constData(), msg->arguments.at(2).constData()),
                 QGenericArgument(m.parameterTypes().at(3).constData(), msg->arguments.at(3).constData()),
                 QGenericArgument(m.parameterTypes().at(4).constData(), msg->arguments.at(4).constData()),
                 QGenericArgument(m.parameterTypes().at(5).constData(), msg->arguments.at(5).constData()),
                 QGenericArgument(m.parameterTypes().at(6).constData(), msg->arguments.at(6).constData())
                 );
        break;
    case 8:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData()),
                 QGenericArgument(m.parameterTypes().at(2).constData(), msg->arguments.at(2).constData()),
                 QGenericArgument(m.parameterTypes().at(3).constData(), msg->arguments.at(3).constData()),
                 QGenericArgument(m.parameterTypes().at(4).constData(), msg->arguments.at(4).constData()),
                 QGenericArgument(m.parameterTypes().at(5).constData(), msg->arguments.at(5).constData()),
                 QGenericArgument(m.parameterTypes().at(6).constData(), msg->arguments.at(6).constData()),
                 QGenericArgument(m.parameterTypes().at(7).constData(), msg->arguments.at(7).constData())
                 );
        break;
    case 9:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData()),
                 QGenericArgument(m.parameterTypes().at(2).constData(), msg->arguments.at(2).constData()),
                 QGenericArgument(m.parameterTypes().at(3).constData(), msg->arguments.at(3).constData()),
                 QGenericArgument(m.parameterTypes().at(4).constData(), msg->arguments.at(4).constData()),
                 QGenericArgument(m.parameterTypes().at(5).constData(), msg->arguments.at(5).constData()),
                 QGenericArgument(m.parameterTypes().at(6).constData(), msg->arguments.at(6).constData()),
                 QGenericArgument(m.parameterTypes().at(7).constData(), msg->arguments.at(7).constData()),
                 QGenericArgument(m.parameterTypes().at(8).constData(), msg->arguments.at(8).constData())
                 );
        break;
    case 10:
        m.invoke(obj, Qt::DirectConnection, QGenericReturnArgument(QMetaType::typeName(m.returnType()), ret.data()),
                 QGenericArgument(m.parameterTypes().at(0).constData(), msg->arguments.at(0).constData()),
                 QGenericArgument(m.parameterTypes().at(1).constData(), msg->arguments.at(1).constData()),
                 QGenericArgument(m.parameterTypes().at(2).constData(), msg->arguments.at(2).constData()),
                 QGenericArgument(m.parameterTypes().at(3).constData(), msg->arguments.at(3).constData()),
                 QGenericArgument(m.parameterTypes().at(4).constData(), msg->arguments.at(4).constData()),
                 QGenericArgument(m.parameterTypes().at(5).constData(), msg->arguments.at(5).constData()),
                 QGenericArgument(m.parameterTypes().at(6).constData(), msg->arguments.at(6).constData()),
                 QGenericArgument(m.parameterTypes().at(7).constData(), msg->arguments.at(7).constData()),
                 QGenericArgument(m.parameterTypes().at(8).constData(), msg->arguments.at(8).constData()),
                 QGenericArgument(m.parameterTypes().at(9).constData(), msg->arguments.at(9).constData())
                 );
        break;
    default:
        break;

    }
    if (m.methodType() != QMetaMethod::Signal) {
        MSG_BODY(InvokeMetaMethodReply) q;
        q.value = ret;
        q.argumentsPtr = msg->argumentsPtr;
        q.transactionId = msg->transactionId;
        //    qDebug() << "Called method" << q.value;
        peer->sendMessage(new InvokeMetaMethodReplyMessage(q));
    }
}

void RPCEngine::h_InvokeMetaMethodReply(Peer *peer, Message *message)
{
    Q_UNUSED(peer);
    GET_MESSAGE(InvokeMetaMethodReply, message, msg);
    void **a = (void**)msg->argumentsPtr;
    QMetaType mt(msg->value.userType());
    mt.construct(a[0], msg->value.data());
    qDebug() << "TRN" << (void*)msg->transactionId << "reply type" << QMetaType::typeName(msg->value.userType());
    Transaction *trn = reinterpret_cast<Transaction *>(msg->transactionId);
    trn->dataReady.wakeAll();
}

void RPCEngine::h_QueryObject(Peer *peer, Message *message)
{
    GET_MESSAGE(QueryObject, message, msg);
    QObject *o = engine()->m_registeredObjects.value(msg->objectName);
    MSG_BODY(QueryObjectReply) reply;
    if (o != nullptr) {
        reply = prepareObjectMessage(o);
    } else {
        reply.objPtr = 0;
    }
    reply.transactionId = msg->transactionId;
    peer->sendMessage(new QueryObjectReplyMessage(reply));

}

void RPCEngine::h_QueryObjectReply(Peer *peer, Message *message)
{
    GET_MESSAGE(QueryObjectReply, message, msg);
    qDebug() << "puppet name" << msg->objectName;
    QObject *puppet = nullptr;
    if (msg->objPtr != 0) {
        RPCPeer *p = engine()->m_peers.value(peer, nullptr);
        QMetaObject *mo = nullptr;
        while (!msg->metaObjects.isEmpty()) {
            QByteArray moData = msg->metaObjects.takeLast();
            QDataStream in(moData);
            QMetaObjectBuilder bld;
            bld.deserialize(in, engine()->m_metaobjects);
            //        bld.setStaticMetacallFunction(&f_staticMetacall);
            mo = bld.toMetaObject();
            engine()->importMetaObject(bld.className(), mo);
        }

        puppet = p->addPuppet(msg->objectName, mo, msg->objPtr);
    }
    Transaction *trn = reinterpret_cast<Transaction *>(msg->transactionId);

    if (trn != nullptr) {
        trn->data = QVariant((quint64)puppet);
        trn->dataReady.wakeAll();
    }
}

void RPCEngine::f_staticMetacall(QObject *object, QMetaObject::Call call, int id, void **arguments)
{
    Q_UNUSED(arguments)
    qDebug() << "static_metacall for" << object << "call" << call << "id" << id;
}
