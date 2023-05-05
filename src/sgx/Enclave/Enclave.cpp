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
#include "crypto/crypto.h"

void *pubkey = (void *)malloc(KEY_SIZE); //public key
void *seckey = (void *)malloc(KEY_SIZE); //secret key

int ecall_test(const char *message, size_t message_len)
{
	ocall_print(message);

	return 31337;
}

// get PPRF key
int ecall_get_key(uint64_t *keyout)
{
	SSLE_Obfuscation ssleobf;
	uint64_t prfkey = ssleobf.get_key();

//	ocall_print(keystring.c_str());

	memcpy(keyout, &prfkey, sizeof(uint64_t));

	return 0;
}

int ecall_election(const uint64_t *prfkey, const int *num_nodes, uint32_t *output, uint32_t *encryptedData) //TODO: seckey should not be received here
{	
	std::string result;
	char * ret = "hoge";
	SSLE_Obfuscation ssleobf;

	/* encryption test */
//	unsigned char *outData;
//	size_t *outlen;

//	sgx_status_t retv = create_rsa_pair(pubkey, seckey);

	ssleobf.initialize(prfkey, num_nodes);

	ssleobf.electLeader();

	result = ssleobf.getPunctKey();
//	memcpy(ret, result.c_str(), result.length());

	/* encryption test */
	// const unsigned char *inData = (unsigned char *)ret;
	// int retv = encrypt(pubkey, inData, 4, outData, outlen);

	const uint8_t *inData = (uint8_t *)ret;
	uint8_t outData[BUFFLEN] = {0};
	sgx_aes_ctr_128bit_key_t *key; 
	key = create_aes_key();

	int encret = encrypt_aes(key, inData, sizeof(uint8_t), outData);

	ocall_print((char *)outData);
//	ocall_print((char *)outlen);
	
//	ocall_print(ssleobf.getRandKey().c_str()); // random key

//	ocall_print(result.c_str()); // punctured key 

//	ocall_print(ssleobf.depunct(result).c_str()); // viterbi decoded
	return 0;
//	return encret; // for debug.

}

// obtain a RSA public key
int ecall_create_rsa_key_pair(void *pkey)
{
	create_rsa_pair(pubkey, seckey);
	memcpy(pkey, pubkey, KEY_SIZE);

	return 0;
}
