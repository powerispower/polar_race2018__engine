#include "log.h"

#include <chrono>
#include <ctime>
#include <iomanip>

namespace powerispower {
namespace polar_race2018__engine {
namespace util {

LogMessage::LogMessage(
        const std::string& file
        , int line) {
    using std::setw;

    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_localtime = std::localtime(&now_time_t);

    stream().fill('0');
    stream() << setw(2) << 1 + now_localtime->tm_mon
        << '-' << setw(2) << now_localtime->tm_mday
        << ' '
        << setw(2) << now_localtime->tm_hour << ':'
        << setw(2) << now_localtime->tm_min << ':'
        << setw(2) << now_localtime->tm_sec
        << ' '
        << file << ':' << line << "] ";
}

LogMessage::~LogMessage() {
    _oss << std::endl;
    std::cerr << _oss.str();
}

std::ostream& LogMessage::stream() {
    return _oss;
}

} // namespace util
} // namespace polar_race2018__engine
} // namespace powerispower
