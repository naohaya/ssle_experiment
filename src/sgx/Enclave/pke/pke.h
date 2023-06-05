#include "sgx_tcrypto.h"
#include "Enclave_t.h"
#include "stdio.h"
#include <cstdio>
#include <cstring>

#define RSA_MOD_SIZE 256
#define KEY_SIZE 8

sgx_status_t create_rsa_pair(void *, void *);
