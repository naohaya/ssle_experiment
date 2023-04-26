#include <iostream>  
#include <unistd.h> 
#include <string> 
#include <sstream>
#include <vector>
#include "sgx_trts.h"
//#include "Node.h"
#include "../pprf/pprf.h"
#include "../utils/utils.h"
#include "SSLE.h"

class SSLE_Obfuscation //: public SSLE
{
    private:
//    vector<Node> nodes; // a list of participants.
    std::vector<std::string> comm_values; // committed values correspoding to nodes
    uint64_t secret = 0x93064E905C127FE5; // key
    uint64_t hash = 0xA4BDE5C4A05E6256; // hash (constant)
    uint64_t lcg = 0; // needed for pprf.
    std::string randKey;
    std::string punctKey;
    int num;
    uint32_t leader_id;


    public:
        void initialize(const int *);
        std::string get_key();
        void electLeader();
        int verifyLeader();
        std::string getRandKey();
        std::string getPunctKey();
        std::string getCommValue(int);
        std::string commit(std::string);
        std::string depunct(std::string punct);

};
