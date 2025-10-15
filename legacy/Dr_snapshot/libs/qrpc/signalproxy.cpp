#include "signalproxy.h"
#include <QMetaMethod>
#include "rpcpeer.h"
#include "msgdefs.h"

int SignalProxy::qt_metacall(QMetaObject::Call call, int id, void **arguments)
{
    id = QObject::qt_metacall(call, id, arguments);
    if (id < 0 || call != QMetaObject::InvokeMetaMethod)
        return id;
    Q_ASSERT(id < m_virtualSlots.size());
    const VirtualSlot &slot = m_virtualSlots[id];

    QVariantList parameters;
    for (int i = 0; i < slot.parameterTypes.count(); ++i) {
        void *parameter = arguments[i + 1];
        parameters.append(QVariant(slot.parameterTypes[i], parameter));
    }
    if (slot.propertyIndex == -1) {
//        qDebug() << "Forwarding signal" << slot.signalIdx << "to puppet" << (void*)slot.puppetPtr << "with args" << parameters;
        MSG_BODY(EmitSignal) q;
        q.puppetPtr = slot.puppetPtr;
        q.signalIdx = slot.signalIdx;
        q.arguments = parameters;
        rpcPeer()->netPeer()->sendMessage(new EmitSignalMessage(q));
    } else {
        MSG_BODY(EmitNotifier) q;
        q.puppetPtr = slot.puppetPtr;
        q.signalIdx = slot.signalIdx;
        q.propertyIndex = slot.propertyIndex;
        q.propertyValue = QVariantStreamer(slot.object->metaObject()->property(slot.propertyIndex).read(slot.object), m_peer);
//        qDebug() << "Forwarding notifier" << slot.signalIdx << "to puppet" << (void*)slot.puppetPtr << "property" << q.propertyIndex << "value" << q.propertyValue;
//        QMetaObject::invokeMethod()
        rpcPeer()->netPeer()->sendMessage(new EmitNotifierMessage(q));
    }

    return -1;
}

SignalProxy::SignalProxy(RPCPeer *peer) :
    QObject(peer),
    m_peer(peer)
{
    RPCPeer *p = qobject_cast<RPCPeer*>(peer);
    if (!p)
        qFatal("SignalProxy must be the child of RPCPeer");
}

SignalProxy::~SignalProxy()
{
    clearMappings();
}

int SignalProxy::addMapping(QObject *sender, int senderSigId, quint64 puppetPtr, int propertyIndex)
{
    if (senderSigId < 0) {
        return -1;
    }
    QVarLengthArray<int, 10> parameterTypes;
    QMetaMethod signalMethod = sender->metaObject()->method(senderSigId);
    for (int i = 0; i < signalMethod.parameterCount(); ++i)
        parameterTypes.append(signalMethod.parameterType(i));
    int slotIdx = -1;
    for (int i = 0; i < m_virtualSlots.size(); ++i) {
        if (m_virtualSlots[i].isEmpty == true) {
            slotIdx = i;
            break;
        }
    }
    bool addEntry = false;
    if (slotIdx == -1) {
        addEntry = true;
        slotIdx = m_virtualSlots.size();
    }
    if (!QMetaObject::connect(sender, senderSigId, this, slotIdx + metaObject()->methodCount())) {
        qWarning() << "Can't connect signal to virtual slot";
        return -1;
    }
    if (addEntry) {
        VirtualSlot slot;
        slot.isEmpty = false;
        slot.object = sender;
        slot.parameterTypes = parameterTypes;
        slot.puppetPtr = puppetPtr;
        slot.signalIdx = senderSigId;
        slot.propertyIndex = propertyIndex;
        m_virtualSlots.append(slot);
    } else {
        VirtualSlot &slot = m_virtualSlots[slotIdx];
        slot.isEmpty = false;
        slot.object = sender;
        slot.signalIdx = senderSigId;
        slot.parameterTypes = parameterTypes;
        slot.puppetPtr = puppetPtr;
        slot.propertyIndex = propertyIndex;
    }
//    qDebug() << "signal" << signalMethod.methodSignature() << "mapped to puppet" << (void*)puppetPtr;
    return slotIdx;
}

void SignalProxy::removeMapping(int id)
{
    Q_ASSERT(id < m_virtualSlots.size());
    VirtualSlot &slot = m_virtualSlots[id];
    if (slot.isEmpty)
        return;
    if (QMetaObject::disconnect(slot.object, slot.signalIdx, this, id))
        slot.isEmpty = true;
    else
        qWarning() << "Can't not disconnect signal of" << slot.object << slot.object->metaObject()->method(slot.signalIdx).name();

}

void SignalProxy::clearMappings()
{
    for (int i = 0; i < m_virtualSlots.size(); ++i) {
        VirtualSlot &slot = m_virtualSlots[i];
        QMetaObject::disconnect(slot.object, slot.signalIdx, this, i);
    }
    m_virtualSlots.clear();

}


