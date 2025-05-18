cmake_minimum_required(VERSION 3.15)

# CLI executable
add_executable(mtfs_cli
    src/main.cpp
)

target_include_directories(mtfs_cli
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(mtfs_cli
    PRIVATE
        fs
        cache
        storage
        common
) 