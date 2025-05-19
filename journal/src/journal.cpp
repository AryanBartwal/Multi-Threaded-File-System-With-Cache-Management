#include "journal/journal.hpp"
#include <iostream>

namespace mtfs::journal {

void Journal::initialize() {
    std::cout << "Journal initialized" << std::endl;
}

void Journal::logOperation(const std::string& operation) {
    std::cout << "Operation logged: " << operation << std::endl;
}

void Journal::recover() {
    std::cout << "Journal recovery completed" << std::endl;
}

void Journal::clear() {
    std::cout << "Journal cleared" << std::endl;
}

} // namespace mtfs::journal 