
#line 39 "darlang.g"


#ifndef DARLANGREADER_H
#define DARLANGREADER_H
#include "darlang_p.h"
#include <QString>
#include <QVarLengthArray>
#include <QStringList>
#include <QVariantMap>

typedef QString string;
typedef qint64 int64;
typedef quint64 uint64;
typedef quint8 byte;

Q_DECLARE_METATYPE(string)
Q_DECLARE_METATYPE(uint64)
Q_DECLARE_METATYPE(byte)
Q_DECLARE_METATYPE(float)

class DarlangReader : protected Darlang
{
    QString sourceCode;
    QStringList tokens;
    int currentPos;
    struct
public:
    DarlangReader(const QString& code);
    void setSourceCode(const QString& code) { sourceCode = code; }

    bool parse();
    QVariantMap variables() const { return _variables; }
    QStringList unresolvedIdentifiers() const { return unresolvedIds; }
    inline int nextToken();

protected:
    inline void reallocateStack();
    inline QVariant &sym(int index) { return stack [tos + index - 1].ival; }
    int tos;
    int stack_size;
    struct StackItem {
        int state;
        QVariant ival;
    };
    QVarLengthArray<StackItem> stack;
    unsigned in_tag: 1;
    QVariantList args;
    QVariantMap _variables;
    QVariantMap _context;
    QStringList unresolvedIds;

private:
    QVariant sumVariants(const QVariant& arg1, const QVariant& arg2);   
    QVariant subVariants(const QVariant &arg1, const QVariant &arg2);
};

#endif // DARLANGREADER_H


