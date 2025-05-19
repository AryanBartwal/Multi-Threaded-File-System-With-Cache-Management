#include "fs/filesystem.hpp"
#include "common/logger.hpp"
#include "storage/block_manager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

using namespace mtfs::fs;
using namespace mtfs::common;

// Helper function to split command line into tokens
std::vector<std::string> splitCommand(const std::string& cmd) {
    std::vector<std::string> tokens;
    std::istringstream iss(cmd);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Helper function to print command usage
void printUsage() {
    std::cout << "\nAvailable commands:\n"
              << "  create-file <filename>\n"
              << "  write-file <filename> <content>\n"
              << "  read-file <filename>\n"
              << "  delete-file <filename>\n"
              << "  create-dir <directoryname>\n"
              << "  list-dir <directoryname>\n"
              << "  exit\n"
              << std::endl;
}

int main() {
    try {
        std::cout << "Multi-threaded File System CLI\n"
                  << "Type 'help' for available commands\n\n";

        // Initialize filesystem with a root directory
        const std::string rootPath = "./fs_root";
        auto fs = FileSystem::create(rootPath);
        LOG_INFO("Filesystem initialized at: " + rootPath);

        std::string line;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, line);

            auto tokens = splitCommand(line);
            if (tokens.empty()) {
                continue;
            }

            const std::string& cmd = tokens[0];

            try {
                if (cmd == "help") {
                    printUsage();
                }
                else if (cmd == "exit") {
                    LOG_INFO("Shutting down filesystem");
                    break;
                }
                else if (cmd == "create-file") {
                    if (tokens.size() != 2) {
                        std::cout << "Usage: create-file <filename>" << std::endl;
                        continue;
                    }
                    if (fs->createFile(tokens[1])) {
                        std::cout << "File created successfully: " << tokens[1] << std::endl;
                        LOG_INFO("Created file: " + tokens[1]);
                    }
                }
                else if (cmd == "write-file") {
                    if (tokens.size() < 3) {
                        std::cout << "Usage: write-file <filename> <content>" << std::endl;
                        continue;
                    }
                    // Combine all remaining tokens as content
                    std::string content;
                    for (size_t i = 2; i < tokens.size(); ++i) {
                        content += tokens[i] + (i < tokens.size() - 1 ? " " : "");
                    }
                    if (fs->writeFile(tokens[1], content)) {
                        std::cout << "Content written successfully to: " << tokens[1] << std::endl;
                        LOG_INFO("Wrote content to file: " + tokens[1]);
                    }
                }
                else if (cmd == "read-file") {
                    if (tokens.size() != 2) {
                        std::cout << "Usage: read-file <filename>" << std::endl;
                        continue;
                    }
                    std::string content = fs->readFile(tokens[1]);
                    std::cout << "Content of " << tokens[1] << ":\n" << content << std::endl;
                    LOG_INFO("Read file: " + tokens[1]);
                }
                else if (cmd == "delete-file") {
                    if (tokens.size() != 2) {
                        std::cout << "Usage: delete-file <filename>" << std::endl;
                        continue;
                    }
                    if (fs->deleteFile(tokens[1])) {
                        std::cout << "File deleted successfully: " << tokens[1] << std::endl;
                        LOG_INFO("Deleted file: " + tokens[1]);
                    }
                }
                else if (cmd == "create-dir") {
                    if (tokens.size() != 2) {
                        std::cout << "Usage: create-dir <directoryname>" << std::endl;
                        continue;
                    }
                    if (fs->createDirectory(tokens[1])) {
                        std::cout << "Directory created successfully: " << tokens[1] << std::endl;
                        LOG_INFO("Created directory: " + tokens[1]);
                    }
                }
                else if (cmd == "list-dir") {
                    if (tokens.size() != 2) {
                        std::cout << "Usage: list-dir <directoryname>" << std::endl;
                        continue;
                    }
                    auto files = fs->listDirectory(tokens[1]);
                    std::cout << "\nContents of directory " << tokens[1] << ":\n";
                    for (const auto& file : files) {
                        std::cout << "  " << file << std::endl;
                    }
                    LOG_INFO("Listed directory: " + tokens[1]);
                }
                else {
                    std::cout << "Unknown command. Type 'help' for available commands." << std::endl;
                }
            }
            catch (const FSException& e) {
                std::cout << "Error: " << e.what() << std::endl;
                LOG_ERROR(e.what());
            }
            catch (const std::exception& e) {
                std::cout << "System error: " << e.what() << std::endl;
                LOG_ERROR(e.what());
            }
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        LOG_ERROR("Fatal error: " + std::string(e.what()));
        return 1;
    }
} 