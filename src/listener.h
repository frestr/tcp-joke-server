#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <string>
#include <vector>
#include <random>
#include <atomic>

class Listener
{
public:
    Listener(int port=4242, int max_threads=10);
    ~Listener();

    void start_listening();
private:
    void handle_connection(int fd, sockaddr_storage conn_addr);

    addrinfo* get_addr_info(const char* port);
    int get_socket(addrinfo *addr);

    void load_jokes();
    void send_joke(int fd);

    int sockfd;
    int port;

    std::vector<std::string> jokes;

    std::atomic<int> thread_count;
    int max_threads;

    std::mt19937 gen;
    std::uniform_int_distribution<> dis;
};
