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
#include "pke/crypto.h"

void *pubkey = (void *)malloc(KEY_SIZE); //public key
void *seckey = (void *)malloc(KEY_SIZE); //secret key

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

	/* encryption test */
	unsigned char *outData;
	size_t *outlen;

//	sgx_status_t retv = create_rsa_pair(pubkey, seckey);

	ssleobf.initialize(num_nodes);

	ssleobf.electLeader();

	result = ssleobf.getPunctKey();
//	memcpy(ret, result.c_str(), result.length());

	/* encryption test */
	const unsigned char *inData = result.c_str();
	encrypt(pubkey, inData, result.length(), outData, outlen);

	ocall_print((char *)outlen);
	
//	ocall_print(ssleobf.getRandKey().c_str()); // random key

//	ocall_print(result.c_str()); // punctured key 

//	ocall_print(ssleobf.depunct(result).c_str()); // viterbi decoded
	return 0;
	//return retv; // for debug
}

// obtain a RSA public key
int ecall_create_rsa_key_pair(void *pkey)
{
	create_rsa_pair(pubkey, seckey);
	memcpy(pkey, pubkey, KEY_SIZE);

	return 0;
}
