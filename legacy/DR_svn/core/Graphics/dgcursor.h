#ifndef DGCURSOR_H
#define DGCURSOR_H
#include <QtQml>
#include <QCursor>
#include <QColor>
#include <QPixmap>
#include <QBitmap>
#include <QUrl>

class DGCursor : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QColor maskColor READ maskColor WRITE setMaskColor)
    Q_PROPERTY(int hotX READ hotX WRITE setHotX)
    Q_PROPERTY(int hotY READ hotY WRITE setHotY)
    Q_PROPERTY(bool status READ status NOTIFY statusChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    DGCursor() : m_status(false) {}
    virtual ~DGCursor() {}

    QCursor cursor() {
        if (!m_status)
            m_status = constructCursor();
        return m_cursor;
    }
    QColor maskColor() const { return m_maskColor; }
    bool status() const { return m_status; }
    int hotX() const { return m_hotX; }
    int hotY() const { return m_hotY; }
    void setMaskColor(QColor arg) { m_maskColor = arg; }
    void setHotX(int arg) { m_hotX = arg; }
    void setHotY(int arg) { m_hotY = arg; }
    QUrl source() const { return m_source; }

public slots:
    void setSource(QUrl arg) { if (m_source == arg) return;
        m_source = arg;
        emit sourceChanged(arg);
    }

protected:
    void componentComplete() { m_status = constructCursor(); }
    void classBegin() {}

signals:
    void statusChanged(bool arg);
    void sourceChanged(QUrl arg);

private:
    bool constructCursor()
    {
        QPixmap pm = QPixmap(m_source.toLocalFile());
        if (pm.isNull()) {
            qWarning() << "Error constructing cursor from" << m_source.toString();
        }
        if (!pm.isNull()) {
            if (m_maskColor.isValid())
                pm.setMask(pm.createMaskFromColor(m_maskColor));
            m_cursor = QCursor(pm, m_hotX, m_hotY);
            return true;
        }
        return false;
    }

    QCursor m_cursor;
    QColor m_maskColor;
    bool m_status;
    int m_hotX;
    int m_hotY;
    QUrl m_source;
};

#endif // DGCURSOR_H
