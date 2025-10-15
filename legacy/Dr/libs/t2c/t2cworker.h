#ifndef T2CWORKER_H
#define T2CWORKER_H
#include <QHostAddress>
#include <QObject>
#include <QThread>

class QTcpSocket;
class QWaitCondition;
class T2CWorker : public QThread
{
  Q_OBJECT
  QTcpSocket*         socket;
  QHostAddress        t2caddr;
  quint16             t2cport;
  QWaitCondition*     sWaiter;
  QByteArray          query;
  QByteArray          lastLine;
  QWaitCondition*     replyWaiter{nullptr};
  QStringList         queryTokens;
  QList<QList<QByteArray> >  replyTokens;
  QByteArray             parsingTReply;
  bool                ready{true};
  bool              wrongReply{false};


public:
  explicit T2CWorker(QHostAddress addr, quint16 port, QWaitCondition* waiter);
  void sendCommand(QByteArray command, QWaitCondition *waiter = 0, bool waitForReply = false);
  void stopWorking();
  void run();
  QList<QList<QByteArray> > getReply();

signals:
  void valueUpdated();

public slots:
  void readReply();

private:
  bool validateReply(const QList<QByteArray> &s);
  bool validateDATReply(const QByteArray &replyLine, QList<QByteArray> *replyItems = 0);


  quint64 ANMqueryIdf();
  quint64 DATreplyIdf(const QByteArray &replyLine);




};

#endif // T2CWORKER_H
