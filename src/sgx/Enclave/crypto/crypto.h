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
int encrypt_aes(void *, size_t, char *, size_t);
int encrypt_aes_ctr(sgx_aes_ctr_128bit_key_t *, const uint8_t *, const int, uint8_t *);
int decrypt_aes(char *, size_t, void *, size_t);
int decrypt_aes_ctr(sgx_aes_ctr_128bit_key_t *, const uint8_t *, const int, uint8_t *);
int random_item(int);

