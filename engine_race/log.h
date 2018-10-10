#pragma once

#include <iostream>
#include <sstream>

#define LOG_STREAM ::powerispower::polar_race2018__engine::util::LogMessage(__FILE__, __LINE__).stream()

namespace powerispower {
namespace polar_race2018__engine {
namespace util {

class LogMessage {
public:
    LogMessage(
            const std::string& file
            , int line);

    ~LogMessage();

    std::ostream& stream();
private:
    std::ostringstream _oss;
};

} // namespace util
} // namespace polar_race2018__engine
} // namespace powerispower
