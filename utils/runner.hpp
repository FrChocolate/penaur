#ifndef RUNNER_H
#define RUNNER_H

#include <unistd.h>
#include <string>
#include <utility>
#include <sys/resource.h>
#include <cerrno>
#include <cstring>
#include "logger.hpp"
#include <seccomp.h>
#include <vector>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>     // fork(), execl(), chroot(), chdir(), _exit()
#include <sys/wait.h>   // waitpid(), WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG
#include <cerrno>       // errno
#include <cstring>
#include <sstream>

inline std::vector<std::string> split_by_comma(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ',')) {
        result.push_back(token);
    }

    return result;
}
class Runner {
public:
    const char* jailRoot;
    const std::string memory;
    const std::string memoryHard;
    const std::string cpu;
    const std::string cpuHard;
    const std::vector<std::string> allowSyscalls;

    explicit Runner(const char* jr, std::string mem, std::string memh, std::string cpu, std::string cpuh, const std::string& alsys)
        : jailRoot(jr), memory(std::move(mem)), memoryHard(std::move(memh)),
          cpu(std::move(cpu)), cpuHard(std::move(cpuh)), allowSyscalls(split_by_comma(alsys)) {}

    void set_syscalls() const {
        // Default action: ALLOW all syscalls
        scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
        if (!ctx) {
            error("seccomp_init failed");
            return;
        }

        // For each syscall in allowSyscalls vector, block it (kill on call)
        for (const auto& name : allowSyscalls) {
            int num = seccomp_syscall_resolve_name(name.c_str());
            if (num == __NR_SCMP_ERROR) {
                warning("Unknown syscall in deny list: " + name);
                continue;
            }
            if (seccomp_rule_add(ctx, SCMP_ACT_KILL, num, 0) < 0) {
                error("seccomp_rule_add failed for " + name);
                seccomp_release(ctx);
                return;
            }
        }

        if (seccomp_load(ctx) < 0) {
            error("seccomp_load failed");
            seccomp_release(ctx);
            return;
        }

        seccomp_release(ctx);
    }

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

        pid_t pid = fork();
        if (pid < 0) {
            error("fork failed: " + std::string(strerror(errno)));
            return;
        }

        if (pid == 0) {
            // CHILD PROCESS

            if (jailRoot != nullptr && std::string(jailRoot) != "NULL") {
                info(std::string("Chrooting to: ") + jailRoot);
                if (chroot(jailRoot) != 0) {
                    error(std::string("chroot failed: ") + strerror(errno));
                    _exit(1);
                }
                if (chdir("/") != 0) {
                    error(std::string("chdir failed: ") + strerror(errno));
                    _exit(1);
                }
            }

            if (memory != "NULL" || memoryHard != "NULL" || cpu != "NULL" || cpuHard != "NULL") {
                info("Limiting resources");
                limit_resources();
            }

            if (!allowSyscalls.empty()) {
                info("Setting syscalls");
                set_syscalls();
            }

            execl(cmd.c_str(), cmd.c_str(), nullptr);

            // If execl returns, it failed
            error(std::string("execl failed: ") + strerror(errno));
            _exit(1);
        } else {
            // PARENT PROCESS
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                info("Child exited with status " + std::to_string(WEXITSTATUS(status)));
            } else if (WIFSIGNALED(status)) {
                info("Child killed by signal " + std::to_string(WTERMSIG(status)));
            } else {
                info("Child stopped or continued");
            }
        }
    }

};

#endif // RUNNER_H
