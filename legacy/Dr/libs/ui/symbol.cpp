#include "symbol.h"
#include <QFont>


void Symbol::componentComplete()
{
    QQuickText::componentComplete();
    QFont fnt = font();
    fnt.setFamily("FontAwesome");
    setFont(fnt);
    setHAlign(QQuickText::AlignHCenter);
    setVAlign(QQuickText::AlignVCenter);
}

int Symbol::symbol() const
{
    return text()[0].unicode();
}

void Symbol::setSymbol(int arg)
{
    QFont f = font();
    if (arg & Font_FontAwesome) {
        arg -= Font_FontAwesome;
        f.setFamily("FontAwesome");
    } else if (arg & Font_Entypo) {
        arg -= Font_Entypo;
        f.setFamily("Entypo");
    } else if (arg & Font_ModernUI) {
        arg -= Font_ModernUI;
        f.setFamily("Modern-UI-Icons---Drawing");
    }
    setFont(f);
    setText(QChar(arg));
//    QChar c(arg);
//    qDebug() << "Font" << f.family() << f.exactMatch() << arg << QString::number(arg,16);
    emit symbolChanged();
}



void Symbol::setPixelSize(int arg)
{
    if (font().pixelSize() == arg)
        return;

    QFont f = font();
    f.setPixelSize(arg);
    setFont(f);
    emit pixelSizeChanged();
}

void Symbol::setPointSize(qreal arg)
{
    if (font().pointSizeF() == arg)
        return;

    QFont f = font();
    f.setPointSizeF(arg);
    setFont(f);
    emit pointSizeChanged();
}


int Symbol::pixelSize() const
{
    return font().pixelSize();
}

int Symbol::pointSize() const
{
    return font().pointSize();
}
