#ifndef HASHSTRING_H
#define HASHSTRING_H

#include <QString>
#include <map>
#include <qhash.h>
#define SEED 0xdeadbabe

class HashString
{
    struct strdata {
        strdata() {}
        strdata(QString str) : string(str) { ref.ref(); }

        QString string;
        QAtomicInt ref;
    };

    inline uint hash(const QString &str) const { return qHash(str, SEED); }
    static std::map<uint, strdata> table;
    uint d = 0;
public:
    HashString() {}
    ~HashString();
    HashString(const QString &string);
    HashString(uint raw);

    inline HashString(const HashString &other) : d(other.d) { table.at(d).ref.ref(); }
    inline HashString(HashString &&other) : d(other.d) {}
    inline HashString(const char *ch) : HashString(QString(ch)) {}

    inline HashString &operator=(HashString &&other) { qSwap(d, other.d); return *this; }
    inline HashString &operator=(const HashString &other) { return *this = HashString(other); }
    inline HashString &operator=(const QString &other) { return *this = HashString(other); }
    inline HashString &operator=(const char *ch) { *this = HashString(QString(ch)); return *this;  }

    inline bool operator==(const HashString &other) const { return d == other.d; }
    inline bool operator==(const QString &other) const { return d == hash(other); }
    inline bool operator==(uint other) const { return d == other; }

    inline bool operator!=(const HashString &other) const { return d != other.d; }
    inline bool operator!=(const QString &other) const { return d != hash(other); }
    inline bool operator!=(uint other) const { return d != other; }

    inline operator QString() const { if (isValid())return table.at(d).string; return QString(); }
    inline operator uint() const { return d; }

    inline uint hashValue() const { return d; }
    inline QString toString() const { return operator QString(); }
    inline bool isValid() const { return d > 0; }
};

Q_DECL_CONST_FUNCTION inline uint qHash(HashString key, uint seed = 0) Q_DECL_NOTHROW { return uint(key) ^ seed; }
#endif // HASHSTRING_H
