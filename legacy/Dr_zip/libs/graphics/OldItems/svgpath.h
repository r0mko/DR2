#ifndef SVGPATH_H
#define SVGPATH_H

#include <QObject>
#include "graphicspath.h"

namespace SvgParser
{
    bool parsePathDataFast(const QString &dataStr, QPainterPath &path);
    void pathArc(QPainterPath &path, qreal rx, qreal ry, qreal x_axis_rotation,
                 int large_arc_flag, int sweep_flag, qreal x, qreal y, qreal curx,
                 qreal cury);
}



class SvgPath : public GraphicsPath
{
    Q_OBJECT
    Q_PROPERTY(QString svgPath READ svgPath WRITE setSvgPath NOTIFY svgPathChanged)
    QString m_svgPath;

public:
    explicit SvgPath(QObject *parent = 0);

    QString svgPath() const;

signals:
    void svgPathChanged(QString arg);

public slots:
    void setSvgPath(QString arg);
};
#endif // SVGPATH_H
