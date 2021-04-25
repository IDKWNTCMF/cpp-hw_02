#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "huffman.h"
#include "additional.h"

bool check_files(const std::string &filename1, const std::string &filename2) {
    std::ifstream in1(filename1);
    std::ifstream in2(filename2);
    in1.seekg(0, std::ifstream::end);
    in2.seekg(0, std::ifstream::end);
    if ((int)in1.tellg() != (int)in2.tellg()) return false;
    int number_of_chars = (int)in1.tellg();
    in1.seekg(0, std::ifstream::beg);
    in2.seekg(0, std::ifstream::beg);
    char ch1, ch2;
    for (int i = 0; i < number_of_chars; i++) {
        in1.read(&ch1, sizeof(char));
        in2.read(&ch2, sizeof(char));
        if (ch1 != ch2) return false;
    }
    return true;
}

TEST_CASE("huffman tree tests") {
    std::map<char, int> freq;

    SUBCASE("empty huffman tree test") {
        huffman::HuffTree tree = huffman::HuffTree(freq);
        CHECK_EQ(nullptr, tree.get_root());
    }

    freq['a'] = 1;
    SUBCASE("one symbol tree test") {
        huffman::HuffTree tree = huffman::HuffTree(freq);
        huffman::TreeNode *root = tree.get_root();
        CHECK_EQ('a', root->ch);
        CHECK_EQ(1, root->val);
    }

    freq['b'] = 2;
    freq['c'] = 4;
    freq['d'] = 8;
    SUBCASE("ordinary huffman tree test") {
        huffman::HuffTree tree = huffman::HuffTree(freq);
        huffman::TreeNode *cur_node = tree.get_root();
        CHECK_EQ(15, cur_node->val);
        CHECK_EQ('d', cur_node->right->ch);
        CHECK_EQ(8, cur_node->right->val);
        cur_node = cur_node->left;
        CHECK_EQ(7, cur_node->val);
        CHECK_EQ('c', cur_node->right->ch);
        CHECK_EQ(4, cur_node->right->val);
        cur_node = cur_node->left;
        CHECK_EQ(3, cur_node->val);
        CHECK_EQ('b', cur_node->right->ch);
        CHECK_EQ(2, cur_node->right->val);
        CHECK_EQ('a', cur_node->left->ch);
        CHECK_EQ(1, cur_node->left->val);
    }
}

TEST_CASE("huffman archiver tests") {
    std::string zipped = "out.bin", unzipped = "out.txt";
    huffman::HuffmanArchiver archiver, dearchiver;

    SUBCASE("zip and unzip an empty file") {
        std::string empty_file = "../data/empty.txt";
        std::ifstream in(empty_file);
        std::ofstream out(zipped);
        int val1 = archiver.zip(in, out);
        in.close();
        out.close();
        std::ifstream fin(zipped);
        std::ofstream fout(unzipped);
        int val2 = dearchiver.unzip(fin, fout);
        fin.close();
        fout.close();
        CHECK(check_files(empty_file, unzipped));
        CHECK_EQ(val1, val2);
    }

    SUBCASE("zip and unzip file with only one symbol") {
        std::string one_symbol_file = "../data/one_symbol_file.txt";
        std::ifstream in(one_symbol_file);
        std::ofstream out(zipped);
        int val1 = archiver.zip(in, out);
        in.close();
        out.close();
        std::ifstream fin(zipped);
        std::ofstream fout(unzipped);
        int val2 = dearchiver.unzip(fin, fout);
        fin.close();
        fout.close();
        CHECK(check_files(one_symbol_file, unzipped));
        CHECK_EQ(val1, val2);
    }

    SUBCASE("zip and unzip an ordinary file") {
        std::string ordinary_file = "../data/AStudyInScarlet.txt";
        std::ifstream in(ordinary_file);
        std::ofstream out(zipped);
        int val1 = archiver.zip(in, out);
        in.close();
        out.close();
        std::ifstream fin(zipped);
        std::ofstream fout(unzipped);
        int val2 = dearchiver.unzip(fin, fout);
        fin.close();
        fout.close();
        CHECK(check_files(ordinary_file, unzipped));
        CHECK_EQ(val1, val2);
    }

    SUBCASE("zip and unzip a bin file") {
        std::string bin_file = "../data/file.bin";
        std::ifstream in(bin_file);
        std::ofstream out(zipped);
        int val1 = archiver.zip(in, out);
        in.close();
        out.close();
        std::ifstream fin(zipped);
        std::ofstream fout(unzipped);
        int val2 = dearchiver.unzip(fin, fout);
        fin.close();
        fout.close();
        CHECK(check_files(bin_file, unzipped));
        CHECK_EQ(val1, val2);
    }
}

TEST_CASE("parse arguments tests") {
    SUBCASE("no arguments test") {
        std::ifstream in;
        std::ofstream out;
        char *argv[0];
        CHECK_THROWS(parse_arguments(0, argv, in, out));
    }

    SUBCASE("invalid arguments test") {
        std::ifstream in;
        std::ofstream out;
        char *argv[] = {(char *)"-u", (char *)"-f"};
        CHECK_THROWS(parse_arguments(2, argv, in, out));
    }

    SUBCASE("normal arguments test") {
        std::ifstream in;
        std::ofstream out;
        char *argv[] = {(char *)"./hw_02", (char *)"-c", (char *)"-f", (char *)"test1.txt", (char *)"-o", (char *)"out.bin"};
        CHECK_NOTHROW(parse_arguments(6, argv, in, out));
    }

    SUBCASE("different flags test") {
        std::ifstream in;
        std::ofstream out;
        char *argv[] = {(char *)"./hw_02", (char *)"-u", (char *)"--file", (char *)"out.bin", (char *)"--output", (char *)"out.txt"};
        CHECK_NOTHROW(parse_arguments(6, argv, in, out));
    }
}