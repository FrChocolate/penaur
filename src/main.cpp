#include <iostream>
#include "../utils/config.hpp"
#include "../utils/runner.hpp"


int main(int argc, char* argv[]){
    const auto chroot = flag("--chroot", argv, argc);
    if (chroot == "NULL") {
        std::cout<<"chroot is NULL, Working without jailing the subsystem."<<std::endl;
    }
    const Runner sandbox(chroot.c_str());
    const auto cmd  = flag("--cmd", argv, argc);
    std::cout << cmd << std::endl;
    sandbox.run(cmd);
    return 0;
}