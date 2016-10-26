#include <sstream>
#include "listener.h"

int get_port(int argc, char *argv[])
{
    if (argc == 1) {
        std::cerr << "No port specified. Defaulting to 4242.\n";
        return 4242;
    }
    if (argc != 2) {
        std::cout << "Usage: ./server <port>\n";
        return -1;
    }

    std::istringstream in(argv[1]);
    int i;
    if (in >> i && in.eof()) {
        if (i < 1024 || i > 65535) {
            std::cerr << "Port number must in range [1024, 65535]\n";
            return -1;
        }
        return i;
    }
    std::cerr << "Invalid port number\n";
    return -1;
}

int main(int argc, char *argv[])
{
    int port = get_port(argc, argv);
    if (port == -1)
        return 1;

    try {
        Listener listener(port, 10);
        listener.start_listening(); 
    } catch (int e) {
        std::cerr << "Unable to recover from error. Quitting..." << std::endl;
    }
    return 0;
}
