/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   external.h
 * Author: duocore
 *
 * Created on 2019年6月4日, 下午9:20
 */

#ifndef EXTERNAL_H
#define EXTERNAL_H

#if defined __GNUC__ || defined __clang__
#define TORRENT_FORMAT(fmt, ellipsis) __attribute__((__format__(__printf__, fmt, ellipsis)))
#else
#define TORRENT_FORMAT(fmt, ellipsis)
#endif

#define LIBTORRENT_VERSION "1.1.8.0"
#define LIBTORRENT_REVISION "78d9c284e"

#ifndef TORRENT_EXPORT
#define TORRENT_EXPORT
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#if defined __GNUC__
#define TORRENT_FUNCTION __PRETTY_FUNCTION__
#else
#define TORRENT_FUNCTION __FUNCTION__
#endif

// this is to disable the warning of conditional expressions
// being constant in msvc
#ifdef _MSC_VER
#define TORRENT_WHILE_0  \
	__pragma( warning(push) ) \
	__pragma( warning(disable:4127) ) \
	while (0) \
	__pragma( warning(pop) )
#else
#define TORRENT_WHILE_0 while (0)
#endif

#define TORRENT_UNUSED(x) (void)(x)

#ifndef _MSC_VER
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS 1
#endif
#include <inttypes.h> // for PRId64 et.al.
#endif

#ifndef PRId64

// MinGW uses microsofts runtime
#if defined _MSC_VER || defined __MINGW32__
#define PRId64 "I64d"
#define PRIu64 "I64u"
#define PRIx64 "I64x"
#define PRIu32 "u"
#else
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIu32 "u"
#endif
#endif

#define TORRENT_ASSERT(x) \
	do { if (x) {} else assert_fail(#x, __LINE__, __FILE__, TORRENT_FUNCTION, 0, 0); } TORRENT_WHILE_0

void print_backtrace(char* out, int len, int max_depth = 0, void* ctx = NULL);

void assert_print(char const* fmt, ...) TORRENT_FORMAT(1, 2);

void assert_fail(const char* expr, int line
        , char const* file, char const* function, char const* val, int kind = 0);


#endif /* EXTERNAL_H */

