#include "test_utils.hpp"

#include <fstream>
#include <filesystem>

std::string __test__writeSourceToFile(const std::string& code, const std::string& filename) {
    std::ofstream out(filename);
    out << code;
    out.close();
    return filename;
}

void __test__removeFile(const std::string& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);
}