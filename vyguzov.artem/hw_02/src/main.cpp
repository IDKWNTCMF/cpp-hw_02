#include "huffman.h"
#include "additional.h"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    try {
        std::ifstream in;
        std::ofstream out;
        int mode = parse_arguments(argc, argv, in, out);
        huffman::HuffmanArchiver archiver;
        int additionalData = (mode == 1) ? archiver.zip(in, out) : archiver.unzip(in, out);
        if (mode == 1) {
            std::cout << (int)in.tellg() << '\n';
            std::cout << (int)out.tellp() - additionalData << '\n';
            std::cout << additionalData << '\n';
        } else {
            std::cout << (int)in.tellg() - additionalData << '\n';
            std::cout << (int)out.tellp() << '\n';
            std::cout << additionalData << '\n';
        }
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
