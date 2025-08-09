// config.h
#ifndef PENAURCONFIG_H
#define PENAURCONFIG_H

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <string>

class PenaurConfig {
public:
    std::string fileName;
    int v1 = 0;
    int v2 = 0;
    int v3 = 0;
    std::unordered_map<std::string, std::string> config;

    explicit PenaurConfig(std::string fn) : fileName(std::move(fn)) {}

    std::vector<char> raw() const {
        std::ifstream file(fileName, std::ios::binary);
        if (!file) {
            std::cerr << "File not found/ Error while loading file" << std::endl;
            return {};
        }
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
        return buffer;
    }

    void parser() {
        const std::vector<char> buffer = raw();
        if (buffer.size() < 3) {
            std::cerr << "File too small to contain version info" << std::endl;
            return;
        }
        v1 = static_cast<unsigned char>(buffer[0]);
        v2 = static_cast<unsigned char>(buffer[1]);
        v3 = static_cast<unsigned char>(buffer[2]);
        // TODO: spec version matching and handle backward compatibility
    }
};

#endif // PENAURCONFIG_H
