#ifndef ENGINE_RACE_STORE_H_
#define ENGINE_RACE_STORE_H_
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <algorithm>
#include <mutex>
#include "include/engine.h"
#include "util.h"

namespace polar_race {

struct Index {

    std::vector<std::pair<std::string, std::string> > memIndex;

    void pushBack(const std::string& key, const std::string& value) {
        memIndex.push_back(std::make_pair(key, value));
    }

    void processIndex() {
        std::stable_sort(memIndex.begin(), memIndex.end(), indexPairCmp);
        std::reverse(memIndex.begin(), memIndex.end());
        memIndex.erase(std::unique(memIndex.begin(), memIndex.end(), indexPairEqual), memIndex.end());
        std::reverse(memIndex.begin(), memIndex.end());
        std::cout << memIndex.size() << "loaded" << std::endl;
    }

    bool getIndex(const std::string& key, std::string &value) {
        std::vector<std::pair<std::string, std::string> >::iterator it =
            std::lower_bound(memIndex.begin(), memIndex.end(), std::make_pair(key, ""), indexPairCmp);
        if (it == memIndex.end()) {
            return false;
        } else {
            value = (*it).second;
            return true;
        }
    }
};

class Store {
public:
    RetCode write(const PolarString& key, const PolarString& Value);

    RetCode loadIndex(Index& memIndex);

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