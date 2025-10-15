#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "TestPlugin_global.h"

#include "../Core/plugininterface.h"
#include "../Core/core.h"

//typedef int(Core::*getIntByString)(const QString&);

class TestPlugin : public Plugin
{
  Q_OBJECT
  Q_INTERFACES(Plugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
  Q_PLUGIN_METADATA(IID "org.PGE.DataReactor.DummyPlugin")
#endif
public:
  int init(QObject *core);
  QVariantHash getInfo();
  QObject* newInstance() { return this; }
};

#endif // TESTPLUGIN_H
