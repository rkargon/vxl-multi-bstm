#ifndef VUL_EXPORT_H
#define VUL_EXPORT_H

#include <vxl_config.h> // get VXL_BUILD_SHARED

#ifndef VXL_BUILD_SHARED  // if not a shared build
# define vul_EXPORT
#else  // this is a shared build
# ifdef vul_EXPORTS  // if building this library
#  if defined(_WIN32) || defined(WIN32)
#   define vul_EXPORT __declspec(dllexport)
#  else
#   define vul_EXPORT
#  endif
# else // we are using this library and it is built shared
#  if defined(_WIN32) || defined(WIN32)
#   define vul_EXPORT __declspec(dllimport)
#  else
#   define vul_EXPORT
#  endif
# endif
#endif

#endif
