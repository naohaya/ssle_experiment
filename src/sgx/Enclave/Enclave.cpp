#include "Enclave_t.h"
#include <sgx_trts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <iostream>
//#include <vector>
//#include "Node.h"
//#include "pprf/pprf.h"
#include "ssle/SSLE_Obfuscation.h"
#include "pke/pke.h"

uint8_t *pubkey; //public key
uint8_t *seckey; //secret key

int ecall_test(const char *message, size_t message_len)
{
	ocall_print(message);

	return 31337;
}


int ecall_election(const int *num_nodes) //TODO: seckey should not be received here
{	
	std::string result;
	char * ret = "hoge";
	SSLE_Obfuscation ssleobf;

	sgx_status_t retv = create_rsa_pair();

	ssleobf.initialize(num_nodes);

	ssleobf.electLeader();

	result = ssleobf.getPunctKey();
//	memcpy(ret, result.c_str(), result.length());
	
	ocall_print(ssleobf.getRandKey().c_str()); // random key

	ocall_print(result.c_str()); // punctured key 

	ocall_print(ssleobf.depunct(result).c_str()); // viterbi decoded
	return 0;
	//return retv; // for debug
}

/*
int ecall_create_rsa_key_pair(uint8_t *pkey)
{

}
*/