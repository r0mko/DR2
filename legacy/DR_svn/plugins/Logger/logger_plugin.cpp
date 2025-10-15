#include "logger_plugin.h"
#include "logger.h"

#include <qqml.h>

void LoggerPlugin::registerTypes(const char *uri)
{
//     @uri DataReactor.Log
    qmlRegisterType<Logger>(uri, 1, 0, "Logger");
}


