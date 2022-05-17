cmake_minimum_required(VERSION 3.21)
project(walafus)


if (NOT DEFINED WALAFUS_THIRDPARTIES_DIR)
    set(WALAFUS_THIRDPARTIES_DIR "thirdparties/")
endif()

# todo fix zstd searching and submoduling

set(WALAFUS_SRCS wltg/wltg_structure.h util/types.h filesystem.cpp include/walafus/filesystem.h wltg/wltg_reader.cpp
        include/walafus/wltg_reader.h wltg/wltg_internal_readers.cpp wltg/wltg_internal_readers.h
        wltg/wltg_packer.cpp include/walafus/wltg_packer.h wltg/wltg_internal_writers.cpp wltg/wltg_internal_writers.h
        include/walafus/errors.h real/real_fs_reader.cpp real/real_fs_reader.h)

# add targets
add_library(walafus ${WALAFUS_SRCS})
add_library(walafus_shared SHARED ${WALAFUS_SRCS})
add_library(walafus_static STATIC ${WALAFUS_SRCS})
set_target_properties(walafus PROPERTIES CXX_STANDARD 20)
set_target_properties(walafus_shared PROPERTIES CXX_STANDARD 20)
set_target_properties(walafus_static PROPERTIES CXX_STANDARD 20)

if (NOT TARGET zstd)
    add_subdirectory(${WALAFUS_THIRDPARTIES_DIR}/zstd/build/cmake)
endif()

# link ztsd
target_include_directories(walafus PRIVATE ${WALAFUS_THIRDPARTIES_DIR}/zstd/lib)
target_include_directories(walafus_shared PRIVATE ${WALAFUS_THIRDPARTIES_DIR}/zstd/lib)
target_include_directories(walafus_static PRIVATE ${WALAFUS_THIRDPARTIES_DIR}/zstd/lib)
target_link_libraries(walafus PUBLIC libzstd_static)
target_link_libraries(walafus_shared PUBLIC libzstd_shared)
target_link_libraries(walafus_static PUBLIC libzstd_static)
