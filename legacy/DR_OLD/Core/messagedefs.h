#ifndef MESSAGEDEFS_H
#define MESSAGEDEFS_H
#include <QEvent>
#include <QDataStream>

class Message : public QEvent
{
  QObject* sender;
public:
  Message(QObject* sender = 0, int type = 0) : QEvent((QEvent::Type)type), sender(sender) {}
};

Q_DECLARE_METATYPE(Message);





#endif // MESSAGEDEFS_H
