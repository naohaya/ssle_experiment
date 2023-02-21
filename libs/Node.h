#include <iostream> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string> 
#include <vector>

class Node {
    public:
    //variables
    std::string ipaddr;
    int portno;

    // methods
    void initialization(void);
    int listen_as_server(void);
    int accept_as_server(void);
    int connect_from_client(void);
    int close_connection(void);
    int close_connection(int);
    ssize_t receive_message(void);
    ssize_t receive_message(int);
    ssize_t receive_message(char *, size_t, int);
    ssize_t send_message(const std::string, size_t, int);
    ssize_t send_message(int, const std::string, size_t, int);
    std::string get_message(void);

    Node(std::string given_addr, int given_port){
        ipaddr = given_addr;
        portno = given_port;
    }

    Node() {
        ipaddr = "127.0.0.1";
        portno = 18888;
    }

    private:
    int socketfd = 0;
    struct sockaddr_in addr;
    std::vector<std::string> messages;
    size_t default_len = 255;
    int default_offset = 0;

};
