# 🐾 Penaur — The Lightweight Linux Sandbox (In Development)

**Penaur** is your minimal, powerful **sandbox tool** designed to safely run commands inside a controlled environment — limiting CPU, memory, and optionally jailing the process with `chroot`. Built for simplicity and flexibility, and still evolving! 🚀

---

## 🔥 Current Features

- ⏱️ **CPU Limits** (in seconds)
    - `--cpu` (soft limit)
    - `--cpuhard` (hard limit)

- 🧠 **Memory Limits** (in megabytes)
    - `--memsoft` (soft limit)
    - `--memhard` (hard limit)

- 🏠 **Filesystem Jail**
    - `--chroot` to set the jail root directory

- ⚙️ **Command Execution**
    - `--cmd` the command you want to run inside the sandbox

---

## 🚀 How to Use

```bash
sudo ./penaur \
  --cmd "./your_program" \
  --memsoft 100 \
  --memhard 200 \
  --cpu 2 \
  --cpuhard 4 \
  --chroot /path/to/jailroot
```

> 💡 Memory is in MB, CPU limits are in seconds.

> 🔒 Requires root privileges for chroot and resource limits.

🎯 Roadmap / Coming Soon

    🔐 System call filtering (e.g., seccomp) for tighter security

    🌐 Network restrictions to isolate network access

    🗂️ Namespace isolation (PID, network, mount namespaces) for full container-like sandboxing

🤝 Contributing & Feedback

This is an early-stage project — your ideas, bug reports, and contributions are highly welcome! Let’s build a safe sandboxing tool together.

Happy sandboxing! 🐾