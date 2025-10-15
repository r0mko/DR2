#ifndef RESOURCEDEFS_H
#define RESOURCEDEFS_H
#include <QtGlobal>
#include <QtCore>
struct RUID // unique resource identifier
{
  int   coreId;
  int   typeId;
  uint  index;
  RUID() : coreId(0), typeId(0), index(0) {}
};

Q_DECLARE_METATYPE(RUID)

#endif // RESOURCEDEFS_H
