#include "hashstring.h"
std::map<uint, HashString::strdata> HashString::table = std::map<uint, HashString::strdata>();

HashString::~HashString()
{
    if (isValid() && !table.at(d).ref.deref())
        table.erase(d);
}

HashString::HashString(const QString &string) :
    d(hash(string))
{
    auto it = table.find(d);
    if (it == table.end()) {
        table.emplace(d, string );
    } else {
        (*it).second.ref.ref();
    }
}

HashString::HashString(uint raw) : d(raw)
{
    auto it = table.find(d);
    if (it == table.end()) {
        d = 0;
    } else {
        (*it).second.ref.ref();
    }
}


