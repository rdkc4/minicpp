#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <string>

std::string __test__writeSourceToFile(const std::string& code, const std::string& filename);

void __test__removeFile(const std::string& path);

#endif