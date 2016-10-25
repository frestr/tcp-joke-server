#include <sstream>
#include <iostream>
#include "util.h"

// Split a string into a a vector of strings, with a delimiter
void util::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (item.at(0) == '\n')
            item.erase(0, 1);
        elems.push_back(item);
    }
}

// Wrapper around other split function
std::vector<std::string> util::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    util::split(s, delim, elems);
    return elems;
}

// Get sockaddr, IPv4 or IPv6
void* util::get_in_addr(sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in*)sa)->sin_addr);
    }
    return &(((sockaddr_in6*)sa)->sin6_addr);
}
