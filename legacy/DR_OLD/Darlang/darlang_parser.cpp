



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
            // fix NUMBER-NUMBER issue ->
            if(isNumber(lastToken) && isNumber(rx.cap(1)) && rx.cap(1).startsWith("-"))
            {
                tokens << "-";
                tokens << rx.cap(1).right(rx.cap(1).size()-1);
            } else
            {
                tokens << rx.cap(1);
            } // <-

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
        if(token.toLatin1() == spell[i])
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



            case 4:
                sym(1) = sym(1).toInt();
                break;
                


            case 5:
                sym(1) = sym(1).toDouble();
                break;



            case 7:
                if(_variables.contains(sym(1).toString())) {
                    qDebug() << "Known identifer" << sym(1).toString();
                    sym(1) = _variables.value(sym(1).toString());
                }
                qDebug() << "identifer" << sym(1);
                break;



            case 8:
                sym(1) = sym(1).toString().remove("\"");
                // qDebug() << "String literal" << sym(1).toString();
                break;



            case 14:
                qDebug() << "Function call" << sym(1).toString() << sym(3);
                break;



            case 15: {
                QVariantList args;
                args << sym(1);
                sym(1) = QVariant::fromValue(args);
                break;
            }



            case 16: {
                QVariantList args = qvariant_cast<QVariantList>(sym(1));
                args << sym(3);
                sym(1) = args;
                break;
            }



            case 17:
                qDebug() << "Variable declaration" << sym(1).toString() << sym(2).toString() << QMetaType::type(sym(1).toString().toStdString().c_str());
                _variables.insert(sym(2).toString(), QVariant(QMetaType::type(sym(1).toString().toStdString().c_str()), (void*)0));
                sym(1) = sym(2);
                break;



            case 18:
                qDebug() << "Variable T_EQ" << sym(1).toString() << sym(3).toString();
                _variables[sym(1).toString()] = sym(3);
                // _variables.insert(sym(1).toString(), sym(3));
                break;



            case 19:
                qDebug() << "Identifer assignment" << sym(1) << sym(3) ;
                break;




            case 32:
                qDebug() << "Field access" << sym(1) << sym(3) ;
                sym(1) = sym(1).toString() + sym(2).toString() + sym(3).toString();
                break;




            case 33:
                qDebug() << "Deep field access" << sym(1) << sym(3) ;
                break;




            case 35:
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



            case 36:
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
