#include "uf2device.h"
#include <QTextCodec>
UF2DeviceFactory* UF2DeviceFactory::self = new UF2DeviceFactory();
QT_USE_NAMESPACE_SERIALPORT

bool operator ==(const UF2Header &l, const UF2Header &r)
{
    return (l.packetNo == r.packetNo) && (r.command.cmd == l.command.cmd) && (r.command.cmd1 == r.command.cmd1) &&
            (l.deviceId == r.deviceId) && (l.deviceType == r.deviceType) && (l.interval.iType == r.interval.iType) &&
            (l.interval.iCount == r.interval.iCount);
}
//inline QDebug &operator<<(const UF2Header & t) { stream->ts  << '\"' << t << '\"'; return maybeSpace(); }
const ParseFunc UF2Device::parsers[] = {UF2Device::dummyParser,     // 0
                                        UF2Device::parse1,          // 1
                                        UF2Device::parse1,          // 2
                                        UF2Device::parse3,          // 3
                                        UF2Device::dummyParser,     // 4
                                        UF2Device::dummyParser,     // 5
                                        UF2Device::dummyParser,     // 6
                                        UF2Device::dummyParser,     // 7
                                        UF2Device::dummyParser,     // 8
                                        UF2Device::dummyParser,     // 9
                                        UF2Device::dummyParser,     // 10
                                        UF2Device::dummyParser,     // 11
                                        UF2Device::dummyParser,     // 12
                                        UF2Device::dummyParser,     // 13
                                        UF2Device::parse14,         // 14
                                        UF2Device::parse15,         // 15
                                        UF2Device::dummyParser,     // 16
                                        UF2Device::dummyParser,     // 17
                                        UF2Device::dummyParser,     // 18
                                        UF2Device::dummyParser,     // 19
                                        UF2Device::dummyParser,     // 20
                                        UF2Device::dummyParser,     // 21
                                        UF2Device::dummyParser,     // 22
                                        UF2Device::dummyParser,     // 23
                                        UF2Device::parse24,         // 24
                                        UF2Device::parse24,         // 25
                                        UF2Device::dummyParser,     // 26
                                        UF2Device::parse27,         // 27
                                        UF2Device::dummyParser,     // 28
                                        UF2Device::dummyParser,     // 29
                                        UF2Device::dummyParser,     // 30
                                        UF2Device::parse31,         // 31
                                        UF2Device::dummyParser,     // 32
                                        UF2Device::dummyParser,     // 33
                                        UF2Device::dummyParser,     // 34
                                        UF2Device::dummyParser};    // 35

void setUF2Stream(QDataStream& stream)
{
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
}

QStringList UF2Device::UF2Errors = QStringList() ;
const QStringList UF2Device::phases = QStringList() << "" << "a" << "b" << "c" << "";
const QStringList UF2Device::interphases = QStringList() << "" << "ab" << "bc" << "ca";


QDataStream & operator>> (QDataStream& stream, UF2HarmonicData& t)
{
    stream >> t.v_Ua >> t.v_Ub >> t.v_Uc >> t.v_Ia >> t.v_Ib >> t.v_Ic;
    return stream;
}

QDataStream & operator<< (QDataStream& stream, const UF2HarmonicData& t)
{
    stream << t.v_Ua << t.v_Ub << t.v_Uc << t.v_Ia << t.v_Ib << t.v_Ic;
    return stream;
}
QDataStream & operator<< (QDataStream& stream, const UF2TimeStamp& t)
{
    stream << t.hour << t.min << t.sec << t.day << t.month << t.year;
    return stream;
}

QDataStream & operator>> (QDataStream& stream, UF2TimeStamp& t)
{
    stream >> t.hour >> t.min >> t.sec >> t.day >> t.month >> t.year;
    return stream;
}

QDataStream & operator<< (QDataStream& stream, const UF2Mv& t)
{
    stream << t.fundamental << t.rms << t.angle;
    return stream;
}

QDataStream & operator>> (QDataStream& stream, UF2Mv& t)
{
    stream >> t.fundamental >> t.rms >> t.angle;
    return stream;
}
QDataStream & operator<< (QDataStream& stream, const HData& t)
{
    stream << t.r << t.i;
    return stream;
}

QDataStream & operator>> (QDataStream& stream, HData& t)
{
    stream >> t.r >> t.i;
    return stream;
}

QDataStream & operator<< (QDataStream& stream, const UF2Frame27_2_3& t)
{
    stream << t.Ua << t.Ub << t.Uc << t.Uab << t.Ubc << t.Uca << t.Uzs << t.Ups << t.Uns << t.Ia << t.Ib << t.Ic << t.Izs << t.Ips << t.Ins << t.frequency << t.Kns_ZS << t.Kns_NS << t.THD_Ua << t.THD_Ub << t.THD_Uc << t.THD_Uab << t.THD_Ubc << t.THD_Uca;
    for(int i=0; i<39; ++i) stream << t.HD_Ua[i];
    for(int i=0; i<39; ++i) stream << t.HD_Ub[i];
    for(int i=0; i<39; ++i) stream << t.HD_Uc[i];
    for(int i=0; i<39; ++i) stream << t.HD_Uab[i];
    for(int i=0; i<39; ++i) stream << t.HD_Ubc[i];
    for(int i=0; i<39; ++i) stream << t.HD_Uca[i];
    stream << t.THD_Ia << t.THD_Ib << t.THD_Ic;
    for(int i=0; i<39; ++i) stream << t.HD_Ia[i];
    for(int i=0; i<39; ++i) stream << t.HD_Ib[i];
    for(int i=0; i<39; ++i) stream << t.HD_Ic[i];
    stream << t.Pa_f << t.Pb_f << t.Pc_f << t.Pa_eff << t.Pb_eff << t.Pc_eff;
    return stream;
}

QDataStream & operator>> (QDataStream& stream, UF2Frame27_2_3& t)
{
    stream >> t.Ua >> t.Ub >> t.Uc >> t.Uab >> t.Ubc >> t.Uca >> t.Uzs >> t.Ups >> t.Uns >> t.Ia >> t.Ib >> t.Ic >> t.Izs >> t.Ips >> t.Ins >> t.frequency >> t.Kns_ZS >> t.Kns_NS >> t.THD_Ua >> t.THD_Ub >> t.THD_Uc >> t.THD_Uab >> t.THD_Ubc >> t.THD_Uca;
    for(int i=0; i<39; ++i) stream >> t.HD_Ua[i];
    for(int i=0; i<39; ++i) stream >> t.HD_Ub[i];
    for(int i=0; i<39; ++i) stream >> t.HD_Uc[i];
    for(int i=0; i<39; ++i) stream >> t.HD_Uab[i];
    for(int i=0; i<39; ++i) stream >> t.HD_Ubc[i];
    for(int i=0; i<39; ++i) stream >> t.HD_Uca[i];
    stream >> t.THD_Ia >> t.THD_Ib >> t.THD_Ic;
    for(int i=0; i<39; ++i) stream >> t.HD_Ia[i];
    for(int i=0; i<39; ++i) stream >> t.HD_Ib[i];
    for(int i=0; i<39; ++i) stream >> t.HD_Ic[i];
    stream >> t.Pa_f >> t.Pb_f >> t.Pc_f >> t.Pa_eff >> t.Pb_eff >> t.Pc_eff;
    return stream;
}



QDataStream & operator<< (QDataStream& stream, const UF2Header& t)
{
    stream << t.packetNo << t.reserve << t.dataSize << t.command.cmd << t.command.cmd1 << t.deviceId << t.deviceType << t.interval.iType << t.interval.iCount << t.startTime << t.dataCount << t.errorCode;
    return stream;
}

QDataStream & operator>> (QDataStream& stream, UF2Header& t)
{
    stream >> t.packetNo >> t.reserve >> t.dataSize >> t.command.cmd >> t.command.cmd1 >> t.deviceId >> t.deviceType >> t.interval.iType >> t.interval.iCount >> t.startTime >> t.dataCount >> t.errorCode;
    return stream;
}

quint8 fromBCD(quint8 bcdEncoded)
{
    return (bcdEncoded >> 4)*10+(bcdEncoded & 0x0f);
}

quint8 toBCD(quint8 decimal)
{
    quint8 d2 = decimal / 10;
    quint8 d1 = decimal % 10;
    return (d2 << 4) + d1;
}

QDebug operator<< ( QDebug s, const UF2Header & h )
{
    s.nospace() << "packetNo:" << h.packetNo << " dataSize:" << h.dataSize << " cmd:" << h.command.cmd << "/" << h.command.cmd1 << " deviceId:" << h.deviceId << " interval:" << h.interval.iType << "|" << h.interval.iCount << " startTime:" << fromBCD(h.startTime.day) << "." << fromBCD(h.startTime.month) << "." << fromBCD(h.startTime.year) << " " << fromBCD(h.startTime.hour) << ":" << fromBCD(h.startTime.min) << ":" << fromBCD(h.startTime.sec) << " dataCount:" << h.dataCount << " errorCode:" << h.errorCode;
    return s.maybeSpace();
}

QString dumpBa(const QByteArray& data)
{
    QString output;
    QString line;
    for(int i=0; i<data.size(); ++i) {
        line.append(QString::number(*(quint8*)(data.data()+i), 'g', 2)).append(" ");
        int lineNo = i >> 3;
        static int lastLine;
        if(lastLine != lineNo) {
            output.append(line + "\n");
            line.clear();
            lastLine = lineNo;
        }
    }
    return output;
}

quint8 UF2CRC (const QByteArray& data)
{
    quint8 crc = 0;
    for(int i=0; i<data.size(); ++i)
    {
        crc ^= *(data.data()+i);
        bool r = crc & 0x1;
        crc = crc >> 1;
        if(r) {
            crc |= 0x80;
            crc ^= 0x1E;
        }
    }
    crc ^= 0xeb;
    return crc;
}

int UF2DeviceFactory::init(QObject *core)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    c = (Core*)core;
    c->log(0, self, "UF2 plugin initialized на русском");
    UF2Device::UF2Errors.clear();
    UF2Device::UF2Errors << "Нет ошибки" << "резерв" << "неверно поле Cmd" << "неверно поле Cmd1" << "неверный интервал измерения для параметра"
                         << "нет данных (не было пуска)" << "логическая ошибка данных нет" << "протокол провалов и перенапряжений пуст"
                         << "неверное время начала данных (больше текущего времени прибора)" << "запрашиваемых данных не нашел (возможно прибор не работал)"
                         << "прибор занят" << "с указанного времени и до конца суток нет данных" << "параметр не сохраняется с усреднением 1 мин"
                         << "суточных данных не нашёл" << "30 минутных данных не нашел" << "некорректный поиск 30 мин интервалов"
                         << "время в запросе слишком мало (не должно быть меньше времени пуска)" << "суток со статистическими данными не нашёл"
                         << "после указанной даты калибровка не производилась" << "в данных о протоколе содержится последнее сообщение"
                         << "для энергии расчетный период является текущим" << "для энергии сутки являются текущими"
                         << "для энергии 30 мин являются текущими" << "начинается передача данных, средних за 160 мс"
                         << "начинается передача данных, средних за 10 мс" << "данных в протоколе за запрашиваемый интервал нет"
                         << "в ответе последние данные из протокола" << "данных в протоколе за запрашиваемый интервал нет"
                         << "в ответе последние данные из протокола" << "не нашел суток с гистограммами";
    QScriptEngine *e = c->engine();
    UF2Device::bindToScriptEngine(e);
    return 0;

}

QVariantHash UF2DeviceFactory::getInfo()
{
    QVariantHash i;
    i.insert("Name", "UF2 Protocol module");
    i.insert("Version", 100);
    i.insert("Author", "r0mko");
    return i;
}


QObject *UF2DeviceFactory::newInstance()
{
    UF2Device* o = new UF2Device(c);
    c->registerObject(o);
    return o;
}

void UF2Device::bindToScriptEngine(QScriptEngine *engine)
{
    qDebug() << "Creating binding";
    QScriptValue metaObject = engine->newQMetaObject(&UF2Device::staticMetaObject, engine->newFunction(UF2Device::JSCtor));
    engine->globalObject().setProperty("UF2", metaObject);

}

QScriptValue UF2Device::JSCtor(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(context)
    QObject* dev = UF2DeviceFactory::instance()->newInstance();
    QScriptValue c = engine->newQObject(dev, QScriptEngine::ScriptOwnership);
    c.setProperty("query", engine->newFunction(UF2Device::sendQuery));
    return c;
}

QByteArray dateTimeToUF2(const QDateTime& ts)
{
    QByteArray ba;
    if(!ts.isValid()) {
        return ba;
    }
    quint8 d, m, y, h, min, sec;
    d = toBCD(ts.date().day());
    m = toBCD(ts.date().month());
    y = toBCD(ts.date().year());
    h = toBCD(ts.time().hour());
    min = toBCD(ts.time().minute());
    sec = toBCD(ts.time().second());
    QDataStream out (&ba, QIODevice::WriteOnly);
    out << d << m << y << h << min << sec;
    return ba;
}

PortResult UF2Device::query(UF2Header& replyHeader, QByteArray& replyData, const UF2Header& sendHeader, const QByteArray& sendData)
{
    if(!sPort.isOpen())
        return PortNotOpen;
    QByteArray out;
    QDataStream s(&out, QIODevice::WriteOnly);
    s.setByteOrder(QDataStream::LittleEndian);
    s << sendHeader;
    qDebug() << sendHeader;
    if(!sendData.isEmpty()) {
        s.writeRawData(sendData.constData(), sendData.size());
    }
    s << UF2CRC(out);
    sPort.write(out);
    if(!sPort.waitForBytesWritten(1000))
        return WriteError;
    if(sPort.waitForReadyRead(500)) {
        int counter = 100;
        qint64 lastBytesAvailable = sPort.bytesAvailable();
        while(counter && (lastBytesAvailable < (qint64)sizeof(UF2Header))) {
            sPort.waitForReadyRead(1);
            qint64 newBA = sPort.bytesAvailable() ;
            if(newBA != lastBytesAvailable)
                lastBytesAvailable = newBA;
            else counter--;

        }
        if(!counter) {
            sPort.readAll();
            return HeaderReadTimeout;
        }
        QByteArray baHeader = sPort.read(sizeof(UF2Header));
        QDataStream h_in(baHeader);
        h_in.setByteOrder(QDataStream::LittleEndian);
        h_in >> replyHeader;
        // вызов функции по указателю из массива парсеров
        if(sendHeader == replyHeader) // заголовки совпадают
        {
            int counter=1000;
            while(counter-- && (sPort.bytesAvailable() < replyHeader.dataSize+1)) sPort.waitForReadyRead(1);
            if(!counter) {
                sPort.readAll();
                return DataReadTimeout;
            }
            replyData = sPort.read(replyHeader.dataSize);
            quint8 crc, calcCRC;
            sPort.read((char*)&crc, 1);
            calcCRC = UF2CRC(baHeader.append(replyData));
            if(crc != calcCRC)
                return CRCError;
            return NoError;
        } else {
            LOG_CORE(QString("Ошибка данных! В буфере %0 байт.").arg(sPort.bytesAvailable()));
            sPort.readAll();
            return DataCorrupt;
        }
    }
    else
        return HeaderReadTimeout;
}


QScriptValue UF2Device::sendQuery(QScriptContext *context, QScriptEngine *engine)
{
    static quint16 packetCounter;
    UF2Device* that = qobject_cast<UF2Device*>(context->thisObject().toQObject());
    UF2Header header;
    header.packetNo = packetCounter++;
    header.deviceId = that->m_deviceId;
    QScriptValue qobj = context->argument(0);
    QScriptValue command = qobj.property("cmd");
    if(!command.isValid()) {
        context->throwError("Command expected! (\"cmd\" property missing)");
        return QScriptValue::UndefinedValue;
    }
    else {
        header.command.cmd = command.property(0).toInt32();
        header.command.cmd1 = command.property(1).toInt32();
    }
    QScriptValue interval = qobj.property("int");
    if(!interval.isValid())  {
        interval = engine->newArray();
        interval.setProperty(0, 0);
        interval.setProperty(1, 0);
    }
    header.interval.iType = interval.property(0).toInt32();
    header.interval.iCount = interval.property(1).toInt32();
    QScriptValue startTime = qobj.property("from");
    if(!startTime.isValid() || !startTime.isDate()) {
        header.startTime.hour = 0;
        header.startTime.min = 0;
        header.startTime.sec = 0;
        header.startTime.day = 0;
        header.startTime.month = 0;
        header.startTime.year = 0;
    }
    else {
        QDateTime d = startTime.toDateTime();
        header.startTime.hour = toBCD(d.time().hour());
        header.startTime.min = toBCD(d.time().minute());
        header.startTime.sec = toBCD(d.time().second());
        header.startTime.day = toBCD(d.date().day());
        header.startTime.month = toBCD(d.date().month());
        header.startTime.year = toBCD(d.date().year()-2000);
    }
    QScriptValue dataCount = qobj.property("count");
    if(!dataCount.isValid())
        header.dataCount = 0;
    else
        header.dataCount = dataCount.toInt32();
    QByteArray sendData;
    if(header.command.cmd == 31 && header.command.cmd1 == 2) { // установка времени
        sendData = dateTimeToUF2(context->argument(1).toDateTime());
        if(sendData.isEmpty()){
            context->throwError(tr("неправильный формат времени"));
            return QScriptValue::UndefinedValue;
        }
    }
    UF2Header replyHeader;
    QByteArray replyData;
    switch(that->query(replyHeader, replyData, header, sendData))
    {
    case NoError:
        return ( *UF2Device::parsers[replyHeader.command.cmd] ) ( replyHeader, replyData, engine, context );
        break;
    case PortNotOpen:
        context->throwError(QString("порт не открыт!"));
        return QScriptValue::UndefinedValue;
        break;
    case HeaderReadTimeout:
        context->throwError(QString("устройство не отвечает!"));
        return QScriptValue::UndefinedValue;
        break;
    case DataReadTimeout:
        context->throwError(QString("превышен интервал ожидания данных!"));
        return QScriptValue::UndefinedValue;
        break;
    case DataCorrupt:
        context->throwError(QString("ошибка данных!"));
        return QScriptValue::UndefinedValue;
        break;
    case CRCError:
        context->throwError(QString("ошибка контрольной суммы!"));
        return QScriptValue::UndefinedValue;
        break;
    default:
        return QScriptValue::UndefinedValue;
        break;
    }
}



bool UF2Device::openDevice(const QString &portName, int baudrate)
{

    sPort.setPort(portName);
    sPort.setBaudRate(baudrate);
    sPort.setDataTerminalReady(true);
    sPort.setDataBits(QSerialPort::Data8);
    sPort.setStopBits(QSerialPort::OneStop);
    sPort.setParity(QSerialPort::NoParity);
    sPort.setFlowControl(QSerialPort::HardwareControl);
    qDebug() << (int)sPort.dataBits();
//    sPort.setDataBits(8);
//    sPort.setParity(1);
    return sPort.open(QIODevice::ReadWrite);

}

QDateTime dateTimeFromBCD(quint8 day, quint8 month, quint8 year, quint8 hour, quint8 min, quint8 sec)
{
    QDate cDate(fromBCD(year)+2000, fromBCD(month), fromBCD(day));
    QTime cTime(fromBCD(hour), fromBCD(min), fromBCD(sec));
    return QDateTime(cDate, cTime);
}

QScriptValue UF2Device::dummyParser(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx)
{
    Q_UNUSED(ctx)
    QScriptValue dataArray = e->newArray();
    for(int i=0; i<data.size(); ++i)
        dataArray.setProperty(i, QScriptValue((quint8)data.at(i)));
    QScriptValue comment(QString("Message type %0/%1 not implemented yet.").arg(h.command.cmd).arg(h.command.cmd1));
    QScriptValue ret = e->newObject();
    ret.setProperty("rawData", dataArray);
    ret.setProperty("comment", comment);
    return ret;
}

QScriptValue UF2Device::parse3(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx)
{
    Q_UNUSED(ctx)
    Q_UNUSED(h)
    QDataStream in(data);
    in.setByteOrder(QDataStream::LittleEndian);
    quint16 f;
    in >> f;
    qDebug() << (double)f / 100;
    QScriptValue ret = e->newObject();
    QScriptValue dataArray = e->newArray();
    for(int i=0; i<data.size(); ++i)
        dataArray.setProperty(i, QScriptValue((quint8)data.at(i)));
    ret.setProperty("rawData", dataArray);
    ret.setProperty("f", QScriptValue((double)f/100));
    return ret;
}

QScriptValue UF2Device::parse1(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx)
{
    Q_UNUSED(ctx)
    Q_UNUSED(h)
    QDataStream in(data);
    //    in.setVersion(QDataStream::Qt_1_0);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in.setByteOrder(QDataStream::LittleEndian);
    float v, v1;
    qint16 fi;
    in >> v >> v1 >>fi;
    qDebug() << v << v1 << (double)fi*0.01;
    QScriptValue ret = e->newObject();
    QScriptValue dataArray = e->newArray();
    for(int i=0; i<data.size(); ++i)
        dataArray.setProperty(i, QScriptValue((quint8)data.at(i)));
    ret.setProperty("rawData", dataArray);
    ret.setProperty("U50", v);
    ret.setProperty("Uдейств", v1);
    ret.setProperty("Fi", (double)fi*0.01);
    return ret;
}
QScriptValue UF2Device::parse14(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx)
{
    Q_UNUSED(ctx)
    Q_UNUSED(h)
    QDataStream in(data);
    //    in.setVersion(QDataStream::Qt_1_0);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in.setByteOrder(QDataStream::LittleEndian);
    float KDF, KDU, KDI;
    in >> KDF >> KDU >> KDI;
    QScriptValue ret = e->newObject();
    ret.setProperty("KDF", KDF);
    ret.setProperty("KDU", KDU);
    ret.setProperty("KDI", KDI);
    return ret;

}

QScriptValue UF2Device::parse15(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx)
{
    Q_UNUSED(ctx)
    Q_UNUSED(h)
    QDataStream in(data);
    in.setByteOrder(QDataStream::LittleEndian);
    qint8 day, month, year, hour, min, sec, running, hasData, startDay, startMonth, startYear, startHour, startMin, startSec, calcDay, calcMonth, calcYear, calcHour, calcMin, calcSec;
    in >> day >> month >> year >> hour >> min >> sec >> running >> hasData >> startDay >> startMonth >> startYear >> startHour >> startMin >> startSec >> calcDay >> calcMonth >> calcYear >> calcHour >> calcMin >> calcSec;
    QDateTime cTimeStamp = dateTimeFromBCD(year, month, day, hour, min, sec);
    QDateTime stTimeStamp = dateTimeFromBCD(startYear, startMonth, startDay, startHour, startMin, startSec);
    QDateTime calcTimeStamp = dateTimeFromBCD(calcYear, calcMonth, calcDay, calcHour, calcMin, calcSec);
    char fwVersion[8];
    char fwDate[8];
    char fwTime[8];
    in.readRawData(&fwVersion[0], 8);
    in.readRawData(&fwDate[0], 8);
    in.readRawData(&fwTime[0], 8);
    QScriptValue ret = e->newObject();
    ret.setProperty("currentTime", e->newDate(cTimeStamp));
    ret.setProperty("startTime", e->newDate(stTimeStamp));
    ret.setProperty("calcTime", e->newDate(calcTimeStamp));
    ret.setProperty("firmwareVersion", QScriptValue(QString(QByteArray::fromRawData(&fwVersion[0], 8))));
    ret.setProperty("firmwareDateTime", QScriptValue(QString(QByteArray::fromRawData(&fwDate[0], 8)).append(" ").append(QString(QByteArray::fromRawData(&fwTime[0], 8)))));
    ret.setProperty("isRunning", running!=0);
    ret.setProperty("hasData", hasData!=0);
    return ret;
}

QScriptValue UF2Device::parse24(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx)
{
    // var v = r.query({cmd:[24,1], int:[2,3]})
    // var arr=new Array();
    // for(var i=0; i<1000; i++) { arr.push(r.query({cmd:[1,1], int:[2,3]})) }
    //

    Q_UNUSED(ctx)
    Q_UNUSED(h)
    QDataStream in(data);
    //    setUF2Stream(in);
    //
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in.setByteOrder(QDataStream::LittleEndian);
    float v1, v2;
    in >> v1 >> v2;
    QString v1name, v2name;
    v2name = "Value2";
    if(h.command.cmd == 24)
        v1name = "P";
    if(h.command.cmd == 25)
        v1name = "Q";
    v1name.append(UF2Device::phases[h.command.cmd1]);
    Q_CHECK_PTR(e);
    QScriptValue ret = e->newObject();
    ret.setProperty(v1name, v1);
    //    ret.setProperty(v2name, v2);
    QScriptValue dataArray = e->newArray();
    for(int i=0; i<data.size(); ++i)
        dataArray.setProperty(i, QScriptValue((quint8)data.at(i)));
    ret.setProperty("rawData", dataArray);
    return ret;
}

QScriptValue UF2Device::parse31(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx)
{
    Q_UNUSED(ctx)
    Q_UNUSED(data)
    Q_UNUSED(e)
    QDateTime time = dateTimeFromBCD(h.startTime.day, h.startTime.month, h.startTime.year, h.startTime.hour, h.startTime.min, h.startTime.sec);
    LOG_CORE(QString("Время прибора скорректировано: %1").arg(time.toString("d.MM.yyyy h:mm:ss")));
    return QScriptValue(QString("Время прибора скорректировано: %1").arg(time.toString("d.MM.yyyy h:mm:ss")));

}

QScriptValue UF2Device::parse27(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx)
{
    // var vl = r.query({cmd:[27,1], int:[2,3]});
    Q_UNUSED(ctx)
    Q_UNUSED(h)
    QDataStream in(data);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    UF2Frame27_2_3 f;
    in >> f;
    QScriptValue o = e->newObject();
    o.setProperty("Ua_f",f.Ua.fundamental);
    o.setProperty("Ua_rms",f.Ua.rms);
    o.setProperty("Ub_f",f.Ub.fundamental);
    o.setProperty("Ub_rms",f.Ub.rms);
    o.setProperty("Uc_f",f.Uc.fundamental);
    o.setProperty("Uc_rms",f.Uc.rms);
    o.setProperty("Uab_f",f.Uab.fundamental);
    o.setProperty("Uab_rms",f.Uab.rms);
    o.setProperty("Ubc_f",f.Ubc.fundamental);
    o.setProperty("Ubc_rms",f.Ubc.rms);
    o.setProperty("Uca_f",f.Uca.fundamental);
    o.setProperty("Uca_rms",f.Uca.rms);
    o.setProperty("Uzs",f.Uzs.rms);
    o.setProperty("Ups",f.Ups.rms);
    o.setProperty("Uns",f.Uns.rms);
    o.setProperty("Ia_rms", f.Ia.rms);
    o.setProperty("Ia_f", f.Ia.fundamental);
    o.setProperty("Ib_rms", f.Ib.rms);
    o.setProperty("Ib_f", f.Ib.fundamental);
    o.setProperty("Ic_rms", f.Ic.rms);
    o.setProperty("Ic_f", f.Ic.fundamental);
    o.setProperty("Izs", f.Izs.fundamental);
    o.setProperty("Ips", f.Ips.fundamental);
    o.setProperty("Ins", f.Ins.fundamental);
    o.setProperty("frequency", (qreal)f.frequency/100);
    o.setProperty("Kns_ZS", f.Kns_ZS);
    o.setProperty("Kns_NS", f.Kns_NS);
    o.setProperty("THD_Ua", f.THD_Ua);
    o.setProperty("THD_Ub", f.THD_Ub);
    o.setProperty("THD_Uc", f.THD_Uc);
    o.setProperty("THD_Uab", f.THD_Uab);
    o.setProperty("THD_Ubc", f.THD_Ubc);
    o.setProperty("THD_Uca", f.THD_Uca);
    o.setProperty("Kns_ZS", (qreal)f.Kns_ZS/100);
    o.setProperty("Kns_NS", (qreal)f.Kns_NS/100);
    o.setProperty("THD_Ua", (qreal)f.THD_Ua/100);
    o.setProperty("THD_Ub", (qreal)f.THD_Ub/100);
    o.setProperty("THD_Uc", (qreal)f.THD_Uc/100);
    o.setProperty("THD_Uab", (qreal)f.THD_Uab/100);
    o.setProperty("THD_Ubc", (qreal)f.THD_Ubc/100);
    o.setProperty("THD_Uca", (qreal)f.THD_Uca/100);
    o.setProperty("THD_Ia", (qreal)f.THD_Ia/100);
    o.setProperty("THD_Ib", (qreal)f.THD_Ib/100);
    o.setProperty("THD_Ic", (qreal)f.THD_Ic/100);
    QScriptValue HD_Ua = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Ua[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Ua[i].r/100);
        HD_Ua.setProperty(i, hdata);
    }
    QScriptValue HD_Ub = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Ub[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Ub[i].r/100);
        HD_Ub.setProperty(i, hdata);
    }
    QScriptValue HD_Uc = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Uc[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Uc[i].r/100);
        HD_Uc.setProperty(i, hdata);
    }
    QScriptValue HD_Uab = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Uab[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Uab[i].r/100);
        HD_Uab.setProperty(i, hdata);
    }
    QScriptValue HD_Ubc = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Ubc[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Ubc[i].r/100);
        HD_Ubc.setProperty(i, hdata);
    }
    QScriptValue HD_Uca = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Uca[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Uca[i].r/100);
        HD_Uca.setProperty(i, hdata);
    }
    QScriptValue HD_Ia = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Ia[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Ia[i].r/100);
        HD_Ia.setProperty(i, hdata);
    }
    QScriptValue HD_Ib = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Ib[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Ib[i].r/100);
        HD_Ib.setProperty(i, hdata);
    }
    QScriptValue HD_Ic = e->newArray();
    for(int i=0; i<39; ++i) {
        QScriptValue hdata = e->newObject();
        hdata.setProperty("i", (qreal)f.HD_Ic[i].i/100);
        hdata.setProperty("r", (qreal)f.HD_Ic[i].r/100);
        HD_Ic.setProperty(i, hdata);
    }
    o.setProperty("HD_Ua", HD_Ua);
    o.setProperty("HD_Ub", HD_Ub);
    o.setProperty("HD_Uc", HD_Uc);
    o.setProperty("HD_Uab", HD_Uab);
    o.setProperty("HD_Ubc", HD_Ubc);
    o.setProperty("HD_Uca", HD_Uca);
    o.setProperty("HD_Ia", HD_Ia);
    o.setProperty("HD_Ib", HD_Ib);
    o.setProperty("HD_Ic", HD_Ic);
    o.setProperty("Pa_f", f.Pa_f);
    o.setProperty("Pb_f", f.Pb_f);
    o.setProperty("Pc_f", f.Pc_f);
    o.setProperty("Pa_eff", f.Pa_eff);
    o.setProperty("Pb_eff", f.Pb_eff);
    o.setProperty("Pc_eff", f.Pc_eff);
    return o;
}


void UF2Device::readData()
{
    //    qDebug() << sPort.readAll().size();
    if(m_CycleMode160ms) {
        qint64 size = 819;
        while(sPort.bytesAvailable() < size)
            sPort.waitForReadyRead(1);
        QByteArray ba = sPort.read(size);
        //        qDebug() << ba.toBase64();
        QDataStream in(ba);

        //        r.query({cmd:[27,1], int:[1,160]})
        QDebug dbgOut(QtDebugMsg);
        while(!in.atEnd()) {
            quint16 f;
            in >> f;
            dbgOut << QString::number((qreal)f/100, 'f', 4) << "\t";
        }

        //        qDebug() << "freq =" << (qreal)freq/100;
        //        for(int i=0; i<82; ++i) {
        //            UF2HarmonicData h;
        //            in >> h;
        //            if(i==1 || i==0 || i==2) qDebug() << i << h.v_Ua << h.v_Ub << h.v_Uc;
        //        }

    }
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2 ( UF2Device, UF2DeviceFactory )
#endif
