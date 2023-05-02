#include "crypto.h"

sgx_aes_ctr_128bit_key_t key;

sgx_aes_ctr_128bit_key_t * create_aes_key()
{
    sgx_aes_ctr_128bit_key_t *newkey = (sgx_aes_ctr_128bit_key_t *)malloc(sizeof(sgx_aes_ctr_128bit_key_t));
    for(int i=0; i<16; i++)
    {
        key[i] = random_item(256);
    }
    memcpy(newkey, key, sizeof(sgx_aes_ctr_128bit_key_t));

    return newkey;
    
}

int encrypt_aes(sgx_aes_ctr_128bit_key_t *p_key, const uint8_t *p_src, const int src_len, uint8_t *p_dest)
{
//    uint8_t *p_ctr = (uint8_t)"0x12345F"; // initialization vector
    const uint32_t ctr_inc_bits = 128;
    uint8_t dest[BUFFLEN] = {0};
//    p_dest = (uint8_t *)malloc(sizeof(dest));

    sgx_status_t ret = sgx_aes_ctr_encrypt(p_key, p_src, src_len, dest, ctr_inc_bits, dest + SGX_AES_IV_SIZE);
    if (ret != SGX_SUCCESS) {
        return -1;
    }
    memcpy(p_dest, dest, src_len + ADD_ENC_DATA_SIZE);

    return 0;
}

int decrypt_aes(sgx_aes_ctr_128bit_key_t *p_key, const uint8_t *p_src, const int src_len, uint8_t *p_dest)
{
    const uint32_t ctr_inc_bits = 128;
    uint8_t *indata = (uint8_t *) malloc(sizeof(uint8_t));
//    uint8_t *p_ctr = (uint8_t)"0x12345F"; // initialization vector
    memcpy(indata, p_src, src_len);

    sgx_aes_ctr_decrypt(p_key, indata + SGX_AES_IV_SIZE, src_len, indata, ctr_inc_bits, p_dest);

    return 0;
}

int random_item(int size)
{
    uint32_t rand;
    sgx_read_rand((unsigned char *)&rand, 4);
    return rand%size;
}