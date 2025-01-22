#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
//#include <bitcoin>
//#include <bitches>
#define MAX_STRING_LENGTH 64

typedef enum {
    LEFT,
    RIGHT
} Direction;



typedef struct {
    char *hash;
    Direction direction;
} MerkleProof;

int fetch_transactions(char ***tr){
    int size_of_array=20;
    FILE *fptr = fopen("textfile","r");
    (*tr) = (char **) malloc((size_of_array + 1) * sizeof(char*));

    for(int i = 0; i <= size_of_array; i++){
        (*tr)[i] = (char*) malloc(64 * sizeof(char));
    }

    (*tr)[size_of_array]=NULL;

    char c;
    int index_trans = 0;
    int index_char = 0;
    
   
    while(1){
        c = fgetc(fptr);
        
        if(c==EOF){
            //break if the file is at the end
            for(int i = index_trans + 1; i < size_of_array; i++){
                free((*tr)[i]);
            }
            (*tr)[index_trans + 1] = NULL;

            return index_trans + 1;

        } else if(index_trans == size_of_array){

            //allocate more memory to the array if it is full:
            (*tr) = (char **) realloc((*tr),(size_of_array + 11) * sizeof(char*));

            for(int i = size_of_array; i < (size_of_array + 10); i++){
                
                (*tr)[i] = (char*) malloc(64 * sizeof(char));
            }
            
            (*tr)[size_of_array + 10]=NULL;

            size_of_array+=10;

            (*tr)[index_trans][index_char]=c;

            index_char++;


        } else if(c == '\n'){
            
            //new line means new transaction
            index_trans++;
            index_char = 0;
            
        } else {
            //read a whole transaction
            (*tr)[index_trans][index_char]=c;
            index_char++;
        }
    }

    

}

void HashAndCombineHashes(char ***hashes, int *size){
    char *result = (char*) malloc((2 * MAX_STRING_LENGTH + 1) * sizeof(char));
    unsigned char *mdrestemp = (char*) malloc((MAX_STRING_LENGTH + 1) * sizeof(char));
    unsigned char *mdres = (char*) malloc((MAX_STRING_LENGTH + 1) * sizeof(char));
    
    
    for(int i = 0; i < *size; i+=2){
        
        strcpy(result, (*hashes)[i]);
        strcat(result, (*hashes)[i + 1]);
        
        result[2 * MAX_STRING_LENGTH] = '\0';
        

        size_t array_len = strlen(result);
        unsigned char md[SHA256_DIGEST_LENGTH];

        strcpy(mdres,"");
        strcpy(mdrestemp,"");

        SHA256_CTX c;
        SHA256_Init(&c);
        SHA256_Update(&c, (const void*) result, array_len);
        SHA256_Final(md, &c);
       
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        
        sprintf(mdrestemp,"%02x",md[i]);
        strcat(mdres,mdrestemp);
    }

    strcpy((*hashes)[i/2],mdres);

    }

    for(int i = *size/2; i < *size; i++){
        free((*hashes)[i]);
    }
    //ONLY SET NULL AFTER YOU HAVE FREED MEMORY, OTHERWISE MEMORY LEAKS WILL HAPPEN.
    (*hashes)[*size/2] = NULL;

    *size = *size/2;
}

void ensureEven(char ***hashes, int *size){
    if(*size == 1){

    }else if((*size) % 2 != 0){
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
    HashAndCombineHashes(hashes,size);
    generateMerkleRoot(hashes, size);
}

int generateMerkleTree(char ****tree, int *size, char ***trans){
    int tree_level = 0;
    int tree_cap = 20;
    (*tree) = (char ***) malloc((tree_cap + 1)* sizeof(char**));
    (*tree)[tree_cap] = NULL;

    if((*trans)[0] == NULL){
        return -1;
    }
    
    while(1){
        

        ensureEven(trans, size);

        (*tree)[tree_level] = (char**) malloc((*size + 1) * sizeof(char*));
        (*tree)[tree_level][*size] = NULL;

       

        for(int i = 0; i < *size; i++){

            (*tree)[tree_level][i] = (char *) malloc((MAX_STRING_LENGTH) * sizeof(char));
            


            for(int j = 0; j < 64; j++){
                
                (*tree)[tree_level][i][j] = (*trans)[i][j];
               
            }

           
        }


        if(!(*trans) || *size == 1){

            return tree_level;
        }
        HashAndCombineHashes(trans, size);
        tree_level++;


    }
}

Direction getLeafNodeDirectionInMerkleTree(char *hash, char ****merkleTree, int *size){
    for(int i = 0; i < *size; i++){
        if(!memcmp(hash,(*merkleTree)[0][i],64)){
            int hashIndex = i;
            return hashIndex % 2 == 0 ? LEFT : RIGHT;
            }
        }

    }
     
    

void generateMerkleProof(char ****tree, int tree_levels, char *hash, MerkleProof **m_p,int *size){
    int hashIndex;
    memcpy((*m_p)[0].hash,hash,64);
    (*m_p)[0].direction = getLeafNodeDirectionInMerkleTree(hash,tree,size);
    
    for(int i = 0; i < *size; i++){
        if(!memcmp(hash,(*tree)[0][i],64)){
            hashIndex = i;
        }
    }

    

    
    for(int level = 0; level < tree_levels; level++) {
        int isLeftChild = hashIndex % 2 == 0;
        int siblingDirection = isLeftChild ? RIGHT : LEFT;
        int siblingIndex = isLeftChild ? hashIndex + 1 : hashIndex - 1;

        memcpy((*m_p)[level + 1].hash,(*tree)[level][siblingIndex],64);
        
        (*m_p)[level + 1].direction = siblingDirection;
        hashIndex = hashIndex / 2;
    }


}

void HashAndCombineIndividualHashes(char **res, char **hash, char **hash2){
    char *result = (char*) malloc((2 * MAX_STRING_LENGTH + 1) * sizeof(char));
    unsigned char *mdrestemp = (char*) malloc((MAX_STRING_LENGTH + 1) * sizeof(char));
    unsigned char *mdres = (char*) malloc((MAX_STRING_LENGTH + 1) * sizeof(char));

        strcpy(result, (*hash));
        strcat(result, (*hash2));
        
        result[2 * MAX_STRING_LENGTH] = '\0';
        
        size_t array_len = strlen(result);
        unsigned char md[SHA256_DIGEST_LENGTH];

        strcpy(mdres,"");
        strcpy(mdrestemp,"");

        SHA256_CTX c;
        SHA256_Init(&c);
        SHA256_Update(&c, (const void*) result, array_len);
        SHA256_Final(md, &c);
       
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        
        sprintf(mdrestemp,"%02x",md[i]);
        strcat(mdres,mdrestemp);
    }
    
    for(int i = 0; i < 64; i++){
        (*res)[i] = mdres[i];
        
    }
}

void getMerkleRootFromMerkleProof(MerkleProof **merkproof, int size, char *res_ret){

    char *hash1 = (char *) malloc(MAX_STRING_LENGTH * sizeof(char));
    char *hash2 = (char *) malloc(MAX_STRING_LENGTH * sizeof(char));
    char *res = (char *) malloc(MAX_STRING_LENGTH * sizeof(char));
    

    memcpy(hash1,(*merkproof)[0].hash,64);
   
    for(int i = 1; i < size + 1; i++){
        memcpy(hash2,(*merkproof)[i].hash,64);
        if((*merkproof)[i].direction == RIGHT){
            HashAndCombineIndividualHashes(&res, &hash1,&hash2);

            memcpy(hash1,res,64);
        } else {
        HashAndCombineIndividualHashes(&res, &hash2,&hash1);
        memcpy(hash1,res,64);
        
        }
    }

    memcpy(res_ret, res,64);
}

int main(){

    
    char **transactions;
    char **old_transactions;
    int size = fetch_transactions(&transactions);
    int oldoldsize = fetch_transactions(&old_transactions);
    int oldsize = size;
    int tree_levels;

   

    //ensureEven(&transactions,&size);
    

    //HashAndCombineHashes(&transactions,&size);
   
    //generateMerkleRoot(&transactions, &size);

    char ***merkletree;

    tree_levels = generateMerkleTree(&merkletree,&size,&transactions);

    //getLeafNodeDirectionInMerkleTree(merkletree[0][1],&merkletree,&size);

    MerkleProof *merkproof = (MerkleProof *) malloc((tree_levels + 1) * sizeof(MerkleProof));
    
    for(int i = 0; i < tree_levels + 1; i++){
        merkproof[i].hash = (char *) malloc(MAX_STRING_LENGTH * sizeof(char));
    }

    generateMerkleProof(&merkletree, tree_levels, merkletree[0][4], &merkproof, &oldsize);

    char *res = (char *) malloc(MAX_STRING_LENGTH * sizeof(char));

    getMerkleRootFromMerkleProof(&merkproof, tree_levels, res);

    generateMerkleRoot(&old_transactions,&oldoldsize);

    if(!memcmp(res,old_transactions[0],64)){
        printf("WE ARE DONE\n");
    }

    return 0;
}
