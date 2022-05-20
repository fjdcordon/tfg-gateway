#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void menu(){
    printf("\n********** Siemens PLC Driver **********\n");
    printf("   usage: ./driver <action> [<value>]\n\n");
    printf("             -- Actions: --\n");
    printf("          on: Start the system\n");
    printf("          off: Stop the system\n");
    printf(" move <value>: Change system inclination\n");
    printf("   energy: Returns power in generation\n");
    printf("      status: Returns system status\n");
    printf("    angle: Returns system inclination\n");
    printf("       reset: Reset system memory\n\n");
}

int main(int argc, char *argv[]){

    if (argc<2 || argc>3){
        menu();
        exit(0);
    }

    if (!strcmp(argv[1],"on")){
        printf("ok");
    } else if (!strcmp(argv[1],"off")){
        printf("ok");
    } else if (!strcmp(argv[1],"move") && argc==3){
        printf("ok");
    } else if (!strcmp(argv[1],"energy")){
        printf("487.55");
    } else if (!strcmp(argv[1],"status")){
        printf("on");
    } else if (!strcmp(argv[1],"angle")){
        printf("27");
    } else if (!strcmp(argv[1],"reset")){
        printf("ok");
    } else{
        menu();
        exit(0);
    }
}