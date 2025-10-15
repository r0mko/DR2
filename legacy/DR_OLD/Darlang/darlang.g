%decl darlangparser.h
%parser DarlangParser
%impl darlang_parser.cpp

%token TypeINT "int"
%token TypeUINT "uint"
%token TypeINT64 "int64"
%token TypeUINT64 "uint64"
%token TypeDOUBLE "double"
%token TypeFLOAT "float"
%token TypeSTRING "string"
%token TypeBYTE "byte"
%token TypeBOOL "bool"
%token TypeOBJECT "object"
%token TypeVOID "void"
%token T_FALSE "false"
%token T_TRUE "true"
%token T_PLUS "+"
%token T_PLUSPLUS "++"
%token T_MINUS "-"
%token T_MINUSMINUS "--"
%token T_STAR "*"
%token T_DIV "/"
%token T_LPAREN "("
%token T_RPAREN ")"
%token T_LCURLYBRACE "{"
%token T_RCURLYBRACE "}"
%token T_LBRACKET "["
%token T_RBRACKET "]"
%token T_QUOTE "\""
%token T_COMMA ","
%token T_DOT "."
%token T_SEMICOLON ";"
%token T_EQ "="
%token T_MORE ">"
%token T_LESS "<"
%token T_MORE_EQ ">="
%token T_LESS_EQ "<="
%token T_AND "&"
%token T_ANDAND "&&"
%token T_OR "|"
%token T_OROR "||"
%token T_IDENTIFIER
%token T_STRING_LITERAL
%token T_NUMBER_INTEGER
%token T_NUMBER_REAL
%start Darcode

/:
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

:/


/.

#include "darlangparser_p.h"
#include "darlangparser.h"
#include <QDebug>

QString DarlangFunction::signature() const
{
    return QString("%1 %2(%3)").arg(returnType).arg(funcName).arg(argTypes.join(", "));
}

DarlangFunction::DarlangFunction(const QString &signature)
{
    QRegExp r("(\\w+)\\s+(\\w+)\\s?\\((.+)\\)");
    if(r.indexIn(signature) != -1) {
        QStringList s = r.capturedTexts();
        returnType = s.at(1);
        funcName = s.at(2);
        argTypes = s.at(3).split(",");
    }

}

QString escapeString(const QString& str)
{
    QString reserved = ".^$*+?()[]{}\\|";
    QString out;
    for(int i=0; i<str.size(); ++i)
    {
        QChar c = str.at(i);
        if(reserved.indexOf(c) != -1)
            out.append("\\").append(c);
        else
            out.append(c);
    }
    return out;
}

// ----------------------------------------------------------------------------
bool isNumber(const QString& str)
{
    bool ok = false;
    str.toDouble(&ok);
    return ok;
}

// ----------------------------------------------------------------------------
DarlangCompiler::DarlangCompiler(const QString &code):
    sourceCode(code),
    currentPos(0),
    tos(0),
    stack_size(256)
{
    // qRegisterMetaType<float>("float");
    // qRegisterMetaType<byte>("byte");
    // qRegisterMetaType<int64>("int64");
    // qRegisterMetaType<uint64>("uint64");
    // qRegisterMetaType<string>("string");
    int i=0;
    QStringList regexpTokens;
    while(spell[i++]) {
        QString t = spell[i];
        if(!t.isEmpty())
            regexpTokens << escapeString(t);
    }
    
    QString regexp = regexpTokens.join("|");
//    regexp.append("|\\b[A-Za-z_]+\\b|\".+\"|[-]?[0-9]*\\.?[0-9]+");
    regexp.append("|(?!\")\\b[A-Za-z_]+\\b(?!\")|\".+\"|-?[0-9]+\\.?[0-9]*");
    QRegExp rx;
    rx.setPattern(QString("(%1)").arg(regexp));
    
    QStringList prog = code.split("\n");
    QString lastToken;
    foreach(QString s, prog) {
        int pos = 0;
        while ((pos = rx.indexIn(s, pos)) != -1) {
            if(isNumber(lastToken) && isNumber(rx.cap(1)) && rx.cap(1).startsWith("-"))
            {
                tokens << "-";
                tokens << rx.cap(1).right(rx.cap(1).size()-1);
            } else
            {
                tokens << rx.cap(1);
            }
            lastToken = rx.cap(1);
//            qDebug() << rx.capturedTexts();
            pos += rx.matchedLength();
        }
    }
}

// ----------------------------------------------------------------------------
int DarlangCompiler::nextToken()
{
    int pos = currentPos++;
    QString token;
    if(pos<tokens.size())
        token = tokens.at(pos);
    else
        return 0;
//   qDebug() << token;
    int i=0;
    while(spell[i++]) {
        if(token.toAscii() == spell[i])
            return i;
    }
    if(token.startsWith("\"") && token.endsWith("\""))
        return T_STRING_LITERAL;
    QRegExp rx("[-]?[0-9]*\\.?[0-9]+");
    if(rx.indexIn(token) != -1) {
        if(token.contains("."))
            return T_NUMBER_REAL;
        else
            return T_NUMBER_INTEGER;
    }
    return T_IDENTIFIER;
}

// ----------------------------------------------------------------------------
void DarlangCompiler::reallocateStack()
{
    if (! stack_size)
        stack_size = 128;
    else
        stack_size <<= 1;
    stack.resize (stack_size);
}

// ----------------------------------------------------------------------------
bool isNumeric(const QVariant& var)
{
    return (var.userType()==QVariant::Int ||
            var.userType()==QVariant::UInt ||
            var.userType()==QVariant::LongLong ||
            var.userType()==QVariant::ULongLong ||
            var.userType()==QVariant::Double);
}

// ----------------------------------------------------------------------------
bool isInteger(const QVariant& var)
{
    return (var.userType()==QVariant::Int ||
            var.userType()==QVariant::UInt ||
            var.userType()==QVariant::LongLong ||
            var.userType()==QVariant::ULongLong);
}

// ----------------------------------------------------------------------------
bool isUnsignedInt(const QVariant& var)
{
    return (var.userType()==QVariant::UInt ||
            var.userType()==QVariant::ULongLong);
}

// ----------------------------------------------------------------------------
QVariant DarlangCompiler::sumVariants(const QVariant &arg1, const QVariant &arg2)
{
    if(arg1.userType() == arg2.userType()) {
        switch (arg1.userType()) {
        case QVariant::Int:
            return arg1.toInt() + arg2.toInt();
            break;
        case QVariant::UInt:
            return arg1.toUInt() + arg2.toUInt();
            break;
        case QVariant::LongLong:
            return arg1.toLongLong() + arg2.toLongLong();
            break;
        case QVariant::ULongLong:
            return arg1.toULongLong() + arg2.toULongLong();
            break;
        case QVariant::Double:
            return arg1.toDouble() + arg2.toDouble();
            break;
        case QVariant::String:
            return arg1.toString() + arg2.toString();
            break;
        default:
            break;
        }
    } else {
        if(isNumeric(arg1) && isNumeric(arg2)) {
            if(isInteger(arg1) && isInteger(arg2)) {
                if(isUnsignedInt(arg1) && isUnsignedInt(arg2)) {
                    return arg1.toULongLong() + arg2.toULongLong();
                }
                else {
                    return arg1.toLongLong() + arg2.toLongLong();
                }
            }
            else
                return arg1.toDouble() + arg2.toDouble();
        }
        else {
            return arg1.toString() + arg2.toString();
        }
    }
}

// ----------------------------------------------------------------------------
QVariant DarlangCompiler::subVariants(const QVariant &arg1, const QVariant &arg2)
{
    if(isNumeric(arg1) && isNumeric(arg2)) {
        if(isInteger(arg1) && isInteger(arg2)) {
            if(isUnsignedInt(arg1) && isUnsignedInt(arg2)) {
                return arg1.toULongLong() - arg2.toULongLong();
            }
            else {
                return arg1.toLongLong() - arg2.toLongLong();
            }
        }
        else
            return arg1.toDouble() - arg2.toDouble();
    } else return QVariant();
}

// ----------------------------------------------------------------------------
bool DarlangCompiler::parse()
{
  const int INITIAL_STATE = 0;
  in_tag = 0;
  int yytoken = -1;
  reallocateStack();
  tos = 0;
  stack [++tos].state = INITIAL_STATE;
  while (true)
    {
      const int state = stack [tos].state;
      if (yytoken == -1 && - TERMINAL_COUNT != action_index [state])
        yytoken = nextToken();

      int act = t_action (state, yytoken);
      if (act == ACCEPT_STATE)
        return true;
      else if (act > 0)
        {
          if (++tos == stack_size)
            reallocateStack();
          stack [tos].ival = tokens.at(currentPos-1); // ### save the token value here
          stack [tos].state = act;
          yytoken = -1;
        }
      else if (act < 0)
        {
          int r = - act - 1;
          tos -= rhs [r];
          act = stack [tos++].state;
          switch (r) 
            { 
./

Darcode : Statements ;
Statements : Statements Statement ;
Statements : Statement;
Statements : T_LCURLYBRACE Statements T_RCURLYBRACE ;

Number : T_NUMBER_INTEGER ;
/.
                case $rule_number:
                    sym(1) = sym(1).toInt();
                    break;
                ./

Number : T_NUMBER_REAL ;
/.
                case $rule_number:
                    sym(1) = sym(1).toDouble();
                    break;
./

Expression : Number ;
Expression : T_IDENTIFIER ;
/.
case $rule_number: 
                if(_variables.contains(sym(1).toString())) {
                    qDebug() << "Known identifer" << sym(1).toString();
                    sym(1) = _variables.value(sym(1).toString());
                }
                qDebug() << "identifer" << sym(1);
                break;
./
Expression : T_STRING_LITERAL ;
/.
                case $rule_number: 
                    sym(1) = sym(1).toString().remove("\"");
                    // qDebug() << "String literal" << sym(1).toString();
                    break;
./
Statement : VariableDeclaration T_SEMICOLON ;
Statement : VariableAssignment T_SEMICOLON ;
Statement : AssignmentExpression T_SEMICOLON ;
Statement : FunctionCall T_SEMICOLON ;
FunctionCall : T_IDENTIFIER T_LPAREN T_RPAREN ;
FunctionCall : T_IDENTIFIER T_LPAREN Args T_RPAREN ;
/.
                case $rule_number: 
                    qDebug() << "Function call" << sym(1).toString() << sym(3);
                    break;
./
Args : Expression ;
/.
                case $rule_number: {
                    QVariantList args;
                    args << sym(1);
                    sym(1) = QVariant::fromValue(args);
                    break;
                }
./
Args : Args T_COMMA Expression;
/.
                case $rule_number: {
                    QVariantList args = qvariant_cast<QVariantList>(sym(1));
                    args << sym(3);
                    sym(1) = args;
                    break;
                }
./
VariableDeclaration : Type T_IDENTIFIER ;
/.
                case $rule_number:
                    qDebug() << "Variable declaration" << sym(1).toString() << sym(2).toString() << QMetaType::type(sym(1).toString().toStdString().c_str());
                    _variables.insert(sym(2).toString(), QVariant(QMetaType::type(sym(1).toString().toStdString().c_str()), (void*)0));
                    sym(1) = sym(2);
                    break;
./
VariableAssignment : VariableDeclaration T_EQ Expression ;
/.
                case $rule_number:
                    qDebug() << "Variable T_EQ" << sym(1).toString() << sym(3).toString();
                    _variables[sym(1).toString()] = sym(3);
                    // _variables.insert(sym(1).toString(), sym(3));
                    break;
./
AssignmentExpression : T_IDENTIFIER T_EQ Expression ;
/.
                case $rule_number:
                    qDebug() << "Identifer assignment" << sym(1) << sym(3) ;
                    break;

./
Type : TypeBOOL | TypeBYTE | TypeSTRING | TypeINT | TypeUINT | TypeINT64 | TypeUINT64 | TypeFLOAT | TypeDOUBLE;
Expression : SumExpression ;
Expression : SubExpression ;
Expression : FunctionCall ;
FieldExpression : T_IDENTIFIER T_DOT T_IDENTIFIER ;
/.
                case $rule_number:
                    qDebug() << "Field access" << sym(1) << sym(3) ;
                    sym(1) = sym(1).toString() + sym(2).toString() + sym(3).toString();
                    break;

./
FieldExpression : FieldExpression T_DOT T_IDENTIFIER ;
/.
                case $rule_number:
                    qDebug() << "Deep field access" << sym(1) << sym(3) ;
                    break;

./
Expression : FieldExpression ;

SumExpression : Expression T_PLUS Expression ;
/.
                case $rule_number:
                    qDebug() << "Summ expression" << sym(1) << sym(2) << sym(3);
                    if(sym(1).isValid() && sym(3).isValid()) {
                      sym(1) = sumVariants(sym(1), sym(3));
                    } else 
                    {
                      qDebug() << "Runtime-calculated add expression";
                      // TODO: Implement sum
                    }
                    // _variables.insert(sym(1).toString(), sym(3));
                    break;
./
SubExpression : Expression T_MINUS Expression ;
/.
                case $rule_number:
                    qDebug() << "Sub expression" << sym(1) << sym(2) << sym(3);
                    if(sym(1).isValid() && sym(3).isValid()) {
                      sym(1) = subVariants(sym(1), sym(3));
                    } else 
                    {
                      qDebug() << "Runtime-calculated sub expression";
                      // TODO: Implement sub
                    }
                    // _variables.insert(sym(1).toString(), sym(3));
                    break;
./
/.       
			} // switch

          stack [tos].state = nt_action (act, lhs [r] - TERMINAL_COUNT);
        }

      else
        {
          // ### ERROR RECOVERY HERE
          break;
        }
    }

    return false;
}
./
