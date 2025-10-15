#ifndef UIHELPER_H
#define UIHELPER_H
#include <QObject>
#include <QMap>
#include <QHash>
#include <QByteArray>
#include <QtQml>
#include <QSet>
#include "targetdef.h"
#include <QColor>


#define UNKNOWN_TYPE "Invalid"

class QQuickItem;

struct Delegate
{
    struct Variant
    {
        QQmlComponent *component;
        QVariant extraData;
    };

    QQmlComponent *delegate;
    QHash<QString, Variant> variants;
};

struct Target
{
    QHash<int, Delegate> delegates;
    TargetDef *source;
};

struct EnumDef
{
    QMap<int, QString> entries;
    void set(const char *strData);
    int decode(const QString &str);
};

class UIHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* targets READ targets WRITE setTargets NOTIFY targetsChanged)

public:
    static QQmlEngine *engine;
    static UIHelper *qmlSingleton;
    static QHash<QString, Target> targetStorage;
    static QMap<QString, EnumDef> enums;
    UIHelper(QObject *parent = 0);
    ~UIHelper() { qmlSingleton = nullptr; }

    Q_INVOKABLE QVariantMap colorToHsla(const QColor &color);
    Q_INVOKABLE QVariantMap colorToHsva(const QColor &color);
    Q_INVOKABLE QColor getColor(const QColor &backgroundColor, double hue, double saturation, double contrast);
    Q_INVOKABLE QColor fromHSLum(double hue, double saturation, double luminance);
    Q_INVOKABLE qreal luminance(const QColor &color);
    Q_INVOKABLE qreal contrastRatio(const QColor &color1, const QColor &color2);
    Q_INVOKABLE QPointF mapToScreen(QQuickItem *item, qreal x, qreal y);

    /**
     * @brief isTargetRegistered checks if targetName is registered
     * @return returns true if target targetName registered, otherwise returns false
     */
    Q_INVOKABLE bool isTargetRegistered(const QString &targetName) const;

    /**
     * @brief returns TargetDef (QML type Target) by targetName
     * @param targetName name of the target
     * @return
     */
    TargetDef* target(const QString &targetName);
//    static void initEnums();
    QObject* targets() const;

public slots:
    /**
     * @brief getChar get Unicode character by char code
     */
    QString getChar(int chr) const;
    void init() const;
    QQmlComponent *getComponent(QString target, QVariant value, QString variant = QString());
    QQmlComponent *getComponent(QString target, QString typeName, QString variant = QString());

    void setTargets(QObject* arg);

signals:
    void targetsChanged(QObject* arg);

private:
    UIHelper(const UIHelper &other) = delete;

    QObject* m_targets;
};

QQmlComponent *getComponentForType(const QString &target, const QByteArray &typeName, QString variantName, QVariant *extraData = 0);
QQmlComponent *getComponentForType(TargetDef *target, const QByteArray &typeName, QString variantName = QString(), QVariant *extraData = 0);

QString addComponent(const QString &target, const QUrl &componentUrl);

bool addComponentAlias(const QString &fullComponentName, const QString &fullAlias);
bool addComponentAlias(const QString &target, const QString &componentName, const QString &aliasTarget, const QString &aliasComponent);

void registerDelegateComponent(TargetDef *source, const QString &target, const QString &typeName, QQmlComponent *component, QString variantName = QString(), QVariant extraData = QVariant());

QDebug operator<<(QDebug dbg, const Delegate &d);
QColor getContrastColor(const QColor &background, double desiredHue, double desiredSaturation, double desiredContrast);
double relativeLuminance(const QColor &color1);
double contrastRatio(const QColor &color1, const QColor &color2);


#endif // UIHELPER_H
