#include <assert.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "engine_race/log.h"
#include "include/engine.h"

static const char kEnginePath[] = "/tmp/test_engine";
static const char kDumpPath[] = "/tmp/test_dump";

using namespace polar_race;

class DumpVisitor : public Visitor {
public:
    DumpVisitor(int* kcnt)
        : key_cnt_(kcnt) {}

    ~DumpVisitor() {}

    void Visit(const PolarString& key, const PolarString& value) {
        printf("Visit %s --> %s\n", key.data(), value.data());
        (*key_cnt_)++;
    }

private:
    int* key_cnt_;
};

int main() {
    Engine *engine = NULL;

    RetCode ret = Engine::Open(kEnginePath, &engine);
    assert (ret == kSucc);


    ret = engine->Write("aaa", "aaaaaaaaaaa");
    assert (ret == kSucc);
    ret = engine->Write("aaa", "111111111111111111111111111111111111111111");
    ret = engine->Write("aaa", "2222222");
    ret = engine->Write("aaa", "33333333333333333333");
    ret = engine->Write("aaa", "4");

    ret = engine->Write("bbb", "bbbbbbbbbbbb");
    assert (ret == kSucc);

    ret = engine->Write("ccd", "cbbbbbbbbbbbb");
    delete engine;

    Engine *engine_read = NULL;
    ret = Engine::Open(kEnginePath, &engine_read);
    
    std::string value;
    ret = engine_read->Read("aaa", &value);

    LOG_STREAM << "Read aaa value=" << value;
    
    ret = engine_read->Read("bbb", &value);
    //std::cout << ret << "\n";
    assert (ret == kSucc);
    //std:: cout << value.size() << value << "\n";

    LOG_STREAM << "Read bbb value=" << value;

    // int key_cnt = 0;
    // DumpVisitor vistor(&key_cnt);
    // ret = engine->Range("b", "", vistor);
    // assert (ret == kSucc);
    // printf("Range key cnt: %d\n", key_cnt);
    delete engine_read;

    return 0;
}
