#pragma once


#include <fstream>
#include "../include/walafus/filesystem.h"


class RealFsFileStream : public BaseFileStream
{
public:
    std::ifstream fp;
    ulong fp_size;

    RealFsFileStream(const char* path);

    [[nodiscard]] ulong size() const override;

    [[nodiscard]] ulong tell() const override;

    void seek(ulong offset) override;

    ulong read_into(void* dst, ulong size) override;

    [[nodiscard]] bool is_closed() const override;
};


class RealFsReader : public BaseFilesystemSource
{
public:
    std::string root_dir;

    explicit RealFsReader(std::string&& root_dir_);

    std::unique_ptr<BaseFileStream> open_file_read(std::string_view filename) override;

    bool is_path_exists(std::string_view path) override;
};
