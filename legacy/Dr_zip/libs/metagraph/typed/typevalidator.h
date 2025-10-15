#ifndef TYPEVALIDATOR_H
#define TYPEVALIDATOR_H
#include <QValidator>
class TypeFactory;
class TypeValidator : public QValidator
{
    Q_OBJECT
    Q_PROPERTY(TypeFactory* typeFactory READ typeFactory WRITE setTypeFactory NOTIFY typeFactoryChanged)

    TypeFactory* m_typeFactory = nullptr;
    QStringList m_typeList;

public:
    TypeValidator(QObject *parent = 0);

    State validate(QString &string, int &pos) const;
    void fixup(QString &string) const;

    TypeFactory* typeFactory() const;
    void setTypeFactory(TypeFactory* typeFactory);

signals:
    void typeFactoryChanged();
private slots:
};


#endif // TYPEVALIDATOR_H
