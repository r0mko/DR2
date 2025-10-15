#include "tokenlist.h"
#include <QDebug>


QByteArray TokenList::join(char separator) const
{
    QByteArray ba;
    for(int i=0; i<d.size(); ++i)
    {
        ba.append(d.at(i));
        if((i+1) != d.size())
            ba.append(separator);
    }
    return ba;
}

QByteArray TokenList::toQuery()
{
    return join(' ').append('\n');
}

TokenList TokenList::tokenizeDatLine(const QByteArray &datLine)
{
    QByteArray line = datLine;
    QList<QByteArray> tokens;
    int pos = line.indexOf("\"")+1;
    int len = line.lastIndexOf("\"")-pos;
    if(pos && len>=0) // однострочный текст
    {
        QByteArray text = line.mid(pos, len);
        line.remove(pos, len);
        tokens = line.split(' ');
        tokens[4] = text;
    }
    if(pos && len==-1)
    {
        qDebug() << "Multiline text not implemented yet";
    }
    if(!pos)
    {
        tokens = line.split(' ');
    }
    TokenList list = tokens;
    return list;
}

uint TokenList::replyIdf()
{
    if(d.length()>2 && d.first()=="DAT")
        return d[2].toInt();
    else
        return 0xfffffff;

}


QSet<uint> TokenList::queryIdfs()
{
    QSet<uint> idfs;
    if(d.isEmpty())
        return idfs;
    if(d.first() != "ANM")
        return idfs;
    for(int i=3; i<d.size(); ++i)
        idfs.insert(d[i].toInt());
    return idfs;
}
