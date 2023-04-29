#include "Enclave_t.h"
#include "sgx_trts.h"
#include "sgx_tcrypto.h"
#include "stdlib.h"
#include <stdlib.h>

#define BUFFLEN 256
#define SGX_AES_IV_SIZE 12  

sgx_aes_ctr_128bit_key_t * create_aes_key();
void encrypt_aes(sgx_aes_ctr_128bit_key_t *, const uint8_t *, const int, uint8_t *);
void decrypt_aes(sgx_aes_ctr_128bit_key_t *, const uint8_t *, const int, uint8_t *);
int random_item(int);

