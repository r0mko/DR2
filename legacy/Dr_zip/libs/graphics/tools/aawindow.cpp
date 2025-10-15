#include "aawindow.h"
#include <QSurfaceFormat>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
AAWindow::AAWindow(QWindow *parent) :
    QQuickWindow(parent),
    m_screenId(-1)
{

}


int AAWindow::samples() const
{
    return this->format().samples();
}

void AAWindow::setSamples(int arg)
{
    QSurfaceFormat format = this->format();
    qDebug() << "Format" << format.samples() << arg;
    format.setSamples(arg);
    this->setFormat(format);
}

int AAWindow::screenId() const
{
    return m_screenId;
}

void AAWindow::componentComplete()
{
//    qDebug() << "Visible" << isVisible();
    if (m_screenId != -1) {
        if (!isVisible())
            setVisible(true);
        setScreenId(m_screenId);
    }
}

void AAWindow::showEvent(QShowEvent *event)
{
    QQuickWindow::showEvent(event);
    qDebug() << "Show event";
    event->ignore();
}

void AAWindow::setScreenId(int arg)
{
    if (!isVisible())
        m_screenId = arg;
    else {
        if (QGuiApplication::screens().size() > arg) {
            m_screenId = arg;
            setScreen(QGuiApplication::screens().at(arg));
            QRect geom = QGuiApplication::screens().at(arg)->availableGeometry();
            setGeometry(geom);
            emit screenIdChanged(arg);
        } else {
            showMaximized();
        }
    }
}

void AAWindow::maximizeAtScreen(int arg)
{
    if (QGuiApplication::screens().size() > arg) {
        m_screenId = arg;
        setScreen(QGuiApplication::screens().at(arg));
        QRect geom = QGuiApplication::screens().at(arg)->availableGeometry();
        setGeometry(geom);
        emit screenIdChanged(arg);
    }
}

