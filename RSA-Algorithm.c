#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <stdio.h>

void generate_keys(){

    //Initializations of BIGNUMS
    RSA *rsa = RSA_new();
    int bits=4096;
    BIGNUM *e = BN_new();
    BN_set_word(e, 65537);

    //Key Pair Generating
    RSA_generate_key_ex(rsa, bits, e, NULL);

    //Writing to .pem file
    BIO *bp_public =NULL, *bp_private=NULL;

    //save public key
	bp_public = BIO_new_file("public.pem", "w+");
	PEM_write_bio_RSAPublicKey(bp_public, rsa);

    //save private key
    bp_private = BIO_new_file("private.pem", "w+");
	PEM_write_bio_RSAPrivateKey(bp_private, rsa, NULL, NULL, 0, NULL, NULL);

    
    RSA_free(rsa);
    BIO_free(bp_public);
    BIO_free(bp_private);

}

int main(){
    generate_keys();
    return 0;

}