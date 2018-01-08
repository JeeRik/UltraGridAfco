//
// Created by maara on 2/24/17.
//

#include <time.h>
#include <ostream>
#include <ctime>
#include <iomanip>
#include <sstream>

long getUnixTime()
{
    struct timespec tv;
    
    if(clock_gettime(CLOCK_REALTIME, &tv) != 0) return 0;
    
    return (tv.tv_sec * 1000 + (tv.tv_nsec / 1000000));
}

std::string getTimeString() {
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);
    std::stringstream ss;
    long nowMillis = (getUnixTime() / 10) % 100;
    ss << std::put_time(&tm, "%H:%M:%S.") << nowMillis;
    return ss.str();
}
