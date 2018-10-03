#ifndef ENGINE_RACE_STORE_H_
#define ENGINE_RACE_STORE_H_
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <mutex>
#include "include/engine.h"
#include "util.h"

namespace polar_race {

class Store {
public:
    RetCode write(const PolarString& key, const PolarString& Value);

    RetCode loadIndex(std::map<std::string, std::string>& memIndex);

    RetCode readValue(const PolarString& key, const std::string& index, std::string* value);

    void setDir(const std::string& dir);

    Store() {}
    ~Store() {}
    
private:
    std::mutex _mu[HASH_NUMBER];

    std::string dir;
};

}  // namespace polar_race
#endif