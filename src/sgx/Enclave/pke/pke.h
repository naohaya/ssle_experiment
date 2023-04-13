#include "sgx_tcrypto.h"
#include <cstdio>
#include <cstring>

#define RSA_MOD_SIZE 1040

sgx_status_t create_rsa_pair();