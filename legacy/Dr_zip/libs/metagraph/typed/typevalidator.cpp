#include "typevalidator.h"
#include "type.h"
#include "qdebug.h"
TypeValidator::TypeValidator(QObject *parent) :
    QValidator(parent)
{}

    QValidator::State TypeValidator::validate(QString &string, int &pos) const
{
    if (!m_typeFactory || m_typeFactory->isValidType(string))
        return QValidator::Acceptable;
    for (QString type : m_typeList) {
        if (type.toLower().startsWith(string.toLower())) {
            return QValidator::Intermediate;
        }
    }
    return QValidator::Invalid;
}

void TypeValidator::fixup(QString &string) const
{
    if (!m_typeFactory)
        return;
    QStringList guess;
    QStringList guess_ic;
    for (QString type : m_typeList) {
        if (type.startsWith(string)) {
            guess.append(type);
        } else if (type.toLower().startsWith(string.toLower())) {
            guess_ic.append(type);
        }
    }
    if (guess.size() == 1)
        string = guess.first();
    else if (guess_ic.size() == 1) {
        string = guess_ic.first();
    }
}

TypeFactory *TypeValidator::typeFactory() const
{
    return m_typeFactory;
}

void TypeValidator::setTypeFactory(TypeFactory *typeFactory)
{
    if (m_typeFactory == typeFactory)
        return;
    if (m_typeFactory) {
        m_typeFactory->disconnect(0, this, 0);
    }
    m_typeFactory = typeFactory;
    m_typeList = m_typeFactory->allTypeNames();
    connect(m_typeFactory, &TypeFactory::typesInfoChanged, this, [this]() { m_typeList = m_typeFactory->allTypeNames(); });
    emit typeFactoryChanged();

}
