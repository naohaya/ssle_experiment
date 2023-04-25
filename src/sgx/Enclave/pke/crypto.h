#include "stdlib.h"
#include "sgx_trts.h"
#include "sgx_tcrypto.h"
#include <string.h>
#include <stdlib.h>


void encrypt(void *, const unsigned char *, size_t, unsigned char *, size_t *);
void decrypt(void *, const unsigned char *, size_t, unsigned char *, size_t *);
