#include <iostream> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string> 
#include <vector>
#include <regex>
#include <algorithm>

using std::string;


class Node {
    public:
    //variables
    string ipaddr; // IP address
    int portno; // Port num.
    void *pubkey; // public key
    double weight = 0.0; // weight
    double positiveFactor = 0.0; // positive factor to calculate the weight.
    double negativeFactor = 0.0; // negative factor to calculate the weight.

    // methods
    void initialization(void);
    int listen_as_server(void);
    int accept_as_server(void);
    int connect_from_client(void);
    int close_connection(void);
    int close_connection(int);
    int setPubkey(const void *);
    void updatePosFactor(double);
    void updateNegFactor(double);
    ssize_t receive_message(void);
    ssize_t receive_message(int);
    ssize_t receive_message(char *, size_t, int);
    ssize_t send_message(const string, size_t, int);
    ssize_t send_message(int, const string, size_t, int);
    string get_message(void);

    Node(string given_addr, int given_port){
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
    std::vector<string> messages;
    size_t default_len = 255;
    int default_offset = 0;

};

// Initialization (for servers and clients)
// 1) socket generation, 2) configuration for an address, 
void Node::initialization(){

	// socket generation
	socketfd = socket(AF_INET, SOCK_STREAM, 0); 
	if(socketfd < 0){ // error handling

		std::cout << "Error socket:" << std::strerror(errno); 
		exit(1); 
	}

    memset(&addr, 0, sizeof(struct sockaddr_in));  // initializing sockaddr_in by memset
    addr.sin_family = AF_INET; //Address family (ipv4)
	addr.sin_port = htons(portno); // Port No. converted by htons()
	addr.sin_addr.s_addr = inet_addr(ipaddr.c_str()); // set IP address

}

// listening the port (for servers)
int Node::listen_as_server() {
	// bind a socket to the designated port.
	if(bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0){ 

		std::cout << "Error bind:" << std::strerror(errno); 
		exit(1); 
	}

    // start listening at the port.
	if(listen(socketfd,SOMAXCONN) < 0){ 
        // error handling
		std::cout << "Error listen:" << std::strerror(errno); 
		close(socketfd); 
		exit(1); 
	}

    return 0;
}

// set a public key
int Node::setPubkey(const void *givenkey){
    pubkey = (void *)malloc(sizeof(givenkey));
    memcpy(pubkey, givenkey, sizeof(givenkey));

    return 0;

}

void Node::updatePosFactor(double pf){
    positiveFactor = pf;
}

void Node::updateNegFactor(double nf){
    negativeFactor = nf;
}

// accepting the connection request (for servers)
int Node::accept_as_server() {
	struct sockaddr_in get_addr; 
	socklen_t len = sizeof(struct sockaddr_in); 

    // prepare for new connection
	int connect = accept(socketfd, (struct sockaddr *)&get_addr, &len); 

	if(connect < 0){ //error handling

		std::cout << "Error accept:" << std::strerror(errno); //標準出力
		exit(1); 
	}

    return connect;
}

// connect to a server (for clients)
int Node::connect_from_client() {
	if(connect(socketfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))){ //ソケット, アドレスポインタ, アドレスサイズ
        printf("connection fails\n");
        exit(1);
    }

    return 0;
}

// receive a message
ssize_t Node::receive_message() {
    return receive_message(socketfd);
}

// receive a message (for servers)
// server should designate a socket fd for the destination
ssize_t Node::receive_message(int sockfd){    
    char buff[default_len];
//    std::regex r("\\s+");
    int ret = recv(sockfd, (void *)buff, default_len, default_offset);
    //std::cout << "recv: " << buff << std::endl;

    string msg(buff);
    // remove unuseful characters
//    msg = std::regex_replace(msg, r, "");
    
    msg.erase(std::remove_if(msg.begin(), msg.end(),
                        [](char c) {
                            return (c == '\n' || c == '\r' ||
                                    c == '\t' || c == '\v' || c == '\f');
                        }),
                        msg.end());
    
    messages.push_back(msg); // added received message.
    return ret;
}

// receive a message (for clients)
ssize_t Node::receive_message(char *msg, size_t len, int offset){
    int ret = recv(socketfd, (void *)msg, len, offset);
    std::string str(msg);
    messages.push_back(str);

    return ret;
}

// send a message (for servers)
// server should designate a socket fd for the destination
ssize_t Node::send_message(int sockfd, const string msg, size_t len, int offset){
    return send(sockfd, msg.c_str(), len, offset);
}

// send a message (for clients)
ssize_t Node::send_message(const string msg, size_t len, int offset){
    return send(socketfd, msg.c_str(), len, offset);
}

//close a listening connection (for servers)
int Node::close_connection(){
    return close(socketfd);
}

//close the connection
int Node::close_connection(int sockfd){
    return close(sockfd);
}

// get a message received at/from the node
string Node::get_message() {
    string msg;
    if(messages.empty()) {
        std::cout << "Message buffer is empty" << std::endl;
        return "";
    }
    auto itr = messages.begin();
    msg = *itr;

    messages.erase(messages.begin()); // delete a message from head
    return msg;
}
