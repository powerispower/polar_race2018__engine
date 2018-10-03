#include <iostream>
#include <errno.h>

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
        std::cerr << dataFileName << " open failed" << "\n";
        return kIOError;
    }
    int valueOffset = GetFileLengthFromFd(fd);
    if (valueOffset == -1) {
        std::cerr << dataFileName << " get file length failed" << "\n";
        close(fd);
        return kIOError;
    }
    int ret = FileAppend(fd, value.data(), value.size());
    if (ret == -1) {
        std::cerr << dataFileName << " write value failed" << "\n";
        close(fd);
        return kIOError;
    }
    close(fd);
    // write index
    std::string indexFileName = IndexFileName(dir, hashKey); 
    fd =  open(indexFileName.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        std::cerr << dataFileName << " open failed" << "\n";
        return kIOError;
    }
    char indexValue[16];
    for(size_t i = 0; i < 8; ++i) {
        if (i < key.size()) {
            indexValue[i] = key.data()[i];
        } else {
            indexValue[i] = 0;
        }
    }
    std::cout << key.ToString() << " :: "<< valueOffset << "\n";
    unsigned int u_valueOffset = valueOffset;
    // follow 6B is valueOffset
    for(size_t i = 8; i < 14; ++i) {
        indexValue[i] = u_valueOffset & (0xff);
        u_valueOffset >>= 8;
    }
    long long int a = 0;
    for (int i = 13; i >= 8 ;--i) {
        std::cout << a * 256 << "+" << (unsigned int)indexValue[i] << "\n";
        a = (a * 256) + indexValue[i];
    }
    std::cout << a << "+++\n";
    // follow 2B is length
    int valueLength = value.size();
    for(size_t i = 14; i < 16; ++i) {
        indexValue[i] = valueLength % 256;
        valueLength >>= 8;
    }
    ret = FileAppend(fd, indexValue, 16);
    if (ret == -1) {
        std::cerr << indexFileName << " write value failed" << "\n";
        close(fd);
        return kIOError;
    }
    close(fd);
    _mu[hashKey].unlock();
    return kSucc;
}

RetCode Store::loadIndex(std::map<std::string, std::string>& memIndex) {
    for (u_int32_t i = 0; i < HASH_NUMBER; ++i) {
        std::string indexFileName = IndexFileName(dir, i);
        if (!FileExists(indexFileName)) {
            continue;
        }
        int fd = open(indexFileName.c_str(), O_RDONLY, 0644);
        if (fd < 0) {
            std::cerr << indexFileName << " open failed" << "\n";
            return kIOError;
        }
        char buf[16];
        while (1) {
            int a = read(fd, buf, 16);
            std::cout << a << " read--\n";
            if (a <= 0) break;
            std::string key;
            key.assign(buf, 8);
            std::string indexValue;
            indexValue.assign(buf + 8, 8);
            std::cout << "loadindex" << key.size() << key << "\n";
            memIndex[key] = indexValue;
        }
        close(fd);
    }
    return kSucc;
}

RetCode Store::readValue(const PolarString& key, const std::string& index, std::string* value) {
    if (index.size() != 8) {
        std::cerr << "index size is " << index.size() << "\n";
        return kInvalidArgument;
    }
    u_int32_t hashKey = KeyHash(key.data(), key.size());
    std::string dataFileName = DataFileName(dir, hashKey);
    size_t len = 0;
    off_t offset = 0;
    int oo = 0;
    for (int i = 5; i >= 0; --i) {
        offset = (offset << 8) + index[i];
        oo = (oo << 8) + index[i];
    }
    std::cout << oo << "\n";
    for (size_t i = 7; i > 5; --i) {
        len = (len << 8) + index[i];
    }
    std::cout << dataFileName << "\n";
    std::cout << "readvalue:len = " << len << " offset = " << offset << "\n";
    char *buf = new char[len];
    char* pos = buf;
    int fd = open(dataFileName.c_str(), O_RDONLY, 0644);
    if (fd < 0) {
        std::cerr << dataFileName << " open failed" << "\n";
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
    //std::cout << "read value" << value << "\n";
    close(fd);
    delete []buf;
    return kSucc;
}

void Store::setDir(const std::string& dir) {
    this->dir = dir;
}

}