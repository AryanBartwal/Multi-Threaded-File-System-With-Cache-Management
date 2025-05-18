add_library(threading
    src/thread_pool.cpp
)

target_include_directories(threading
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(threading
    PUBLIC
        common
)

# Find threading package
find_package(Threads REQUIRED)
target_link_libraries(threading
    PUBLIC
        Threads::Threads
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Install template implementations
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.tpp"
) 