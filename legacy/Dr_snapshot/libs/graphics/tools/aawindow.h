#ifndef AAWINDOW_H
#define AAWINDOW_H

#include <QQuickWindow>
#include <qqml.h>
class AAWindow : public QQuickWindow, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(int samples READ samples WRITE setSamples STORED false)
    Q_PROPERTY(int screenId READ screenId WRITE setScreenId NOTIFY screenIdChanged)

    int m_screenId;

public:
    explicit AAWindow(QWindow *parent = 0);

    int samples() const;
    void setSamples(int arg);
    int screenId() const;

    void componentComplete();
    void classBegin() {}
protected:
//    bool event(QEvent *e);
    void showEvent(QShowEvent *event);
signals:
    void screenIdChanged(int arg);
protected:

public slots:
    void setScreenId(int arg);
    void maximizeAtScreen(int id);
};

#endif // AAWINDOW_H
