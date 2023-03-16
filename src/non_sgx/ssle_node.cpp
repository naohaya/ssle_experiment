/**
 * This program is for experiments of SSLE without SGX.
 * 
*/

#include <iostream> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string> 
#include <sstream>
#include "Node.h"
#include "pprf.h"
#include "commitment.h"
#include "randbit.hpp"

using namespace std;

Node registration(std::string);
string commit(string, int);
string commit(uint32_t, int);
string b2s(uint32_t);
unsigned int stou(std::string const &, size_t *, int);


int main(int argc, char *argv[]){

    std::vector<Node> nodes;
	Node thisNode; 
    Node remoteNode;
    bool leader = false;
    int inconnect;
    int localport;
    int remoteport;
    std::ostringstream oss;

	if(argc == 4) {
        localport = std::atoi(argv[1]);
        remoteport = std::atoi(argv[2]);
		thisNode = Node("127.0.0.1", localport);
        remoteNode = Node("127.0.0.1", remoteport);

        if(std::atoi(argv[3])) {
            leader = true;
        }

	} else {
        std::cout << "./node <local port>, <remote port>, <flag>" << std::endl;
	}


    if(leader) { // what a leader does
        // preparing for information for pprf
        uint64_t secret = 0x93064E905C127FE5; // key
        uint64_t hash = 0xA4BDE5C4A05E6256; // hash (constant)
        uint64_t lcg = 0;
        string rval;
        string punct;
        string comm;

        // creating an object of PPRF
        PPRF pprf = PPRF();
        rval = pprf.prf(&secret, &lcg, &hash);
        punct = pprf.puncturing(rval);
        
        // register itself
        nodes.push_back(thisNode);

	    thisNode.initialization();

    	thisNode.listen_as_server();

	    inconnect = thisNode.accept_as_server();

        // registration of participants
	    thisNode.receive_message(inconnect);
    	std::string str = thisNode.get_message();
	    std::cout << "Leader received: " << str << std::endl; 
        nodes.push_back(registration(str));
        std::cout << "Leader registered: " << nodes[nodes.size() - 1].ipaddr << ":" << nodes[nodes.size() - 1].portno << std::endl;


        // decide the next leader TODO


        // send information to participants
        str = punct;
    	thisNode.send_message(inconnect, str, str.length(), 0); //send message to participants
    	std::cout << "Leader sent puct. value: " << str << "(" << str.length() << ")" << std::endl; 

        // send committed value
        comm = commit(rval, 1);
        thisNode.send_message(inconnect, comm, comm.length(), 0); // send message to participants
        std::cout << "Leader sent com value: " << comm << "(" << comm.length() << ")" << std::endl; 

	    thisNode.receive_message(inconnect);
    	str = thisNode.get_message();
	    std::cout << "Leader received: " << str << std::endl; 

    	// socket close
	    thisNode.close_connection(inconnect); // close incoming connection
    	thisNode.close_connection(); // close the listening port

    } else {  // what a participant does
    	remoteNode.initialization();

	    remoteNode.connect_from_client();
    

    	//データ送信
    	std::string msg = thisNode.ipaddr + "," + std::to_string(thisNode.portno);
	    remoteNode.send_message(msg, msg.length(), 0); //送信
    	std::cout << "Participant sent: " << msg << std::endl;

	    //データ受信
    	char r_str[162]; // receive buffer
//	    remoteNode.receive_message(r_str, 12, 0);
        remoteNode.receive_message();
        std::string received = remoteNode.get_message();
	    std::cout << "Client get_messages(): " << received << "(" << received.length() << ")" << std::endl;

        // correction of the received punctured key
        int remain = received.length() % 18;
        received.erase(received.length() - remain, remain);
        std::cout << "corrected string: " << received << "(" << received.length() << ")" << std::endl;

        // depuncturing
        PPRF pprf = PPRF();
        string depunct = pprf.depuncturing(received);
        std::cout << "depunctured: " << depunct << "(" << depunct.length() << ")"  << std::endl;

        char c_str[162];
        remoteNode.receive_message();
        std::string comm = remoteNode.get_message();
        std::cout << "Participant received commitment: " << comm << std::endl;

    	std::string rmsg = "hello";
	    remoteNode.send_message(rmsg, rmsg.length(), 0);
        std::cout << "Participant sent: " << rmsg << std::endl;

    	// socket close
	    remoteNode.close_connection();

        
    }
	//socket close
//	thisNode.close_connection(inconnect); // close incoming connection
//	thisNode.close_connection(); // close the listening port
//    if(!leader)
//        remoteNode.close_connection();

	return 0;
}

Node registration(std::string str) {
    int first = 0;
    char del = ',';
    int last = str.find_first_of(del);
 
    std::vector<std::string> result;
 
    while (first < str.size()) {
        std::string subStr(str, first, last - first);
 
        result.push_back(subStr);
 
        first = last + 1;
        last = str.find_first_of(del, first);
 
        if (last == std::string::npos) {
            last = str.size();
        }
    }

    Node ret = Node(result[0], atoi(result[1].c_str()));

    return ret;

}

/* Function for bit commitment based on Naor's approach.
* It requires "randbit.hpp".
* It generates a committed value from the given bits and 1-bit value. 
*/
string commit(string rand, int b) {
  size_t *idx = 0;
  int base = 2;

  if(b) {
    return rand;
  } else {
      uint32_t rand_val = stou(rand, idx, base);

      return commit(rand_val, b);
  }

}

string commit(uint32_t rand, int b){
    const double prob = 0.6;
    uint32_t key;
    uint32_t result;

    rbs::generator gen(prob);
    key = gen(); // generate a random key

    if (b) {
      result = rand; // replace to pprf(rand)

    } else{
      result = rand ^ key; // replace to pprf(rand)
    }


    return b2s(result);

}

string b2s(uint32_t bs) {
  std::stringstream ss;
  for (int i = 0; i < 32; i++) {
    ss << ((bs & (1 << (32 - i - 1))) ? 1 : 0);
  }
  return ss.str();
}

unsigned int stou(std::string const &str, size_t *idx = 0, int base = 2) {
  const unsigned long val = std::stoul(str, idx, base);
  if (std::numeric_limits<unsigned>::max() < val) {
    std::cout << val << std::endl;
    throw std::out_of_range("stou");
  }
  return static_cast<unsigned>(val);
}

/* Function for verifying the commitment.
*/
bool verify(string bits, string rand) {
  uint32_t input = stoi(bits);
  uint32_t rvalue = stoi(rand);
  uint32_t result;

  result = rvalue; // temporary

//  result = pprf(rand);
  if (result == input) {
    return true;
  } else {
    return false;
  }

}


