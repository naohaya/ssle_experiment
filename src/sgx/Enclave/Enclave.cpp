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
#include "pke/test_crypto.h"
#include "crypto/crypto.h"

//void *pubkey = (void *)malloc(KEY_SIZE); //public key
//void *seckey = (void *)malloc(KEY_SIZE); //secret key
unsigned char *testkey = (unsigned char *)"01234567890123456789012345678901";
unsigned char ciphertext[1048];
unsigned char pubkey[256];
unsigned char seckey[256];
static sgx_aes_gcm_128bit_key_t key = {0x76, 0x39, 0x79, 0x24, 0x42, 0x26, 0x45, 0x28, 0x48, 0x2b, 0x4d, 0x3b, 0x62, 0x51, 0x5e, 0x8f};

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

int ecall_election(const uint64_t *prfkey, 
	const int *num_nodes, 
	uint64_t *output,  // committed value
	uint64_t *encryptedData) //TODO: seckey should not be received here
{	
	std::string result;
	const unsigned char *inData = (unsigned char *)"hoge";
	SSLE_Obfuscation ssleobf;
	uint64_t commitValue;

	/* pke encryption test */
	// unsigned char *outData = (unsigned char *) malloc (255);
	unsigned char *outData = NULL;
	size_t outlen = 256;

	sgx_status_t retv = create_rsa_pair(pubkey, seckey);

	ssleobf.initialize(prfkey, num_nodes);

	ssleobf.electLeader();

	commitValue = ssleobf.getCommValue(0);
//	ocall_print((char *)&commitValue);
	memcpy(output, &commitValue, sizeof(uint64_t)); // copy the committed value to the outside pointer.

	result = ssleobf.getPunctKey();
//	memcpy(ret, result.c_str(), result.length());

	/* pke encryption test */
	// the following key generation code works well
	unsigned char pkey[256];
	//void *pkey = NULL;
	unsigned char skey[256];
	//void *skey = NULL;
	test_key_create(pkey, skey); // generation of pub/sec keys.

	// check the generated public key
	// public key is NULL here
	
	if (pkey != NULL) {
		ocall_print("public key:");
		ocall_print((char *)pkey);
	} else {
		ocall_print("public key is NULL");
	}
	


	//test_encrypt(pkey, outData); // encryption test by public key
	//test_decrypt(skey, outData); // decryption test by public key
	test_crypto_with_keys(pubkey, seckey);
	//test_crypto(); // just copy from the sample code (it works corectlly).
	//int ret = encrypt(pubkey, inData, 5, ciphertext, &outlen);

	/* test for common key based encryption  */
	//const uint8_t *inData = (uint8_t *)ret;
//	char outData[BUFFLEN] = {0};
	//sgx_aes_ctr_128bit_key_t *key; 
	//key = create_aes_key();

//	int encret = encrypt_aes((void *)inData, sizeof(inData), outData, outlen);
	ocall_print("Encrypted data:");
//	ocall_print((char *)ciphertext);
	ocall_print((char *)outData);
//	ocall_print((char *)outlen);
	
//	ocall_print(ssleobf.getRandKey().c_str()); // random key

//	ocall_print(result.c_str()); // punctured key 

//	ocall_print(ssleobf.depunct(result).c_str()); // viterbi decoded
	return 0;
//	return ret; // for debug.f

}

// obtain a RSA public key
int ecall_create_rsa_key_pair(void *pkey)
{
	create_rsa_pair(pubkey, seckey);
	memcpy(pkey, pubkey, KEY_SIZE);

	return 0;
}
