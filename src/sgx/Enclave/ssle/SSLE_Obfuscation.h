#include <iostream>  
#include <unistd.h> 
#include <string> 
#include <sstream>
//#include "Node.h"
#include "../pprf/pprf.h"
#include "../utils/utils.h"

class SSLE_Obfuscation : public SSLE
{
    private:
//    vector<Node> nodes; // a list of participants.
    vector<string> comm_values; // committed values correspoding to nodes
    uint64_t secret = 0x93064E905C127FE5; // key
    uint64_t hash = 0xA4BDE5C4A05E6256; // hash (constant)
    uint64_t lcg = 0; // needed for pprf.
    string randKey;
    string punctKey;
    int num;
    uint32_t leader_id;


    public:
        void initialize(const int *);
        void electLeader();
        static int verifyLeader();
        string getRandKey();
        string getPunctKey();
        string getCommValue(int);

};
