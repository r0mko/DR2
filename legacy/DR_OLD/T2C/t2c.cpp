#include "t2c.h"
#include <QFile>

QDataStream & operator<< ( QDataStream& out, const PV & obj)
{
    out << obj.time;
    out << obj.value;
    out << obj.status;
    out << obj.iType;
    return out;
}

QDataStream & operator>> ( QDataStream& in, PV & obj)
{
    in >> obj.time;
    in >> obj.value;
    in >> obj.status;
    in >> obj.iType;
    return in;
}


T2CManager::T2CManager()
{
    sim = 0;
    codec = QTextCodec::codecForName("ISO-8859-5");
    p_immediateUpdate = true;
    m_rawMode = false;
}

QScriptValue toScriptValue(QScriptEngine* e, const PV &s)
{
    QScriptValue pv = e->newObject();
    pv.setProperty("status", s.status);
    switch(s.value.type())
    {
    case QVariant::Int:
        pv.setProperty("value", s.value.toInt());
        break;
    case QVariant::Double:
        pv.setProperty("value", s.value.toDouble());
        break;
    case QVariant::BitArray:
    {
        QBitArray ba = s.value.toBitArray();
        QScriptValue bits = e->newArray(ba.size());
        for(int i=0; i<ba.size();++i)
        {
            bits.setProperty(i, ba.testBit(i));
        }
        pv.setProperty("value", bits);
        break;
    }
    case QVariant::String:
        pv.setProperty("value", s.value.toString());
        break;
    default:
        break;
    }
    QScriptValue date = e->newDate(s.time);
    pv.setProperty("time", date);
    pv.setProperty("type", QString(s.iType));
    return pv;
}

void fromScriptValue(const QScriptValue &obj, PV &s)
{
    s.status = obj.property("status").toUInt32();
    s.value = obj.property("value").toVariant();
    s.iType = obj.property("type").toString().toAscii();

}

QScriptValue PVListenerToSV(QScriptEngine *engine, PVListener* const &in)
{ return engine->newQObject(in); }

void PVListenerFromSV(const QScriptValue &object, PVListener* &out)
{ out = qobject_cast<PVListener*>(object.toQObject()); }

QScriptValue PVCacheToSV(QScriptEngine *engine, PVCache const &in)
{
    QScriptValue v = engine->newObject();
    QList<int> keys = in.keys();
    foreach(int key, keys) {
        v.setProperty(key, toScriptValue(engine, in.value(key)));
    }
    return v;
}

void SVToPVCache(const QScriptValue &obj, PVCache &s)
{
    QVariant v = obj.toVariant();
    QVariantMap qscache = qvariant_cast<QVariantMap>(v);
    QMapIterator<QString, QVariant> i(qscache);
    while(i.hasNext()) {
        i.next();
        int idf = i.key().toInt();
        PV v;
        QVariantMap pv = qvariant_cast<QVariantMap>(i.value());
        v.iType = pv.value("type").toString().toAscii();
        QVariant pvval = pv.value("value");
        if(v.iType[0] == 'I')
            v.value = pvval.toInt();
        if(v.iType[0] == 'F')
            v.value = pvval.toDouble();
        v.changed = false;
        v.subscribed = false;
        s.insert(idf, v);
    }
}

void T2CManager::bindToScriptEngine(QScriptEngine *engine)
{
    QScriptValue metaObject = engine->newQMetaObject(&T2CManager::staticMetaObject, engine->newFunction(T2CManager::JST2CCtor));
    engine->globalObject().setProperty("T2C", metaObject);
    qScriptRegisterMetaType<PV>(engine, toScriptValue, fromScriptValue);
    qScriptRegisterMetaType<PVListener*>(engine, PVListenerToSV, PVListenerFromSV);
    qScriptRegisterMetaType<PVCache>(engine, PVCacheToSV, SVToPVCache);
    qScriptRegisterSequenceMetaType<QList<PV> >(engine);
}



QScriptValue T2CManager::JST2CCtor(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(context)
    T2CManager* manager = new T2CManager();
    return engine->newQObject(manager, QScriptEngine::ScriptOwnership);
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

uint TokenList::replyIdf()
{
    if(d.length()>2 && d.first()=="DAT")
        return d[2].toInt();
    else
        return 0xfffffff;

}

bool T2CManager::connectToT2C(QHostAddress addr, quint16 port, bool verbose)
{
    socket.connectToHost (addr, port);
    verboseMode = verbose;
    if(socket.waitForConnected (1000))
    {
        if(!verbose)
            qDebug() << QString("Connected to T2C %1:%2").arg(addr.toString()).arg(port).toStdString().c_str();
        initParams();
        connect(&socket, SIGNAL(readyRead()), this, SLOT(readReply()), Qt::QueuedConnection);
        return true;
    }
    return false;
}

bool T2CManager::connectToT2C(QString addr, quint16 port, bool verbose)
{
    return connectToT2C(QHostAddress(addr), port, verbose);
}

PV T2CManager::getPV(int idf, bool forceCached)
{
    if(forceCached && cache.contains(idf) && cache[idf].value.isValid()) {
        //    qDebug() << "Returning cached value"<< idf << cache.contains(idf);
        return cache.value(idf);
    }
    if(subscribedIdfs.contains(idf) && cache.contains(idf) && cache[idf].value.isValid()) {
        return cache.value(idf);
    }
    QList<int> ids;
    ids << idf;
    QList<PV> vals = getPVs(ids, false);
    if(!vals.isEmpty())
        return vals.first();
    else
        return PV();
}

QVariant T2CManager::getValue(int idf, bool forceCached)
{
    PV pv = getPV(idf, forceCached);
    return pv.value;
}


QList<PV> T2CManager::getPVs(const QList<int> &idfList, bool subscribe)
{
    QList<PV> reply;

    if(subscribe)
        subscribedIdfs.unite(idfList.toSet());
    int c=0;
    do
    {
        TokenList q = generateANMQuery(subscribe, true);
        int cmax = params.max_idf;
        while(cmax && c<idfList.size())
        {
            q.append(idfList.at(c));
            c++;
            cmax--;
        };
        //        qDebug() << q.join(' ');
        socket.write(q.toQuery());
        QSet<uint> ids = q.queryIdfs();
        if(!verboseMode)
            qDebug() << "Querying" << ids.size() << "values of" << idfList.size();
        if(socket.waitForReadyRead(1000))
        {
            while(socket.canReadLine())
            {
                QByteArray line = socket.readLine().simplified();
                TokenList tokens = TokenList::tokenizeDatLine(line);
                if(tokens.size() > 2) {
                    uint idf = tokens.tokenAt(2).toInt();
                    PV v = parseDatTokens(tokens);
                    if(cache.contains(idf) && cache[idf].listener)
                        v.listener = cache[idf].listener;
                    if(ids.remove(idf))
                    {
                        v.subscribed = subscribe;
                        cache.insert(idf, v);
                        reply.append(v);
                    }
                    else
                    {
                        //          qDebug() << "Strange IDF" <<idf;
                        cache.insert(idf, v);
                        emit PVupdated(idf);
                    }
                }
                if(!ids.isEmpty() && (socket.atEnd() || !socket.canReadLine()))
                    socket.waitForReadyRead(100);
            }
        }
        //    qDebug() << "Reply is now" << reply.size();
    } while(c<idfList.size());
    if(!verboseMode)
        qDebug() << "Completed query size" << reply.size();
    return reply;
}

QList<QVariant> T2CManager::getValues(const QList<int> &idfList, bool subscribe)
{
    QList<QVariant> reply;
    QList<PV> pvs = getPVs(idfList, subscribe);
    reply.reserve(pvs.size());
    for(int i=0; i<pvs.size(); ++i) {
        reply.append(pvs[i].value);
    }
    return reply;
}

void T2CManager::subscribeValues(const QList<int> &idfList)
{
    subscribedIdfs.unite(idfList.toSet());
    int c=0;
    do
    {
        TokenList q = generateANMQuery(true, false);
        int cmax = params.max_idf;
        while(cmax && c<idfList.size())
        {
            int idf = idfList.at(c);
            PV v;
            v.subscribed = true;
            cache.insert(idf, v);
            q.append(idf);
            c++;
            cmax--;
        };
        socket.write(q.toQuery());
    } while(c<idfList.size());
}

void T2CManager::unsubscribeValues(const QList<int> &idfList)
{
    subscribedIdfs.subtract(idfList.toSet());
    int c=0;
    do
    {
        TokenList q;
        q.append("ANM");
        q.append(QByteArray::number(sim));
        q.append("END");
        uint cmax = params.max_idf;
        while(cmax && c<idfList.size())
        {
            q.append(idfList.at(c));
            c++;
            cmax--;
        };
        socket.write(q.toQuery());
    } while(c<idfList.size());
}

void T2CManager::updateValue(int idf, QVariant newValue, const QString &status)
{
    if(!cache.contains(idf))
    {
        PV v = getPV(idf);
    }
    PV &var = cache[idf];
    if(newValue.canConvert(var.value.type()))
    {
        var.value = newValue;
    }
    if(p_immediateUpdate)
    {
        TokenList cmdt = generateDATCommand(idf);
        if(!status.isEmpty())
            cmdt.append(status.toLatin1());
        QByteArray cmd = cmdt.toQuery();
        //        cmd.append(' ').append(status.toAscii());
        //        qDebug() << "Written command" << cmd.simplified();
        socket.write(cmd);
        socket.waitForBytesWritten();
    }
    else
    {
        //        qDebug() << "Queued update" << idf;
        var.changed = true;
        updatedVariables.insert(idf);
    }
}

PVListener *T2CManager::listenPVs(const QList<int> &idfList)
{
    PVListener* listener = new PVListener(idfList.size());

    subscribeValues(idfList);
    foreach (int id, idfList) {
        cache[id].listener = listener;
    }
    if(!verboseMode)
        qDebug() << "Subscribed to" << idfList.size() << "PVs";
    return listener;
}

void T2CManager::commit()
{
    if(p_immediateUpdate)
        return;
    if(!verboseMode)
        qDebug() << "Commiting" << updatedVariables.size() << "PVs";

    QSet<int>::iterator i;
    for (i = updatedVariables.begin(); i != updatedVariables.end(); ++i)
    {
        QByteArray cmd = generateDATCommand(*i).toQuery();
        //        qDebug() << "Written command" << cmd.simplified();
        socket.write(cmd);
        socket.waitForBytesWritten();
        cache[*i].changed = false;
    }
    updatedVariables.clear();
    if(!verboseMode)
        qDebug() << "Update done";
}

void T2CManager::sendRawQuery(const QString &query)
{
    if(m_rawMode)
    {
        socket.write(codec->fromUnicode(query));
        socket.write("\n");
        socket.waitForBytesWritten();
    }
}

QString T2CManager::readRawReply(int timeout)
{
    if(!m_rawMode)
        return QString();
    qint64 current = QDateTime::currentMSecsSinceEpoch();
    while((QDateTime::currentMSecsSinceEpoch() < current+timeout) && !socket.canReadLine())
    {
        socket.waitForReadyRead(10);
    }
    return codec->toUnicode(socket.readLine());
}

bool T2CManager::hasRawData()
{
    return socket.bytesAvailable() > 0;
}

bool T2CManager::saveCache(QString filename) const
{
    QFile f(filename);
    if(f.open(QIODevice::WriteOnly)) {
        QDataStream out(&f);
        out << cache;
        f.flush();
        f.close();
        return true;
    } else
    {
        return false;
    }
}

bool T2CManager::loadCache(QString filename)
{
    QFile f(filename);
    if(f.open(QIODevice::ReadOnly)) {
        cache.clear();
        QDataStream in(&f);
        in >> cache;
        f.close();
        return true;
    }
    else
        return false;
}

int T2CFactory::init(QObject *core)
{
    this->core = (Core*)core;
    qRegisterMetaType<T2CManager*>();
    qRegisterMetaType<PVListener*>();
    qRegisterMetaType<PV>();
    this->core->log(0,this,"T2C factory plugin initialized");
    QScriptEngine* e = this->core->engine();
    T2CManager::bindToScriptEngine(e);
    return 0;

}

QVariantHash T2CFactory::getInfo()
{
    QVariantHash i;
    i.insert("Name", "T2C Manager");
    i.insert("Version", 102);
    i.insert("Author", "r0mko");
//    i.insert("", );
//    i.insert("", );
//    i.insert("", );
//    i.insert("", );
//    i.insert("", );
    return i;
}
void T2CManager::readReply()
{
    while(socket.canReadLine() && !m_rawMode)
    {
        TokenList tokens = TokenList::tokenizeDatLine(socket.readLine().simplified());
        if(tokens.first() == "DAT") {
            int idf = tokens.replyIdf();
            PV v = parseDatTokens(tokens);
            if(subscribedIdfs.contains(idf))
                v.subscribed = true;
            if(cache.contains(idf))
                v.listener = cache[idf].listener;
            cache.insert(idf, v);
            //            qDebug() << "new value" << idf << cache[idf].listener;
            if(v.listener) {
                //        qDebug() << "PV" << idf << "sent to listener" << v.listener;
                emit v.listener->onUpdate(idf);
            }
            emit PVupdated(idf);
        }
        else
            qDebug() << "Unknown reply" <<tokens.join(' ');
    }
}

void T2CManager::initParams()
{
    socket.write ("T2C PRP MAX_IDF\nT2C PRP MAX_INP\nT2C PRP MAX_OUT\n");
    if(socket.waitForReadyRead (1000))
    {
        uint i = 0;
        while(i<3)
        {
            if(socket.canReadLine()) {
                QByteArray ba = socket.readLine();
                //            qDebug() << "Got line" << ba.simplified();
                switch(i)
                {
                case 0:
                    params.max_idf = ba.simplified().toInt();
                    break;
                case 1:
                    params.max_inp = ba.simplified().toInt();
                    break;
                case 2:
                    params.max_out = ba.simplified().toInt();
                    break;
                }
                i++;
            }
            if(!socket.canReadLine() && i<3)
                socket.waitForReadyRead(100);
        }
    }
}

bool T2CManager::validateLine(const QByteArray &query, const QByteArray &replyLine)
{
    Q_UNUSED(query)
    Q_UNUSED(replyLine)
    return true;
}

PV T2CManager::parseDatTokens(const TokenList &tokens)
{
    // 0 = DAT
    // 1 = <sim>
    // 2 = <idf>
    // 3 = <typ>
    // 4 = value
    // 5 = <sta>
    // 6 = <tim>
    PV var;
    if(tokens.first() != "DAT")
        return var;
    if(tokens.tokenAt(3).startsWith("F"))
    {
        var.value = tokens.tokenAt(4).toDouble();
    }
    if(tokens.tokenAt(3).startsWith("I"))
    {
        var.value = tokens.tokenAt(4).toInt();
    }
    if(tokens.tokenAt(3).startsWith("B"))
    {
        QBitArray bits;
        bits.resize(tokens.tokenAt(3).right(2).toInt());
        bool ok;
        quint32 n = tokens.tokenAt(4).toUInt(&ok, 16);
        quint32 b1 = 1;
        for(int i=0; i<bits.size();++i)
        {
            //            qDebug() << b1;
            bits.setBit(i, b1 & n);
            b1 = b1 << 1;
        }
        var.value = bits;
    }
    if(tokens.tokenAt(3).startsWith("T"))
    {
        var.value = codec->toUnicode(tokens.tokenAt(4));
        //        qDebug() << var.value.toString() << tokens.tokenAt(4);
    }
    var.iType = tokens.tokenAt(3).data();
    QByteArray timestamp = tokens.tokenAt(6);

    var.time = QDateTime::fromMSecsSinceEpoch(timestamp.replace(".", "").toLongLong());
    return var;
}

TokenList T2CManager::generateANMQuery(bool subscribe, bool returnValues)
{
    QByteArray cmd;
    if(subscribe && returnValues)
        cmd = "ORG";
    if(!subscribe && returnValues)
        cmd = "TST";
    if(subscribe && !returnValues)
        cmd = "DYN";
    TokenList q;
    q.append("ANM");
    q.append(sim);
    q.append(cmd);
    return q;
}

TokenList T2CManager::generateDATCommand(uint idf)
{
    TokenList tok;
    if(cache.contains(idf))
    {
        PV &var = cache[idf];
        tok.append("DAT");
        tok.append(QByteArray::number(sim));
        tok.append(QByteArray::number(idf));
        tok.append(var.iType);
        QString val = var.value.toString();
        //        qDebug() << var.value;
        QByteArray sval;
        if(var.iType.startsWith("T"))
            sval = codec->fromUnicode(val.prepend("\"").append("\""));
        else
            sval = val.toLatin1();
        tok.append(sval);
        //        qDebug() << tok.join(' ');
    }
    return tok;
}

bool TokenList::containsAll(const TokenList &tokens)
{
    bool result = true;
    for(int i=0; i<d.size(); ++i)
    {
        bool t = d.contains(tokens.tokenAt(i));
        result &= t;
    }
    //    result &= d.contains(tokens.tokenAt(i));
    return result;
}

bool TokenList::containsSome(const TokenList &tokens)
{
    bool result = false;
    for(int i=0; i<d.size(); ++i)
    {
        bool t = d.contains(tokens.tokenAt(i));
        result |= t;
    }
    return result;
}



TokenList::TokenList()
{
    d.clear();
}


TokenList::TokenList(const QList<QByteArray> &other)
{
    d = other;
}

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
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2 ( T2C, T2CFactory )
#endif
