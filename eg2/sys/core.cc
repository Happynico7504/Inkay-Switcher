#include <coreinit/core.h>
#include <coreinit/thread.h>
#include <coreinit/memory.h>
#include <coreinit/systeminfo.h>
#include "eg2/libs/typedefs.h"
#include "eg2/sys/report.hh"

namespace SYS {
    static const char* CompilerSelect(void) {
        #ifdef __clang__
            return "Clang Embedded PPC Compiler";
        #elif defined(__GNUC__)
            return "GNU Embedded PPC Compiler";
        #elif defined(__ghs__) // never happening
            return "GHS Embedded PPC Compiler";
        #else
            return "Unknown Compiler";
        #endif
    }

    static const char* ThreadGet(void) {
        const char* name = OSGetThreadName(OSGetCurrentThread());
        return (name && name[0]) ? name : "Unk";
    }

    bool Init(void) {
        SYS::Report::Log("\n----------------------------------------------------------------------\n");
        SYS::Report::Log("EG2L BOOT TITLE\n");
        SYS::Report::Log("----------------------------------------------------------------------\n");
        SYS::Report::Log("Compiler  : %s\n", CompilerSelect());
        SYS::Report::Log("SDK       : WUT - WiiU Toolchain\n");
        SYS::Report::Log("Date      : %s | %s\n", __DATE__, __TIME__);
        SYS::Report::Log("Thread    : %s\n", ThreadGet());
        SYS::Report::Log("Core      : %u\n", OSGetCoreId());
        SYS::Report::Log("----------------------------------------------------------------------\n\n");

        return true;
    }
}