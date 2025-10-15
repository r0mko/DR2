#ifndef TOKENLIST_H
#define TOKENLIST_H

#include <QList>

class TokenList
{
public:
    TokenList (){ d.clear(); }
    TokenList (const QList<QByteArray>&other) { d = other; }
    QByteArray join (char separator) const;
    QByteArray toQuery();
    const QByteArray& tokenAt (int pos) const { return d.at(pos); }
    int size () const { return d.size(); }
    void clear () { d.clear(); }
    void append (const QByteArray& token) { d.append(token); }
    void append (int idf) { d.append(QByteArray::number(idf)); }
    void append (QList<int> idf) { foreach (int id, idf) { d.append(QByteArray::number(id)); }; }
    void append (qreal real) { d.append(QByteArray::number(real)); }
    void prepend (const QByteArray& token) { d.prepend(token); }
    bool contains (const QByteArray& token) const { return d.contains(token); }
    bool containsAll (const TokenList& tokens);
    bool containsSome (const TokenList& tokens);
    void remove (const QByteArray& token);
    void remove (const TokenList& tokens);
    void removeAt (int pos) {d.removeAt(pos); }
    bool isEmpty() const { return d.isEmpty(); }
    QByteArray first() const { return d.first(); }
    QByteArray last() const { return d.last(); }
    TokenList &operator=(const TokenList & other );
    TokenList &operator=(const QList<QByteArray>& other ) { d = other; return *this; }
    QSet<uint> queryIdfs();
    uint replyIdf();
    static TokenList tokenizeDatLine(const QByteArray &datLine);


protected:
    QList<QByteArray> d;
};
#endif // TOKENLIST_H
