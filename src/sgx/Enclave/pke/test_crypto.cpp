#include "sgx_tcrypto.h"
#include "Enclave_t.h"
#include <cstring>
#include <cstdio>
#include <string>
#include <string.h>

int test_key_create(void *pkey, void *skey)
{
    unsigned char p_n[256];
    unsigned char p_d[256];
    unsigned char p_p[256];
    unsigned char p_q[256];
    unsigned char p_dmp1[256];
    unsigned char p_dmq1[256];
    unsigned char p_iqmp[256];

    int n_byte_size = 256;
    long e = 65537;

    int ret = 0; // return value

    sgx_status_t ret_create_key_params = sgx_create_rsa_key_pair(n_byte_size, sizeof(e), p_n, p_d, (unsigned char *)&e, p_p, p_q, p_dmp1, p_dmq1, p_iqmp);

    if (ret_create_key_params != SGX_SUCCESS)
    {
        ocall_print("Key param generation failed");
        ocall_print(std::to_string(ret_create_key_params).c_str());
        ret = -1;
    }
    else
    {
        // ocall_print((char *)p_q);
    }

    void *private_key = NULL;
    //unsigned char private_key[256];

    sgx_status_t ret_create_private_key = sgx_create_rsa_priv2_key(n_byte_size, sizeof(e), (unsigned char *)&e, p_p, p_q, p_dmp1, p_dmq1, p_iqmp, &private_key);

    if (ret_create_private_key != SGX_SUCCESS)
    {
        ocall_print("Private key generation failed");
        ocall_print(std::to_string(ret_create_private_key).c_str());
        ret = -1;
    } 

    void *public_key = NULL;
    //unsigned char public_key_char[256];

    sgx_status_t ret_create_public_key = sgx_create_rsa_pub1_key(n_byte_size, sizeof(e), p_n, (unsigned char *)&e, &public_key);

    if (ret_create_public_key != SGX_SUCCESS)
    {
        ocall_print("Public key generation failed");
        ocall_print(std::to_string(ret_create_public_key).c_str());
        ret = -1;
    }
    else {
        /*
        ocall_print("Public key size: ");
        ocall_print(std::to_string(sizeof(public_key)).c_str());
        ocall_print((char *)public_key);
        ocall_print("Private key size: ");
        ocall_print(std::to_string(sizeof(private_key)).c_str());
        ocall_print((char *)private_key);
        */
    }

    // copy the generated keys to given augments.
    pkey = (void *) malloc (256);
    skey = (void *) malloc (256);
    memcpy(pkey, public_key, 256);
    memcpy(skey, private_key, 256);

    // check copied key is empty or not
    // pkey is not NULL here.
    if (pkey != NULL)
    {
        ocall_print("pkey is:");
        ocall_print((char *)pkey);
    } else {
        ocall_print("pkey is NULL");
    }


    return ret;

}

int test_encrypt(void *pkey, unsigned char *outData)
{
    char *pin_data = "Hello World!";
    size_t out_len = 0;

    sgx_status_t ret_get_output_len = sgx_rsa_pub_encrypt_sha256(pkey, NULL, &out_len, (unsigned char *)pin_data, strlen(pin_data));



    if (ret_get_output_len != SGX_SUCCESS)
    {
        ocall_print("Determination of output length failed");
        ocall_print(std::to_string(ret_get_output_len).c_str());

        // Error analysis
        if (ret_get_output_len == SGX_ERROR_INVALID_PARAMETER) {
            ocall_print("encrypt: Invalid Parameter");
        } else if (ret_get_output_len == SGX_ERROR_OUT_OF_MEMORY) {
            ocall_print("encrypt: Out of Memory");
        } else if (ret_get_output_len == SGX_ERROR_UNEXPECTED) {
            ocall_print("encrypt: Unexpected Error");
        }
    }

    unsigned char pout_data[out_len];

    sgx_status_t ret_encrypt = sgx_rsa_pub_encrypt_sha256(pkey, pout_data, &out_len, (unsigned char *)pin_data, strlen(pin_data));

    if (ret_encrypt != SGX_SUCCESS)
    {
        ocall_print("Encryption failed");
        ocall_print(std::to_string(ret_encrypt).c_str());

        // Error analysis
        if (ret_encrypt == SGX_ERROR_INVALID_PARAMETER) {
            ocall_print("encrypt: Invalid Parameter");
        } else if (ret_encrypt == SGX_ERROR_OUT_OF_MEMORY) {
            ocall_print("encrypt: Out of Memory");
        } else if (ret_encrypt == SGX_ERROR_UNEXPECTED) {
            ocall_print("encrypt: Unexpected Error");
        }
    }
    else
    {
        ocall_print(std::to_string(out_len).c_str());
    }
    outData = (unsigned char *) malloc(out_len);

    memcpy(outData, pout_data, out_len);

    return 0;
}

int test_decrypt(void *skey, unsigned char *ciphertext)
{
    size_t decrypted_out_len = 0;

    sgx_status_t ret_determine_decrypt_len = sgx_rsa_priv_decrypt_sha256(skey, NULL, &decrypted_out_len, ciphertext, sizeof(ciphertext));

    if (ret_determine_decrypt_len != SGX_SUCCESS)
    {
        ocall_print("Determination of decrypted output length failed");
        ocall_print(std::to_string(ret_determine_decrypt_len).c_str());
    }

    unsigned char decrypted_pout_data[decrypted_out_len];

    sgx_status_t ret_decrypt = sgx_rsa_priv_decrypt_sha256(skey, decrypted_pout_data, &decrypted_out_len, ciphertext, sizeof(ciphertext));

    if (ret_decrypt != SGX_SUCCESS)
    {
        ocall_print("Decryption failed");
        ocall_print(std::to_string(ret_decrypt).c_str());
    }
    else
    {
        ocall_print("Decrypted MESSAGE:");
        ocall_print((char *)decrypted_pout_data);
        ocall_print(std::to_string(decrypted_out_len).c_str());
    }
}

void test_crypto_with_keys(void *public_key, void *private_key)
{
    /*--- Encryption ---*/
    char *pin_data = "Hello World!";
    size_t out_len = 0;

    sgx_status_t ret_get_output_len = sgx_rsa_pub_encrypt_sha256(public_key, NULL, &out_len, (unsigned char *)pin_data, strlen(pin_data));

    if (ret_get_output_len != SGX_SUCCESS)
    {
        ocall_print("Determination of output length failed");
        ocall_print(std::to_string(ret_get_output_len).c_str());
    }

    unsigned char pout_data[out_len];

    sgx_status_t ret_encrypt = sgx_rsa_pub_encrypt_sha256(public_key, pout_data, &out_len, (unsigned char *)pin_data, strlen(pin_data));

    if (ret_encrypt != SGX_SUCCESS)
    {
        ocall_print("Encryption failed");
        ocall_print(std::to_string(ret_encrypt).c_str());
    }
    else
    {
        ocall_print(std::to_string(out_len).c_str());
    }

    /*--- Dencryption ---*/
    size_t decrypted_out_len = 0;

    sgx_status_t ret_determine_decrypt_len = sgx_rsa_priv_decrypt_sha256(private_key, NULL, &decrypted_out_len, pout_data, sizeof(pout_data));

    if (ret_determine_decrypt_len != SGX_SUCCESS)
    {
        ocall_print("Determination of decrypted output length failed");
        ocall_print(std::to_string(ret_determine_decrypt_len).c_str());
    }

    unsigned char decrypted_pout_data[decrypted_out_len];

    sgx_status_t ret_decrypt = sgx_rsa_priv_decrypt_sha256(private_key, decrypted_pout_data, &decrypted_out_len, pout_data, sizeof(pout_data));

    if (ret_decrypt != SGX_SUCCESS)
    {
        ocall_print("Decryption failed");
        ocall_print(std::to_string(ret_decrypt).c_str());
    }
    else
    {
        ocall_print("Decrypted MESSAGE:");
        ocall_print((char *)decrypted_pout_data);
        ocall_print(std::to_string(decrypted_out_len).c_str());
    }
}

void test_crypto()
{

    /*--- Key generation ---*/

    unsigned char p_n[256];
    unsigned char p_d[256];
    unsigned char p_p[256];
    unsigned char p_q[256];
    unsigned char p_dmp1[256];
    unsigned char p_dmq1[256];
    unsigned char p_iqmp[256];

    int n_byte_size = 256;
    long e = 65537;

    sgx_status_t ret_create_key_params = sgx_create_rsa_key_pair(n_byte_size, sizeof(e), p_n, p_d, (unsigned char *)&e, p_p, p_q, p_dmp1, p_dmq1, p_iqmp);

    if (ret_create_key_params != SGX_SUCCESS)
    {
        ocall_print("Key param generation failed");
        ocall_print(std::to_string(ret_create_key_params).c_str());
    }
    else
    {
        // ocall_print((char *)p_q);
    }

    void *private_key = NULL;
    //unsigned char private_key[256];

    sgx_status_t ret_create_private_key = sgx_create_rsa_priv2_key(n_byte_size, sizeof(e), (unsigned char *)&e, p_p, p_q, p_dmp1, p_dmq1, p_iqmp, &private_key);

    if (ret_create_private_key != SGX_SUCCESS)
    {
        ocall_print("Private key generation failed");
        ocall_print(std::to_string(ret_create_private_key).c_str());
    }

    void *public_key = NULL;
    //unsigned char public_key[256];

    sgx_status_t ret_create_public_key = sgx_create_rsa_pub1_key(n_byte_size, sizeof(e), p_n, (unsigned char *)&e, &public_key);

    if (ret_create_public_key != SGX_SUCCESS)
    {
        ocall_print("Public key generation failed");
        ocall_print(std::to_string(ret_create_public_key).c_str());
    } else {
        ocall_print("Public key:");
        ocall_print((char *)public_key);
        ocall_print("Private key:");
        ocall_print((char *)private_key);
    }


    /*--- Encryption ---*/
    char *pin_data = "Hello World!";
    size_t out_len = 0;

    sgx_status_t ret_get_output_len = sgx_rsa_pub_encrypt_sha256(public_key, NULL, &out_len, (unsigned char *)pin_data, strlen(pin_data));

    if (ret_get_output_len != SGX_SUCCESS)
    {
        ocall_print("Determination of output length failed");
        ocall_print(std::to_string(ret_get_output_len).c_str());
    }

    unsigned char pout_data[out_len];

    sgx_status_t ret_encrypt = sgx_rsa_pub_encrypt_sha256(public_key, pout_data, &out_len, (unsigned char *)pin_data, strlen(pin_data));

    if (ret_encrypt != SGX_SUCCESS)
    {
        ocall_print("Encryption failed");
        ocall_print(std::to_string(ret_encrypt).c_str());
    }
    else
    {
        ocall_print(std::to_string(out_len).c_str());
    }

    /*--- Decryption ---*/

    size_t decrypted_out_len = 0;

    sgx_status_t ret_determine_decrypt_len = sgx_rsa_priv_decrypt_sha256(private_key, NULL, &decrypted_out_len, pout_data, sizeof(pout_data));

    if (ret_determine_decrypt_len != SGX_SUCCESS)
    {
        ocall_print("Determination of decrypted output length failed");
        ocall_print(std::to_string(ret_determine_decrypt_len).c_str());
    }

    unsigned char decrypted_pout_data[decrypted_out_len];

    sgx_status_t ret_decrypt = sgx_rsa_priv_decrypt_sha256(private_key, decrypted_pout_data, &decrypted_out_len, pout_data, sizeof(pout_data));

    if (ret_decrypt != SGX_SUCCESS)
    {
        ocall_print("Decryption failed");
        ocall_print(std::to_string(ret_decrypt).c_str());
    }
    else
    {
        ocall_print("Decrypted MESSAGE:");
        ocall_print((char *)decrypted_pout_data);
        ocall_print(std::to_string(decrypted_out_len).c_str());
    }
}