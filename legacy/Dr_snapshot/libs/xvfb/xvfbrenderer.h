#ifndef XVFBRENDERER_H
#define XVFBRENDERER_H

#include <QObject>
#include <QtQuick/QQuickFramebufferObject>
#include <QtGui/QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QImage>

#define DEFAULT_SIZE_X 1920
#define DEFAULT_SIZE_Y 1080

class Xvfb;
class pRunner;
class XvfbEvent;
class XvfbRenderer : public QQuickFramebufferObject
{
    Q_OBJECT

    Xvfb *fb{nullptr};
    pRunner *pr{nullptr};
    XvfbEvent *ev{nullptr};

public:
    QQuickFramebufferObject::Renderer *createRenderer() const Q_DECL_OVERRIDE;

    // QQmlParserStatus interface
public:
    void classBegin();
    void componentComplete();
};


class XvfbInFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    XvfbInFboRenderer();
    void render() Q_DECL_OVERRIDE;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE;
    ~XvfbInFboRenderer();


private:
    void                        initTexture();
    GLuint                      m_textures[1];
    bool                        m_xvfbStarted = false;
    quint64                     m_texWidth = 0;
    quint64                     m_texHeight = 0;
    char                       *m_texCache = nullptr;
    quint8                      m_texDepth = 0;
    char                       *m_texPointer = nullptr;
    GLenum                      m_ImgFmt;
    GLenum                      m_ImgType;
    QOpenGLShaderProgram        m_program1;
};

#endif // XVFBRENDERER_H
