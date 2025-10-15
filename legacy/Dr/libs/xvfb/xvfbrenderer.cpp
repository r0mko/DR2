#include "xvfbrenderer.h"

#include <QtQuick/QQuickWindow>
#include <QGLWidget>
#include <QDateTime>
#include <QOpenGLFunctions>

#include "xvfb.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#include "prunner.h"
#include "xvfbevent.h"
#include <QApplication>
#include <QQmlEngine>
#include <QQmlContext>

QQuickFramebufferObject::Renderer *XvfbRenderer::createRenderer() const
{

    return new XvfbInFboRenderer();
}

void XvfbRenderer::classBegin(){
    fb = Xvfb::instance(qApp);
    pr = new pRunner(qApp);
    ev = new XvfbEvent(qApp);

    QObject::connect(fb, &Xvfb::started, pr, &pRunner::onXvfbStarted);
    QObject::connect(fb, &Xvfb::finished, pr, &pRunner::onXvfbfinished);

    #ifdef __APPLE__
        fb->start(1920, 1080, 24, Xvfb::File);
    #else
        fb->start(1920, 1080, 24, Xvfb::SharedMemory);
    #endif
}

void XvfbRenderer::componentComplete(){
    auto e = qmlEngine(this);
    e->rootContext()->setContextProperty("processRunner", pr);
    e->rootContext()->setContextProperty("X11Event", ev);
}


XvfbInFboRenderer::XvfbInFboRenderer()
{
    QOpenGLContext::currentContext()->functions()->initializeOpenGLFunctions();

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, &m_program1);
    const char *fsrc =
            "uniform sampler2D texture;\n"
            "varying vec4 gl_TexCoord[];\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = texture2D(texture, gl_TexCoord[0].xy);\n"
            "}\n";
    fshader->compileSourceCode(fsrc);

    m_program1.addShader(fshader);
    m_program1.link();
}

void XvfbInFboRenderer::render()
{
    if (!m_xvfbStarted) {
        initTexture();
        if (!m_xvfbStarted)
            return;
    }

    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    const float c_01SquareVertices[8] ={0.0f, 0.0f,
                                        1.0f, 0.0f,
                                        1.0f, 1.0f,
                                        0.0f, 1.0f};
    glVertexPointer(2, GL_FLOAT, 0, c_01SquareVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, c_01SquareVertices);

    m_program1.bind();
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);

    #ifdef __APPLE__
        // TODO: on Mac os I had to make Xvfb on file rather than shared mem that's why I copy image to texture, not the m_texPointer.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texWidth, m_texHeight,  m_ImgFmt, m_ImgType, Xvfb::instance()->getFullImage().bits());
    #else
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texWidth, m_texHeight,  m_ImgFmt, m_ImgType, m_texPointer);
    #endif

    m_program1.setUniformValue("texture", 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    m_program1.release();
    update();
}

QOpenGLFramebufferObject *XvfbInFboRenderer::createFramebufferObject(const QSize &size)
{
    qDebug() << "createFramebufferObject" << size.width() << size.height();
    m_WinWidth = size.width();
    m_WinHeight = size.height();
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(16);
    return new QOpenGLFramebufferObject(size, format);
}

void XvfbInFboRenderer::initTexture()
{
    QImage pic = Xvfb::instance()->getFullImage();

    if (pic.width() < 2)
        return;
    m_xvfbStarted = true;
    qDebug() << "Init texture" << pic.width() << pic.height() << QDateTime::currentMSecsSinceEpoch();

    m_texWidth = pic.width();
    m_texHeight = pic.height();
    m_texPointer = Xvfb::instance()->imagePointer();

    switch (Xvfb::instance()->getImageFormat()) {
    case QImage::Format_RGB888:
    case QImage::Format_RGB16:
        m_ImgFmt = GL_RGB;
        m_ImgType = GL_UNSIGNED_SHORT_5_6_5;
        break;
    default:
        m_ImgFmt = GL_BGRA;
        m_ImgType = GL_UNSIGNED_BYTE;
        break;
    }
    qDebug() << "format" << Xvfb::instance()->getImageFormat();
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, m_textures);
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, (m_ImgFmt == GL_BGRA) ? GL_RGBA : GL_RGB, m_texWidth, m_texHeight, 0, m_ImgFmt, m_ImgType, m_texPointer );

}


