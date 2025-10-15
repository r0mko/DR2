#include "t2cmanager.h"
//#include <iostream>
#include <QTextCodec>
#include <QDebug>
#include "t2cworker.h"

T2CManager::T2CManager() :
    QObject()
{
    //  Debug << "Initializing T2C manager";
    qRegisterMetaType<PVariable>();
    codec = QTextCodec::codecForName("ISO-8859-5");
}

T2CManager::~T2CManager()
{
    if(worker)
    {
        qDebug() << "Останавливаю клиент T2C...";
        worker->stopWorking();
        worker->exit();
        worker->wait();
        qDebug() << "\e[1;37m готово.\e[0m";
    }
}

bool T2CManager::connectToT2C(QHostAddress addr, quint16 port, bool verbose)
{
    //  Debug << "Connecting to T2C at " << addr.toString().append(":%1").arg(port).toStdString().c_str();
    if(!verbose)
        qDebug() << "Подключаюсь к T2C " << addr.toString().append(":%1").arg(port).toStdString().c_str() << ":";
    worker = new T2CWorker(addr, port, &readyWaiter);
    //  connect(worker, SIGNAL(valueUpdated(quint64,PVariable)), this, SLOT(updateValue(quint64,PVariable)));
    connect(worker, SIGNAL(valueUpdated()), this, SLOT(onValueChange()), Qt::DirectConnection);
    worker->start();
    startMtx.lock();
    T2Cready = readyWaiter.wait(&startMtx, 10000);
    if(!verbose) {
        if(T2Cready) {
            qDebug() << " есть контакт!";
        } else {
            qDebug() << " не получилось. Проверьте адрес, порт и работу T2C.";
        }
    }
    return T2Cready;
}

void T2CManager::disconnectFromT2C()
{
    if(!T2Cready)
        return;
    worker->stopWorking();
    worker->exit();
    worker->wait(10000);
    delete worker;

}

void T2CManager::setTextEncoding(const char *name)
{
    codec = QTextCodec::codecForName(name);
}

PVariable T2CManager::probePVariable(quint64 idf, bool subscribe)
{
    PVariable var;
    if(T2Cready)
    {
        worker->sendCommand(QString("ANM 0 %1 %2").arg(subscribe ? "ORG" : "TST").arg(idf).toLocal8Bit(), &replyWaiter);
        replyMtx.lock();
        replyWaiter.wait(&replyMtx, 5000);
        replyMtx.unlock();
        QList<QList<QByteArray> > tokens = worker->getReply();
        for(int i=0; i<tokens.size(); ++i)
        {
            quint64 idf_rep;
            var = parseDATReply(tokens.at(i), idf_rep);
            if(idf_rep == idf)
            {
                PVCache.insert(idf, var);
                //                qDebug() << "Inserting value" << var.value.toString() << "idf =" << idf << "type" << var.vtype << "into hash";
            }
            else
            {
                qDebug() << "Unwanted idf" << idf_rep;
            }

        }
    }
    return var;

}

void T2CManager::subcribePVariables(QList<quint64> idf)
{
    Q_UNUSED(idf)
    if(T2Cready)
    {

    }
}

void T2CManager::updatePVariable(quint64 idf, const QVariant &newValue)
{
    QString cmd = makeUpdateCmd(idf, newValue, true, true);
    worker->sendCommand(codec->fromUnicode(cmd));
}

void T2CManager::updatePVariable_blocking(quint64 idf, const QVariant &newValue)
{
    //  Debug << "Updating PV " << idf;
    QString cmd = makeUpdateCmd(idf, newValue);
    worker->sendCommand(codec->fromUnicode(cmd), &replyWaiter);
    replyMtx.lock();
    replyWaiter.wait(&replyMtx, 1000);
    replyMtx.unlock();
    //  Debug << "Write succeeded!";
}

void T2CManager::listPVs()
{
    //  Debug << "Listing all PV's";
    //  worker->sendCommandAndWaitReply("LST 0", &replyWaiter);
    //  replyMtx.lock();
    //  replyWaiter.wait(&replyMtx, 30000);
    //  replyMtx.unlock();
    //  QList<QStringList> lst = worker->getReply();
    //  Debug << "Got " << lst.size() << " PVs; inserting to hash";
    //  for (int i=0; i<lst.size(); ++i)
    //  {
    //    QStringList tokens = lst.at(i).simplified().split(" ");
    //    if(tokens.size() == 7)
    //    {
    //      quint64 idf = tokens.at(2).toULongLong();
    //      PVariable var = parseDATReply(tokens);
    //      PVCache.insert(idf, var);
    //    }
    //    else
    //    {
    //      if(tokens.size()>7 && tokens.at(3).startsWith("T"))
    //      {
    //        //        Debug << tokens;
    //      }

    //    }
    //  }
    qDebug() << "Inserted " << PVCache.size() << " PVs into hash";
}

QString T2CManager::resolveName(quint64 idf)
{
    QString n;
    if(T2Cready)
    {
        QString cmd = QString("DAS %1").arg(idf);
        worker->sendCommand(cmd.toLocal8Bit(), &replyWaiter);
        replyMtx.lock();
        replyWaiter.wait(&replyMtx, 5000);
        replyMtx.unlock();
        QList<QList<QByteArray> > tokens = worker->getReply();

        if(tokens.size() != 1)
        {
            qDebug() << tokens.size() << " is wrong reply size!";
            return n;
        }
        QByteArray rep = tokens.first().first();

        qDebug() << codec->toUnicode(rep.remove(0, 8).simplified());
    }
    return n;
}

QString T2CManager::encoding() const
{
    return QString(codec->name());
}

void T2CManager::onValueChange()
{
    QList<QList<QByteArray> > reply = worker->getReply();
    for(int i=0; i<reply.size(); ++i)
    {
        const QList<QByteArray> &tokens = reply.at(i);
        if(checkDATReply(tokens))
        {
            quint64 idf;
            PVariable var = parseDATReply(tokens, idf);
            PVCache[idf] = var;
            qDebug() << "PV \"" << idf << "\" changed to " << tokens.at(4);
            emit valueUpdated(idf);
        }
    }
}

void T2CManager::setEncoding(QString arg)
{

    codec = QTextCodec::codecForName(arg.toLocal8Bit());

}

PVariable T2CManager::parseDATReply(const QList<QByteArray>& replyTokens, quint64& idf)
{
    PVariable var;
    QByteArray timestamp = replyTokens[6];
    var.time = QDateTime::fromMSecsSinceEpoch(timestamp.replace(".", "").toLongLong());
    var.vtype = replyTokens.at(3);
    if(replyTokens.at(3).startsWith("I"))
    {
        var.value = replyTokens.at(4).toInt();
    }
    if(replyTokens.at(3) == "F32")
    {
        var.value = replyTokens.at(4).toFloat();
    }
    if(replyTokens.at(3) == "F64")
    {
        var.value = replyTokens.at(4).toDouble();
    }
    if(replyTokens.at(3).startsWith("B"))
    {
        QBitArray bits;
        bits.resize(replyTokens.at(3).right(2).toInt());
        //    Debug << "got bitarray size=" << bits.size();
        var.value = bits;
    }
    if(replyTokens.at(3).startsWith("T"))
    {
        var.value = codec->toUnicode(replyTokens.at(4));
        //    Debug << "Got text " <<  var.value.toString() << " idf=" << replyTokens.at(2);
    }
    QByteArray ststr = replyTokens.at(5);
    var.status = 0;
    for(int i=0; i<ststr.size(); ++i)
    {
        if(quint8(ststr.at(i)) != 95)
            var.status += 0x1 << i;
    }
    idf = replyTokens[2].toULongLong();
    //  Debug << "idf=" << replyTokens.at(2)
    //        << " type=" << var.value.typeName()
    //        << " status=" << QString::number(var.status, 16).prepend("0x").toStdString().c_str()
    //        << " value=" << replyTokens.at(4).toStdString().c_str();
    return var;
}

bool T2CManager::checkDATReply(const QList<QByteArray> &tokens)
{
    bool ok = false;
    if(tokens.size() == 7)
        ok = true;
    return ok;
}

QString T2CManager::makeUpdateCmd(quint64 idf, QVariant newValue, bool setStatus, bool setTime)
{
    QString cmd;
    if(!PVCache.contains(idf))
    {
        probePVariable(idf, true);
    }
    PVariable& v = PVCache[idf];
    QString strval;
    bool ok = false;
    if(v.vtype == "I08" || v.vtype == "I16" || v.vtype == "I32")
        strval = QString::number(newValue.toInt(&ok));
    if(v.vtype == "F32")
        strval = QString::number(newValue.toFloat(&ok));
    if(v.vtype == "F64")
        strval = QString::number(newValue.toDouble(&ok));
    if(v.vtype == "T16" || v.vtype == "T32" || v.vtype == "T80")
        strval = newValue.toString();

    cmd = QString("DAT 0 %1 %2 %3").arg(idf).arg(v.vtype.data()).arg(strval).toLocal8Bit();
    if(setStatus || setTime)
    {
        QByteArray statuses = "ZRAEBGTN";
        QByteArray s;
        for(int i=0; i<8; ++i)
        {
            if(v.status & (1<<i))
            {
                s.append(statuses[i]);
            }
            else
            {
                s.append("_");
            }

        }
        //    Debug << "Status string=" << s << " status = " << QString::number(v.status,2);
        cmd.append(" ").append(s);

        if(setTime)
        {
            QString timestamp = QString::number(v.time.toMSecsSinceEpoch());
            timestamp.insert(timestamp.size()-3, ".");
            //      Debug << "Timestamp string=" << timestamp;
            cmd.append(" ").append(timestamp);
        }

    }
    if(!ok)
    {
        qDebug() << "Incopatible type " << newValue.typeName() << ", the source value type is " << v.vtype;
        return QString();
    }
    //  Debug << cmd;
    return cmd;
}

