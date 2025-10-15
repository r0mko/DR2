#ifndef PRUNNER_H
#define PRUNNER_H

#include <QObject>
#include <QProcess>

class pRunner : public QObject
{
    Q_OBJECT
public:
    explicit pRunner(QObject *parent = 0);
    ~pRunner();
    Q_INVOKABLE void runProgram(const QString &filename);
    Q_INVOKABLE void runXGS(const QString &host, const quint8 XGSno);
    Q_INVOKABLE void stopProgram();

public Q_SLOTS:
    void onXvfbStarted(quint8 window, quint8 screen);
    void onXvfbfinished();

    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError error);
    void onReadyReadStandardError();
    void onReadyReadStandardOutput();
    void onStarted();
    void onStateChanged(QProcess::ProcessState newState);

Q_SIGNALS:
    void error(const QString &error);
    void started();
    void finished();


private:
    QProcess    m_process;
    QString     m_filename;
    QString     m_XGShost;
    quint8      m_XGSno = 0;
    quint8      m_window;
    quint8      m_screen;
    bool        m_started = false;
    bool        m_fbStarted = false;
};

#endif // PRUNNER_H
