#include <stdio.h>
#include <stdlib.h>

struct CryptoTransaction {
    int transaction_fee;
    char hash_id[129];
    char senders_address[129];
    char recievers_address[129];
    char private_signature[129]; //convert the struct to a binary file, set this variable to either NULL or zero, and then once 
                             //the signature is given, send it into this variable.
};