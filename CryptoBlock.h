#include<string.h>
#include "CryptoTransaction.h"

struct CryproBlock{
    char *header;
    char *prevBlockHash;
    CryptoTransaction merkleroot;

};