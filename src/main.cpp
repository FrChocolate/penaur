#include <iostream>
#include <oneapi/tbb/info.h>

#include "../utils/config.hpp"
#include "../utils/runner.hpp"
#include "../utils/logger.hpp"


int main(int argc, char* argv[]){
    const auto chroot = flag("--chroot", argv, argc);
    if (chroot == "NULL") {
        info("chroot is NULL, Running without jail root.");
    }
    const auto memsoft = flag("--memsoft", argv, argc);
    if (memsoft != "NULL") {
        info("--memsoft is set");
    }
    const auto memhard = flag("--memhard", argv, argc);
    if (memhard != "NULL") {
        info("--memhard is set");
    }
    const auto cpu = flag("--cpu", argv, argc);
    if (cpu != "NULL") {
        info("--cpu is set");
    }
    const auto cpusoft = flag("--cpusoft", argv, argc);
    if (cpusoft != "NULL") {
        info("--cpusoft is set");
    }

    const auto sysallow = flag("--sys-deny", argv, argc);
    if (sysallow != "NULL") {
        info("--sys-deny is set");
    }

    const Runner sandbox(chroot.c_str(), memsoft, memhard, cpusoft, cpu, sysallow);
    const auto cmd  = flag("--cmd", argv, argc);
    if (cmd == "NULL") {
        error("--cmd is not set, exiting...");
    }
    info("Running cmd...");
    sandbox.run(cmd);
    return 0;
}