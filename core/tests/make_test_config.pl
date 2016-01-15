#! /bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

# wheeler at crd.ge.com
#
# Prints test_config.cxx to stdout.
#
# Usage:
#   cd $VXLSRC/core/tests
#   rm test_config.cxx
#   ./make_test_config.pl > test_config.cxx

use strict;
use diagnostics;

############################################################################

print <<EOF;
// Do not edit this file.  It is created automatically from make_test_config.pl

#include <vcl_compiler.h>
#include <vxl_config.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>

// It is quite difficult to do anything out of the norm with the C preprocessor

// quote() has the effect of preprocessor expansion of the argument
// Inner parentheses are needed with quote() to protect empty definitions
// and definitions that have commas
// quote0() protects (\") and quotes ("") the expanded argument
#define quote0(x) #x
#define quote(x) quote0((x))

void test_config()
{
  // This test does not test much.  Mostly, it prints configuration
  // macros so they can be viewed through Dart.  Some sanity checks are
  // at the end.

  vcl_cout << "All macro definitions are shown inside parentheses\\n";

EOF

############################################################################

my $var_exp;    # preprocessor variable expression (with paren)
my $var_name;   # preprocessor variable name (no paren)
for $var_exp (

  # from vcl_config_compiler.h
  'VCL_HAS_BOOL',
  'VCL_HAS_DYNAMIC_CAST',
  'VCL_HAS_RTTI',
  'VCL_HAS_TYPENAME',
  'VCL_HAS_EXPORT',
  'VCL_HAS_MUTABLE',
  'VCL_HAS_EXPLICIT',
  'VCL_FOR_SCOPE_HACK',
  'VCL_DEFAULT_VALUE(x)',
  'VCL_CAN_STATIC_CONST_INIT_INT',
  'VCL_STATIC_CONST_INIT_INT_DECL(x)',
  'VCL_STATIC_CONST_INIT_INT_DEFN(x)',
  'VCL_STATIC_CONST_INIT_INT_NO_DEFN',
  'VCL_CAN_STATIC_CONST_INIT_FLOAT',
  'VCL_STATIC_CONST_INIT_FLOAT_DECL(x)',
  'VCL_STATIC_CONST_INIT_FLOAT_DEFN(x)',
  'VCL_STATIC_CONST_INIT_FLOAT_NO_DEFN',
  'VCL_HAS_MEMBER_TEMPLATES',
  'VCL_CAN_DO_PARTIAL_SPECIALIZATION',
  'VCL_DEFINE_SPECIALIZATION',
  'VCL_CANNOT_SPECIALIZE_CV',
  'VCL_NULL_TMPL_ARGS',
  'VCL_ALLOWS_INLINE_INSTANTIATION',
  'VCL_NEEDS_INLINE_INSTANTIATION',
  'VCL_DO_NOT_INSTANTIATE(text,ret)',
  'VCL_UNINSTANTIATE_SPECIALIZATION(symbol)',
  'VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(symbol)',
  'VCL_CAN_DO_STATIC_TEMPLATE_MEMBER',
  'VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(symbol)',
  'VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(symbol)',
  'VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER',
  'VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD',
  'VCL_OVERLOAD_CAST(T,x)',
  'VCL_NO_STATIC_DATA_MEMBERS',
  'VCL_HAS_TEMPLATE_SYMBOLS',
  'VCL_DEFAULT_TMPL_ARG(arg)',
  'VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER',
  'VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER',
  'VCL_SUNPRO_CLASS_SCOPE_HACK(A)',
  'VCL_HAS_EXCEPTIONS',
  'VCL_HAS_NAMESPACES',
  'VCL_ALLOWS_NAMESPACE_STD',
  'VCL_NEEDS_NAMESPACE_STD',

  # from vcl_config_headers.h
  'VCL_CXX_HAS_HEADER_CASSERT',
  'VCL_CXX_HAS_HEADER_CSETJMP',
  'VCL_CXX_HAS_HEADER_CSTDIO',
  'VCL_CXX_HAS_HEADER_CTIME',
  'VCL_CXX_HAS_HEADER_CCTYPE',
  'VCL_CXX_HAS_HEADER_CLIMITS',
  'VCL_CXX_HAS_HEADER_CSIGNAL',
  'VCL_CXX_HAS_HEADER_CSTDLIB',
  'VCL_CXX_HAS_HEADER_CWCHAR',
  'VCL_CXX_HAS_HEADER_CERRNO',
  'VCL_CXX_HAS_HEADER_CLOCALE',
  'VCL_CXX_HAS_HEADER_CSTDARG',
  'VCL_CXX_HAS_HEADER_CSTRING',
  'VCL_CXX_HAS_HEADER_CWCTYPE',
  'VCL_CXX_HAS_HEADER_CMATH',
  'VCL_CXX_HAS_HEADER_CSTDDEF',
  'VCL_CXX_HAS_HEADER_ALGORITHM',
  'VCL_CXX_HAS_HEADER_IOMANIP',
  'VCL_CXX_HAS_HEADER_LIST',
  'VCL_CXX_HAS_HEADER_OSTREAM',
  'VCL_CXX_HAS_HEADER_STREAMBUF',
  'VCL_CXX_HAS_HEADER_BITSET',
  'VCL_CXX_HAS_HEADER_IOS',
  'VCL_CXX_HAS_HEADER_LOCALE',
  'VCL_CXX_HAS_HEADER_QUEUE',
  'VCL_CXX_HAS_HEADER_STRING',
  'VCL_CXX_HAS_HEADER_COMPLEX',
  'VCL_CXX_HAS_HEADER_IOSFWD',
  'VCL_CXX_HAS_HEADER_MAP',
  'VCL_CXX_HAS_HEADER_SET',
  'VCL_CXX_HAS_HEADER_TYPEINFO',
  'VCL_CXX_HAS_HEADER_DEQUE',
  'VCL_CXX_HAS_HEADER_IOSTREAM',
  'VCL_CXX_HAS_HEADER_MEMORY',
  'VCL_CXX_HAS_HEADER_SSTREAM',
  'VCL_CXX_HAS_HEADER_UTILITY',
  'VCL_CXX_HAS_HEADER_EXCEPTION',
  'VCL_CXX_HAS_HEADER_ISTREAM',
  'VCL_CXX_HAS_HEADER_NEW',
  'VCL_CXX_HAS_HEADER_STACK',
  'VCL_CXX_HAS_HEADER_VALARRAY',
  'VCL_CXX_HAS_HEADER_FSTREAM',
  'VCL_CXX_HAS_HEADER_ITERATOR',
  'VCL_CXX_HAS_HEADER_NUMERIC',
  'VCL_CXX_HAS_HEADER_STDEXCEPT',
  'VCL_CXX_HAS_HEADER_VECTOR',
  'VCL_CXX_HAS_HEADER_FUNCTIONAL',
  'VCL_CXX_HAS_HEADER_STRSTREAM',

  # from vxl_config.h
  'VXL_LITTLE_ENDIAN',
  'VXL_BIG_ENDIAN',
  'VXL_HAS_BYTE',
  'VXL_BYTE_STRING',
  'VXL_HAS_INT_8',
  'VXL_INT_8_STRING',
  'VXL_HAS_INT_16',
  'VXL_INT_16_STRING',
  'VXL_HAS_INT_32',
  'VXL_INT_32_STRING',
  'VXL_HAS_INT_64',
  'VXL_INT_64_STRING',
  'VXL_INT_64_IS_LONG',
  'VXL_HAS_IEEE_32',
  'VXL_IEEE_32_STRING',
  'VXL_HAS_IEEE_64',
  'VXL_IEEE_64_STRING',
  'VXL_HAS_IEEE_96',
  'VXL_IEEE_96_STRING',
  'VXL_HAS_IEEE_128',
  'VXL_IEEE_128_STRING',
  'VXL_HAS_PTHREAD_H',
  'VXL_HAS_SEMAPHORE_H',
  'VXL_UNISTD_HAS_USECONDS_T',
  'VXL_UNISTD_HAS_INTPTR_T',
  'VXL_UNISTD_HAS_UALARM',
  'VXL_UNISTD_HAS_USLEEP',
  'VXL_UNISTD_HAS_LCHOWN',
  'VXL_UNISTD_HAS_PREAD',
  'VXL_UNISTD_HAS_PWRITE',
  'VXL_UNISTD_HAS_TELL',
  'VXL_UNISTD_HAS_GETPID',
  'VXL_UNISTD_HAS_GETHOSTNAME',
  'VXL_STDLIB_HAS_QSORT',
  'VXL_STDLIB_HAS_LRAND48',
  'VXL_STDLIB_HAS_DRAND48',
  'VXL_IEEEFP_HAS_FINITE',
  'VXL_C_MATH_HAS_FINITE',
  'VXL_C_MATH_HAS_SQRTF',
  'VXL_C_MATH_HAS_LROUND',
  'VXL_UNISTD_USLEEP_IS_VOID',
  'VXL_TWO_ARG_GETTIME',
  'VXL_HAS_IEEEFP_H',

  # from vcl_compiler.h
  'VCL_SUNPRO_CC',
  'VCL_SUNPRO_CC_5',
  'VCL_SUNPRO_CC_50',
  'VCL_SUNPRO_CC_56',
  'VCL_GCC',
  'VCL_GCC_295',
  'VCL_GCC_28',
  'VCL_GCC_27',
  'VCL_GCC_3',
  'VCL_GCC_30',
  'VCL_GCC_31',
  'VCL_GCC_32',
  'VCL_GCC_33',
  'VCL_GCC_34',
  'VCL_GCC_4',
  'VCL_GCC_40',
  'VCL_GCC_41',
  'VCL_GCC_5',
  'VCL_GCC_50',
  'VCL_GCC_51',
  'VCL_GCC_52',
  'VCL_GCC_53',
  'VCL_ICC',
  'VCL_ICC_8',
  'VCL_ICC_80',
  'VCL_ICC_81',
  'VCL_WIN32',
  'VCL_VC',
  'VCL_VC_DOTNET',
  'GNU_LIBSTDCXX_V3',

  # from VXL CMake configuration files
  'VXL_WARN_DEPRECATED',
  'VXL_WARN_DEPRECATED_ONCE',
  'VXL_WARN_DEPRECATED_ABORT',

  # from core/tests/CMakeLists.txt (defines derived from CMake variables)
  'CMAKE_DEFINES_VARIABLE_UNIX',
  'CMAKE_DEFINES_VARIABLE_CYGWIN',
  'CMAKE_DEFINES_VARIABLE_MINGW',
  'CMAKE_DEFINES_VARIABLE_WIN32',
  'CMAKE_DEFINES_VARIABLE_APPLE',

  # language related
  '__cplusplus',
  '__STDC__',
  '__STDC_VERSION__',
  'NDEBUG',

  # compiler related
  '__STRICT_ANSI__',    # gcc
  '__GNUC__',           # gcc, icc
  '__GNUG__',           # gcc, icc
  '__GNUC_MINOR__',     # gcc, icc
  '__GNUC_PATCHLEVEL__',# gcc, icc
  '__VERSION__',        # gcc, icc
  '__OPTIMIZE__',       # gcc, icc
  '__ICC',              # icc
  '__ECC',              # icc?
  '__INTEL_COMPILER',   # icc
  '__INTEL_CXXLIB_ICC', # icc using its own library?
  '__SUNPRO_CC',        # sun
  '_MSC_VER',           # msvc++
  '_COMPILER_VERSION',  # sgi

  # OS related
  '__CYGWIN__',         # cygwin
  '__MINGW32__',        # MinGW
  '_WIN32',             # windows
  '__sgi',              # sgi
  '__alpha__',          # Alpha
  '__APPLE__',          # MacOS X
  'macintosh',          #
  'unix',               #
  '__unix',             #
  '__unix__',           #
  '__linux',            # icc
  '__linux__',          # icc

  # other
  '__DATE__',           # gcc (set at compile time)
  '__TIME__',           # gcc (set at compile time)

  ) {
$var_name = $var_exp;
$var_name =~ s/\(.*\)//;        # remove the arguments and parentheses
print <<EOF;
  vcl_cout << "$var_exp ";
#ifdef $var_name
  vcl_cout << "is set to " << quote($var_exp);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

EOF
}

############################################################################

print <<EOF;
  vcl_cout << vcl_endl;

  // These are types that we expect every machine to have.

EOF

############################################################################

my $var_type_size;
for $var_type_size (
  'VXL_HAS_BYTE,vxl_byte,1',
  'VXL_HAS_INT_8,vxl_uint_8,1',
  'VXL_HAS_INT_16,vxl_uint_16,2',
  'VXL_HAS_INT_32,vxl_uint_32,4',
  'VXL_HAS_IEEE_32,vxl_ieee_32,4',
  'VXL_HAS_IEEE_64,vxl_ieee_64,8'
  ) {
my $var;
my $type;
my $size;
($var,$type,$size) = split /,/, $var_type_size;

print <<EOF;
  testlib_test_begin( "Size of $type" );
#if $var
  testlib_test_perform( sizeof( $type ) == $size );
#else
  vcl_cout << "(no $type)" << vcl_endl;
  testlib_test_perform( false );
#endif

EOF
}

############################################################################

print <<EOF;
  vcl_cout << vcl_endl;

  // These may not exist on some platforms.  If they exist, they should
  // be of the correct size.

EOF

############################################################################

for $var_type_size (
  'VXL_HAS_INT_64,vxl_uint_64,8',
  'VXL_HAS_IEEE_96,vxl_ieee_96,12',
  'VXL_HAS_IEEE_128,vxl_ieee_128,16'
  ) {
my $var;
my $type;
my $size;
($var,$type,$size) = split /,/, $var_type_size;

print <<EOF;
#if $var
  testlib_test_begin( "Size of $type" );
  testlib_test_perform( sizeof( $type ) == $size );
#else
  vcl_cout << "(no $type)" << vcl_endl;
#endif

EOF
}

############################################################################

print <<EOF;
  vcl_cout << vcl_endl;

EOF

############################################################################

my $type;
for $type (
  'char',
  'short',
  'int',
  'long',
  'float',
  'double',
  ) {

print <<EOF;
  vcl_cout << "sizeof($type) is " << sizeof($type) << vcl_endl;
EOF
}

############################################################################

print <<EOF;
}

TESTMAIN(test_config);
EOF

############################################################################

exit 0;
