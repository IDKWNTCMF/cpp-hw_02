#include "huffman.h"
#include <fstream>
#include <climits>
#include <algorithm>

namespace huffman {

    static void swap_min_with_last(std::vector<std::unique_ptr<TreeNode>> &nodes) {
        std::iter_swap(nodes.end() - 1,
                       std::min_element(nodes.begin(),
                                        nodes.end(),
                                        [](const std::unique_ptr<TreeNode> &a, const std::unique_ptr<TreeNode> &b) { return a->val <= b->val; }));
    }

    HuffTree::HuffTree(const std::vector<int> &freq) {
        std::vector<std::unique_ptr<TreeNode>> nodes;
        chars.resize(freq.size());
        for (int ch = 0; ch < freq.size(); ch++) {
            chars[ch] = freq[ch];
            if (freq[ch] != 0) {
                size++;
                nodes.push_back(std::make_unique<TreeNode>(ch, freq[ch]));
            }
        }

        while (nodes.size() > 1) {
            swap_min_with_last(nodes);
            std::unique_ptr<TreeNode> min1 = std::move(nodes.back());
            nodes.pop_back();
            swap_min_with_last(nodes);
            std::unique_ptr<TreeNode> min2 = std::move(nodes.back());
            nodes.pop_back();
            nodes.push_back(std::make_unique<TreeNode>(min1, min2));
        }

        root = (nodes.empty()) ? nullptr : std::move(nodes[0]);
    }

    StatHandler HuffTree::decode(std::ifstream &in, std::ofstream &out, int cnt) const {
        StatHandler statistics;
        statistics.additionalData = in.tellg();
        TreeNode *cur = root.get();
        if (cur == nullptr) return statistics;
        if (cur->left == nullptr) {
            while (cnt != 0) {
                out.write((char *)&cur->ch, sizeof(char));
                cnt--;
            }
        }
        while (cnt != 0) {
            uint8_t byte;
            in.read((char *)(&byte), sizeof(uint8_t));
            for (int i = CHAR_BIT - 1; i >= 0 && cnt != 0; i--) {
                cur = (byte & (1 << i)) ? cur->right.get() : cur->left.get();
                if (cur->left == nullptr) {
                    out.write((char *)&cur->ch, sizeof(char));
                    cur = root.get();
                    cnt--;
                }
            }
        }
        statistics.inputData = in.tellg();
        statistics.outputData = out.tellp();
        statistics.inputData -= statistics.additionalData;
        return statistics;
    }

    void HuffTree::archive(std::ofstream &out) const {
        out.write((char *)(&size), sizeof(uint16_t));
        for (int ch = 0; ch < chars.size(); ch++) {
            if (chars[ch] != 0) {
                out.write((char *)&ch, sizeof(uint8_t));
                out.write((char *)&chars[ch], sizeof(int));
            }
        }
    }

    void HuffmanArchiver::build_table(TreeNode *root, std::vector<bool> &cur_code) {
        if (root->left == nullptr) {
            table[uint8_t(root->ch)] = cur_code;
            return;
        }
        cur_code.push_back(false);
        build_table(root->left.get(), cur_code);
        cur_code.pop_back();
        cur_code.push_back(true);
        build_table(root->right.get(), cur_code);
        cur_code.pop_back();
    }

    StatHandler HuffmanArchiver::zip(std::ifstream &in, std::ofstream &out) {
        StatHandler statistics;
        std::vector<int> freq(1 << CHAR_BIT);
        uint8_t ch;
        in.seekg(0, std::ifstream::end);
        int number_of_chars = in.tellg();
        in.seekg(0, std::ifstream::beg);
        for (int i = 0; i < number_of_chars; i++) {
            in.read((char *)&ch, sizeof(uint8_t));
            freq[ch]++;
        }
        tree = std::make_unique<HuffTree>(freq);
        tree->archive(out);
        statistics.additionalData = out.tellp();

        in.seekg(0, std::ifstream::beg);
        TreeNode *cur = tree->get_root();
        std::vector<bool> cur_code;
        cur_code.reserve(1 << CHAR_BIT);
        if (cur != nullptr) {
            table.resize(1 << CHAR_BIT);
            build_table(cur, cur_code);
        }
        uint8_t tmp;
        int cnt = 0;
        for (int i = 0; i < number_of_chars; i++) {
            in.read((char *)&ch, sizeof(uint8_t));
            for (bool chr : table[ch]) {
                cnt++;
                tmp = 2 * tmp + chr;
                if (cnt == CHAR_BIT) {
                    out.write((char *)&tmp, sizeof(uint8_t));
                    tmp = 0;
                    cnt = 0;
                }
            }
        }
        bool flag = false;
        while (cnt % 8 != 0) {
            cnt++;
            tmp *= 2;
            flag = true;
        }
        if (flag) out.write((char *)&tmp, sizeof(uint8_t));
        statistics.inputData = in.tellg();
        statistics.outputData = out.tellp();
        statistics.outputData -= statistics.additionalData;
        return statistics;
    }

    StatHandler HuffmanArchiver::unzip(std::ifstream &in, std::ofstream &out) {
        uint16_t size;
        in.read((char *)&size, sizeof(uint16_t));
        std::vector<int> freq(1 << CHAR_BIT);
        int sum = 0;
        for (int i = 0; i < size; i++) {
            uint8_t ch;
            int tmp;
            in.read((char *)&ch, sizeof(uint8_t));
            in.read((char *)&tmp, sizeof(int));
            freq[ch] = tmp;
            sum += tmp;
        }
        tree = std::make_unique<HuffTree>(freq);
        return tree->decode(in, out, sum);
    }

}
