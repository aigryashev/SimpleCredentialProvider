#ifndef CRM_DLL_GLOBAL_H
#define CRM_DLL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CRM_DLL_LIBRARY)
#  define CRM_DLL_EXPORT Q_DECL_EXPORT
#else
#  define CRM_DLL_EXPORT Q_DECL_IMPORT
#endif

#endif // CRM_DLL_GLOBAL_H
