#ifndef BGCMS_GLOBAL_H
#define BGCMS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BGCMSAPP_LIBRARY)
#  define BGCMSAPP_EXPORT Q_DECL_EXPORT
#else
#  define BGCMSAPP_EXPORT Q_DECL_IMPORT
#endif

#endif // BGCMS_GLOBAL_H