#include "Enclave_t.h"
#include "sgx_trts.h"
#include "sgx_tcrypto.h"
#include "stdlib.h"
#include <stdlib.h>
#include <cstring>

#define BUFFLEN 256
#define SGX_AES_IV_SIZE 12
#define SGX_AES_MAC_SIZE 16   //128 bit mac
#define CIPHERTEXT_SIZE 36
#define ADD_ENC_DATA_SIZE (SGX_AES_MAC_SIZE + SGX_AES_IV_SIZE)

sgx_aes_ctr_128bit_key_t * create_aes_key();
int encrypt_aes(sgx_aes_gcm_128bit_key_t key, void *dataIn, size_t len, char *dataOut, size_t lenOut);
int encrypt_aes_ctr(sgx_aes_ctr_128bit_key_t *, const uint8_t *, const int, uint8_t *);
int decrypt_aes(sgx_aes_gcm_128bit_key_t key, char *dataIn, size_t len, void *dataOut, size_t lenOut);
int decrypt_aes_ctr(sgx_aes_ctr_128bit_key_t *, const uint8_t *, const int, uint8_t *);
int random_item(int);

