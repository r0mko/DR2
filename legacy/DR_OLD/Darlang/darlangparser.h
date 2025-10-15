
#line 49 "darlang.g"

#ifndef DARLANGREADER_H
#define DARLANGREADER_H
#include "darlangparser_p.h"
#include <QString>
#include <QVarLengthArray>
#include <QStringList>
#include <QStringRef>
#include <QVariantMap>
#include <QSet>

typedef void(*FunctionPtr)(int&, void**);
typedef QString string;
typedef qint64 int64;
typedef quint64 uint64;
typedef quint8 byte;

Q_DECLARE_METATYPE(string)
Q_DECLARE_METATYPE(uint64)
Q_DECLARE_METATYPE(byte)
Q_DECLARE_METATYPE(float)


class DarlangFunction
{
    QString funcName;
    QString returnType;
    QStringList argTypes;
    QStringList argNames;
    FunctionPtr* ptr;
    void** argptr;
public:
    DarlangFunction() {}
    DarlangFunction(const QString& signature);
    ~DarlangFunction() {}
    const QString& name() const { return funcName; }
    QString signature() const;
};

Q_DECLARE_METATYPE(DarlangFunction)

class DarlangContext
{
    QStringList dataTypes;
    QList<DarlangFunction> functions;
    QVariantMap variables;
    QSet<QStringRef> funcnames;
public:
    DarlangContext() {}
    void allocateVariable(const QString& name, int typeId);
    
};

class DarlangCompiler : protected DarlangParser
{
    QString sourceCode;
    QStringList tokens;
    int currentPos;
    DarlangContext* context;

public:
    DarlangCompiler(const QString& code);
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

