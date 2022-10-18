#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
    // Our cipher parameters
    unsigned char *iv;
    unsigned char *key;

    // File information -- only one for testing right now
    FILE *curr_file;
    char fileName[] = "file_to_encrypt.txt";
    
    // Read the binaries of the current file
    curr_file = fopen(fileName, "rb");
}