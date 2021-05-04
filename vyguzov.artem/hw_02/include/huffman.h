#pragma once

#include <map>
#include <vector>
#include <memory>
#include <iostream>

namespace huffman {

    struct TreeNode {
        int val = 0;
        char ch = 0;
        std::unique_ptr<TreeNode> left, right;

        TreeNode() = default;
        explicit TreeNode(char chr, int cnt) : val(cnt), ch(chr) {}
        TreeNode(std::unique_ptr<TreeNode> &l, std::unique_ptr<TreeNode> &r) : val(l->val + r->val), left(std::move(l)), right(std::move(r)) {}
    };

    class HuffTree {
    private:
        int size = 0;
        std::vector<int> chars;
        std::unique_ptr<TreeNode> root;
    public:
        HuffTree() = default;
        explicit HuffTree(const std::vector<int> &freq);

        void archive(std::ofstream &out) const;
        int decode(std::ifstream &in, std::ofstream &out, int cnt) const;

        TreeNode * get_root() const {
            return root.get();
        }
    };

    class HuffmanArchiver {
    private:
        std::unique_ptr<HuffTree> tree;
        std::vector<std::vector<bool>> table;
    public:
        HuffmanArchiver() = default;

        void build_table(TreeNode *root, std::vector<bool> &cur_code);
        int zip(std::ifstream &in, std::ofstream &out);
        int unzip(std::ifstream &in, std::ofstream &out);
    };

}
