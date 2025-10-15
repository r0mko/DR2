#ifndef WWW_H
#define WWW_H

#include <QObject>
#include <QWebView>
class WWW: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString dir READ dir WRITE setDir NOTIFY dirChanged)
    QWebView* m_v{new QWebView( nullptr )};


    QString m_position;

    QUrl constructUrl() const;

    QString m_dir;

public:
    WWW(QObject* parent = nullptr);
    ~WWW();

    QString position() const
    {
        return m_position;
    }

    QString dir() const
    {
        return m_dir;
    }

public slots:
    void test();
    void prepare();
    void show();
    void stopLoading();

    void setPosition(QString arg)
    {
        if (m_position == arg)
            return;

        m_position = arg;
//        m_v->load( constructUrl() );
        emit positionChanged(arg);
    }
    void setDir(QString arg)
    {
        if (m_dir == arg)
            return;

        m_dir = arg;
//        m_v->load( constructUrl() );
        emit dirChanged(arg);
    }

signals:
    void positionChanged(QString arg);
    void dirChanged(QString arg);
};

#endif // WWW_H
