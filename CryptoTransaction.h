#include <stdio.h>
#include <stdlib.h>

struct CryptoTransaction {
    int transaction_fee;
    char *hash_id;
    char *senders_address;
    char *recievers_address;
    char *private_signature; //convert the struct to a binary file, set this variable to either NULL or zero, and then once 
                             //the signature is given, send it into this variable.
};