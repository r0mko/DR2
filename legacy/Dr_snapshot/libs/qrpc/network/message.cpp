#include "message.h"
#include <QDebug>

QAtomicInt Message::count = 0;

void Message::save(QDataStream &out) const
{
    out << MAGIC << m_id << m_type << m_checksum << m_body;
}

Message *Message::loadFromStream(QDataStream &in)
{
    QScopedPointer<Message> ret(new Message(MessageType::Invalid));
    if (ret->loadHeader(in) && ret->loadBody(in))
        return ret.take();
//    qDebug() << in.status();
    return 0;
}

Message::Message(int type)
    : m_type(type),
      m_ok(false)
{}

Message::Message(const Message &other)
    : m_id(other.m_id),
      m_type(other.m_type),
      m_checksum(other.m_checksum),
      m_body(QByteArray::fromRawData(other.m_body.constData(), other.m_body.size()))
{}

bool Message::loadHeader(QDataStream &stream)
{
    if (m_ok)
        return true;
    uint magic = 0;
    while (magic != MAGIC && stream.status() == QDataStream::Ok) {
        stream >> magic;
    }
    if (stream.status() != QDataStream::Ok) {
        qDebug() << "Magic token not found";
        return false;
    }
    int dataSize;
    while((uint)stream.device()->bytesAvailable() < (sizeof(uint) + sizeof(int) + sizeof(quint16) + sizeof(int))) {
        if (!stream.device()->waitForReadyRead(READ_TIMEOUT)) {
            stream.setStatus(QDataStream::ReadCorruptData);
            qDebug() << "Message read timeout!";
            return false;
        }
    }
    stream >> m_id;
    stream >> m_type;
    stream >> m_checksum;
    stream >> dataSize;
    m_body.resize(dataSize);
    return true;
}

bool Message::loadBody(QDataStream &stream)
{
    if (m_ok)
        return true;
    int pos = 0;
    while (pos < m_body.size()) {
        int next = qMin((qint64)(m_body.size() - pos), stream.device()->bytesAvailable());
        if (stream.readRawData(m_body.data() + pos, next) != next) {
            stream.setStatus(QDataStream::ReadPastEnd);
            qDebug() << "Message read error! Data size" << m_body.size() << "pos" << pos;
            return false;
        }
        pos += next;
        if (pos < m_body.size() && !stream.device()->waitForReadyRead(READ_TIMEOUT)) {
            stream.setStatus(QDataStream::ReadCorruptData);
            qDebug() << "Message read timeout! Data size" << m_body.size() << "pos" << pos;
            return false;
        }
    }
    if (qChecksum(m_body.data(), m_body.size()) != m_checksum) {
        stream.setStatus(QDataStream::ReadCorruptData);
        qDebug() << "Checksum error. Body size:" << m_body.size() << "Checksum" << qChecksum(m_body.constData(), m_body.size()) << "received checksum:" << m_checksum << "Message id:" << m_id;
        return false;
    }
    m_ok = true;
//    if (stream.status() != QDataStream::Ok) {
//        qDebug() << "Status error" << stream.status() << stream.device()->errorString();
//    }
    return true;
}
