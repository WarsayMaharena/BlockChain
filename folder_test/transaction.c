#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#define MAX_STRING_LENGTH 64

enum{LEFT, RIGHT};

//compile: gcc transaction.c -o a -lssl -lcrypto

void ensureEven(char ***hashes, int *size);
void generateMerkleRoot(char ***hashes, int *size);

int main(){
    
    char hashes[21][64] = {
    "95cd603fe577fa9548ec0c9b50b067566fe07c8af6acba45f6196f3a15d511f6",
    "709b55bd3da0f5a838125bd0ee20c5bfdd7caba173912d4281cae816b79a201b",
    "27ca64c092a959c7edc525ed45e845b1de6a7590d173fd2fad9133c8a779a1e3",
    "1f3cb18e896256d7d6bb8c11a6ec71f005c75de05e39beae5d93bbd1e2c8b7a9",
    "41b637cfd9eb3e2f60f734f9ca44e5c1559c6f481d49d6ed6891f3e9a086ac78",
    "a8c0cce8bb067e91cf2766c26be4e5d7cfba3d3323dc19d08a834391a1ce5acf",
    "d20a624740ce1b7e2c74659bb291f665c021d202be02d13ce27feb067eeec837",
    "281b9dba10658c86d0c3c267b82b8972b6c7b41285f60ce2054211e69dd89e15",
    "df743dd1973e1c7d46968720b931af0afa8ec5e8412f9420006b7b4fa660ba8d",
    "3e812f40cd8e4ca3a92972610409922dedf1c0dbc68394fcb1c8f188a42655e2",
    "3ebc2bd1d73e4f2f1f2af086ad724c98c8030f74c0c2be6c2d6fd538c711f35c",
    "9789f4e2339193149452c1a42cded34f7a301a13196cd8200246af7cc1e33c3b",
    "aefe99f12345aabc4aa2f000181008843c8abf57ccf394710b2c48ed38e1a66a",
    "64f662d104723a4326096ffd92954e24f2bf5c3ad374f04b10fcc735bc901a4d",
    "95a73895c9c6ee0fadb8d7da2fac25eb523fc582dc12c40ec793f0c1a70893b4",
    "315987563da5a1f3967053d445f73107ed6388270b00fb99a9aaa26c56ecba2b",
    "09caa1de14f86c5c19bf53cadc4206fd872a7bf71cda9814b590eb8c6e706fbb",
    "9d04d59d713b607c81811230645ce40afae2297f1cdc1216c45080a5c2e86a5a",
    "ab8a58ff2cf9131f9730d94b9d67f087f5d91aebc3c032b6c5b7b810c47e0132",
    "c7c3f15b67d59190a6bbe5d98d058270aee86fe1468c73e00a4e7dcc7efcd3a0",
    "27ef2eaa77544d2dd325ce93299fcddef0fae77ae72f510361fa6e5d831610b2"
};

    //Initialisera en double pointer för att hålla
    // en array av strängar
    char **strings = NULL;

    //Deklarera en initial storlek för den dynamiska arrayen
    int size = 21;

    //Allokera minne för arrayen av strängar
    strings = (char**) malloc(size * sizeof(char*));
    if(strings==NULL){
        fprintf(stderr, "Memory allcation failed\n");
        return 1;
    }

    // allokera minne för varje sträng 
    for (int i = 0; i<size; i++){
        strings[i] = (char*)malloc((MAX_STRING_LENGTH + 1)
            *   sizeof(char));

            if(strings[i] == NULL){
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }

            //Assign values to each string
            snprintf(strings[i], 65, hashes[i], i);
    }

    //ensureEven(&strings, &size);
    generateMerkleRoot(&strings, &size);


    /*for (int i = 0; i < size; i++){
        printf("%s\n", strings[i]);
    }*/

    return 0;

}

void ensureEven(char ***hashes, int *size){
    if((*size) % 2 != 0){
        *hashes = (char**)realloc(*hashes, sizeof(char*) * (*size+1));
        (*hashes)[*size] = (char *)malloc(sizeof(char)*MAX_STRING_LENGTH);
        strcpy((*hashes)[*size],(*hashes)[*size-1]);
        *size +=1;
    }
}

void generateMerkleRoot(char ***hashes, int *size){
    if(!(*hashes) || *size == 1){
        return;
    }



    ensureEven(hashes, size);
    for (int i = 0; i < *size; i++){
        //printf("%s\n", (*hashes)[i]);
    }
    char *result = (char*) malloc(2 * (MAX_STRING_LENGTH + 2) * sizeof(char));
    unsigned char *mdrestemp = (char*) malloc((MAX_STRING_LENGTH + 2) * sizeof(char));
    unsigned char *mdres = (char*) malloc((MAX_STRING_LENGTH + 2) * sizeof(char));
    
    printf("----------------------------------\n");
    for(int i = 0; i < *size; i+=2){
        
        strcpy(result, (*hashes)[i]);
        strcat(result, (*hashes)[i + 1]);

        //printf("HASH 1:%s\nHASH 2:%s\n",(*hashes)[i],(*hashes)[i+1]);
        
        printf("Res: %s\n",result);

        size_t array_len = strlen(result);
        unsigned char md[SHA256_DIGEST_LENGTH];
        //mdres='\0';
        //mdrestemp='\0';
        strcpy(mdres,"");
        strcpy(mdrestemp,"");

            SHA256_CTX c;
            SHA256_Init(&c);
            SHA256_Update(&c, (const void*) result, array_len);
            SHA256_Final(md, &c);
            //printf("MD: ");
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            //printf("%02x", md[i]);
            sprintf(mdrestemp,"%02x",md[i]);
            strcat(mdres,mdrestemp);
    }

    printf("MDRES: %s\n",mdres);
    i == 0 ? strcpy((*hashes)[i],mdres) : strcpy((*hashes)[i/2],mdres);
    strcpy(mdres,"");
    //printf("\n");
        
    }

    for (int i = 0; i < *size; i++){
        //printf("%s\n", (*hashes)[i]);
    }

    for (int i = *size/2; i<*size; i++){
        free((*hashes)[i]);
    }
    printf("\n_________\n");

    *size = *size/2;

    for (int i = 0; i < *size; i++){
        printf("%s\n", (*hashes)[i]);
    }

    generateMerkleRoot(hashes, size);
}


int check_size(){

}