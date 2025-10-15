#ifndef UF2DEVICE_H
#define UF2DEVICE_H
#include "../Core/plugininterface.h"
#include "../Core/core.h"
#include "UF2Device_global.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#define LOG_CORE(x) UF2DeviceFactory::instance()->core()->log(0, UF2DeviceFactory::instance(), x)
QT_USE_NAMESPACE_SERIALPORT

class UF2DeviceFactory;




struct UF2Mv
{
    float   fundamental;
    float   rms;
    quint16 angle;
};

struct HData
{
    qint16 r;
    qint16 i;
};

struct UF2Frame27_2_3
{
    UF2Mv   Ua;
    UF2Mv   Ub;
    UF2Mv   Uc;
    UF2Mv   Uab;
    UF2Mv   Ubc;
    UF2Mv   Uca;
    UF2Mv   Uzs;
    UF2Mv   Ups;
    UF2Mv   Uns;
    UF2Mv   Ia;
    UF2Mv   Ib;
    UF2Mv   Ic;
    UF2Mv   Izs;
    UF2Mv   Ips;
    UF2Mv   Ins;
    qint16  frequency;
    qint16  Kns_ZS;
    qint16  Kns_NS;
    qint16  THD_Ua;
    qint16  THD_Ub;
    qint16  THD_Uc;
    qint16  THD_Uab;
    qint16  THD_Ubc;
    qint16  THD_Uca;
    HData   HD_Ua[39];
    HData   HD_Ub[39];
    HData   HD_Uc[39];
    HData   HD_Uab[39];
    HData   HD_Ubc[39];
    HData   HD_Uca[39];
    qint16  THD_Ia;
    qint16  THD_Ib;
    qint16  THD_Ic;
    HData   HD_Ia[39];
    HData   HD_Ib[39];
    HData   HD_Ic[39];
    float   Pa_f;
    float   Pb_f;
    float   Pc_f;
    float   Pa_eff;
    float   Pb_eff;
    float   Pc_eff;

};

struct UF2HarmonicData
{
    quint16 v_Ua;
    quint16 v_Ub;
    quint16 v_Uc;
    quint16 v_Ia;
    quint16 v_Ib;
    quint16 v_Ic;
};


struct UF2TimeStamp
{
    quint8  hour;
    quint8  min;
    quint8  sec;
    quint8  day;
    quint8  month;
    quint8  year;
};

struct UF2Header
{
    quint16 packetNo; // 1 - совпадает в запросе и ответе
    quint16 reserve; // 2 - совпадает в запросе и ответе
    quint16 dataSize; // 3
    struct
    {
        quint8  cmd;
        quint8  cmd1;
    } command; // 4 - совпадает в запросе и ответе
    quint16 deviceId; // 5 - совпадает в запросе и ответе
    quint16 deviceType; // 6 - Ресурс-UF2 = 4 - совпадает в запросе и ответе
    struct
    {
        quint8  iType;
        quint8  iCount;
    } interval;  // 7 - совпадает в запросе и ответе
    UF2TimeStamp startTime; // 8
    quint16 dataCount; // 9
    quint16 errorCode; // 10
    UF2Header() {
        packetNo = 0;
        reserve = 0;
        dataSize = 0;
        command.cmd = 0;
        command.cmd1 = 0;
        deviceId = 0;
        deviceType = 4;
        interval.iType = 0;
        interval.iCount = 0;
        startTime.hour = 0;
        startTime.min = 0;
        startTime.sec = 0;
        startTime.day = 0;
        startTime.month = 0;
        startTime.year = 0;
        dataCount = 0;
        errorCode = 0;
    }
};
//QDebug & operator<< ( QDebug & s, const UF2Header & h );
typedef QScriptValue(*ParseFunc)(const UF2Header&, const QByteArray&, QScriptEngine*, QScriptContext*);

enum PortResult
{
    NoError,
    PortNotOpen,
    WriteError,
    HeaderReadTimeout,
    DataReadTimeout,
    DataCorrupt,
    CRCError
};

class UF2Device : public QObject
{
    Q_OBJECT
    Core* c;
    QSerialPort sPort;
    Q_PROPERTY (QString port READ getPort)
    Q_PROPERTY (int baudRate READ getBaudRate)
    Q_PROPERTY (quint16 deviceId READ deviceId WRITE setDeviceId)
    Q_PROPERTY (bool CycleMode10ms READ get10msCycleMode)
    Q_PROPERTY (bool CycleMode160ms READ get160msCycleMode)
    quint16 m_deviceId;
    bool m_CycleMode10ms;
    bool m_CycleMode160ms;
    static QStringList UF2Errors;
    friend class UF2DeviceFactory;
    static const ParseFunc parsers[];
    static const QStringList phases;
    static const QStringList interphases;

public:
    UF2Device(Core* core) : c(core) { /*   m_CycleMode160ms = true; connect(&sPort, SIGNAL(readyRead()), this, SLOT(readData())); */}
    static QScriptValue sendQuery(QScriptContext *context, QScriptEngine *engine);

    static QScriptValue JSCtor(QScriptContext *context, QScriptEngine *engine);
    static void bindToScriptEngine(QScriptEngine *engine);

    QString getPort() const { return sPort.portName(); }
    int getBaudRate() const { return sPort.baudRate(); }
    quint16 deviceId() const { return m_deviceId; }
    void setDeviceId(quint16 arg) { m_deviceId = arg; }
    bool get10msCycleMode() const { return m_CycleMode10ms; }
    bool get160msCycleMode() const { return m_CycleMode160ms; }
    PortResult query(UF2Header &replyHeader, QByteArray &replyData, const UF2Header &sendHeader, const QByteArray &sendData = QByteArray());

public slots:
    bool openDevice(const QString& portName, int baudrate);
    void closeDevice() { sPort.close(); }

private slots:
    void readData();

private:
    static QScriptValue parse14(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx);
    static QScriptValue dummyParser(const UF2Header& h, const QByteArray& data, QScriptEngine* e, QScriptContext* ctx);
    static QScriptValue parse1(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx);
    static QScriptValue parse3(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx);
    static QScriptValue parse15(const UF2Header& h, const QByteArray& data, QScriptEngine* e, QScriptContext* ctx);
    static QScriptValue parse24(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx);
    static QScriptValue parse27(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx);
    static QScriptValue parse31(const UF2Header &h, const QByteArray &data, QScriptEngine *e, QScriptContext *ctx);

};



class UF2DeviceFactory : public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID "org.PGE.DataReactor.UF2Device")
#endif
    Core *c;
    static UF2DeviceFactory* self;
    friend class UF2Device;
public:
    UF2DeviceFactory() { self = this; }
    int init(QObject *core);
    QVariantHash getInfo();
    QObject* newInstance();
    Core* core() const { return c; }
    static UF2DeviceFactory* instance() { return self; }
private:
};



#endif // TESTPLUGIN_H
