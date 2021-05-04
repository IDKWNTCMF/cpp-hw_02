#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "huffman.h"

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
    std::vector<int> freq(1 << CHAR_BIT);

    SUBCASE("empty huffman tree test") {
        huffman::HuffTree tree = huffman::HuffTree(freq);
        CHECK_EQ(nullptr, tree.get_root());
    }

    freq[uint8_t('a')] = 1;
    SUBCASE("one symbol tree test") {
        huffman::HuffTree tree = huffman::HuffTree(freq);
        huffman::TreeNode *root = tree.get_root();
        CHECK_EQ('a', root->ch);
        CHECK_EQ(1, root->val);
    }

    freq[uint8_t('b')] = 2;
    freq[uint8_t('c')] = 4;
    freq[uint8_t('d')] = 8;
    SUBCASE("ordinary huffman tree test") {
        huffman::HuffTree tree = huffman::HuffTree(freq);
        huffman::TreeNode *cur_node = tree.get_root();
        CHECK_EQ(15, cur_node->val);
        CHECK_EQ('d', cur_node->right->ch);
        CHECK_EQ(8, cur_node->right->val);
        cur_node = cur_node->left.get();
        CHECK_EQ(7, cur_node->val);
        CHECK_EQ('c', cur_node->right->ch);
        CHECK_EQ(4, cur_node->right->val);
        cur_node = cur_node->left.get();
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
        std::string empty_file = "data/empty.txt";
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
        std::string one_symbol_file = "data/OneSymbolFile.txt";
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
        std::string ordinary_file = "data/AStudyInScarlet.txt";
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
        std::string bin_file = "data/file.bin";
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

    SUBCASE("zip and unzip a file with every char") {
        std::string bin_file = "data/EveryChar.bin";
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
