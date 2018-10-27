#include <iostream>
#include <algorithm>
#include <errno.h>
#include <string.h>

#include "store.h"
#include "util.h"

namespace polar_race {

RetCode Store::write(const PolarString& key, const PolarString& value) {
    u_int32_t hashKey = KeyHash(key.data(), key.size());
    _mu[hashKey].lock();
    std::string dataFileName = DataFileName(dir, hashKey);
    // write value
    int fd = open(dataFileName.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        std::cout << dataFileName << " open failed" << "\n";
        return kIOError;
    }
    int valueOffset = GetFileLengthFromFd(fd);
    if (valueOffset == -1) {
        std::cout << dataFileName << " get file length failed" << "\n";
        close(fd);
        return kIOError;
    }
    int ret = FileAppend(fd, value.data(), value.size());
    if (ret == -1) {
        std::cout << dataFileName << " write value failed" << "\n";
        close(fd);
        return kIOError;
    }
    close(fd);
    // write index
    std::string indexFileName = IndexFileName(dir, hashKey); 
    fd =  open(indexFileName.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        std::cout << dataFileName << " open failed" << "\n";
        return kIOError;
    }
    char indexValue[16];
    // all char set to 0
    memset(indexValue, 0, sizeof(indexValue));
    // first 8 bytes is key
    memcpy(indexValue, key.data(), key.size());
    // follows 6 bytes is offset
    unsigned int u_valueOffset = valueOffset;
    memcpy(indexValue + 8, (char*)&u_valueOffset, 4);
    // last 2 bytes is length little-endian first 2 bytes
    unsigned int valueLength = value.size();
    memcpy(indexValue + 14, (char*)&valueLength, 2);
    ret = FileAppend(fd, indexValue, 16);
    if (ret == -1) {
        std::cout << indexFileName << " write value failed" << "\n";
        close(fd);
        return kIOError;
    }
    close(fd);
    _mu[hashKey].unlock();
    return kSucc;
}

RetCode Store::loadIndex(Index& memIndex) {
    for (u_int32_t i = 0; i < HASH_NUMBER; ++i) {
        std::string indexFileName = IndexFileName(dir, i);
        if (!FileExists(indexFileName)) {
            continue;
        }
        int fd = open(indexFileName.c_str(), O_RDONLY, 0644);
        if (fd < 0) {
            std::cout << indexFileName << " open failed" << "\n";
            return kIOError;
        }
        char buf[16];
        while (read(fd, buf, 16) > 0) {
            std::string key;
            key.assign(buf, 8);
            std::string indexValue;
            indexValue.assign(buf + 8, 8);
            memIndex.pushBack(key, indexValue);
        }
        close(fd);
    }
    // sort and unique
    memIndex.processIndex();

    return kSucc;
}

RetCode Store::readValue(const PolarString& key, const std::string& index, std::string* value) {
    if (index.size() != 8) {
        std::cout << "index size is " << index.size() << "\n";
        return kInvalidArgument;
    }
    u_int32_t hashKey = KeyHash(key.data(), key.size());
    std::string dataFileName = DataFileName(dir, hashKey);
    unsigned int len = 0;
    unsigned int offset = 0;
    const char * c_index = index.c_str();
    // memcpy offset
    memcpy((char*)&offset, c_index, 4);
    // memcpy len little-endian first 2 bytes
    memcpy((char*)&len, c_index + 6, 2);
    char *buf = new char[len];
    char* pos = buf;
    int fd = open(dataFileName.c_str(), O_RDONLY, 0644);
    if (fd < 0) {
        std::cout << dataFileName << " open failed" << "\n";
        return kIOError;
    }
    int l = len;
    while (len > 0) {
        ssize_t r = pread(fd, pos, len, offset);
        if (r < 0) {
            if (errno == EINTR) {
                continue;  // Retry
            }
            close(fd);
            return kIOError;
        }
        pos += r;
        offset += r;
        len -= r;
    }
    *value = std::string(buf, l);
    close(fd);
    delete []buf;
    return kSucc;
}

void Store::setDir(const std::string& dir) {
    this->dir = dir;
}

}