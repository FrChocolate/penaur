#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>

class CgroupManager {
public:
    explicit CgroupManager(const std::string &cgroupName)
        : cgroupPath("/sys/fs/cgroup/" + cgroupName) {
        createCgroup();
    }

    
    ~CgroupManager(){
        if(rmdir(cgroupPath.c_str())== -1){
            perror("Error removing cgroup directory"); 

        }
    }



    // Function to enable memory control in cgroup v2
    void enableMemoryControl() const {
        std::ofstream controlFile(cgroupPath + "/cgroup.subtree_control");
        if (!controlFile) {
            perror("Error opening cgroup.subtree_control");
            exit(EXIT_FAILURE);
        }
        controlFile << "+memory" << std::endl;
        controlFile.close();
    }
    // Function to create the cgroup directory for the current process
    void createCgroup() const {
        // Create the cgroup directory if it doesn't exist
        if (mkdir(cgroupPath.c_str(), 0755) == -1 && errno != EEXIST) {
            perror("Error creating cgroup directory");
            exit(EXIT_FAILURE);
        }

        // Enable memory controller by writing to cgroup.subtree_control
        enableMemoryControl();
    }



    // Function to set the memory limit for the cgroup (in bytes)
    void setMemoryLimit(const uint64_t memoryLimitBytes) const {
        std::ofstream memoryFile(cgroupPath + "/memory.max");
        if (!memoryFile) {
            perror("Error opening memory.max file");
            exit(EXIT_FAILURE);
        }

        // Set the memory limit (in bytes)
        memoryFile << memoryLimitBytes << std::endl;
        memoryFile.close();
    }

    // Function to add the current process (using getpid()) to the cgroup
    void addCurrentProcessToCgroup() const {
        const pid_t pid = getpid();  // Get current process ID
        std::ofstream cgroupProcsFile(cgroupPath + "/cgroup.procs");
        if (!cgroupProcsFile) {
            perror("Error opening cgroup.procs");
            exit(EXIT_FAILURE);
        }

        // Add the current process's PID to the cgroup
        cgroupProcsFile << pid << std::endl;
        cgroupProcsFile.close();
    }

private:
    std::string cgroupPath;
};