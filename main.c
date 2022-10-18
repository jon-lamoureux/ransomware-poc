typedef struct _cipher_params {
    unsigned char *iv;
    unsigned char *key;
} cipher_params;

int main () {
    cipher_params *params = (cipher_params *)malloc(sizeof(cipher_params));
    params->key = 1;
    params->iv = 2;
}