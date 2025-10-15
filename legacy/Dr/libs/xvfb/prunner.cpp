#include "prunner.h"
#include <QDebug>
#include <QFile>

pRunner::pRunner(QObject *parent) : QObject(parent)
{
    connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));
    connect(&m_process, &QProcess::readyReadStandardError, this, &pRunner::onReadyReadStandardError);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &pRunner::onReadyReadStandardOutput);
    connect(&m_process, &QProcess::started, this, &pRunner::onStarted);
    connect(&m_process, &QProcess::stateChanged, this, &pRunner::onStateChanged);
}

pRunner::~pRunner()
{
    if (m_started && (m_process.state() == QProcess::Running)) {
        m_process.kill();
        m_process.waitForFinished();
    }
}

void pRunner::runProgram(const QString &filename)
{
    qDebug() << "Attempt to run" << filename;
    if (!m_fbStarted) {
        emit error("Virtual frame buffer not available");
        return;
    }
    if (m_started)
        emit error("Another program already running");
    else {
        QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
        env.insert("DISPLAY", QString(":%1.%2").arg(m_window).arg(m_screen));
        m_process.setProcessEnvironment(env);
        m_process.start(filename, QStringList());
        m_started = true;
    }
}

void pRunner::stopProgram()
{
    m_process.terminate();
}

void pRunner::onXvfbStarted(quint8 window, quint8 screen)
{
    qDebug() << "FB started" << window << screen;
    m_fbStarted = true;
    m_window = window;
    m_screen = screen;
}

void pRunner::onXvfbfinished()
{
    qDebug() << "FB finished";
    m_fbStarted = false;
    if (m_started)
        m_process.kill();
}

void pRunner::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Program finished" << exitCode << exitStatus;
    m_started = false;
    emit finished();
}

void pRunner::onError(QProcess::ProcessError error)
{
    qDebug() << "Run program error" << error;
    if (error == QProcess::FailedToStart) {
        m_started = false;
        emit this->error("Process failed to start");
    } else if (error == QProcess::Crashed) {
        m_started = false;
        emit this->error("Process crashed");
    }
}

void pRunner::onReadyReadStandardError()
{
    qDebug() << "Program readyReadStandardError" << m_process.readAllStandardError();
}

void pRunner::onReadyReadStandardOutput()
{
    qDebug() << "Program readyReadStandardOutput" << m_process.readAllStandardOutput();
}

void pRunner::onStarted()
{
    qDebug() << "Program started" << m_process.processId();
    emit started();
}

void pRunner::onStateChanged(QProcess::ProcessState newState)
{
    qDebug() << "Program stateChanged" << newState;
}

