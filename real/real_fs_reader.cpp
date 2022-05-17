#include <filesystem>
#include "real_fs_reader.h"


RealFsFileStream::RealFsFileStream(const char* path) : fp(path), fp_size(0) {
    fp.seekg(0, std::ios::end);
    fp_size = fp.tellg();
    fp.seekg(0, std::ios::beg);
}

ulong RealFsFileStream::size() const {
    return fp_size;
}

ulong RealFsFileStream::tell() const {
    return const_cast<RealFsFileStream*>(this)->fp.tellg();
}

void RealFsFileStream::seek(ulong offset) {
    fp.seekg(offset);
}

ulong RealFsFileStream::read_into(void* dst, ulong size) {
    fp.read((char*) dst, size);
    return fp.gcount();
}

bool RealFsFileStream::is_closed() const {
    return false;
}

RealFsReader::RealFsReader(std::string&& root_dir_) : root_dir(std::filesystem::absolute(root_dir_).string()) { }

static std::string filter_path(std::string_view src_path) {
    std::string out_path;

    std::vector<std::string_view> name_stack;
    name_stack.reserve(8);

    while (!src_path.empty()) {
        auto slash_index = src_path.find('/');
        auto substring_last = slash_index == std::string_view::npos ? src_path.size() : slash_index;
        auto substring = src_path.substr(0, substring_last);

        if (substring.empty() || substring == ".") {
            src_path.remove_prefix(substring_last + 1);
            continue;
        }
        if (substring == "..") {
            if (name_stack.size() > 1)
                name_stack.pop_back();
            src_path.remove_prefix(substring_last + 1);
            continue;
        }

        name_stack.push_back(substring);

        src_path.remove_prefix(std::min(substring_last + 1, src_path.size()));
    }

    for (int i = 0; i < name_stack.size(); ++i) {
        out_path += name_stack[i];
        if (i + 1 < name_stack.size())
            out_path += '/';
    }

    return out_path;
}

std::unique_ptr<BaseFileStream> RealFsReader::open_file_read(std::string_view filename) {
    auto full_path = root_dir + '/' + filter_path(filename);
    auto stream = std::make_unique<RealFsFileStream>(full_path.data());
    if (!stream->fp.good())
        return nullptr;
    return stream;
}

bool RealFsReader::is_path_exists(std::string_view path) {
    return std::filesystem::exists(path);
}
