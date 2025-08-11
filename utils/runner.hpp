#ifndef RUNNER_H
#define RUNNER_H

#include <unistd.h>
#include <string>
#include <utility>
#include <sys/resource.h>
#include <cerrno>
#include <cstring>
#include "logger.hpp"

class Runner {
public:
    const char* jailRoot;
    const std::string memory;
    const std::string memoryHard;
    const std::string cpu;
    const std::string cpuHard;

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
                warning("CPU soft limit > hard limit; adjusting soft to hard.");
                cpu_soft = cpu_hard;
            }
            struct rlimit rl{};
            rl.rlim_cur = cpu_soft;
            rl.rlim_max = cpu_hard;
            info("Setting RLIMIT_CPU soft=" + std::to_string(rl.rlim_cur) + " hard=" + std::to_string(rl.rlim_max));
            if (setrlimit(RLIMIT_CPU, &rl) != 0) {
                error("setrlimit CPU failed: " + std::string(strerror(errno)) + " (errno=" + std::to_string(errno) + ")");
            }
        }

        if (mem_soft != RLIM_INFINITY && mem_hard != RLIM_INFINITY) {
            if (mem_soft > mem_hard) {
                warning("Memory soft limit > hard limit; adjusting soft to hard.");
                mem_soft = mem_hard;
            }
            struct rlimit rl{};
            rl.rlim_cur = mem_soft;
            rl.rlim_max = mem_hard;
            info("Setting RLIMIT_AS soft=" + std::to_string(rl.rlim_cur) + " hard=" + std::to_string(rl.rlim_max));
            if (setrlimit(RLIMIT_AS, &rl) != 0) {
                error("setrlimit AS failed: " + std::string(strerror(errno)) + " (errno=" + std::to_string(errno) + ")");
            }
        }
    }

    void run(const std::string& cmd) const {
        info("Memory limit (soft): " + memory);

        if (jailRoot != nullptr && std::string(jailRoot) != "NULL") {
            info(std::string("Chrooting to: ") + jailRoot);
            if (chroot(jailRoot) != 0) {
                error(std::string("chroot failed: ") + strerror(errno));
            }
            if (chdir("/") != 0) {
                error(std::string("chdir failed: ") + strerror(errno));
            }
        }

        if (memory != "NULL" || memoryHard != "NULL" || cpu != "NULL" || cpuHard != "NULL") {
            info("Limiting resources");
            limit_resources();
        }

        execl(cmd.c_str(), cmd.c_str(), nullptr);
        error(std::string("execl failed: ") + strerror(errno));
    }
};

#endif // RUNNER_H
