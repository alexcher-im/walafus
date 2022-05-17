#pragma once


#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include "../../util/types.h"


class WltgPackerNode
{
public:
    std::string name;
    std::string real_path;
    slong data_size{-1};
    std::unordered_map<std::string, std::unique_ptr<WltgPackerNode>> children;

    struct { // used in writing filesystem and flattening the node tree
        int self_index = -1; // current node index in some temporary flattened collection
        WltgPackerNode* child = nullptr; // ptr to a single children node
        WltgPackerNode* next = nullptr; // ptr to a next node in the same directory as current one
    } tmp_inode_info;

    WltgPackerNode() = default;
    explicit WltgPackerNode(std::string_view name_) : name(name_) {}

    WltgPackerNode* operator[](std::string& child_name);

    void print_contents(int indent = 0) const {
        for (int i = 0; i < indent; ++i)
            std::cout << "|- ";
        std::cout << name << std::endl;

        for (const auto& [node_name, node] : children) {
            node->print_contents(indent + 1);
        }
    }
};


class WltgPacker
{
public:
    std::unique_ptr<WltgPackerNode> root;

    WltgPacker();

    WltgPackerNode* lookup_path(const std::filesystem::path& path);

    void index_real_dir(const std::filesystem::path& virtual_path, const std::filesystem::path& real_path);

    void index_real_dir(WltgPackerNode* virtual_location, const std::filesystem::path& real_path);

    void write_fs_blob(const char* real_path, int compression_level, ubyte encryption_key[32]);

    void write_fs_blob(const char* real_path, int compression_level = 11, const char* password = nullptr);

protected:
    std::vector<WltgPackerNode*> get_nodes_list();
};
