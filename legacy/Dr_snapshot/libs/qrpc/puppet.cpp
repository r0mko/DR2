#include "puppet.h"
#include <QDebug>
#include <QMetaMethod>
#include "rpcpeer.h"
#include "msgdefs.h"
#include "engine.h"
#include <QThread>

qint64 now()
{
    return QDateTime::currentMSecsSinceEpoch();
}

void Puppet::connectNotify(const QMetaMethod &signal)
{
//    qDebug() << "receiver connected to signal" << signal.methodSignature() << "receivers count" << receivers(signal.methodSignature().prepend("1"));
    for (int i = 0; i < metaObject()->propertyCount(); ++i) {
        QMetaProperty prop = metaObject()->property(i);
        if (prop.notifySignal() == signal) {
//            qDebug() << signal.methodSignature() << signal.methodIndex() << "is notify signal for property" << prop.name() << i ;
            forwardPropertyNotifier(signal.methodIndex(), i);
            return;
        }
    }
    forwardSignal(signal.methodIndex());
}

void Puppet::disconnectNotify(const QMetaMethod &signal)
{
    if (receivers(signal.methodSignature().prepend("1")) == 0) {
        qDebug() << "forwarding of signal" << signal.methodSignature() << "is no longer needed";
        cancelSignalForwarding(signal.methodIndex());
    }
}

RPCPeer *Puppet::rpcPeer() const
{
    return m_rpcPeer;
}

void Puppet::setRpcPeer(RPCPeer *peer)
{
    m_rpcPeer = peer;
}


Puppet::Puppet(QMetaObject *metaObject, RPCPeer *peer, quint64 objPtr) :
    m_rpcPeer(peer),
    m_ptr(objPtr)
{
    mo = *metaObject;
    mo.d.static_metacall = puppetStaticMetacall;
    m_propertyCache.resize(metaObject->propertyCount());
    for (int i = 0; i < m_propertyCache.size(); ++i) {
        m_propertyCache[i].type = mo.property(i).userType();
    }
}

void Puppet::emitSignal(int index, void **arguments)
{
//    qDebug() << "Emitting signal id" << index << mo.method(index).methodSignature() << "in thread" << QThread::currentThread();
    const QMetaObject *m = &mo;
    int loc_id = index - m->methodOffset();
    while (loc_id < 0) {
        m = m->superClass();
        loc_id = index - m->methodOffset();
    }
    QMetaObject::activate(this, m, loc_id, arguments);
}

int Puppet::qt_metacall(QMetaObject::Call call, int id, void **arguments)
{
//    qDebug() << "qt_metacall" << (int)call << id << arguments;
    switch (call) {
    case QMetaObject::ReadProperty: {
        CachedProperty &prop = m_propertyCache[id];
//        qDebug() << "Reading remote property" << this->metaObject()->property(id).name() << this->metaObject()->property(id).typeName();
        if (now() - prop.lastQuery > 10) {
            ReadPropertyMessage::Body q;
            q.id = id;
            q.objPtr = m_ptr;
            q.arguments = (quint64)arguments;
            QScopedPointer<Transaction> trn(new Transaction);
            q.transactionId = (quint64)trn.data();
            m_rpcPeer->netPeer()->sendMessage(new ReadPropertyMessage(q));
            if(!trn->waitForDataReady())
                qDebug() << "Transaction failed";
            QVariant pval(this->metaObject()->property(id).type(), arguments[0]);
            prop.lastQuery = now();
            prop.value = pval;
//            qDebug() << "property was read from remote object" << pval;
        } else {
//            qDebug() << "returning cached property";
            QMetaType::construct(prop.value.userType(), arguments[0], prop.value.data());
        }
        break;
    }
    case QMetaObject::WriteProperty: {
        MSG_BODY(WriteProperty) q;
        q.propertyIndex = id;
        q.objPtr = m_ptr;
        QVariant val(m_propertyCache[id].type, arguments[0]);
        q.value = QVariantStreamer(val, m_rpcPeer);
//        qDebug() << "Writing property" << metaObject()->property(id).name() << "at" << (void*)m_ptr << "value" << val;
        m_rpcPeer->netPeer()->sendMessage(new WritePropertyMessage(q));
        break;
    }
    case QMetaObject::InvokeMetaMethod: {
        MSG_BODY(InvokeMetaMethod) q;
        q.argumentsPtr = (quint64)arguments;
        q.methodIndex = id;
        q.remotePtr = m_ptr;
        QMetaMethod m = mo.method(id);
        QScopedPointer<Transaction> trn;
        if (m.methodType() != QMetaMethod::Signal) {
            trn.reset(new Transaction);
            q.transactionId = (quint64)trn.data();
        } else {
//            qDebug() << "Emitting a signal via direct call. No transaction needed";
            q.transactionId = 0;
        }
        for (int i = 0; i < m.parameterCount(); ++i) {
            q.arguments.append(QVariant(m.parameterType(i), arguments[i + 1]));
        }
        m_rpcPeer->netPeer()->sendMessage(new InvokeMetaMethodMessage(q));
//        qDebug() << "Invoking method" << m.methodSignature() << "at" << (void*)m_ptr << "with params" << q.arguments;
        if (!trn.isNull() && !trn->waitForDataReady())
            qDebug() << "Transaction" << trn.data() << "failed";
        break;
    }
    default:
        qDebug() << "unable to handle" << call;
        break;
    }
    return -1;
}

const QMetaObject *Puppet::metaObject() const
{
    return &mo;
}

void Puppet::cancelSignalForwarding(int signalIndex)
{
    MSG_BODY(SignalDisconnected) q;
    q.puppetPtr = (quint64)this;
    q.remotePtr = m_ptr;
    q.signalIdx = signalIndex;
    m_rpcPeer->netPeer()->sendMessage(new SignalDisconnectedMessage(q));
}

void Puppet::forwardSignal(int signalIndex)
{
    MSG_BODY(SignalConnected) q;
    q.puppetPtr = (quint64)this;
    q.remotePtr = m_ptr;
    q.signalIdx = signalIndex;
    m_rpcPeer->netPeer()->sendMessage(new SignalConnectedMessage(q));
}

void Puppet::forwardPropertyNotifier(int signalIndex, int propertyIndex)
{
    MSG_BODY(NotifierConnected) q;
    q.puppetPtr = (quint64)this;
    q.remotePtr = m_ptr;
    q.signalIdx = signalIndex;
    q.propertyIndex = propertyIndex;
    m_rpcPeer->netPeer()->sendMessage(new NotifierConnectedMessage(q));
}

Puppet::~Puppet()
{
    emitSignal(0, (void**)this);
}

void Puppet::puppetStaticMetacall(QObject *object, QMetaObject::Call call, int id, void **arguments)
{
    Q_UNUSED(arguments)
    if (call == QMetaObject::InvokeMetaMethod) {
        qDebug() << "static: invoke method" << id << object;
    }
}
