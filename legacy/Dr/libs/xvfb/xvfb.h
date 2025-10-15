#ifndef XVFB_H
#define XVFB_H

#include <QObject>
#include <QProcess>
#include <QImage>
#include <QFile>
#include <QDebug>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "idsharedmemory.h"

#ifdef __APPLE__
    #define XvfbLocation "/opt/X11/bin/Xvfb"
#else
    #define XvfbLocation "/usr/bin/Xvfb"
#endif
#define XvfbFileLocation "/var/tmp"

class Xvfb : public QObject
{
    Q_OBJECT
public:
    enum exchangeType {
        File,
        SharedMemory
    };
    ~Xvfb();
    static Xvfb *instance(QObject *parent = 0);
    void start(const quint32 width, const quint32 height, const quint8 bpp = 24, const exchangeType type = File, const quint8 window = 5, const quint8 screen = 0);
    QImage getFullImage();
    QImage::Format getImageFormat() { return m_fmt; }
    char *imagePointer();

    void sendKeyEvent(const int qtkey, const int modifiers, bool pressed);
    void sendMoveMouseEvent(const int x, const int y, const int width, const int height);
    void sendMouseClick(const int button, bool pressed);

signals:
    void started(quint8 window, quint8 screen);
    void finished();
    void updated();

public slots:
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError error);
    void onReadyReadStandardError();
    void onReadyReadStandardOutput();
    void onStarted();
    void onStateChanged(QProcess::ProcessState newState);

private:

    explicit Xvfb(QObject *parent = 0);
    static QImage emptyImage();
    static void swaplong(char *bp, unsigned int n);
    static void swapshort(char *bp, unsigned int n);
    static unsigned int X11imageSize(XImage *image);
    bool initFbData();
    qint64 readFbData(qint64 &dataPosition, char * data, qint64 maxSize);
    QByteArray readFbData(qint64 &dataPosition, qint64 maxSize);

    QProcess        m_process;
    bool            m_started = false;
    bool            m_parsedHeader = false;
    XImage          m_xRawImage;
    QImage::Format  m_fmt;
    quint8          m_window;
    quint8          m_screen;
    static Xvfb    *m_instance;
    IdSharedMemory  m_sharedMem;
    uchar           *m_mmap = nullptr;
    int             m_shmid = 0;
    exchangeType    m_type = File;
    QFile           m_xwdFile;

    Display        *m_display = 0;
    bool            m_eventDisabled = true;
};

#endif // XVFB_H
