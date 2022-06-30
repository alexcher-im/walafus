# Walafus

An extensible read-only virtual filesystem for C++.

It implements it's `.wltg` format with support of ChaCha20 encryption and ZSTD block compression.


## How to use
### In your project directory:
Clone project or add it as a git submodule. 
If your project already has a ZSTD somewhere - make sure to add its subdirectory 
before the current project to make walafus use your version.


### In your CMakeLists.txt:

```cmake
add_subdirectory(walafus)
target_link_library(YOUR_PROJECT PRIVATE walafus)
target_include_directories(YOUR_PROJECT PRIVATE walafus/include)
```

There are targets `walafus_static`, `walafus_shared`, `walafus` (shared if `${BUILD_SHARED_LIBS}`, static otherwise).


### In your C++ code
```C++
#include "walafus/filesystem.h"

void main() {
    // the base filesystem, container of sources
    Filesystem filesystem;
    
    // opening a pack.wltg file in real filesystem
    // specify password (const char*) or encryption key (ubyte[32])
    //  as a second argument if the source may require decryption
    // second argument is ignored if passed, but have not used
    WltgReader pack_wltg("pack.wltg");
    
    // a real file system as a source of file data
    // argument - a "root" path for looking up files, should be a directory
    RealFsReader real_reader(".");
    
    // adding sources to container
    // the last added has higher priority while looking up files
    filesystem.add_source(&pack_wltg);
    filesystem.add_source(&real_reader);
    
    // open read-only file
    std::unique_ptr<BaseFileStream> handle = filesystem.open_file_read("filename.bin");
    if (!handle) {
        std::cout << "failed to open file" << std::endl;
        return;
    }
    
    // reading file contents to vector. read_into() function, 
    //  that read into user-specified buffer is also available
    // .size(), .tell(), .seek() methods are also here
    std::vector<ubyte> data = file->read();
    std::cout << data.size() << std::endl; // can also be accessed by file->size()
}
```


### Creating `.wltg` file
```C++
#include "walafus/wltg_packer.h"

void main() {
    // the packer instance
    WltgPacker packer;
    
    // indexing real file system directory (/home/example/) 
    //  into a specific place in virtual path (/example/)
    // path masks are not supported now
    // multiple real directories can be indexed in the same packer
    // if virtual paths interleave, the last indexed is recorded
    packer.index_real_dir("/example", "/home/example");
    
    // packing (real filename, ZSTD compression level (from 1 to 22 inclusive), password, is compact layout)
    // a specific encryption key can also be specified by passing ubyte[32] instead of const char*
    packer.write_fs_blob("pack.wltg", 11, "password", true);
}
```


### Error handling
By default, error will print to stdout (using std::cout) in human-readable
way and then throw std::runtime_error exception. You can override this by 
setting `walafus_on_error` (include `walafus/errors.h`) function pointer to your own callback function.
Also, you can decide, whether the caller should throw an exception or not.

Some functions are plainly marked as returning nullptr on error, so they do not 
call the `walafus_on_error` on such errors at all.


### Adding support for your own format
Inherit a `BaseFilesystemSource` and `BaseFileStream`, implement their abstract (pure virtual) methods,
and there you are. In-code documentation provided.
