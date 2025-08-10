#ifndef RUNNER_H
#define RUNNER_H

#include <string>
#include <unistd.h>   // chroot, chdir, execl
#include <iostream>   // std::cerr
#include <cstdlib>    // exit, EXIT_FAILURE
#include <cstdio>     // perror

class Runner {
public:
    explicit Runner(const char* jr) : jailRoot(jr) {}

    void run(const std::string& cmd) const {
        if (jailRoot != nullptr) {
            if (chroot(jailRoot) != 0) {
                perror("chroot failed");
                exit(EXIT_FAILURE);
            }
            if (chdir("/") != 0) {
                perror("chdir failed");
                exit(EXIT_FAILURE);
            }
        }
        execl("/bin/sh", "sh", "-c", cmd.c_str(), nullptr);

        // If execl returns, it failed
        perror("execl failed");
        exit(EXIT_FAILURE);
    }

private:
    const char* jailRoot;
};

#endif // RUNNER_H
