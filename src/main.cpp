#include <iostream>
#include "../utils/config.hpp"
#include "../utils/runner.hpp"




int main(int argc, char* argv[]){
    const Runner sandbox(argv[1]);
    sandbox.run("/bin/sh -c ./test");
    return 0;
}