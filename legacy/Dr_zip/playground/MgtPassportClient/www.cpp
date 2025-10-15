#include "www.h"
#include <QDebug>
#include <QWebFrame>
QUrl WWW::constructUrl() const
{
    QString s = "https://maps.yandex.ru/?panorama[point]=" + m_position + "&panorama[direction]=" + m_dir;
    qDebug() << s;
    return QUrl{ s };
}

WWW::WWW(QObject *parent):
    QObject(parent)
{
    m_v->resize( 900, 500);
    connect( m_v, SIGNAL(loadFinished(bool)), this, SLOT(test()));
}

WWW::~WWW()
{
    m_v->deleteLater();
}

void WWW::test()
{
    m_v->page()->mainFrame()->evaluateJavaScript( "document.getElementsByTagName( 'header' )[0].style.display = 'none'" );
}

void WWW::prepare()
{
    m_v->load( QUrl( "https://maps.yandex.ru/?panorama[point]=37.555331,55.747183" ) );
}

void WWW::show()
{
    m_v->stop();
    m_v->show();
    m_v->load( constructUrl() );
}

void WWW::stopLoading()
{

}

