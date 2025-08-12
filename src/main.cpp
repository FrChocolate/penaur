#include <iostream>
#include <oneapi/tbb/info.h>

#include "../utils/config.hpp"
#include "../utils/runner.hpp"
#include "../utils/logger.hpp"


int main(int argc, char* argv[]){
    const auto chroot = flag("--chroot", argv, argc);
    std::string limits = flag("--limits", argv, argc);
    const auto sysallow = flag("--sys-deny", argv, argc);
    auto hostname = flag("--hostname", argv, argc);
    if (hostname == "NULL") {
        hostname = "penaur-box";
    }
    std::string ipc = flag("--ipc", argv, argc);
    std::string pid = flag("--pid", argv, argc);
    std::string vnet = flag("--vnet", argv, argc);
    const Runner sandbox(chroot.c_str(), sysallow, limits, hostname, ipc, pid,vnet);
    const auto cmd  = flag("--cmd", argv, argc);
    if (cmd == "NULL") {
        error("--cmd is not set, exiting...");
    }



    sandbox.run(cmd);
    return 0;
}