/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "external.h"
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <cstdarg>


void print_backtrace(char* out, int len, int /*max_depth*/, void* /* ctx */) {
    out[0] = 0;
    strncat(out, "<not supported>", len);
}

TORRENT_FORMAT(1, 2)
void assert_print(char const* fmt, ...) {
#ifdef TORRENT_PRODUCTION_ASSERTS
    if (assert_counter > 500) return;

    FILE* out = fopen(libtorrent_assert_log, "a+");
    if (out == 0) out = stderr;
#else
    FILE* out = stderr;
#endif
    va_list va;
    va_start(va, fmt);
    vfprintf(out, fmt, va);
    va_end(va);

#ifdef TORRENT_PRODUCTION_ASSERTS
    if (out != stderr) fclose(out);
#endif
}

void assert_fail(char const* expr, int line
        , char const* file, char const* function, char const* value, int kind) {
    char stack[8192];
    stack[0] = '\0';
    print_backtrace(stack, sizeof (stack), 0);

    char const* message = "assertion failed. Please file a bugreport at "
            "https://github.com/arvidn/libtorrent/issues\n"
            "Please include the following information:\n\n"
            "version: " LIBTORRENT_VERSION "-" LIBTORRENT_REVISION "\n";

    switch (kind) {
        case 1:
            message = "A precondition of a libtorrent function has been violated.\n"
                    "This indicates a bug in the client application using libtorrent\n";
    }

    assert_print("%s\n"
            "file: '%s'\n"
            "line: %d\n"
            "function: %s\n"
            "expression: %s\n"
            "%s%s\n"
            "stack:\n"
            "%s\n"
            , message
#ifdef TORRENT_PRODUCTION_ASSERTS
            , assert_counter.load()
#endif
            , file, line, function, expr
            , value ? value : "", value ? "\n" : ""
            , stack);

    // if production asserts are defined, don't abort, just print the error
#ifndef TORRENT_PRODUCTION_ASSERTS
#ifdef _MSC_VER
    __debugbreak();
#else
    // send SIGINT to the current process
    // to break into the debugger
    raise(SIGABRT);
    abort();
#endif
#endif
}
