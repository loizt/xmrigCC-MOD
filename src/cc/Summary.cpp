#include <string>
#include <uv.h>

#include "log/Log.h"
#include "Options.h"
#include "Summary.h"
#include "version.h"

static void print_versions()
{
    char buf[16];

#   if defined(__clang__)
    snprintf(buf, 16, " clang/%d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#   elif defined(__GNUC__)
    snprintf(buf, 16, " gcc/%d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#   elif defined(_MSC_VER)
    snprintf(buf, 16, " MSVC/%d", MSVC_VERSION);
#   else
    buf[0] = '\0';
#   endif


    Log::i()->text(Options::i()->colors() ? "\x1B[01;32m * \x1B[01;37mVERSIONS:     \x1B[01;36m%s/%s\x1B[01;37m libuv/%s%s" : " * VERSIONS:     %s/%s libuv/%s%s",
                   APP_NAME, APP_VERSION, uv_version_string(), buf);
}

static void print_commands()
{
    if (Options::i()->colors()) {
        Log::i()->text("\x1B[01;32m * \x1B[01;37mCOMMANDS:     \x1B[01;35mq\x1B[01;37muit");
    }
    else {
        Log::i()->text(" * COMMANDS:     'q' Quit");
    }
}


void Summary::print()
{
    print_versions();
    print_commands();
}
