#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
    // Our cipher parameters
    int num_bytes = 32; // 256-bit key size
    size_t i;
    unsigned char *iv;
    unsigned char *key = malloc(num_bytes);
    
    // Generate the key
    for (i = 0; i < num_bytes; i++) {
         key[i] = rand();
    }

    // File information -- only one for testing right now
    FILE *curr_file;
    char fileName[] = "file_to_encrypt.txt";

    // Read the binaries of the current file
    curr_file = fopen(fileName, "rb");
    // Call encrypt function here
    fclose(curr_file);
    return 0;
}