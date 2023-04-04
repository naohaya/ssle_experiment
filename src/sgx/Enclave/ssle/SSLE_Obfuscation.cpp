/**
* An implementation of SSLE using Obfuscation
* Author: Naohiro Hayashibara
* Created: Fri. 17 Mar. 2023
*/
#include "SSLE.h"
#include "SSLE_Obfuscation.h"

using namespace std;


void SSLE_Obfuscation::initialize(const int *participants)
{
    // secure key should be generated in Enclave.
//    nodes = participants;
    num = *participants;


}

void SSLE_Obfuscation::electLeader()
{
    uint32_t rand_value;
    PPRF pprf = PPRF();

    /* electing a leader */
    randKey = pprf.prf(&secret, &lcg, &hash); // obtain a random key.
    punctKey = pprf.puncturing(randKey); // obtain a punctured key.

    /* leader election in randomized way */
    sgx_read_rand((unsigned char *)&rand_value, 4);
    leader_id = rand_value % num;

    /* bit commitment NOTE: it should use oblivious transfer */
    for (int i = 0; i < num; i++)
    {
        if(i == leader_id){
            comm_values.push_back(randKey); // if he is a leader
        } else{
            comm_values.push_back(commit(randKey)); // if he is not a leader
        }
    }

}

int SSLE_Obfuscation::verifyLeader()
{

}

string SSLE_Obfuscation::getRandKey()
{
    return randKey;
}

string SSLE_Obfuscation::getPunctKey()
{
    return punctKey;
}

/* get a commit value correspoding to the given node ID (n) */
string SSLE_Obfuscation::getCommValue(int n)
{
    return comm_values[n];
}

string SSLE_Obfuscation::commit(string val)
{
    char rand[10];
    uint32_t rand_val;
    uint32_t result;
    uint32_t input = stringToUint32(val);
    sgx_read_rand((unsigned char *)&rand_val, 4);

    result = input ^ rand_val;

    return uint32ToBinString(result);

}