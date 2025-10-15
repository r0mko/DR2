#ifndef PROCESSVARIABLE_H
#define PROCESSVARIABLE_H

#include <QObject>
#include <QDateTime>
#include <QVariant>

class ProcessVariable
{
    Q_GADGET
    Q_ENUMS(Status)
    Q_FLAGS(Statuses)


    Q_PROPERTY(int idf READ idf WRITE setIdf)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(QDateTime time READ time WRITE setTime)
    Q_PROPERTY(QByteArray valueType READ valueType WRITE setValueType)
    Q_PROPERTY(int status READ status WRITE setStatus)
    Q_PROPERTY(QString name READ name WRITE setName)


public:
    enum Status
    {
        OK =          0x00,
        NotOK =       0x01,
        Test =        0x02,
        Blocked =     0x04,
        UserEntry =   0x08,
        Substituted = 0x10,
        Queried =     0x20,
        Reserved =    0x40,
        InvalidTime = 0x80
    };
    Q_DECLARE_FLAGS(Statuses, Status)

private:
    int         m_idf{0};
    QVariant    m_value;
    QDateTime   m_time;
    Status      m_status{OK};
    QByteArray  m_valueType;
    QString     m_name;

public:
    int idf() const
    {
        return m_idf;
    }
    QVariant value() const
    {
        return m_value;
    }

    QDateTime time() const
    {
        return m_time;
    }

    QByteArray valueType() const
    {
        return m_valueType;
    }

    int status() const
    {
        return (int)m_status;
    }

    QString name() const
    {
        return m_name;
    }

    void setIdf(int idf)
    {
        if (m_idf == idf)
            return;

        m_idf = idf;
    }
    void setValue(QVariant value)
    {
        if (m_value == value)
            return;

        m_value = value;
    }

    void setTime(QDateTime time)
    {
        if (m_time == time)
            return;

        m_time = time;
    }

    void setValueType(QByteArray valueType)
    {
        if (m_valueType == valueType)
            return;

        m_valueType = valueType;
    }

    void setStatus(int status)
    {
        if ((int)m_status == status)
            return;

        m_status = (Status)status;
    }

    void setName(QString name)
    {
        if (m_name == name)
            return;

        m_name = name;
    }


};
Q_DECLARE_METATYPE(ProcessVariable)
#endif // PROCESSVARIABLE_H
