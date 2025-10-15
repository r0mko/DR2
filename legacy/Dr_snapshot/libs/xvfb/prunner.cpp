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
        m_filename = filename;
        emit error("Virtual frame buffer not available");
        return;
    }
    if (m_started)
        emit error("Another program already running");
    else {
        m_filename = filename;

        QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
        env.insert("DISPLAY", QString(":%1.%2").arg(m_window).arg(m_screen));
        m_process.setProcessEnvironment(env);

        if (int pos = filename.indexOf(" ")) {
            QString file = filename.left(pos);
            QStringList argv = filename.mid(pos + 1).split(QRegExp("\\s+"), QString::SkipEmptyParts);
            m_process.start(file, argv);
        } else
            m_process.start(m_filename, QStringList());

        m_started = true;
    }
}

void pRunner::runXGS(const QString &host, const quint8 XGSno)
{
    qDebug() << "Attempt to run XGS" << host << XGSno;
    if (!m_fbStarted) {
        m_XGShost = host;
        m_XGSno = XGSno;
        emit error("Virtual frame buffer not available");
        return;
    }
    if (m_started)
        emit error("Another program already running");
    else {
        m_XGShost = host;
        m_XGSno = XGSno;

        QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
        env.insert("DISPLAY", QString(":%1.%2").arg(m_window).arg(m_screen));
        m_process.setProcessEnvironment(env);

        QStringList argv;
        argv.append("-XY");
        argv.append( QString("awl@%1").arg(m_XGShost) );
        argv.append( QString("export display=$DISPLAY\n \
                               if [[ -z $AWL_ENV ]] then source ~/bin/source_env.sh; fi\n \
                               if [[ -r $AWL_ENVM/XGS%1.env ]] then source $AWL_ENVM/XGS%1.env; fi\n \
                               if [[ $AWL_ENVM != $AWL_ENV && -r $AWL_ENV/XGS%1.env ]] then\n \
                                 source $AWL_ENV/XGS%1.env \n \
                               fi \n \
                               ~/bin/lstop.sh XGS%1 \n \
                               cd $MY_AWL_ROOT/run \n \
              ~/bin/logfiles -a 10 -d XGS%1 10000000 awl_XGS%1").arg(m_XGSno) );

        qDebug() << "Attempt to run XGS arg" << argv;
        m_process.start("/usr/bin/ssh", argv);

        m_started = true;
    }
}

void pRunner::stopProgram()
{
    m_process.terminate();
    m_filename = "";
}

void pRunner::onXvfbStarted(quint8 window, quint8 screen)
{
    qDebug() << "FB started" << window << screen;
    m_fbStarted = true;
    m_window = window;
    m_screen = screen;
    if (m_filename.size())
        runProgram(m_filename);

    if (m_XGShost.size() && m_XGSno)
        runXGS(m_XGShost, m_XGSno);
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

