/* Copyright (C) Code Red Technologies Ltd. */
/*
 * <stddef.h>             Copyright (C) Codemist 2007
 *
 *                        A C Norman
 */

//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Portions copyright (c) 2010-11 Code Red Technologies Ltd.
//
//*****************************************************************************

#ifndef __STDDEF_H_INCLUDED
#define __STDDEF_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

#include <sys/libconfig.h>

typedef int ptrdiff_t;

#ifndef WCHAR_T_DEFINED
#define WCHAR_T_DEFINED
typedef unsigned short int wchar_t;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define offsetof(type, member_designator) \
    ((size_t)((char *)&(((type *)0)->member_designator) - (char *)0))

#endif /* __STDDEF_H_INCLUDED */

/* end of <stddef.h> */
