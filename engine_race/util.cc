// Copyright [2018] Alibaba Cloud All rights reserved
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <functional>
#include "include/engine.h"
#include "util.h"

namespace polar_race {

uint32_t KeyHash(const char* s, int size) {
  std::hash<std::string> hash_fn;
  std::string needToHash;
  needToHash.assign(s, size);
  return hash_fn(needToHash) % HASH_NUMBER;
}

int GetDirFiles(const std::string& dir, std::vector<std::string>* result) {
  int res = 0;
  result->clear();
  DIR* d = opendir(dir.c_str());
  if (d == NULL) {
    return errno;
  }
  struct dirent* entry;
  while ((entry = readdir(d)) != NULL) {
    if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0) {
      continue;
    }
    result->push_back(entry->d_name);
  }
  closedir(d);
  return res;
}

int GetFileLengthFromName(const std::string& file) {
  struct stat stat_buf;
  int rc = stat(file.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

int GetFileLengthFromFd(const int fd) {
  struct stat stat_buf;
  int rc = fstat(fd, &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

int FileAppend(int fd, const char* buf, size_t value_len) {
  if (fd < 0) {
    return -1;
  }
  const char* pos = buf;
  while (value_len > 0) {
    ssize_t r = write(fd, pos, value_len);
    if (r < 0) {
      if (errno == EINTR) {
        continue;  // Retry
      }
      return -1;
    }
    pos += r;
    value_len -= r;
  }
  return 0;
}

bool FileExists(const std::string& path) {
  return access(path.c_str(), F_OK) == 0;
}

}  // namespace polar_race
