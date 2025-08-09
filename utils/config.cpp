// Made By Chocolate, 9 Aug
// Config file format for Penaur (binary):
//
// [3 bytes]   : Version (e.g. major/minor/patch)
// [4 bytes]   : Offset (uint32) to the start of 'data' section
// [variable]  : Custom headers (format TBD)
// [variable]  : Data section starting at offset, structured as:
//
// Data section layout:
// - Environment variables:
//     Each variable stored as: key (UTF-8 encoded, null-terminated) + value (UTF-8 encoded, null-terminated)
//     Variables concatenated, ended by 3 consecutive null bytes ('\0\0\0') as a section delimiter
//
// - Followed by 5 consecutive null bytes ('\0\0\0\0\0') as a separator
//
// - Runners section:
//     Same format as environment variables (key\0value\0...), ended by 3 consecutive null bytes
//
// - Followed by 5 consecutive null bytes as a separator
//
// - CPU time:
//     4 bytes (uint32), e.g. milliseconds CPU time, little endian
//
// - Memory usage:
//     16 bytes ASCII string, null-padded if shorter
//
// - CPU frequency:
//     16 bytes ASCII string, null-padded if shorter
//
// - OS uname:
//     64 bytes ASCII string, null-padded if shorter
//
// - User space:
//     64 bytes ASCII string, null-padded if shorter
//
// Notes:
// - All string data (env keys, values, runners) are UTF-8 encoded and null-terminated.
// - Fixed-size string fields (memory usage, cpu freq, os uname, user space) are ASCII or UTF-8, padded with nulls if shorter.
// - Endianness for numeric fields is little endian.
// - Null bytes ('\0') are used as delimiters and padding.
// - When parsing, treat env and runner strings as UTF-8 sequences.
// - Be careful when splitting by null bytes to properly handle multi-byte UTF-8 characters.
//

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

class PenaurConfig {
    public:
        std::string fileName;
        int v1=0;
        int v2=0;
        int v3=0;
        std::unordered_map<std::string, std::string> config;
        explicit PenaurConfig(const std::string &fn): fileName(fn){};

        std::vector<char> raw() const {
            std::ifstream file(fileName, std::ios::binary);
            if (!file) {
                std::cerr << "File not found/ Error while loading file" << std::endl;
                return std::vector<char>();
            }
            std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
            return buffer;
        }

        void parser() {
            const std::vector<char> buffer = raw();
            v1 = buffer[0];
            v2 = buffer[1];
            v3 = buffer[2];
            // TODO spect version matching and handle backward compatibility

        }
};