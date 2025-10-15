#ifndef MESSAGE_H
#define MESSAGE_H
#include "defines.h"
#include <QAtomicInt>
#include <QByteArray>
#include <QDataStream>
#include <QScopedPointer>
#include <QStringList>
#include <QVariant>
#include <QDateTime>

inline void serialize(QDataStream &) {}

template<typename Arg1, typename ...Args>
inline void serialize(QDataStream &s, const Arg1& arg1, const Args&... args) { s << arg1; serialize(s, args...); }

inline void deserialize(QDataStream &) {}

template<typename Arg1, typename ...Args>
inline void deserialize(QDataStream &s, Arg1& arg1, Args&... args) { s >> arg1; deserialize(s, args...); }

#define STREAM_FN(...)                                                      \
    QT_BEGIN_NAMESPACE                                                      \
    inline void save(QDataStream &s) const { serialize(s, __VA_ARGS__); }   \
    inline void load(QDataStream &s) { deserialize(s, __VA_ARGS__); }       \
    QT_END_NAMESPACE

#define DECLARE_STREAM_OPERATORS(x)                                         \
    QT_BEGIN_NAMESPACE                                                      \
    inline QDataStream &operator <<(QDataStream &s, const x::Body &d)       \
{ d.save(s); return s; }                                                \
    inline QDataStream &operator >>(QDataStream &s, x::Body &d)             \
{ d.load(s); return s; }                                                \
    QT_END_NAMESPACE                                                        \

#define _DR_MESSAGE_DEF(x) x ## Message : public Message
#define _DR_MESSAGE_CTORS(x)                                                    \
    struct Body;                                                            \
    x ## Message() : Message(x) {}                             \
    x ## Message(Body data) : Message(data, x) {}
#define GET_MESSAGE(msgtype, msgptr, variable) QScopedPointer<msgtype ## Message::Body> variable(msgptr->get<msgtype ## Message::Body>())
#define MSG_BODY(Type) Type ## Message::Body

#define MESSAGE_CLASS(TypeName)                                     \
_DR_MESSAGE_DEF(TypeName)                                                 \
{                                                                       \
    public:                                                             \
    _DR_MESSAGE_CTORS(TypeName)                                             \
    struct Body
#define END_MESSAGE_DEF };
enum MessageType
{
    Invalid = 0xffff,
    Handshake = 1,
    SetUserData = 2,
    NegativeAck = 3
};

class Message
{
    static QAtomicInt count;
public:

    Message() : m_type((int)Invalid) {}
    Message(const Message &other);
    void save(QDataStream &out) const;
    /**
     * @brief Load message from stream
     * @param in - datastream opened for reading
     * @return Returns message pointer or 0 if any error occurs. This function does not take ownership of returned pointer.
     */
    static Message *loadFromStream(QDataStream &in);

    template <class T>
    /**
     * @brief Extracts message body
     * @return Returns newly constructed message body.
     */
    T *get() const
    {
        if (!m_ok)
            return 0;
        T *ret = new T();
        QDataStream in(m_body);
        ret->load(in);
        return ret;
    }
    inline const QByteArray &body() const { return m_body; }
    /**
     * @brief Message size
     * @return returns message size in bytes including header and checksum
     */
    inline int size() const { return m_body.size() + sizeof(m_checksum) + sizeof(MAGIC) + sizeof(uint); }
    /**
     * @brief Message type
     * @return Returns message type identifier.
     */
    int type() const { return m_type; }
    inline bool isOk() const { return m_ok; }
    /**
     * @brief Unique identifier
     * @return Returns unique identifier of this message on sender's side
     */
    inline uint id() const { return m_id; }

protected:
    Message(int type);

    template <class T>
    Message(const T &t, int m_type)
        : m_id(count++),
          m_type(m_type),
          m_ok(true)
    {
        setBody(t);
    }

    template <class T>
    void setBody(const T &body)
    {
        QDataStream str(&m_body, QIODevice::WriteOnly);
        body.save(str);
        m_checksum = qChecksum(m_body.constData(), m_body.size());
    }
    bool loadHeader(QDataStream &stream);
    bool loadBody(QDataStream &stream);

private:
    uint m_id;
    int m_type;
    quint16 m_checksum;
    QByteArray m_body;
    bool m_ok;

};

class MESSAGE_CLASS(Handshake)
{
    int protocolVersion;
    STREAM_FN(protocolVersion)
};
END_MESSAGE_DEF



class MESSAGE_CLASS(SetUserData)
{
    QVariant messageData;
    STREAM_FN(messageData)
};
END_MESSAGE_DEF

class MESSAGE_CLASS(NegativeAck)
{
    int unrecognizedMessageType;
    QDateTime timestamp;
    STREAM_FN(unrecognizedMessageType, timestamp)
};
END_MESSAGE_DEF

#endif // MESSAGE_H
