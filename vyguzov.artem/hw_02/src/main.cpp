#include "huffman.h"
#include <iostream>
#include <fstream>
#include <cstring>

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
        std::string_view str(argv[i]);
        if (str.size() == 1) throw std::invalid_argument("Invalid arguments!");
        if (str[0] == '-' && str[1] == '-') {
            if (str ==  "--file") {
                if (++i == argc || !IFile.empty()) throw std::invalid_argument("Invalid arguments!");
                IFile = argv[i];
                continue;
            }
            if (str == "--output") {
                if (++i == argc || !OFile.empty()) throw std::invalid_argument("Invalid arguments!");
                OFile = argv[i];
                continue;
            }
            throw std::invalid_argument("Invalid arguments!");
        }
        if (str[0] == '-') {
            switch (str[1]) {
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

int main(int argc, char *argv[]) {
    try {
        std::ifstream in;
        std::ofstream out;
        int mode = parse_arguments(argc, argv, in, out);
        huffman::HuffmanArchiver archiver;
        huffman::StatHandler statistics = (mode == 1) ? archiver.zip(in, out) : archiver.unzip(in, out);
        std::cout << statistics.inputData << '\n';
        std::cout << statistics.outputData << '\n';
        std::cout << statistics.additionalData << '\n';
    } catch (std::invalid_argument &e) {
        std::cout << e.what() << '\n';
        return 1;
    } catch (FileNotFoundException &e) {
        std::cout << e.what() << '\n';
        return 2;
    } catch (std::bad_alloc &e) {
        std::cout << "Unable to allocate memory!\n";
        return 3;
    } catch (std::ifstream::failure &e) {
        std::cout << "Invalid input file format!\n";
        return 4;
    }
    return 0;
}

