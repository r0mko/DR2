#include "ui.h"
#include <QMetaType>
#include <QMetaObject>
#include <QDebug>
#include <QScopedPointer>
#include <QColor>
#include <QQuickItem>
#include "targetdef.h"
#include <QQuickWindow>
//#include "private/qquickwindow_p.h"
QQmlEngine *UIHelper::engine = nullptr;
UIHelper *UIHelper::qmlSingleton = nullptr;
QHash<QString, Target> UIHelper::targetStorage = QHash<QString, Target>();
QMap<QString, EnumDef> UIHelper::enums = QMap<QString, EnumDef>();

QString UIHelper::getChar(int chr) const
{
    return QString(QChar(chr));
}

void UIHelper::init() const
{
    //    initEnums();

    qt_noop();
}

QQmlComponent *UIHelper::getComponent(QString target, QVariant value, QString variant)
{
    return getComponentForType(target, value.typeName(), variant);
}

QQmlComponent *UIHelper::getComponent(QString target, QString typeName, QString variant)
{
    //    QString fullCompName = QString("%1/%2").arg(target).arg(typeName);
    return getComponentForType(target, typeName.toLocal8Bit(), variant);

}


void UIHelper::setTargets(QObject *arg)
{
    if (m_targets == arg)
        return;

    m_targets = arg;
    emit targetsChanged(arg);
}

UIHelper::UIHelper(QObject *parent) :
    QObject(parent)
{
    Q_ASSERT(engine != nullptr);
    qDebug() << "Constructed UIHelper";
    QList<QQmlError> errors;

    //    initEnums();
}

QVariantMap UIHelper::colorToHsla(const QColor &color)
{
    QVariantMap ret;
    ret["h"] = color.hslHueF();
    ret["s"] = color.hslSaturationF();
    ret["l"] = color.lightnessF();
    ret["a"] = color.alphaF();
    return ret;
}

QVariantMap UIHelper::colorToHsva(const QColor &color)
{
    QVariantMap ret;
    ret["h"] = color.hsvHueF();
    ret["s"] = color.hsvSaturationF();
    ret["v"] = color.valueF();
    ret["a"] = color.alphaF();
    return ret;
}

QColor UIHelper::getColor(const QColor &backgroundColor, double hue, double saturation, double contrast)
{
    return getContrastColor(backgroundColor, hue, saturation, contrast);
}

QColor UIHelper::fromHSLum(double hue, double saturation, double luminance)
{
    qreal targetLum = 0;
    qreal l = 0;
    QColor ret;
    while (targetLum < luminance && l <= 1.0) {
        ret = QColor::fromHslF(hue, saturation, l);
        targetLum = relativeLuminance(ret);
        l += 0.002;
    }
    return ret;
}

qreal UIHelper::luminance(const QColor &color)
{
    return relativeLuminance(color);
}

qreal UIHelper::contrastRatio(const QColor &color1, const QColor &color2)
{
    return contrastRatio(color1, color2);
}

QPointF UIHelper::mapToScreen(QQuickItem *item, qreal x, qreal y)
{
    QPointF scenePt = item->mapToScene(QPointF(x, y));
    qDebug() << "Scene pt" << scenePt << x << y << item;
    return item->window()->mapToGlobal(scenePt.toPoint());
}

bool UIHelper::isTargetRegistered(const QString &targetName) const
{
    return targetStorage.contains(targetName);
}

TargetDef *UIHelper::target(const QString &targetName)
{
    if (isTargetRegistered(targetName))
        return 0;
    return targetStorage[targetName].source;
}

QObject *UIHelper::targets() const
{
    return m_targets;
}


QList<int> findTypes(const QString &typeName)
{
    QString p = typeName;
    p.replace("?", "[A-Za-z_][A-Za-z_0-9]*");
    QRegularExpression rx(p);
    QList<int> ret;
    int i = QMetaType::FirstCoreType;
    while (i < QMetaType::HighestInternalId) {
        QString t = QString(QMetaType::typeName(i));
        if (rx.match(t).hasMatch()) {
            ret.append(i);
        }
        ++i;
    }
    i = QMetaType::User;
    while (QMetaType::isRegistered(i)) {
        QString t = QString(QMetaType::typeName(i));
        if (rx.match(t).hasMatch()) {
//            qDebug() << "Type" << t << "accepted for pattern" << typeName;
            ret.append(i);
        }
        ++i;
    }
    qDebug() << "Registered" << ret.size() << "types for pattern" << typeName;
    return ret;
}

void registerTypeDelegate(TargetDef *source, const QString &target, int typeId, QQmlComponent *component, QString variantName, QVariant extraData)
{
    QString componentName = component->url().fileName().replace(".qml", "");
    Target &t = UIHelper::targetStorage[target];
    t.source = source;
    auto it = t.delegates.find(typeId);
    if (it == t.delegates.end()) {
        Delegate d;
        if (variantName.isEmpty())
            d.delegate = component;
        else {
            d.variants[variantName] = { component, extraData };

        }
        t.delegates[typeId] = d;
    } else {
        Delegate &del = it.value();
        int c = 0;
        bool nameSubst = false;
        if (variantName.isEmpty()) {
            variantName = "variant";
            nameSubst = true;
        }
        QString vname = variantName;
        while (del.variants.contains(vname)) {
            vname = QString("%1%2").arg(variantName).arg(++c);
        }
        del.variants[vname] = { component, extraData };
        componentName.append(QString("_%1").arg(vname));
        if (nameSubst) {
            qDebug() << "WARNING! Delegate for" << QMetaType::typeName(typeId) << "type for target" << target\
                     << "already registered, but no variant name provided. Component registered as variant"\
                     << vname << QString("(%1)").arg(componentName);
        }
    }
}

void registerDelegateComponent(TargetDef *source, const QString &target, const QString &typeName, QQmlComponent *component, QString variantName, QVariant extraData)
{
    Q_ASSERT(component != nullptr);
    int typeId = 0;

    if (typeName.contains("?")) {
        QList<int> typeList = findTypes(typeName);
        for (int typeId:typeList) {
            registerTypeDelegate(source, target, typeId, component, variantName, extraData);
        }
        return;
    }

    if (typeName != UNKNOWN_TYPE) {
        typeId = QMetaType::type(typeName.toLocal8Bit().constData());
        if (typeId == QMetaType::UnknownType) {
            qWarning().nospace() << "Unable to register delegate for target " << target << ": unknown type <" << typeName.toLocal8Bit().constData() << ">";
            return;
        }
    }
    registerTypeDelegate(source, target, typeId, component, variantName, extraData);

}



QString addComponent(const QString &target, const QUrl &componentUrl)
{

    QScopedPointer<QQmlComponent> comp(new QQmlComponent(UIHelper::engine, componentUrl, UIHelper::engine));
    if (comp->isError()) {
        qWarning() << comp->errorString();
        return QString();
    }
    QString componentName = componentUrl.fileName().replace(".qml", "");
    QString name = QString("%1/%2").arg(target).arg(componentName);
    UIHelper::targetStorage[target];
    return name;
}


QQmlComponent *getComponentForType(const QString &target, const QByteArray &typeName, QString variantName, QVariant *extraData)
{
    int typeId = QMetaType::type(typeName.constData());
    if (typeId == QMetaType::UnknownType && typeName != UNKNOWN_TYPE) {
        //        qWarning().nospace() << "Unable to load component for unknown type <" << typeName.constData() << ">";
        return nullptr;
    }
    const auto &delegates = UIHelper::targetStorage[target].delegates;
    auto i = delegates.find(typeId);
    if (i == delegates.cend())
        return nullptr;
    const Delegate &del = i.value();
    if (variantName.isEmpty())
        return del.delegate;
    if (del.variants.contains(variantName)) {
//        qDebug() << "Variant found" << variantName << "for type" << typeName;
        const Delegate::Variant &var = del.variants.value(variantName);
        if (extraData && var.extraData.isValid())
            *extraData = var.extraData;
        return var.component;
    }
//    qDebug() << "Variant not found" << variantName;
    return del.delegate;
}

//void registerDelegateComponent(const QByteArray &typeName, const QString &fullComponentName, QString variantName, TargetDef *source)
//{
//    QQmlComponent *comp = UIHelper::components.value(fullComponentName, nullptr);
//    if (!comp)
//        return;

//    int typeId = QMetaType::type(typeName.constData());
//    if (typeId == QMetaType::UnknownType) {
//        qWarning() << "Unknown type" << typeName.constData();
//        return;
//    }
//    QString target = fullComponentName.section("/", 0, 0);

//    if (!UIHelper::targetStorage.contains(target)) {
//        qDebug() << "No target" << target;
//        return;
//    }
//    Delegate &d = UIHelper::targetStorage[target].delegates[typeId];
//    if (variantName.isEmpty())
//        d.delegate = comp;
//    else
//        d.variants[variantName] = comp;
//    qDebug() << "Registered delegate" << fullComponentName << comp << "for type" << typeName;
//}


QColor getContrastColor(const QColor &background, double desiredHue, double desiredSaturation, double desiredContrast)
{
    double m_hue = desiredHue;
    double m_sat = desiredSaturation;
    double m_cont = desiredContrast * 20.0 + 1.0;

    double rl, contrast;
    double lightness = 0;
    for( int  l = 0; l < 256; ++l )
    {
        rl = relativeLuminance( QColor::fromHslF( m_hue, m_sat, l / 255.0 ) );
        contrast = contrastRatio( background, QColor::fromHslF( m_hue, m_sat, l / 255.0 ) );
        if( qAbs( contrast - m_cont ) < 0.1 )
            lightness = l;
    }
    if( m_cont < 3 ) qDebug() << "contrast is lower than defined by W3C";
    QColor ret = QColor::fromHslF(desiredHue, desiredSaturation, lightness / 255.0);
    //    qDebug() << background << relativeLuminance(background) << relativeLuminance(ret) << desiredSaturation << desiredContrast << contrast;
    return ret;
}


double relativeLuminance(const QColor &color1)
{
    double r = color1.redF();
    double g = color1.greenF();
    double b = color1.blueF();
    double R, G, B;
    if (r <= 0.03928) R = r / 12.92; else R = qPow ((r + 0.055) / 1.055, 2.4 );
    if (g <= 0.03928) G = g / 12.92; else G = qPow ((g + 0.055) / 1.055, 2.4 );
    if (b <= 0.03928) B = b / 12.92; else B = qPow ((b + 0.055) / 1.055, 2.4 );
    return R * 0.2126 + G * 0.7152 + B * 0.0722;
}


double contrastRatio(const QColor &color1, const QColor &color2)
{
    double a = relativeLuminance (color1) + 0.05;
    double b = relativeLuminance (color2) + 0.05;
    return  ( b > a ) ? b / a : a / b;
}

QQmlComponent *getComponentForType(TargetDef *target, const QByteArray &typeName, QString variantName, QVariant *extraData)
{
    Q_ASSERT(target != nullptr);
    return getComponentForType(target->targetName(), typeName, variantName, extraData);
}



//void EnumDef::set(const char* strData)
//{
//    QString str = QString(strData);
//    QStringList elist = str.split(",");
//    int value;
//    for (QString s:elist) {
//        s = s.trimmed();
//        if (s.indexOf("=") != -1) {
//            // assign
//            QStringList sides = s.split("=");
//            QString rhs = sides.last().trimmed();
//            QString lhs = sides.first().trimmed();
//            if (rhs.contains("|")) {
//                // OR
//                QStringList elems = rhs.split("|");
//                for (QString e:elems) {

//                }
//            }
//            bool isNum = false;
//            if (rhs.startsWith("0x")) {
//                value = rhs.toInt(&isNum, 16);
//            }
//        }
//    }
//}

//int EnumDef::decode(const QString &str)
//{
//    bool isNum = false;
//    int value;
//    value = str.toInt(&isNum, 0);
//    if (!isNum) {
//        // check if str is local id
//        if (str.startsWith("Qt::")) {
//            // global id
//            if (UIHelper::enums.contains(str)) {

//            }
//        } else {
//            if (entries.contains(str))
//                return entries.value(str);
//            else
//                qDebug() << "Unknown entry" << str;
//        }
//    }
//}
