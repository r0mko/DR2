#include "darlangreader.h"

DarlangReader::DarlangReader(const QString &code):
    sourceCode(code),
    currentPos(0)
{
    QRegExp rx("(([A-Za-z]+|\\(|\\)|,|\"|\\d|\\.|\\;))");
    int pos = 0;
    while ((pos = rx.indexIn(code, pos)) != -1) {
        tokens << rx.cap(1);
        pos += rx.matchedLength();
    }
}

int DarlangReader::nextToken()
{
    int pos = currentPos++;

    if(tokens.at(pos).size() == "1")
    {
        switch(tokens.at(pos).at(0).toAscii())
        {
        case '(':
            return LPAREN;
            break;
        case ')':
            return RPAREN;
            break;
        case ',':
            return COMMA;
            break;
        case '"':
            return QUOTE;
            break;
        case ';':
            return SEMICOLON;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return DIGIT;
            break;
        case '.':
            return DOT;
        }
    }
    if(tokens.at(pos) == "var")
    {
        return VAR;
    }
}

void DarlangReader::reallocateStack()
{
    if (! stack_size)
        stack_size = 128;
    else
        stack_size <<= 1;
    stack.resize (stack_size);
}
