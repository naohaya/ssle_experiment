#include "crypto.h"

#define BUFLEN 4 * 1024 * 1024 //1MB: increase this if you have to encrypt larger buffers
#define SGX_AESGCM_MAC_SIZE 16   //128 bit mac
#define SGX_AESGCM_IV_SIZE 12    // 96 bit iv
#define CIPHERTEXT_SIZE 36
#define ADD_ENC_DATA_SIZE (SGX_AESGCM_MAC_SIZE + SGX_AESGCM_IV_SIZE)

#define ENABLE_CRYPTO
static sgx_aes_gcm_128bit_key_t gcmkey = {0x76, 0x39, 0x79, 0x24, 0x42, 0x26, 0x45, 0x28, 0x48, 0x2b, 0x4d, 0x3b, 0x62, 0x51, 0x5e, 0x8f};
sgx_aes_ctr_128bit_key_t ctrkey;

sgx_aes_ctr_128bit_key_t *create_aes_key()
{
    sgx_aes_ctr_128bit_key_t *newkey = (sgx_aes_ctr_128bit_key_t *)malloc(sizeof(sgx_aes_ctr_128bit_key_t));
    for (int i = 0; i < 16; i++)
    {
        ctrkey[i] = random_item(256);
    }
    memcpy(newkey, ctrkey, sizeof(sgx_aes_ctr_128bit_key_t));

    return newkey;
}

int encrypt_aes(void *dataIn, size_t len, char *dataOut, size_t lenOut)
{
    uint8_t *clairText = (uint8_t *)dataIn;
    uint8_t p_dst[BUFLEN] = {0};
    const uint32_t num_inc_bits = 128;

    // encryption
    /*
     If the IV/nonce is random, it can be combined together with the counter (XOR, addition etc)
    to produce the actual unique counter block for encryption */

    sgx_read_rand(p_dst + SGX_AESGCM_MAC_SIZE, SGX_AESGCM_IV_SIZE);
    ocall_print("hoge");
    sgx_rijndael128GCM_encrypt(
        &gcmkey,
        clairText, len,
        p_dst + SGX_AESGCM_MAC_SIZE + SGX_AESGCM_IV_SIZE,
        p_dst + SGX_AESGCM_MAC_SIZE, SGX_AESGCM_IV_SIZE,
        NULL, 0,
        (sgx_aes_gcm_128bit_tag_t *)(p_dst));
    // dataOut[lenOut] = '\0'; //Terminate encrypted data
    memcpy(dataOut, p_dst, lenOut);
}

int encrypt_aes_ctr(sgx_aes_ctr_128bit_key_t *p_key, const uint8_t *p_src, const int src_len, uint8_t *p_dest)
{
    //    uint8_t *p_ctr = (uint8_t)"0x12345F"; // initialization vector
    const uint32_t ctr_inc_bits = 128;
    uint8_t dest[BUFLEN] = {0};
    //    p_dest = (uint8_t *)malloc(sizeof(dest));

    sgx_status_t ret = sgx_aes_ctr_encrypt(p_key, p_src, src_len, dest, ctr_inc_bits, dest + SGX_AES_IV_SIZE);
    if (ret != SGX_SUCCESS)
    {
        return -1;
    }
    memcpy(p_dest, dest, src_len + ADD_ENC_DATA_SIZE);

    return 0;
}

int decrypt_aes(char *dataIn, size_t len, void *dataOut, size_t lenOut)
{
    uint8_t *cipherText = (uint8_t *)dataIn;
    uint8_t p_dst[BUFLEN] = {0};
    const uint32_t num_inc_bits = 128;

    // decryption
    sgx_rijndael128GCM_decrypt(
        &gcmkey,
        cipherText + SGX_AESGCM_MAC_SIZE + SGX_AESGCM_IV_SIZE,
        lenOut,
        p_dst,
        cipherText + SGX_AESGCM_MAC_SIZE, SGX_AESGCM_IV_SIZE,
        NULL, 0,
        (sgx_aes_gcm_128bit_tag_t *)cipherText);
    // dataOut[lenOut] = '\0';
    memcpy(dataOut, p_dst, lenOut);
}

int decrypt_aes_ctr(sgx_aes_ctr_128bit_key_t *p_key, const uint8_t *p_src, const int src_len, uint8_t *p_dest)
{
    const uint32_t ctr_inc_bits = 128;
    uint8_t *indata = (uint8_t *)malloc(sizeof(uint8_t));
    //    uint8_t *p_ctr = (uint8_t)"0x12345F"; // initialization vector
    memcpy(indata, p_src, src_len);

    sgx_aes_ctr_decrypt(p_key, indata + SGX_AES_IV_SIZE, src_len, indata, ctr_inc_bits, p_dest);

    return 0;
}

int random_item(int size)
{
    uint32_t rand;
    sgx_read_rand((unsigned char *)&rand, 4);
    return rand % size;
}