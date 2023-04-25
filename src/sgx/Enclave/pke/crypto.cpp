#include "crypto.h"

int encrypt(void *pkey, const unsigned char *inData, size_t in_size, unsigned char *outData, size_t *out_size)
{
    sgx_status_t status = sgx_rsa_pub_encrypt_sha256(
        pkey, 
        outData, 
        out_size, 
        inData, 
        in_size
    ); 

// for debug
    if (pkey == NULL) {
        return -2;
    } else if(outData == NULL) {
        return -1;
    } else {
        return 0;
    }
    
}

int decrypt(void *skey, const unsigned char *inData, size_t in_size, unsigned char *outData, size_t *out_size)
{
    sgx_status_t status = sgx_rsa_priv_decrypt_sha256(
        skey,
        outData,
        out_size,
        inData,
        in_size
    );

    return status;
}