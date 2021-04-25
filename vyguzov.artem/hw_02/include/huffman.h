#pragma once

#include <map>
#include <vector>

namespace huffman {

    class TreeNode {
    public:
        int val = 0;
        char ch = 0;
        TreeNode *left = nullptr, *right = nullptr;

        TreeNode() = default;
        explicit TreeNode(char chr, int cnt) : val(cnt), ch(chr) {}
        TreeNode(TreeNode *l, TreeNode *r) : val(l->val + r->val), left(l), right(r) {}
        ~TreeNode() {
            delete left;
            delete right;
        }
    };

    class HuffTree {
    private:
        std::map<char, int> chars;
        TreeNode *root = nullptr;
    public:
        HuffTree() = default;
        explicit HuffTree(const std::map<char, int> &freq);
        ~HuffTree() {
            delete root;
        }

        void archive(std::ofstream &out);
        int decode(std::ifstream &in, std::ofstream &out, int cnt);

        TreeNode * get_root() const {
            return root;
        }
    };

    class HuffmanArchiver {
    private:
        HuffTree *tree = nullptr;
        std::map<char, std::vector<bool> > table;
    public:
        HuffmanArchiver() = default;
        ~HuffmanArchiver() {
            delete tree;
        }

        void build_table(TreeNode *root, std::vector<bool> &cur_code);
        int zip(std::ifstream &in, std::ofstream &out);
        int unzip(std::ifstream &in, std::ofstream &out);
    };

}