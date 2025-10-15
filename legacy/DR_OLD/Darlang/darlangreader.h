#ifndef DARLANGREADER_H
#define DARLANGREADER_H
#include "darlang_p.h"
#include <QString>
#include <QVarLengthArray>
#include <QStringList>

class DarlangReader : protected Darlang
{
    QString sourceCode;
    QStringList tokens;
    int currentPos;
public:
    DarlangReader(const QString& code);
    void setSourceCode(const QString& code) { sourceCode = code; }
    bool parse();
    inline int nextToken();
protected:
    inline void reallocateStack();

    inline int &sym(int index)
    { return stack [tos + index - 1].ival; }

protected:
    int tos;
    int stack_size;

    struct StackItem {
        int state;
        int ival;
    };
    QVarLengthArray<StackItem> stack;
    unsigned in_tag: 1;
    QByteArray bytes;
    const char *bufptr;
};
#endif // DARLANGREADER_H
