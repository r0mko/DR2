#ifndef UI_PLUGIN_H
#define UI_PLUGIN_H

#include <QPainter>
#include <QPicture>
#include <QQmlExtensionPlugin>

class UIPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

extern "C" void rasterizeQPicture(QPainter *painter, const QSizeF &requestedSize, const QPicture &picture, QColor colorize, bool antialiasing = true, Qt::AspectRatioMode aspectRatio = Qt::KeepAspectRatio, qreal adjustX = 0, qreal adjustY = 0);

#endif // UI_PLUGIN_H

