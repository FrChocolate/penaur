#include <iostream>
#include "../utils/config.hpp"
#include "../utils/runner.hpp"



int main(int argc, char* argv[]){
    const auto chroot = flag("--chroot", argv, argc);
    if (chroot == "NULL") {
        std::cout<<"chroot is NULL, Working without jailing the subsystem."<<std::endl;
    }

    u_int64_t msoft;
    u_int64_t mhard;
    u_int64_t csoft;
    u_int64_t chard;
    const auto memsoft = flag("--memsoft", argv, argc);
    if (memsoft == "NULL") {
        std::cout << "WARN: --memsoft is not set" << std::endl;

    }
    const auto memhard = flag("--memhard", argv, argc);
    if (memhard == "NULL") {
        std::cout << "WARN: --memhard is not set" << std::endl;
    }
    const auto cpu = flag("--cpu", argv, argc);
    if (cpu == "NULL") {
        std::cout << "WARN: --cpu is not set" << std::endl;
    }
    const auto cpusoft = flag("--cpusoft", argv, argc);
    if (cpusoft == "NULL") {
        std::cout << "WARN: --cpusoft is not set" << std::endl;
    }
    const Runner sandbox(chroot.c_str(), memsoft, memhard, cpusoft, cpu);
    const auto cmd  = flag("--cmd", argv, argc);
    std::cout << cmd << std::endl;
    sandbox.run(cmd);
    return 0;
}