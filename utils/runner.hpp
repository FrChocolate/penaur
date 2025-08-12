#ifndef RUNNER_H
#define RUNNER_H

#include "logger.hpp"
#include <cerrno>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <seccomp.h>
#include <sstream>
#include <string>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <sys/resource.h>
#include <string>
#include <cctype>
#include <cstdlib>
#include <iostream>

inline std::vector<std::string> split_by(const std::string &input, char delimiter = ',') {
  std::vector<std::string> result;
  std::stringstream ss(input);
  std::string token;

  while (std::getline(ss, token, delimiter)) {
    result.push_back(token);
  }

  return result;
}



inline std::string
rlimit_to_str(const int resource) { // THIS FUNCTION IS AI GENERATED!
  switch (resource) {
  case RLIMIT_CPU:
    return "RLIMIT_CPU";
  case RLIMIT_FSIZE:
    return "RLIMIT_FSIZE";
  case RLIMIT_DATA:
    return "RLIMIT_DATA";
  case RLIMIT_STACK:
    return "RLIMIT_STACK";
  case RLIMIT_CORE:
    return "RLIMIT_CORE";
  case RLIMIT_RSS:
    return "RLIMIT_RSS";
  case RLIMIT_NOFILE:
    return "RLIMIT_NOFILE";
  case RLIMIT_AS:
    return "RLIMIT_AS";
  case RLIMIT_NPROC:
    return "RLIMIT_NPROC";
  case RLIMIT_MEMLOCK:
    return "RLIMIT_MEMLOCK";
  case RLIMIT_LOCKS:
    return "RLIMIT_LOCKS";
  case RLIMIT_SIGPENDING:
    return "RLIMIT_SIGPENDING";
  case RLIMIT_MSGQUEUE:
    return "RLIMIT_MSGQUEUE";
  case RLIMIT_NICE:
    return "RLIMIT_NICE";
  case RLIMIT_RTPRIO:
    return "RLIMIT_RTPRIO";
  case RLIMIT_RTTIME:
    return "RLIMIT_RTTIME";
  default:
    return "UNKNOWN_RLIMIT";
  }
}

inline int str_to_rlimit(const std::string &name) { // THIS FUNCTION IS AI GENERATED
  std::string upper = name;
  std::transform(upper.begin(), upper.end(), upper.begin(),
                 [](unsigned char c){ return std::toupper(c); });

  if (upper == "RLIMIT_CPU")        return RLIMIT_CPU;
  if (upper == "RLIMIT_FSIZE")      return RLIMIT_FSIZE;
  if (upper == "RLIMIT_DATA")       return RLIMIT_DATA;
  if (upper == "RLIMIT_STACK")      return RLIMIT_STACK;
  if (upper == "RLIMIT_CORE")       return RLIMIT_CORE;
#ifdef RLIMIT_RSS
  if (upper == "RLIMIT_RSS")        return RLIMIT_RSS;
#endif
  if (upper == "RLIMIT_NOFILE")     return RLIMIT_NOFILE;
  if (upper == "RLIMIT_AS")         return RLIMIT_AS;
  if (upper == "RLIMIT_NPROC")      return RLIMIT_NPROC;
  if (upper == "RLIMIT_MEMLOCK")    return RLIMIT_MEMLOCK;
  if (upper == "RLIMIT_LOCKS")      return RLIMIT_LOCKS;
  if (upper == "RLIMIT_SIGPENDING") return RLIMIT_SIGPENDING;
  if (upper == "RLIMIT_MSGQUEUE")   return RLIMIT_MSGQUEUE;
  if (upper == "RLIMIT_NICE")       return RLIMIT_NICE;
  if (upper == "RLIMIT_RTPRIO")     return RLIMIT_RTPRIO;
  if (upper == "RLIMIT_RTTIME")     return RLIMIT_RTTIME;

  return -1;
}

inline rlim_t parse_size(const std::string& str) {
  size_t i = 0;
  while (i < str.size() && (std::isdigit(str[i]) || str[i] == '.')) i++;

  if (i == 0) return 0;

  const double number = std::atof(str.substr(0, i).c_str());
  std::string unit = str.substr(i);
  unit.erase(remove_if(unit.begin(), unit.end(), ::isspace), unit.end());
  for (auto &c : unit) c = std::toupper(c);

  if (unit.empty() || unit == "B") return (rlim_t)number;
  if (unit == "KB") return static_cast<rlim_t>(number * 1024);
  if (unit == "MB") return static_cast<rlim_t>(number * 1024 * 1024);
  if (unit == "GB") return static_cast<rlim_t>(number * 1024 * 1024 * 1024);
  return 0;
}


class Runner {
public:
  const char *jailRoot;
  const std::string limits;
  const std::vector<std::string> allowSyscalls;
  const std::string hostname;
  const std::string unshareIPC;

  explicit Runner(const char *jr, const std::string &alsys, std::string &lts, std::string &hn, std::string uipc)
      : jailRoot(jr),
        limits(lts),
        allowSyscalls(split_by(alsys)),
        hostname(hn),
        unshareIPC(std::move(uipc)){}

  void set_UTS_IPC() const {
    if (unshare(CLONE_NEWUTS) == -1) {
      perror("unshare");
      error("Couldn't set UTS");
    }
    if (sethostname(hostname.c_str(), hostname.size()) == -1) {
      perror("sethostname");
      error("Couldn't set hostname");
    }
    info("Hostname set to "+ hostname);

    if (unshareIPC != "NULL") {
      info("Cloning IPC.");
      if (unshare(CLONE_NEWIPC) == -1) {
        perror("unshareIPC");
        error("Couldn't set IPC");
      }
      info("IPC Cloned.");
    }
  }

  static void set_limit(const int resource, const rlim_t soft,
                        const rlim_t hard) {
    const struct rlimit rl = {soft, hard};
    info("Limiting " + rlimit_to_str(resource));
    if (setrlimit(resource, &rl) != 0) {
      error("Limiting " + rlimit_to_str(resource) + " failed.");
      perror("setrlimit");
      exit(EXIT_FAILURE);
    }
  }

  void set_syscalls() const {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
      error("seccomp_init failed");
      return;
    }

    for (const auto &name : allowSyscalls) {
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
    const std::vector<std::string> comps = split_by(limits, ',');
    for (const auto& i:comps) {
      std::vector<std::string> comp = split_by(i, ':');
      const auto rlimit = str_to_rlimit(comp.at(0));
      if (rlimit == -1) {
        error("Failed to parse limit: " + comp.at(0));
      }
      auto cur = parse_size(comp.at(1));
      if (cur <= 0) {
        error("Failed to parse limit: " + comp.at(1));
      }
      const auto max = parse_size(comp.at(2));
      if (max <= 0) {
        error("Failed to parse limit: " + comp.at(2));
      }

      if (cur > max) {
          warning("Stack soft limit > stack hard limit; adjusting soft to hard.");
          cur = max;
      }
      set_limit(rlimit, cur, max);

    }
  }

  void run(const std::string &cmd) const {

    const pid_t pid = fork();
    if (pid < 0) {
      error("fork failed: " + std::string(strerror(errno)));
      return;
    }
    set_UTS_IPC();
    if (pid == 0) {

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

      if (limits != "NULL"){
        limit_resources();
      }

      if (!allowSyscalls.empty()) {
        info("Setting syscalls");
        set_syscalls();
      }

      execl(cmd.c_str(), cmd.c_str(), nullptr);

      error(std::string("execl failed: ") + strerror(errno));
      _exit(1);
    }
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
};

#endif // RUNNER_H
