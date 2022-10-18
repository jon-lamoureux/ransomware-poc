#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h> // use openssl evp for encryption
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

void encrypt(unsigned char *key, unsigned char *iv) {
    // AES Encryption parameters
    int block_size = 16; // AES uses 128-bit block size encryption
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new(); // New ctx cipher

    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32); // 256-bit AES key is 32 bytes
    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == block_size); // Block size is 16
    
    // File information -- only one for testing right now
    FILE *curr_file;
    char fileName[] = "file_to_encrypt.txt";

    // Read the binaries of the current file
    curr_file = fopen(fileName, "rb");

    // Close the file when encryption is complete
    fclose(curr_file);
}

int main () {
    // Our cipher parameters
    int num_bytes = 32; // 256-bit key size
    unsigned char *iv = malloc(16); // IV is always 16 bytes
    unsigned char *key = malloc(num_bytes);
    
    // Generate the key
    size_t i;
    for (i = 0; i < num_bytes; i++) {
         key[i] = rand();
    }
    
    // Generate the IV
    for (i = 0; i < 16; i++) {
         iv[i] = rand();
    }

    // Call the encryption function to encrypt the files
    encrypt(key, iv);
    
    // We don't like our keys being stored in the memory
    free(key);
    free(iv);
    return 0;
}