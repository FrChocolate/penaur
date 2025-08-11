#pragma once
#ifdef RUNNER_H
#define RUNNER_H

#include <iostream>
#include <string>
#include "colors.hpp"
#include "config.hpp"

using namespace std;

inline void info(const std::string& msg) {
    cout << Colors::Blue << "[INFO] " << Colors::Reset << msg << endl;

}

inline void warning(const std::string& msg) {
    cout << Colors::Yellow << "[WARNING] " << Colors::Reset << msg << endl;
}

inline void error(const std::string& msg) {
    cout << Colors::Red << "[ERROR] " << Colors::Reset << msg << endl;
    exit(1);
}

#endif // RUNNER_H