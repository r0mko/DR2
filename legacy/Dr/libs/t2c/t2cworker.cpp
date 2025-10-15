#include "t2cworker.h"
//#include <QHostAddress>
#include <QTcpSocket>
//#include <QDateTime>
//#include <QMutex>
#include <QWaitCondition>
//#include <QStringList>
//#include "PVariable.h"
//#include <QTextCodec>
//#include "common.h"
#include <QDebug>

T2CWorker::T2CWorker(QHostAddress addr, quint16 port, QWaitCondition *waiter) :
    t2caddr(addr),
    t2cport(port),
    sWaiter(waiter)
{

}

void T2CWorker::sendCommand(QByteArray command, QWaitCondition *waiter, bool waitForReply)
{
    ready = false;
    replyWaiter = waiter;
    query = command;
    queryTokens.clear();
    replyTokens.clear();
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        //    queryTokens = command.split(" ");
        socket->write(query.append("\n"));
        socket->waitForBytesWritten();
//        Debug << "Sent command " << command;
    }
    if(waitForReply)
        if(replyWaiter)
            replyWaiter->wakeAll();
}

void T2CWorker::stopWorking()
{
    //  std::cout << "Отключаюсь от " << socket->peerAddress().toString().append(":").append(socket->peerPort()).toStdString().c_str();
    socket->disconnectFromHost();
    //  socket->waitForDisconnected();
    delete socket;
}

void T2CWorker::run()
{
    //  Debug << "Initializing TCP-socket";
    socket = new QTcpSocket();
    //  connect(socket, SIGNAL(connected()), this, SLOT(onConnected()), Qt::DirectConnection);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readReply()), Qt::DirectConnection);
    socket->connectToHost(t2caddr, t2cport);
    if(socket->waitForConnected(1000))
    {
        sendCommand("T2C VER");
    }
    exec();
}

QList<QList<QByteArray> > T2CWorker::getReply()
{
    QList<QList<QByteArray> > res = replyTokens;
    replyTokens.clear();
    ready = true;
    query = "";
    return res;
}

// читаем ответ при появлении данных в сокете
void T2CWorker::readReply()
{
    wrongReply = false;
    QList<QByteArray> reply;
    QByteArray replyLine;
    do
    {
        while(socket->canReadLine())
        {
            replyLine = socket->readLine();
            if(replyLine != lastLine)
            {
                reply.append(replyLine);
                 qDebug() << "got line" << replyLine.simplified() << socket->bytesAvailable() << socket->atEnd();
                lastLine = replyLine;
            }
            else
            {
                qDebug() << "Duplicate reply" << replyLine.simplified() << "query is" << query.simplified();
                socket->waitForReadyRead(1000);
            }
        }
        if(!socket->atEnd())
        {
//            qDebug() << "Broken line";
            if(!socket->waitForReadyRead(1000))
            {
                qDebug() << "T2C read error: operation timed out!";
                return;
            }
        }
    } while(!validateReply(reply) && socket->atEnd());
}

bool T2CWorker::validateReply(const QList<QByteArray> &reply)
{
    QList<QList<QByteArray> > fullReply;
    parsingTReply.clear();
    //  qDebug() << reply.size() << query;
    if(query.contains("T2C VER"))
    {
        char delim = '.';
        if(reply.size() == 1 && reply.first().split(delim).size() == 2)
        {
            if(sWaiter)
            {
                sWaiter->wakeAll();
                sWaiter=0;
            }
            return true;
        }
    }
    if(query.startsWith("LST"))
    {
        if(reply.last() == "LST END")
            return true;
    }
    // обработка ответов на запрос имени
    if(query.startsWith("DAS"))
    {

        if(reply.first().startsWith("DAS NAM"))
        {
            replyTokens.append(reply);

            if(replyWaiter)
            {
                replyWaiter->wakeAll();
                replyWaiter = 0;
            }
            return true;
        }
    }
    // обработка ответов на ANM-запросы и спорадических значений ПП
    if(query.isEmpty() || query.startsWith("ANM"))
    {
        bool ok = true;
        bool containsIdf = query.isEmpty(); // если запрос пустой, то ID возвращённых строк заведомо корректны
        for(int i=0; i< reply.size(); ++i)
        {
            QList<QByteArray> rtokens;
            bool result = validateDATReply(reply.at(i).simplified(), &rtokens); // проверяем строчку ответа

            if(parsingTReply.isEmpty()) // если не парсим текстовый многострочный ответ, то учитываем результат
                ok &= result;
            // если валидатор создал токены, добавляем их к ответу
            if(!rtokens.isEmpty())
            {
                //        Debug << rtokens;
                fullReply.append(rtokens);
                if(!query.isEmpty()) // если запрос не пустой, проверяем, совпадают ли id в ответе и запросе
                {
                    containsIdf |= (query.split(' ')[3].simplified() == rtokens[2]);
//                    qDebug() << "Comparing idfs" << query.split(' ')[3].simplified() << rtokens[2] << containsIdf;
                }

            }
        }
        if(!parsingTReply.isEmpty())
            return false;
        if(ok)
        {
            replyTokens = fullReply;
            //      Debug << fullReply;
            if(replyWaiter)
            {
                replyWaiter->wakeAll();
                replyWaiter = 0;
            }
            if(query.isEmpty())
            {
                //        qDebug() << "Spontaneous data";
                emit valueUpdated();
            }
        }
        if(!containsIdf)
        {
            qDebug() << "Idf not found in reply; Wait 100 ms for new data.";
            socket->waitForReadyRead(100);
        }
        return ok && containsIdf;
    }

    // синхронная запись значения
    if(query.startsWith("DAT"))
    {
        // если на команду записи получен подтверждающий ответ
        if(reply.size()==1 && reply.at(0).simplified().contains(query.simplified()))
        {
            //          Debug << "Write succeeded";
            ready = true;
            //          qDebug() << query;
            query = "";
            // Если кто-то ждал подтверждения
            if(replyWaiter)
            {
                replyWaiter->wakeAll();
                replyWaiter = 0;
            }
        }
        return true;
    }

    if(!wrongReply)
    {
        qDebug() << "Unknown reply: " << reply;
        qDebug() << "Last reply: " << lastLine;
        qDebug() << "Query was: " << query.simplified();
        wrongReply = true;
    }
    //    quit();
    return false;
}

// проверка корректности строки ответа
bool T2CWorker::validateDATReply(const QByteArray &replyLine, QList<QByteArray> *replyItems)
{
    //    qDebug() << "Validating line " << replyLine;
    QByteArray line;
    // если уже парсится многострочный текстовый ответ
    if(!parsingTReply.isEmpty())
    {
        // добавляем принятую строку в конец многострочного ответа
        parsingTReply.append(replyLine.simplified()).append("; ");
        // если в принятом тексте обнаружилась кавычка, то, вероятно, это была последняя строка многострочного ответа
        if(!replyLine.contains("\""))
            return false;
        else
        {
            // передаём текст на дальнейший разбор
            line = parsingTReply;
            //      qDebug() << "Got full multiline reply!" << parsingTReply;
            // очищаем многострочный ответ
            parsingTReply.clear();
        }
    }
    else
        line = replyLine;
    QList<QByteArray> tokens = line.simplified().split(' ');
    //  qDebug() << line;
    if(tokens.size() < 4)
        return false;
    //  qDebug() << "Parsing tokens" << tokens.size() << tokens;
    // если тип данных == текст
    if(tokens.at(3).startsWith("T"))
    {
        int qpos1 = line.indexOf("\"");
        int qpos2 = line.lastIndexOf("\"");
        // если в строке всего одна кавычка, значит текст многострочный. Начинаем парсинг многострочного текста.
        if(qpos1 == qpos2)
        {
            parsingTReply = line.append("; ");
            return false;
        }
        // Выкусываем текст между кавычками
        QByteArray text = line.mid(qpos1+1, qpos2-qpos1-1);
        tokens[4] = text;
        // подсчитываем количество слов в тексте
        int wordsCount = text.split(' ').size();
        // убираем лишние токены
        for(int i = 1; i < wordsCount ; ++i)
            if(tokens.size() > 5)
                tokens.removeAt(5);
    }
    // нормальный ответ состоит из семи токенов, последний из которых содержит одну точку:
    // DAT 0 <IDF> <TYPE> <VALUE> <STATUS> <TIME>
    if(tokens.size() == 7 && tokens.last().split('.').size()==2)
    {
        if(replyItems)
            *replyItems = tokens;
        return true;
    }
    else
        return false;
}

quint64 T2CWorker::ANMqueryIdf()
{
    QList<QByteArray> tokens = query.split(' ');

    return tokens.at(3).toULongLong();
    //    return 0;
}

quint64 T2CWorker::DATreplyIdf(const QByteArray &replyLine)
{
    QList<QByteArray> tokens = replyLine.split(' ');
    return tokens.at(2).toULongLong();
    //  return idf;
}


