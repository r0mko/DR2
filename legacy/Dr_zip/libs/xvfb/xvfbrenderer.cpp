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

#define DIVFACTOR 16

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

    fb->start(DEFAULT_SIZE_X, DEFAULT_SIZE_X, 24, Xvfb::SharedMemory);
    setTextureFollowsItemSize(false);
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

    const float c_01SquareVertices[8] ={0.0f, 0.0f,
                                        1.0f, 0.0f,
                                        1.0f, 1.0f,
                                        0.0f, 1.0f};
    bool prepared = false;
    quint64 posY = 0;
    while (posY <= m_texHeight) {
        quint64 windowHeight = ( (posY + DIVFACTOR)<= m_texHeight ) ? DIVFACTOR : m_texHeight - posY;

        int offset = posY * m_texWidth * m_texDepth;
        int bufsize = m_texWidth * m_texDepth * windowHeight;
        if (memcmp(m_texCache + offset, m_texPointer + offset, bufsize) != 0) {
            if (!prepared) {
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
                glVertexPointer(2, GL_FLOAT, 0, c_01SquareVertices);
                glTexCoordPointer(2, GL_FLOAT, 0, c_01SquareVertices);

                m_program1.bind();
                glBindTexture(GL_TEXTURE_2D, m_textures[0]);

                prepared = true;
//                qDebug() << "New frame";
            }
            memcpy(m_texCache + offset, m_texPointer + offset, bufsize);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, posY, m_texWidth, windowHeight,  m_ImgFmt, m_ImgType, m_texPointer + offset);
//            qDebug() << "SubTexture" << posY << offset << bufsize;
        }
        posY += DIVFACTOR;
    }

    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texWidth, m_texHeight,  m_ImgFmt, m_ImgType, m_texPointer);

    if (prepared) {
        m_program1.setUniformValue("texture", 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        m_program1.release();
    }
    update();
}

QOpenGLFramebufferObject *XvfbInFboRenderer::createFramebufferObject(const QSize &size)
{
    qDebug() << "createFramebufferObject" << size.width() << size.height();
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(16);
    QSize texSize;
    if (!m_texWidth) {
        QImage pic = Xvfb::instance()->getFullImage();
        if (pic.width() < 2) {
            texSize = QSize(DEFAULT_SIZE_X, DEFAULT_SIZE_Y);
        } else {
            texSize = pic.size();
        }

    } else {
        texSize = QSize(m_texWidth, m_texHeight);
    }
    qDebug() << "createFramebufferObject" << size << texSize;
    return new QOpenGLFramebufferObject(texSize, format);
}

XvfbInFboRenderer::~XvfbInFboRenderer()
{
    if (m_texCache)
        free(m_texCache);
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

    m_texDepth = Xvfb::instance()->getBitsPerPixel() / 8;

    if (m_texCache)
        free(m_texCache);

    m_texCache = (char*)malloc(m_texWidth * m_texHeight * m_texDepth);

    //qDebug() << "QImage::Format" << Xvfb::instance()->getImageFormat() << "depth" << m_texDepth;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, m_textures);
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
//    glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, (m_ImgFmt == GL_BGRA) ? GL_RGBA : GL_RGB, m_texWidth, m_texHeight, 0, m_ImgFmt, m_ImgType, m_texPointer );
}


