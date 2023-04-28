#include "crypto.h"

sgx_aes_ctr_128bit_key_t key;

sgx_aes_ctr_128bit_key_t * create_aes_key()
{
    for(int i=0; i<16; i++)
    {
        key[i] = random_item(256);
    }
//    memcpy(newkey, key, sizeof(sgx_aes_ctr_128bit_key_t));

    return key;
    
}

void encrypt_aes(sgx_aes_ctr_128bit_key_t *p_key, const uint8_t *p_src, const int src_len, uint8_t *p_dest)
{
    uint8_t *p_ctr = "0x12345F"; // initialization vector
    const uint32_t ctr_inc_bits = 128;
    uint8_t dest[BUFFLEN] = {0};
//    p_dest = (uint8_t *)malloc(sizeof(dest));

    sgx_aes_ctr_encrypt(p_key, p_src, src_len, p_dest, ctr_inc_bits, p_dest + SGX_AES_IV_SIZE);


}

void decrypt_aes(sgx_aes_ctr_128bit_key_t *p_key, const uint8_t *p_src, const int src_len, uint8_t *p_dest)
{
    const uint32_t ctr_inc_bits = 128;
    uint8_t *p_ctr = "0x12345F"; // initialization vector

    sgx_aes_ctr_decrypt(p_key, p_src, src_len, p_ctr, ctr_inc_bits, p_dst);


}

int random_item(int size)
{
    uint32_t rand;
    sgx_read_rand((unsigned char *)&rand, 4);
    return rand%size;
}