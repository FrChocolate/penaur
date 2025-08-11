#ifndef RUNNER_H
#define RUNNER_H

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <utility>
#include <sys/resource.h>
#include <cerrno>
#include <cstring>
class Runner {
public:
    const char* jailRoot;
    const std::string memory;
    const std::string  memoryHard;
    const std::string  cpu;
    const std::string  cpuHard;
    explicit Runner(const char* jr, std::string mem, std::string memh, std::string cpu, std::string cpuh)
    : jailRoot(jr), memory(std::move(mem)), memoryHard(std::move(memh)),
      cpu(std::move(cpu)), cpuHard(std::move(cpuh)) {}

   void limit_resources() const {

    auto to_rlim = [](const std::string& s) -> rlim_t {
        if (s == "NULL") return RLIM_INFINITY;
        try {
            return std::stoull(s);
        } catch (...) {
            return RLIM_INFINITY;
        }
    };


    rlim_t mem_soft = to_rlim(memory);
    rlim_t mem_hard = to_rlim(memoryHard);
    rlim_t cpu_soft = to_rlim(cpu);
    rlim_t cpu_hard = to_rlim(cpuHard);

    mem_soft *= 1024 * 1024;
    mem_hard *= 1024 * 1024;

    if (cpu_soft != RLIM_INFINITY && cpu_hard != RLIM_INFINITY) {
        if (cpu_soft > cpu_hard) {
            std::cerr << "[WARN] CPU soft limit > hard limit; adjusting soft to hard.\n";
            cpu_soft = cpu_hard;
        }
        struct rlimit rl{};
        rl.rlim_cur = cpu_soft;
        rl.rlim_max = cpu_hard;
        std::cerr << "[DEBUG] Setting RLIMIT_CPU soft=" << rl.rlim_cur << " hard=" << rl.rlim_max << std::endl;
        if (setrlimit(RLIMIT_CPU, &rl) != 0) {
            std::cerr << "[ERROR] setrlimit CPU failed: " << strerror(errno) << " (errno=" << errno << ")\n";
            exit(EXIT_FAILURE);
        }
    }

    if (mem_soft != RLIM_INFINITY && mem_hard != RLIM_INFINITY) {
        if (mem_soft > mem_hard) {
            std::cerr << "[WARN] Memory soft limit > hard limit; adjusting soft to hard.\n";
            mem_soft = mem_hard;
        }
        struct rlimit rl{};
        rl.rlim_cur = mem_soft;
        rl.rlim_max = mem_hard;
        std::cerr << "[DEBUG] Setting RLIMIT_AS soft=" << rl.rlim_cur << " hard=" << rl.rlim_max << std::endl;
        if (setrlimit(RLIMIT_AS, &rl) != 0) {
            std::cerr << "[ERROR] setrlimit AS failed: " << strerror(errno) << " (errno=" << errno << ")\n";
            exit(EXIT_FAILURE);
        }
    }
}

    void run(const std::string& cmd) const {
        std::cerr << "--" << memory << "\n";

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
        if (memory != "NULL" || memoryHard != "NULL" || cpu != "NULL" || cpuHard != "NULL") {
            std::cout << "Limiting resources" << std::endl;
            limit_resources();
        }
        execl(cmd.c_str(), cmd.c_str(), nullptr);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
};

#endif // RUNNER_H
