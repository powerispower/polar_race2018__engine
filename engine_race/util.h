#ifndef ENGINE_SIMPLE_UTIL_H_
#define ENGINE_SIMPLE_UTIL_H_
#include <stdint.h>
#include <pthread.h>
#include <string>
#include <vector>

namespace polar_race  {

static const uint32_t HASH_NUMBER = 64;

static std::string DataFileName(const std::string &dir, uint32_t fileno) {
    return dir + "/data_" + std::to_string(fileno);
}

static std::string IndexFileName(const std::string &dir, uint32_t fileno) {
    return dir + "/index_" + std::to_string(fileno);
}

static bool indexPairCmp(const std::pair<std::string, std::string> &index1, const std::pair<std::string, std::string> &index2) {
    return index1.first < index2.first;
}

static bool indexPairEqual(const std::pair<std::string, std::string> &index1, const std::pair<std::string, std::string> &index2) {
    return index1.first == index2.first;
}

// Hash
uint32_t KeyHash(const char* s, int size);

// Env
int GetDirFiles(const std::string& dir, std::vector<std::string>* result);
int GetFileLengthFromName(const std::string& file);
int GetFileLengthFromFd(const int fd);
int FileAppend(int fd, const char* buf, size_t value_len);
bool FileExists(const std::string& path);

}  // namespace polar_race

#endif 
