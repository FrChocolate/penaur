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

#include <string>
#include <variant>

std::variant<std::string, bool> flag(const std::string key, char* argv[], const int argc) {
    bool reached = false;
    for (int i = 0; i < argc; i++) {
        if (reached & (argv[i][0] == '-')) return true;
        if (const auto val = std::string(argv[i]); val == key) reached = true;
        else if (reached) return val;
    }
    return false;
}
