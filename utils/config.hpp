
// config.h
#ifndef PENAURCONFIG_H
#define PENAURCONFIG_H


#include <string>
#include <variant>

inline std::string flag(const std::string& key, char* argv[], const int argc) {
    bool reached = false;
    for (int i = 0; i < argc; i++) {
        if (reached & (argv[i][0] == '-')) return "true";
        if (const auto val = std::string(argv[i]); val == key) reached = true;
        else if (reached) return val;
    }
    if (reached) {
        return "true";
    }
    return "NULL";
}


#endif // PENAURCONFIG_H
