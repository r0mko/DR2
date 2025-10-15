#ifndef DROBJECT_H
#define DROBJECT_H

#include <QObject>
#include <QVariant>
#include <QMap>
#include <QVector>

typedef void(*FunctionPtr)(int&, int&, void**);
typedef QVector<void*> ArgsStream;

class DRObject : public QObject
{
    Q_OBJECT
    QVariantMap variables;
    QList<ArgsStream> algArgs;
public:
    explicit DRObject(DRObject *parent = 0);
    static void sumf(int& callOffset, int& argOffset, void** args);
    int addAlgorithm(const ArgsStream& args) { algArgs.append(args); return algArgs.size()-1; }

signals:
    
public slots:
    
};

class DAlgorithm
{
    QVector<FunctionPtr> callStream;
    DRObject *obj;
    int algNum;
public:
    DAlgorithm() { };
    ~DAlgorithm();
    void exec();
    void setup(DRObject* obj);
};

#endif // DROBJECT_H
