#include "t2cmanager.h"
#include <QBitArray>
//#include <QDataStream>
#include <QFile>

#include <QTextCodec>
#include "pvlistener.h"

T2CManager::T2CManager(): m_codec(QTextCodec::codecForName("ISO-8859-5")) {}

bool T2CManager::connectToT2C(QHostAddress addr, quint16 port, bool verbose)
{
    m_socket.connectToHost (addr, port);
    m_verboseMode = verbose;
    if(m_socket.waitForConnected (1000))
    {
        if(!verbose)
            qDebug() << QString("Connected to T2C %1:%2").arg(addr.toString()).arg(port).toStdString().c_str();
        initParams();
        connect(&m_socket, SIGNAL(readyRead()), this, SLOT(readReply()), Qt::QueuedConnection);
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
    if(forceCached && m_cache.contains(idf) && m_cache[idf].value.isValid()) {
        //    qDebug() << "Returning cached value"<< idf << cache.contains(idf);
        return m_cache.value(idf);
    }
    if(m_subscribedIdfs.contains(idf) && m_cache.contains(idf) && m_cache[idf].value.isValid()) {
        return m_cache.value(idf);
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
        m_subscribedIdfs.unite(idfList.toSet());
    int c=0;
    do
    {
        TokenList q = generateANMQuery(subscribe, true);
        int cmax = m_params.max_idf;
        while(cmax && c<idfList.size())
        {
            q.append(idfList.at(c));
            c++;
            cmax--;
        };
        //        qDebug() << q.join(' ');
        m_socket.write(q.toQuery());
        QSet<uint> ids = q.queryIdfs();
        if(!m_verboseMode)
            qDebug() << "Querying" << ids.size() << "values of" << idfList.size();
        if(m_socket.waitForReadyRead(1000))
        {
            while(m_socket.canReadLine())
            {
                QByteArray line = m_socket.readLine().simplified();
                TokenList tokens = TokenList::tokenizeDatLine(line);
                if(tokens.size() > 2) {
                    uint idf = tokens.tokenAt(2).toInt();
                    PV v = parseDatTokens(tokens);
                    if(m_cache.contains(idf) && m_cache[idf].listener)
                        v.listener = m_cache[idf].listener;
                    if(ids.remove(idf))
                    {
                        v.subscribed = subscribe;
                        m_cache.insert(idf, v);
                        reply.append(v);
                    }
                    else
                    {
                        //          qDebug() << "Strange IDF" <<idf;
                        m_cache.insert(idf, v);
                        emit PVupdated(idf);
                    }
                }
                if(!ids.isEmpty() && (m_socket.atEnd() || !m_socket.canReadLine()))
                    m_socket.waitForReadyRead(100);
            }
        }
        //    qDebug() << "Reply is now" << reply.size();
    } while(c<idfList.size());
    if(!m_verboseMode)
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
    m_subscribedIdfs.unite(idfList.toSet());
    int c=0;
    do
    {
        TokenList q = generateANMQuery(true, false);
        int cmax = m_params.max_idf;
        while(cmax && c<idfList.size())
        {
            int idf = idfList.at(c);
            PV v;
            v.subscribed = true;
            m_cache.insert(idf, v);
            q.append(idf);
            c++;
            cmax--;
        };
        m_socket.write(q.toQuery());
    } while(c<idfList.size());
}

void T2CManager::unsubscribeValues(const QList<int> &idfList)
{
    m_subscribedIdfs.subtract(idfList.toSet());
    int c=0;
    do
    {
        TokenList q;
        q.append("ANM");
        q.append(QByteArray::number(m_sim));
        q.append("END");
        uint cmax = m_params.max_idf;
        while(cmax && c<idfList.size())
        {
            q.append(idfList.at(c));
            c++;
            cmax--;
        };
        m_socket.write(q.toQuery());
    } while(c<idfList.size());
}

void T2CManager::updateValue(int idf, QVariant newValue, const QString &status)
{
    if(!m_cache.contains(idf))
    {
        PV v = getPV(idf);
    }
    PV &var = m_cache[idf];
    if(newValue.canConvert(var.value.type()))
    {
        var.value = newValue;
    }
    if(m_immediateUpdate)
    {
        TokenList cmdt = generateDATCommand(idf);
        if(!status.isEmpty())
            cmdt.append(status.toLocal8Bit());
        QByteArray cmd = cmdt.toQuery();
        //        cmd.append(' ').append(status.toAscii());
        //        qDebug() << "Written command" << cmd.simplified();
        m_socket.write(cmd);
        m_socket.waitForBytesWritten();
    }
    else
    {
        //        qDebug() << "Queued update" << idf;
        var.changed = true;
        m_updatedVariables.insert(idf);
    }
}

PVListener *T2CManager::listenPVs(const QList<int> &idfList)
{
    PVListener* listener = new PVListener(idfList.size());

    subscribeValues(idfList);
    foreach (int id, idfList) {
        m_cache[id].listener = listener;
    }
    if(!m_verboseMode)
        qDebug() << "Subscribed to" << idfList.size() << "PVs";
    return listener;
}

void T2CManager::commit()
{
    if(m_immediateUpdate)
        return;
    if(!m_verboseMode)
        qDebug() << "Commiting" << m_updatedVariables.size() << "PVs";

    QSet<int>::iterator i;
    for (i = m_updatedVariables.begin(); i != m_updatedVariables.end(); ++i)
    {
        QByteArray cmd = generateDATCommand(*i).toQuery();
        //        qDebug() << "Written command" << cmd.simplified();
        m_socket.write(cmd);
        m_socket.waitForBytesWritten();
        m_cache[*i].changed = false;
    }
    m_updatedVariables.clear();
    if(!m_verboseMode)
        qDebug() << "Update done";
}

QString T2CManager::encoding() const {
    return QString(m_codec->name());
}

void T2CManager::sendRawQuery(const QString &query)
{
    if(m_rawMode)
    {
        m_socket.write(m_codec->fromUnicode(query));
        m_socket.write("\n");
        m_socket.waitForBytesWritten();
    }
}

QString T2CManager::readRawReply(int timeout)
{
    if(!m_rawMode)
        return QString();
    qint64 current = QDateTime::currentMSecsSinceEpoch();
    while((QDateTime::currentMSecsSinceEpoch() < current+timeout) && !m_socket.canReadLine())
    {
        m_socket.waitForReadyRead(10);
    }
    return m_codec->toUnicode(m_socket.readLine());
}

bool T2CManager::hasRawData()
{
    return m_socket.bytesAvailable() > 0;
}

bool T2CManager::saveCache(QString filename) const
{
    QFile f(filename);
    if(f.open(QIODevice::WriteOnly)) {
        QDataStream out(&f);
        out << m_cache;
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
        m_cache.clear();
        QDataStream in(&f);
        in >> m_cache;
        f.close();
        return true;
    }
    else
        return false;
}

void T2CManager::setEncoding(QString arg)
{
    m_codec = QTextCodec::codecForName(arg.toLocal8Bit());
}




void T2CManager::readReply()
{
    while(m_socket.canReadLine() && !m_rawMode)
    {
        TokenList tokens = TokenList::tokenizeDatLine(m_socket.readLine().simplified());
        if(tokens.first() == "DAT") {
            int idf = tokens.replyIdf();
            PV v = parseDatTokens(tokens);
            if(m_subscribedIdfs.contains(idf))
                v.subscribed = true;
            if(m_cache.contains(idf))
                v.listener = m_cache[idf].listener;
            m_cache.insert(idf, v);
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
    m_socket.write ("T2C PRP MAX_IDF\nT2C PRP MAX_INP\nT2C PRP MAX_OUT\n");
    if(m_socket.waitForReadyRead (1000))
    {
        uint i = 0;
        while(i<3)
        {
            if(m_socket.canReadLine()) {
                QByteArray ba = m_socket.readLine();
                //            qDebug() << "Got line" << ba.simplified();
                switch(i)
                {
                case 0:
                    m_params.max_idf = ba.simplified().toInt();
                    break;
                case 1:
                    m_params.max_inp = ba.simplified().toInt();
                    break;
                case 2:
                    m_params.max_out = ba.simplified().toInt();
                    break;
                }
                i++;
            }
            if(!m_socket.canReadLine() && i<3)
                m_socket.waitForReadyRead(100);
        }
    }
}

bool T2CManager::validateLine(const QByteArray &query, const QByteArray &replyLine)
{
    Q_UNUSED(query)
    Q_UNUSED(replyLine)

    // TODO: ???
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
        var.value = m_codec->toUnicode(tokens.tokenAt(4));
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
    q.append(m_sim);
    q.append(cmd);
    return q;
}

TokenList T2CManager::generateDATCommand(uint idf)
{
    TokenList tok;
    if(m_cache.contains(idf))
    {
        PV &var = m_cache[idf];
        tok.append("DAT");
        tok.append(QByteArray::number(m_sim));
        tok.append(QByteArray::number(idf));
        tok.append(var.iType);
        QString val = var.value.toString();
        //        qDebug() << var.value;
        QByteArray sval;
        if(var.iType.startsWith("T"))
            sval = m_codec->fromUnicode(val.prepend("\"").append("\""));
        else
            sval = val.toLocal8Bit();
        tok.append(sval);
        //        qDebug() << tok.join(' ');
    }
    return tok;
}


