# Comprehensive benchmark suite with all operations

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/comprehensive_benchmark.cpp")
    add_executable(fs_benchmark
        src/comprehensive_benchmark.cpp
    )

    # Include directories for headers
    target_include_directories(fs_benchmark PRIVATE
        ${CMAKE_SOURCE_DIR}/fs/include
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_SOURCE_DIR}/cache/include
        ${CMAKE_SOURCE_DIR}/storage/include
        ${CMAKE_SOURCE_DIR}/journal/include
    )    # Link necessary libraries
    target_link_libraries(fs_benchmark
        fs
        common
        cache
        storage
        journal
    )

    # Install benchmark executable
    install(TARGETS fs_benchmark
        RUNTIME DESTINATION bin
    )
endif()