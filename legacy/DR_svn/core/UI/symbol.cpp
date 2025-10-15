#include "symbol.h"
#include <QFont>

//void Symbol::classBegin()
//{
//    QQuickText::classBegin();
//}

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
    if (QString(QChar(arg)) == text())
        return;
    setText(QString(QChar(arg)));
    emit symbolChanged();
}

void Symbol::setSymbolFont(Symbol::SymbolFont arg)
{
    if (m_font == arg)
        return;
    QFont f = font();
    switch (arg) {
    case FontAwesome:
        f.setFamily("FontAwesome");
        break;
    case Entypo: {
        f.setFamily("Entypo");
        break;
    }
    case ModernUI: {
        f.setFamily("Modern-UI-Icons---Drawing");
        break;
    }
    default:
        break;
    }
    setFont(f);
    m_font = arg;
    emit symbolFontChanged();
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


Symbol::SymbolFont Symbol::symbolFont() const
{
    return m_font;
}

int Symbol::pixelSize() const
{
    return font().pixelSize();
}

int Symbol::pointSize() const
{
    return font().pointSize();
}
