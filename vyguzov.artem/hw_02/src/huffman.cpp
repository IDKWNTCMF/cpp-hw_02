#include "huffman.h"
#include <fstream>
#include <climits>

namespace huffman {

    static int get_min(const std::vector<TreeNode *> &vec) {
        int idx = 0;
        for (int i = 0; i < vec.size(); i++) {
            if (vec[i]->val < vec[idx]->val) {
                idx = i;
            }
        }
        return idx;
    }

    HuffTree::HuffTree(const std::map<char, int> &freq) {
        std::vector<TreeNode *> nodes;
        try {
            nodes.reserve(freq.size());
            for (std::pair<char, int> tmp : freq) {
                chars[tmp.first] = tmp.second;
                nodes.push_back(new TreeNode(tmp.first, tmp.second));
            }

            size_t cur_size = nodes.size();
            while (cur_size > 1) {
                int idx = get_min(nodes);
                std::swap(nodes[idx], nodes[cur_size - 1]);
                TreeNode *min1 = nodes.back();
                nodes.pop_back();
                cur_size--;
                idx = get_min(nodes);
                std::swap(nodes[idx], nodes[cur_size - 1]);
                TreeNode *min2 = nodes.back();
                nodes.pop_back();
                nodes.push_back(new TreeNode(min1, min2));
            }

            root = (nodes.empty()) ? nullptr : nodes[0];
        } catch (std::bad_alloc &e) {
            for (TreeNode *node : nodes) {
                delete node;
            }
            throw e;
        }
    }

    int HuffTree::decode(std::ifstream &in, std::ofstream &out, int cnt) {
        int additionalData = (int)in.tellg();
        TreeNode *cur = root;
        if (cur == nullptr) return additionalData;
        if (cur->left == nullptr) {
            while (cnt != 0) {
                out.write((char *) (&cur->ch), sizeof(char));
                cnt--;
            }
        }
        while (cnt != 0) {
            uint8_t byte;
            in.read((char *)(&byte), sizeof(uint8_t));
            for (int i = CHAR_BIT - 1; i >= 0 && cnt != 0; i--) {
                cur = (byte & (1 << i)) ? cur->right : cur->left;
                if (cur->left == nullptr) {
                    out.write((char *)(&cur->ch), sizeof(char));
                    cur = root;
                    cnt--;
                }
            }
        }
        return additionalData;
    }

    void HuffTree::archive(std::ofstream &out) {
        uint8_t size = chars.size();
        out.write((char *)(&size), sizeof(uint8_t));
        for (std::pair<char, int> ch : chars) {
            out.write(&ch.first, sizeof(char));
            out.write((char *)(&ch.second), sizeof(int));
        }
    }

    void HuffmanArchiver::build_table(TreeNode *root, std::vector<bool> &cur_code) {
        if (root->left == nullptr) {
            table[root->ch] = cur_code;
            return;
        }
        cur_code.push_back(false);
        build_table(root->left, cur_code);
        cur_code.pop_back();
        cur_code.push_back(true);
        build_table(root->right, cur_code);
        cur_code.pop_back();
    }

    int HuffmanArchiver::zip(std::ifstream &in, std::ofstream &out) {
        std::map<char, int> freq;
        char ch;
        in.seekg(0, std::ifstream::end);
        int number_of_chars = (int)in.tellg();
        in.seekg(0, std::ifstream::beg);
        for (int i = 0; i < number_of_chars; i++) {
            in.read(&ch, sizeof(char));
            freq[ch]++;
        }
        tree = new HuffTree(freq);
        tree->archive(out);
        int additionalData = (int)out.tellp();

        in.seekg(0, std::ifstream::beg);
        TreeNode *cur = tree->get_root();
        std::vector<bool> cur_code;
        cur_code.reserve(1 << CHAR_BIT);
        if (cur != nullptr) build_table(cur, cur_code);
        uint8_t tmp;
        int cnt = 0;
        for (int i = 0; i < number_of_chars; i++) {
            in.read(&ch, sizeof(char));
            for (bool chr : table[ch]) {
                cnt++;
                tmp = 2 * tmp + chr;
                if (cnt == CHAR_BIT) {
                    out.write((char *)(&tmp), sizeof(uint8_t));
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
        if (flag) out.write((char *)(&tmp), sizeof(uint8_t));
        return additionalData;
    }

    int HuffmanArchiver::unzip(std::ifstream &in, std::ofstream &out) {
        uint8_t size;
        in.read((char *) (&size), sizeof(uint8_t));
        std::map<char, int> freq;
        int sum = 0;
        for (int i = 0; i < size; i++) {
            char ch;
            int tmp;
            in.read(&ch, sizeof(char));
            in.read((char *) (&tmp), sizeof(int));
            freq[ch] = tmp;
            sum += tmp;
        }
        tree = new HuffTree(freq);
        return tree->decode(in, out, sum);
    }

}