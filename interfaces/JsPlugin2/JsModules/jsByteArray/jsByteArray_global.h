#ifndef JSBYTEARRAY_GLOBAL_H
#define JSBYTEARRAY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(JSBYTEARRAY_LIBRARY)
#  define JSBYTEARRAY_EXPORT Q_DECL_EXPORT
#else
#  define JSBYTEARRAY_EXPORT Q_DECL_IMPORT
#endif

#endif // JSBYTEARRAY_GLOBAL_H
