#include "test_utils.hpp"

#include <fstream>
#include <filesystem>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

extern "C" {
    extern char** environ;
}

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

int __test__runCommand(std::vector<std::string>&& args) {
    std::vector<char*> argv;
    argv.reserve(args.size() + 1);
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    argv.push_back(nullptr);

    pid_t pid;
    int status;

    int res = posix_spawnp(&pid, argv[0], nullptr, nullptr, argv.data(), environ);
    if(res != 0){
        return res;
    }

    if (waitpid(pid, &status, 0) == -1) {
        return -1;
    }

    return status;
}