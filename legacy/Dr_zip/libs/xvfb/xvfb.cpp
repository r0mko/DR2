#include "xvfb.h"
#include <QMutex>
#include <QFile>

#include <X11/XWDFile.h>
#include <X11/X.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

Xvfb* Xvfb::m_instance = 0;

Xvfb::Xvfb(QObject *parent) : QObject(parent)
{
    connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));
    connect(&m_process, &QProcess::readyReadStandardError, this, &Xvfb::onReadyReadStandardError);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &Xvfb::onReadyReadStandardOutput);
    connect(&m_process, &QProcess::started, this, &Xvfb::onStarted);
    connect(&m_process, &QProcess::stateChanged, this, &Xvfb::onStateChanged);
}

QImage Xvfb::emptyImage()
{
    QImage img(1, 1, QImage::Format_RGBX8888);
    img.fill(Qt::transparent);
    return img;
}

void Xvfb::swaplong(char *bp, unsigned int n)
{
    register char *ep = bp + n;
    register char *sp;

    while (bp < ep) {
        sp = bp + 3;
        register char c = *sp;
        *sp = *bp;
        *bp++ = c;
        sp = bp + 1;
        c = *sp;
        *sp = *bp;
        *bp++ = c;
        bp += 2;
    }
}

void Xvfb::swapshort(char *bp, unsigned int n)
{
    register char *ep = bp + n;

    while (bp < ep) {
        register char c = *bp;
        *bp = *(bp + 1);
        bp++;
        *bp++ = c;
    }
}

unsigned int Xvfb::X11imageSize(XImage *image)
{
    if (image->format != ZPixmap)
        return(image->bytes_per_line * image->height * image->depth);

    return((unsigned)image->bytes_per_line * image->height);
}

Xvfb::~Xvfb()
{
    m_sharedMem.detach();
    if (m_started) {
        if (m_display) {
            XFlush(m_display);
            XCloseDisplay(m_display);
            m_display = 0;
        }
        m_process.kill();
        m_process.waitForFinished();
    }
    m_instance = 0;
}

Xvfb *Xvfb::instance(QObject *parent)
{
    if (m_instance == 0) {
        static QMutex mutex;
        mutex.lock();
        if(m_instance == 0)
            m_instance = new Xvfb(parent);
        mutex.unlock();
    }
    return m_instance;
}

void Xvfb::start(const quint32 width, const quint32 height, const quint8 bpp, exchangeType type, const quint8 window, const quint8 screen)
{
    if (!m_started || (m_process.state() !=QProcess::Starting) ) {
        m_window = window;
        m_screen = screen;
        m_type = type;
        QString program = XvfbLocation;
        QStringList arguments;
        arguments.append( QString(":%1").arg(window) );

        QStringList dirs;
        dirs << "/usr/share/fonts/liberation-fonts"; // gentoo
        dirs << "/usr/share/fonts/truetype/liberation"; // ubuntu
        dirs << "/Users/romko/Library/Fonts"; // Mac

        foreach (QString dir, dirs) {
            if (QFile(dir + "/fonts.dir").exists()) {
                arguments.append( "-fp" );
                arguments.append( dir );
                break;
            }
        }

        arguments.append("-screen");
        arguments.append(QString("%1").arg(screen));
        arguments.append( QString("%1x%2x%3").arg(width).arg(height).arg(bpp) );
        if (m_type == Xvfb::File) {
            arguments.append( "-fbdir" );
            arguments.append( XvfbFileLocation );
        } else
            arguments.append( "-shmem" );

        qDebug() << program << arguments;
        QProcess::execute("/usr/bin/killall", QStringList("Xvfb"));
        m_process.start(program, arguments);
    }
}

QImage Xvfb::getFullImage()
{
    if (!m_started)
        return emptyImage();

    if (!m_parsedHeader) {

        qint64 dataPosition = 0;
        XWDFileHeader header;
        unsigned long swaptest = 1;

        if (!initFbData())
            return emptyImage();

        if (readFbData(dataPosition, (char*)&header, sizeof(XWDFileHeader)) != sizeof(XWDFileHeader)) {
            qDebug() << "Failed to read XWD header";
            return emptyImage();
        }

        if (*(char *) &swaptest)
            swaplong((char *) &header, sizeof(XWDFileHeader));

        if (header.file_version != XWD_FILE_VERSION) {
            qDebug() << "XWD file format version mismatch.";
            return emptyImage();
        }

        if (header.header_size < sizeof(XWDFileHeader)) {
            qDebug() << "XWD header size is too small.";
            return emptyImage();
        }

        if (header.pixmap_format != ZPixmap) {
            qDebug() << "Can read only ZPixmap format";
            return emptyImage();
        }

        int win_name_size = (header.header_size - sizeof(XWDFileHeader));
        if (win_name_size > 0) {
            readFbData(dataPosition, win_name_size);
            //QByteArray winName = readFbData(dataPosition, win_name_size);
            //qDebug() << "Read winName" << winName;
        }

        if (header.ncolors > 0) {
            //qDebug() << "Number of color" << header.ncolors;
            XWDColor xwdcolor;
            for (quint32 i = 0; i < header.ncolors; ++i) {
                if (readFbData(dataPosition, (char *) &xwdcolor, sizeof(XWDColor)) != sizeof(XWDColor)) {
                    qDebug() << "Unable to read color map from dump file";
                }
                if (*(char *) &swaptest) {
                    swaplong((char *) &xwdcolor.pixel, sizeof(long));
                    swapshort((char *) &xwdcolor.red, 3 * sizeof(short));
                }
                //            QColor color(xwdcolor.red, xwdcolor.green, xwdcolor.blue);
                //            colors.append(color.rgb());
            }
        }

        m_xRawImage.depth = header.pixmap_depth;
        m_xRawImage.format = header.pixmap_format;
        m_xRawImage.xoffset = header.xoffset;
        m_xRawImage.data = NULL;
        m_xRawImage.width = header.pixmap_width;
        m_xRawImage.height = header.pixmap_height;
        m_xRawImage.bitmap_pad = header.bitmap_pad;
        m_xRawImage.bytes_per_line = header.bytes_per_line;
        m_xRawImage.byte_order = header.byte_order;
        m_xRawImage.bitmap_unit = header.bitmap_unit;
        m_xRawImage.bitmap_bit_order = header.bitmap_bit_order;
        m_xRawImage.bits_per_pixel = header.bits_per_pixel;
        m_xRawImage.red_mask = header.red_mask;
        m_xRawImage.green_mask = header.green_mask;
        m_xRawImage.blue_mask = header.blue_mask;

        if (!XInitImage(&m_xRawImage)) {
            qDebug() << "Invalid input image header data.";
        }

        if (m_type == SharedMemory)
            m_xRawImage.data = (char*)m_sharedMem.data() + dataPosition;
        else
            m_xRawImage.data = (char*)m_mmap + dataPosition;

        m_fmt = QImage::Format_RGBX8888; //QImage::Format_ARGB32_Premultiplied; QImage::Format_RGBX8888
        switch (m_xRawImage.bits_per_pixel) {
        case 24:
            m_fmt = QImage::Format_RGB888;
            break;
        case 16:
            m_fmt = QImage::Format_RGB16;
            break;
        case 8:
            m_fmt = QImage::Format_RGB16;
            break;
        default:
            break;
        }

        m_parsedHeader = true;
    }

    //QByteArray data = readFbData(workdata, X11imageSize(&xRawImage));
    //qDebug() << "Read" << data.size() << "of" << X11imageSize(&xRawImage);

    QImage img = QImage((uchar *)m_xRawImage.data, m_xRawImage.width, m_xRawImage.height, m_xRawImage.bytes_per_line, m_fmt);

    if (m_xRawImage.bits_per_pixel == 32) {
        return img.rgbSwapped();
    }

    if ( (QSysInfo::ByteOrder == QSysInfo::LittleEndian && m_xRawImage.byte_order == MSBFirst) ||
         (QSysInfo::ByteOrder == QSysInfo::BigEndian && m_xRawImage.byte_order == LSBFirst)) {
        qDebug() << "swap";
        for (int i=0; i < img.height(); i++) {
            if (m_xRawImage.depth == 16) {
                ushort* p = reinterpret_cast<ushort*>(img.scanLine(i));
                ushort *end = p + img.width();
                while (p < end) {
                    *p = ((*p << 8) & 0xff00) | ((*p >> 8) & 0x00ff);
                    p++;
                }
            } else {
                uint* p = reinterpret_cast<uint*>(img.scanLine(i));
                uint *end = p + img.width();
                while (p < end) {
                    *p = ((*p << 24) & 0xff000000) | ((*p << 8) & 0x00ff0000)
                            | ((*p >> 8) & 0x0000ff00) | ((*p >> 24) & 0x000000ff);
                    p++;
                }
            }
        }
    }

    // fix alpha channel
    if (m_fmt == QImage::Format_RGB32) {
        QRgb* p = reinterpret_cast<QRgb*>(img.bits());
        for (int y = 0; y < m_xRawImage.height; ++y) {
            for (int x = 0; x < m_xRawImage.width; ++x)
                p[x] |= 0xff000000;
            p += m_xRawImage.bytes_per_line / 4;
        }
    }

    return img;
}

char *Xvfb::imagePointer()
{
    return m_xRawImage.data;
}

void Xvfb::sendKeyEvent(const int qtkey, const int modifiers, bool pressed)
{
    if (!m_display || m_eventDisabled)
        return;

    Window winFocus;
    int    revert;
    XGetInputFocus(m_display, &winFocus, &revert);

    unsigned int key = 0;
    unsigned int mod_key = 0;

    switch( qtkey )
    {
    // modifiers are handled separately
    case Qt::Key_Shift: key = XK_Shift_L; break;
    case Qt::Key_Control: key = XK_Control_L; break;
    case Qt::Key_Meta: key = XK_Meta_L; break;
    case Qt::Key_Alt: key = XK_Alt_L; break;
    case Qt::Key_Escape: key = XK_Escape; break;
    case Qt::Key_Tab: key = XK_Tab; break;
    case Qt::Key_Backtab: key = XK_Tab; break;
    case Qt::Key_Backspace: key = XK_BackSpace; break;
    case Qt::Key_Return: key = XK_Return; break;
    case Qt::Key_Enter: key = XK_KP_Enter; break;
    case Qt::Key_Insert: key = XK_Insert; break;
    case Qt::Key_Delete: key = XK_Delete; break;
    case Qt::Key_Pause: key = XK_Pause; break;
    case Qt::Key_Print: key = XK_Print; break;
    case Qt::Key_Home: key = XK_Home; break;
    case Qt::Key_End: key = XK_End; break;
    case Qt::Key_Left: key = XK_Left; break;
    case Qt::Key_Up: key = XK_Up; break;
    case Qt::Key_Right: key = XK_Right; break;
    case Qt::Key_Down: key = XK_Down; break;
    case Qt::Key_PageUp: key = XK_Prior; break;
    case Qt::Key_PageDown: key = XK_Next; break;
    case Qt::Key_CapsLock: key = XK_Caps_Lock; break;
    case Qt::Key_NumLock: key = XK_Num_Lock; break;
    case Qt::Key_ScrollLock: key = XK_Scroll_Lock; break;
    case Qt::Key_Super_L: key = XK_Super_L; break;
    case Qt::Key_Super_R: key = XK_Super_R; break;
    case Qt::Key_Menu: key = XK_Menu; break;
    case Qt::Key_Hyper_L: key = XK_Hyper_L; break;
    case Qt::Key_Hyper_R: key = XK_Hyper_R; break;
    case Qt::Key_Help: key = XK_Help; break;
    case Qt::Key_AltGr: key = XK_ISO_Level3_Shift; break;
    case Qt::Key_Multi_key: key = XK_Multi_key; break;
    case Qt::Key_SingleCandidate: key = XK_SingleCandidate; break;
    case Qt::Key_MultipleCandidate: key = XK_MultipleCandidate; break;
    case Qt::Key_PreviousCandidate: key = XK_PreviousCandidate; break;
    case Qt::Key_Mode_switch: key = XK_Mode_switch; break;
    case Qt::Key_Kanji: key = XK_Kanji; break;
    case Qt::Key_Muhenkan: key = XK_Muhenkan; break;
    case Qt::Key_Henkan: key = XK_Henkan; break;
    case Qt::Key_Romaji: key = XK_Romaji; break;
    case Qt::Key_Hiragana: key = XK_Hiragana; break;
    case Qt::Key_Katakana: key = XK_Katakana; break;
    case Qt::Key_Hiragana_Katakana: key = XK_Hiragana_Katakana; break;
    case Qt::Key_Zenkaku: key = XK_Zenkaku; break;
    case Qt::Key_Hankaku: key = XK_Hankaku; break;
    case Qt::Key_Zenkaku_Hankaku: key = XK_Zenkaku_Hankaku; break;
    case Qt::Key_Touroku: key = XK_Touroku; break;
    case Qt::Key_Massyo: key = XK_Massyo; break;
    case Qt::Key_Kana_Lock: key = XK_Kana_Lock; break;
    case Qt::Key_Kana_Shift: key = XK_Kana_Shift; break;
    case Qt::Key_Eisu_Shift: key = XK_Eisu_Shift; break;
    case Qt::Key_Eisu_toggle: key = XK_Eisu_toggle; break;
    case Qt::Key_Hangul: key = XK_Hangul; break;
    case Qt::Key_Hangul_Start: key = XK_Hangul_Start; break;
    case Qt::Key_Hangul_End: key = XK_Hangul_End; break;
    case Qt::Key_Hangul_Hanja: key = XK_Hangul_Hanja; break;
    case Qt::Key_Hangul_Jamo: key = XK_Hangul_Jamo; break;
    case Qt::Key_Hangul_Romaja: key = XK_Hangul_Romaja; break;
    case Qt::Key_Hangul_Jeonja: key = XK_Hangul_Jeonja; break;
    case Qt::Key_Hangul_Banja: key = XK_Hangul_Banja; break;
    case Qt::Key_Hangul_PreHanja: key = XK_Hangul_PreHanja; break;
    case Qt::Key_Hangul_PostHanja: key = XK_Hangul_PostHanja; break;
    case Qt::Key_Hangul_Special: key = XK_Hangul_Special; break;
    case Qt::Key_Dead_Grave: key = XK_dead_grave; break;
    case Qt::Key_Dead_Acute: key = XK_dead_acute; break;
    case Qt::Key_Dead_Circumflex: key = XK_dead_circumflex; break;
    case Qt::Key_Dead_Tilde: key = XK_dead_tilde; break;
    case Qt::Key_Dead_Macron: key = XK_dead_macron; break;
    case Qt::Key_Dead_Breve: key = XK_dead_breve; break;
    case Qt::Key_Dead_Abovedot: key = XK_dead_abovedot; break;
    case Qt::Key_Dead_Diaeresis: key = XK_dead_diaeresis; break;
    case Qt::Key_Dead_Abovering: key = XK_dead_abovering; break;
    case Qt::Key_Dead_Doubleacute: key = XK_dead_doubleacute; break;
    case Qt::Key_Dead_Caron: key = XK_dead_caron; break;
    case Qt::Key_Dead_Cedilla: key = XK_dead_cedilla; break;
    case Qt::Key_Dead_Ogonek: key = XK_dead_ogonek; break;
    case Qt::Key_Dead_Iota: key = XK_dead_iota; break;
    case Qt::Key_Dead_Voiced_Sound: key = XK_dead_voiced_sound; break;
    case Qt::Key_Dead_Semivoiced_Sound: key = XK_dead_semivoiced_sound; break;
    case Qt::Key_Dead_Belowdot: key = XK_dead_belowdot; break;
    }

    if( qtkey >= Qt::Key_F1 && qtkey <= Qt::Key_F35 )
    {
        key = XK_F1 + qtkey - Qt::Key_F1;
    }

    if (modifiers) {
        unsigned int mod = 0;
        switch (modifiers) {
        case Qt::ShiftModifier:
            mod = XK_Shift_L;
            break;
        case Qt::AltModifier:
            mod = XK_Alt_L;
            break;
        case Qt::MetaModifier:
            mod = XK_Meta_L;
            break;
        case Qt::ControlModifier:
            mod = XK_Control_L;
            break;
        default:
            break;
        }
        if (mod) {
            mod_key = XKeysymToKeycode (m_display, mod);
        }
    }

    if (key)
        key = XKeysymToKeycode(m_display, key);
    else
        key = (qtkey >= 0x20) ? XKeysymToKeycode(m_display, qtkey) : XKeysymToKeycode(m_display, XStringToKeysym(QString("%1").arg(qtkey).toLatin1().constData()));

    //qDebug() << "keycode1,2" << key << qtkey;
    if (pressed) {
        if (modifiers && mod_key) {
            XTestFakeKeyEvent(m_display, mod_key, True, CurrentTime);
            XFlush(m_display);
        }
        XTestFakeKeyEvent(m_display, key, True, CurrentTime);
        XFlush(m_display);
    } else {
        XTestFakeKeyEvent(m_display, key, False, CurrentTime);
        XFlush(m_display);
        if (modifiers && mod_key) {
            XTestFakeKeyEvent(m_display, mod_key, False, CurrentTime);
            XFlush(m_display);
        }
    }

    emit updated();
}

void Xvfb::sendMoveMouseEvent(const int x, const int y, const int width, const int height)
{
    if (!m_display || m_eventDisabled)
        return;
    //XQueryPointer(m_display, RootWindow(m_display, DefaultScreen(m_display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    XTestFakeMotionEvent(m_display, -1, x * m_xRawImage.width/width, y * m_xRawImage.height/height, CurrentTime);
    XFlush(m_display);
    emit updated();
}

void Xvfb::sendMouseClick(const int button, bool pressed)
{
    if (!m_display || m_eventDisabled)
        return;
    //qDebug() << "Mouse click" << button << pressed;
    XTestFakeButtonEvent(m_display, button, pressed, CurrentTime);
    XFlush(m_display);
    emit updated();
}

void Xvfb::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Xvfb finished" << exitCode << exitStatus;
    m_started = false;
    emit finished();
    if (m_display) {
        XFlush(m_display);
        XCloseDisplay(m_display);
        m_display = 0;
    }
    if (m_type == SharedMemory)
        m_sharedMem.detach();
    else {
        m_xwdFile.unmap(m_mmap);
        m_xwdFile.close();
    }
    m_parsedHeader = false;
}


void Xvfb::onError(QProcess::ProcessError error)
{
    qDebug() << "Xvfb error" << error;
}

void Xvfb::onReadyReadStandardError()
{
    QRegExp rx("shmid (\\d+)");
    QString in = QString(m_process.readAllStandardError());

    if (rx.indexIn(in) > -1) {
        //qDebug() << "Found id" << rx.cap(1);
        m_shmid = rx.cap(1).toInt();
    }
    qDebug() << "Xvfb readyReadStandardError" << in;
}

void Xvfb::onReadyReadStandardOutput()
{
    qDebug() << "Xvfb readyReadStandardOutput" << m_process.readAllStandardOutput();
}

void Xvfb::onStarted()
{
    qDebug() << "Xvfb started" << m_process.processId();
    m_started = true;

    QString displayName = QString(":%1.%2").arg(m_window).arg(m_screen);

    m_display = XOpenDisplay(displayName.toLatin1().constData());
    if(!m_display) {
        qDebug() << "Can't open display" << displayName;
        m_process.terminate();
        return;
    }

    int event_base;
    int error_base;
    int major_version;
    int minor_version;

    if (XTestQueryExtension(m_display, &event_base, &error_base, &major_version, &minor_version)) {
        qDebug() << QString( "XTest extension version %1.%2 enabled. Event %3, Error %4").arg(major_version).arg(minor_version).arg(event_base).arg(error_base);
        m_eventDisabled = false;
    } else {
        qDebug() << "Xtest extension disabled. Mouse and keyboard event will be disabled";
        m_eventDisabled = true;
    }
    emit started(m_window, m_screen);
}

void Xvfb::onStateChanged(QProcess::ProcessState newState)
{
    qDebug() << "Xvfb stateChanged" << newState;
}

bool Xvfb::initFbData()
{
    if (m_type == SharedMemory) {
        if (m_shmid && !m_sharedMem.isAttached()) {
            m_sharedMem.setKey(m_shmid);
            if ( m_sharedMem.attach(IdSharedMemory::ReadOnly) ) {
                qDebug() << "Attached. Size:" << m_sharedMem.size();
            } else {
                qDebug() << "Fail to attach to Shared Mem of Xvfb" << m_sharedMem.key() << m_sharedMem.error() << m_sharedMem.errorString();
                return false;
            }
        }
    } else {
        m_mmap = nullptr;
        m_xwdFile.setFileName(QString("%1/Xvfb_screen%2").arg(XvfbFileLocation).arg(m_screen));
        if (!m_xwdFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open" << m_xwdFile.fileName() << m_xwdFile.errorString();
            return false;
        } else {
            m_mmap = m_xwdFile.map(0, m_xwdFile.size());
            if (!m_mmap) {
                qDebug() << "Failed to map file to memory" << m_xwdFile.fileName();
                return false;
            }
        }
    }
    return true;
}

qint64 Xvfb::readFbData(qint64 &dataPosition, char *data, qint64 maxSize)
{
    if (m_type == SharedMemory) {
        qint64 size = ((m_sharedMem.size() - dataPosition) >= maxSize) ? maxSize : (m_sharedMem.size() - dataPosition);
        memcpy(data, (const char*)m_sharedMem.data() + dataPosition, size);
        dataPosition += size;
        return size;
    } else {
        qint64 size = ((m_xwdFile.size() - dataPosition) >= maxSize) ? maxSize : (m_xwdFile.size() - dataPosition);
        memcpy(data, (const char*)m_mmap + dataPosition, size);
        dataPosition += size;
        return size;
    }
}

QByteArray Xvfb::readFbData(qint64 &dataPosition, qint64 maxSize)
{
    if (m_type == SharedMemory) {
        qint64 size = ((m_sharedMem.size() - dataPosition) >= maxSize) ? maxSize : (m_sharedMem.size() - dataPosition);
        QByteArray data;
        data.append((const char*)m_sharedMem.data() + dataPosition, size);
        dataPosition += size;
        return data;
    } else {
        qint64 size = ((m_xwdFile.size() - dataPosition) >= maxSize) ? maxSize : (m_xwdFile.size() - dataPosition);
        QByteArray data;
        data.append((const char*)m_mmap + dataPosition, size);
        dataPosition += size;
        return data;
    }
}
