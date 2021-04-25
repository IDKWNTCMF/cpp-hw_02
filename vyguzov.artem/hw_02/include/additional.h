#pragma once

#include <cstring>
#include <fstream>

class FileNotFoundException : public std::exception {
private:
    std::string error;
public:
    explicit FileNotFoundException(std::string err) : error(std::move(err)) {}
    const char * what() const noexcept override {
        return error.c_str();
    }
};

static int parse_arguments(int argc, char *argv[], std::ifstream &in, std::ofstream &out) {
    int mode = 0;
    std::string IFile, OFile;
    for (int i = 1; i < argc; i++) {
        if (strlen(argv[i]) == 1) throw std::invalid_argument("Invalid arguments!");
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            if (strcmp(argv[i], "--file") == 0) {
                if (++i == argc || !IFile.empty()) throw std::invalid_argument("Invalid arguments!");
                IFile = argv[i];
                continue;
            }
            if (strcmp(argv[i], "--output") == 0) {
                if (++i == argc || !OFile.empty()) throw std::invalid_argument("Invalid arguments!");
                OFile = argv[i];
                continue;
            }
            throw std::invalid_argument("Invalid arguments!");
        }
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'c':
                    if (mode != 0) throw std::invalid_argument("Invalid arguments!");
                    mode = 1;
                    break;
                case 'u':
                    if (mode != 0) throw std::invalid_argument("Invalid arguments!");
                    mode = 2;
                    break;
                case 'f':
                    if (++i == argc || !IFile.empty()) throw std::invalid_argument("Invalid arguments!");
                    IFile = argv[i];
                    break;
                case 'o':
                    if (++i == argc || !OFile.empty()) throw std::invalid_argument("Invalid arguments!");
                    OFile = argv[i];
                    break;
                default:
                    throw std::invalid_argument("Invalid arguments!");
            }
        }
    }
    if (mode == 0 || IFile.empty() || OFile.empty()) throw std::invalid_argument("Invalid arguments!");
    in.open(IFile);
    if (!in.is_open()) throw FileNotFoundException("Unable to open input file!");
    in.exceptions(std::ifstream::failbit | std::ifstream::eofbit);
    out.open(OFile);
    if (!out.is_open()) throw FileNotFoundException("Unable to open output file!");
    return mode;
}