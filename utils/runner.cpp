#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>


class Runner {
public:
    const char* jailRoot;
    explicit Runner(const char* jr): jailRoot(jr) {}

    void run(const std::string& cmd) const {
        if (jailRoot != nullptr && std::string(jailRoot) != "NULL") {
            std::cout << jailRoot << std::endl;
            if (chroot(jailRoot) != 0) {
                perror("chroot failed");
                exit(EXIT_FAILURE);
            }
            if (chdir("/") != 0) {
                perror("chdir failed");
                exit(EXIT_FAILURE);
            }
        }
        std::cout << cmd << std::endl;
        execl(cmd.c_str(), cmd.c_str(), nullptr);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
};
