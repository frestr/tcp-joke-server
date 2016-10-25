#pragma once

#include <vector>
#include <string>
#include <netinet/in.h>

namespace util {
    void split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);
    void* get_in_addr(sockaddr *sa);
}
