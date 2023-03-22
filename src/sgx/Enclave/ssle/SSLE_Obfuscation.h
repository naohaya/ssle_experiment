#include <iostream>  
#include <unistd.h> 
#include <string> 
#include <sstream>
#include "Node.h"
#include "../pprf/pprf.h"
#include "../utils/utils.h"

class SSLE_Obfuscation : public SSLE
{
    public:
        void initialize(uint64_t, vector<Node>);
        void electLeader();
        static int verifyLeader();
        std::string getRandKey();
        std::string getPunctKey();
        std::string getCommValue(int);

};
