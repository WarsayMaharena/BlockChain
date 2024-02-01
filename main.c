#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RSA-Algorithm.h"

int main () {
    while(1){
        char str1[20];
        printf("\n\nprint your argument, your options are...\n\n");
         printf("1. Generate_Keys: a functon that generates keys\n");
         printf("2. Quit: quit the application\n\n");
         printf("Enter Argument: ");
        scanf("%19s", str1);
        
        if(!strcmp("Generate_Keys",str1)){
            printf("\nyou decided to generate keys");
            generate_keys();
        }

        else if(!strcmp("Quit",str1)){
            break;
        }

        else{
            printf("\nGive a valid command");
            
        }
    }


return 0;
}