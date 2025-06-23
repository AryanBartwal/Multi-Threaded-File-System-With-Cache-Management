# Comprehensive benchmark suite with all operations

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/comprehensive_benchmark.cpp")
    add_executable(fs_benchmark
        src/comprehensive_benchmark.cpp
    )

    # Install benchmark executable
    install(TARGETS fs_benchmark
        RUNTIME DESTINATION bin
    )
endif()