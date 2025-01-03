#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
//#include <bitcoin>
//#include <bitches>

typedef enum {
    LEFT,
    RIGHT
} Direction;



typedef struct {
    char *hash;
    Direction direction;
} MerkleProof;

#define MAX_STRING_LENGTH 64

//compile: gcc transaction.c -o a -lssl -lcrypto

void ensureEven(char ***hashes, int *size);
void generateMerkleRoot(char ***hashes, int *size);
void generateMerkleTree(char ***hashes, int *size, char ****tree, int *sizeoftree);
void generate(int *sizeoftree, char ***hashes, int *size, char ****tree);
void HashAndCombineHashes(char ***hashes, int *size);
void createArrayOfStrings(FILE *file_ptr, char ***new_string, int *new_size);
void generateMerkleProof(char* hash, char ***hashes, int *size, MerkleProof **merkleproof);
Direction getLeafNodeDirectionMerkleTree(char *hash, char ****merkleTree, int *size, int *hashindex);


int main(){


    //Initialisera en double pointer för att hålla
    // en array av strängar
    char **new_string = NULL;

    int new_size = 0;

    char *hash = "41b637cfd9eb3e2f60f734f9ca44e5c1559c6f481d49d6ed6891f3e9a086ac78";

    FILE *fptr = fopen("textfile", "r");

    createArrayOfStrings(fptr,&new_string, &new_size);

    //generateMerkleRoot(&new_string, &new_size);
    char ***tree = (char***) malloc(10 * sizeof(char**));
    //generateMerkleTree(&new_string,&new_size,&tree);
    MerkleProof *merkleproof;
    generateMerkleProof(new_string[7],&new_string,&new_size,&merkleproof);

    printf("newstring size: %d\n", new_size);

    for (int i = 0; i < new_size; i++){
        printf("%s\n", new_string[i]);
    }
    
    return 0;
    
}


//Problem when there are empty lines at the end of a textfile, they count as the elements of the array just as the strings
//so the size will be bigger than it actually is.
void createArrayOfStrings(FILE *file_ptr, char ***new_String, int *new_size){
    char c;
    int row_counter = 0;
    int col_counter = 0;
    int curr_cap = 30;
    *new_String = (char**) malloc(curr_cap * sizeof(char*));
    (*new_String)[col_counter] = (char*) malloc((MAX_STRING_LENGTH+1) * sizeof(char));

    while(1){
        c = fgetc(file_ptr);
        if(c == EOF){
            *new_size = col_counter + 1;
            (*new_String) = realloc((*new_String), (*new_size) * sizeof(char*));
            break;
        }
        if (c != '\n'){
            (*new_String)[col_counter][row_counter] = c;
            row_counter++;
        } else{
            col_counter++;
            if(curr_cap == col_counter){
                curr_cap+=30;
                (*new_String) = (char**) realloc((*new_String), curr_cap * sizeof(char*));
            }
            (*new_String)[col_counter] = (char*) malloc((MAX_STRING_LENGTH+1) * sizeof(char));
            row_counter=0;
        }
        
    }
}

void ensureEven(char ***hashes, int *size){
    if((*size) % 2 != 0){
        *hashes = (char**)realloc(*hashes, sizeof(char*) * (*size+1));
        (*hashes)[*size] = (char *)malloc(sizeof(char)*MAX_STRING_LENGTH);
        strcpy((*hashes)[*size],(*hashes)[*size-1]);
        *size +=1;
    }
}

//***hashes (a pointer to an array of strings) 
//*size (the size of the array)
void generateMerkleRoot(char ***hashes, int *size){
    if(!(*hashes) || *size == 1){
        return;
    }



    ensureEven(hashes, size);
    for (int i = 0; i < *size; i++){
        //printf("%s\n", (*hashes)[i]);
    }

    HashAndCombineHashes(hashes,size);


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

void HashAndCombineHashes(char ***hashes, int *size){
    char *result = (char*) malloc(2 * (MAX_STRING_LENGTH + 2) * sizeof(char));
    unsigned char *mdrestemp = (char*) malloc((MAX_STRING_LENGTH + 2) * sizeof(char));
    unsigned char *mdres = (char*) malloc((MAX_STRING_LENGTH + 2) * sizeof(char));
    
    //printf("----------------------------------\n");
    for(int i = 0; i < *size; i+=2){
        
        strcpy(result, (*hashes)[i]);
        strcat(result, (*hashes)[i + 1]);
        
        //printf("Res: %s\n",result);

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

    //printf("MDRES: %s\n",mdres);
    strcpy((*hashes)[i/2],mdres);
    //strcpy(mdres,"");
    //printf("\n");
        
    }
}



void generateMerkleTree(char ***hashes, int *size, char ****tree, int *sizeoftree){
    if(!(*hashes) || *size == 1){
        return;
    }


    
    
    for(int i = 0; i < 10; i++){
        (*tree)[i] = (char**) malloc(*size * sizeof(char*));
        for(int j = 0; j < 21; j++){
            (*tree)[i][j] = (char*) malloc(MAX_STRING_LENGTH * sizeof(char));
        }
    }

    
    (*tree)[0] = *hashes;
    generate(sizeoftree, hashes,size,tree);
}

void generate(int *sizeoftree, char ***hashes, int *size, char ****tree){

    int curr_cap = 10;
    while(1){

        //resize if tree isnt big enough
        if(*sizeoftree == curr_cap){
            *sizeoftree+=10;
            *tree = (char***) realloc(*tree,*sizeoftree * sizeof(char**));
            
            for(int i = *sizeoftree; i < *sizeoftree; i++){
                (*tree)[i] = (char**) malloc(*size * sizeof(char*));
                for(int j = 0; j < 21; j++){
                    tree[i][j] = (char**) malloc(MAX_STRING_LENGTH * sizeof(char*));
                }
            }
        }

        if(*size == 1){

            break;
        }
        ensureEven(hashes, size);
        for (int i = 0; i < *size; i++){
            //printf("%s\n", (*hashes)[i]);
        }
        //printf("\n");
        HashAndCombineHashes(hashes,size);

        //free unused space
        for (int i = *size/2; i<*size; i++){
            free((*hashes)[i]);
        }

        *size = (*size)/2;
        
        //REMEMBER PARENTHESIS WHEN DEREFERENCING AND THEN INDEXING
        (*tree)[*sizeoftree] = *hashes;
        
        *sizeoftree+=1;
        printf("sizeoftree: %d\n",*sizeoftree);
        //printf("tree existss %s\n",(*tree)[5][0]);
    }

}

Direction getLeafNodeDirectionMerkleTree(char *hash, char ****merkleTree, int *size, int *hashindex){
    int index;
    for(int i = 0; i < *size; i++){
        if((*merkleTree)[0][i] == hash){
            //printf("truesssssssssss?\n");
            index=i;
            *hashindex = i;
        }
    }
    return index%2 == 0 ? LEFT : RIGHT;

}

void generateMerkleProof(char* hash, char ***hashes, int *size, MerkleProof **merkleproof){
    if(!*(hashes) || *size==0){
        return;
    }
    int sizeoftree = 1;
    int hashindex;
    int oldsize = *size;
    char ***tree = (char***) malloc(10 * sizeof(char**));
    generateMerkleTree(hashes,size,&tree, &sizeoftree);
    int direction = getLeafNodeDirectionMerkleTree(hash,&tree,&oldsize, &hashindex);
    //printf("directionsssss: %d\n", direction);
    (*merkleproof) = (MerkleProof*) malloc(oldsize * sizeof(MerkleProof));
    (*merkleproof)->hash = (char*) malloc(64 * sizeof(char));
    (*merkleproof)[0].hash = hash;
    (*merkleproof)[0].direction = direction;
    printf("valueinhere: %d\n",sizeoftree);
    for(int level = 0; level < sizeoftree - 1; level++){
        int isLeftChild = hashindex % 2 == 0;
        int siblingDirection = isLeftChild ? RIGHT : LEFT;
        int siblingIndex = isLeftChild ? hashindex + 1 : hashindex - 1;
        (*merkleproof)[level+1].hash = (*tree)[level][siblingIndex];
        (*merkleproof)[level+1].direction = siblingDirection;
        hashindex = hashindex/2; 
    }
    

}