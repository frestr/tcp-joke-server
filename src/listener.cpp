#include <fstream>
#include <thread>
#include <ctime>
#include "listener.h"
#include "util.h"

Listener::Listener(int port, int max_threads)
{
    sockfd = -1;
    thread_count = 0;
    this->port = port;
    this->max_threads = max_threads;

    // Make a socket
    addrinfo *addr = get_addr_info(std::to_string(port).c_str());
    sockfd = get_socket(addr);
    freeaddrinfo(addr);

    load_jokes();
}

Listener::~Listener()
{
    if (sockfd != -1)
        close(sockfd);
}

// Run in an endless loop, waiting for connections
void Listener::start_listening()
{
    const int backlog_limit = 10;
    if (listen(sockfd, backlog_limit) == -1) {
        std::cerr << "Error listening on port. Errno: " << errno << "\n";
        throw 1;
    }

    std::cout << "Listening on [" << listener_addr_repr << "]:" << port << " ...\n";
    while (true) {
        // Socket for incoming connection
        int new_fd;
        sockaddr_storage conn_addr = {0};
        socklen_t addr_size = sizeof(conn_addr);

        // Wait for connection
        if ((new_fd = accept(sockfd, (sockaddr *)&conn_addr, &addr_size)) == -1) {
            std::cerr << "Error accepting connection. Errno: " << errno << "\n";
            continue;
        }

        // Sleep until the number of threads is below max
        while (thread_count >= max_threads)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Handle each connection in a detached thread
        std::thread thread(&Listener::handle_connection, this, new_fd, conn_addr);
        thread.detach();
    }
    close(sockfd);
}

void Listener::handle_connection(int fd, sockaddr_storage conn_addr)
{
    ++thread_count;

    // Get address of connection source
    char conn_addr_repr[INET6_ADDRSTRLEN];
    inet_ntop(conn_addr.ss_family, util::get_in_addr((sockaddr *)&conn_addr),
              conn_addr_repr, sizeof(conn_addr_repr));

    // Prepend with Unix time
    std::cout << std::time(0) << " | Got conection from " << conn_addr_repr << ", sending joke...\n";

    send_joke(fd);
    close(fd);

    --thread_count;
}

addrinfo* Listener::get_addr_info(const char* port)
{
    addrinfo hints = {0};
    addrinfo *addr;

    hints.ai_family = AF_UNSPEC;        // both ipv4 and ipv6
    hints.ai_socktype = SOCK_STREAM;    // tcp
    hints.ai_flags = AI_PASSIVE;        // addr will be used for listening

    int status;
    if ((status = getaddrinfo(NULL, port, &hints, &addr)) != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << "\n";
        throw 1;
    }
    return addr;
}

int Listener::get_socket(addrinfo *addr)
{
    for(addrinfo *p = addr; p != NULL; p = p->ai_next) {
        // Want IPv6 address, so continue if not IPv6
        if (p->ai_family != AF_INET6) {
            continue;
        }

        // Ask for a socket file descriptor
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        // Allow reuse of port
        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            std::cerr << "Error: could not reuse port\n";
            throw 1;
        }

        // Set IPV6_V6ONLY to no, so we can use IPv4 and IPv6 simultaneously
        // (should be turned off by default, but we'll make sure it is off)
        int no = 0;
        if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) == -1) {
            std::cerr << "Error turning off IPV6_V6ONLY for socket\n";
        }

        // Save representation of assigned address for later
        char addr_repr[INET6_ADDRSTRLEN];
        inet_ntop(p->ai_family, util::get_in_addr(p->ai_addr), addr_repr, sizeof(addr_repr));
        listener_addr_repr = std::string(addr_repr);

        // Bind socket
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            std::cerr << "Error: could not bind socket to port\n";
            continue;
        }
        break;
    }
    return sockfd;
}

void Listener::load_jokes()
{
    std::ifstream ifs("jokes.txt");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    

    jokes = util::split(content, '#');

    // Set up the random number generator now that we know the number of jokes
    std::random_device rd;
    gen = std::mt19937(rd());
    dis = std::uniform_int_distribution<>(0, jokes.size()-1);
}

void Listener::send_joke(int fd)
{
    // Get random joke
    std::string joke = jokes.at(dis(gen));
    joke.append("\n");

    int bytes_sent;
    if ((bytes_sent = send(fd, joke.c_str(), joke.size(), 0)) == -1) {
        std::cerr << "Error sending message. Errno: " << errno << "\n";
    }
}
