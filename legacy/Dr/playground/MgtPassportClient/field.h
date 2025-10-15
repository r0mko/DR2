#ifndef FIELD_H
#define FIELD_H

#include <QObject>
#include <QMetaEnum>

class Field : public QObject
{
public:
    enum ControlType {
        NO_CONTROL,
        TEXT_CONTROL,
        PASS_CONTROL,
        COMBO_CONTROL,
        BOOL_CONTROL,
        INT_CONTROL
    };
    enum FilterType {
        NO_FILTER,
        STRING_FILTER,
        DATERANGE_FILTER
    };
    Q_ENUMS(ControlType)
    Q_ENUMS(FilterType)

private:
    Q_OBJECT
    Q_PROPERTY(QString name READ name) // field name in database // role name in table
    Q_PROPERTY(QString caption READ caption) // column caption in table
    Q_PROPERTY(bool hidden READ hidden) // whether to show in table / form
    Q_PROPERTY(ControlType control READ control) // control type to use in form
    Q_PROPERTY(FilterType filter READ filter) // filter type to use

    Q_PROPERTY(QString datasource READ datasource) // related table
    Q_PROPERTY(QString id READ id) // id field to use in related table
    Q_PROPERTY(QString textRole READ textRole) // field from related table to get substitution from


public:
    QString m_name;
    QString m_caption;
    bool m_hidden;
    ControlType m_control;
    FilterType m_filter;
    QString m_textRole;
    QString m_datasource;
    QString m_id;

public:
    explicit Field(QObject *parent = 0);
    ~Field();




public:// property getters
    QString name() const
    {
        return m_name;
    }

    QString caption() const
    {
        return m_caption;
    }

    bool hidden() const
    {
        return m_hidden;
    }

    ControlType control() const
    {
        return m_control;
    }

    FilterType filter() const
    {
        return m_filter;
    }

    QString textRole() const
    {
        return m_textRole;
    }

    QString datasource() const
    {
        return m_datasource;
    }

    QString id() const
    {
        return m_id;
    }

};

#endif // FIELD_H
