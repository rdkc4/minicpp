#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <string>
#include <string_view>
#include <vector>

std::string __test__writeSourceToFile(std::string_view source, const std::string& filename);

void __test__removeFile(const std::string_view path);

int __test__runCommand(std::vector<std::string>&& args);

#endif