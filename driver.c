#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "OT_proto.h"

#define PORT 102

void menu()
{
    printf("\n**************  PLC Driver **************\n");
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

struct ind_ap gen_apl_proto()
{
    struct ind_ap apl_proto;
    apl_proto.type = 0;
    apl_proto.option = 0;
    apl_proto.err = 0;
    apl_proto.mem_addr = 0;
    apl_proto.value = 0;
    return apl_proto;
}

struct ind_tp gen_trans_proto()
{
    struct ind_tp trans_proto;
    trans_proto.ID_proto = 7;
    strcpy(trans_proto.ID_source, "gtw");
    strcpy(trans_proto.ID_dest, "plc");
    trans_proto.credits = 0;
    trans_proto.tpdu_type = 0;
    return trans_proto;
}

void notify(int sock, int credits)
{
    struct ind_ap apl_proto = (struct ind_ap)gen_apl_proto();
    struct ind_tp trans_proto = (struct ind_tp)gen_trans_proto();
    trans_proto.credits = credits;
    trans_proto.ap_proto = apl_proto;
    send(sock, &trans_proto, sizeof(trans_proto), 0);
}

void read_mem(int sock, unsigned int mem_addr)
{
    notify(sock, 1);
    struct ind_ap apl_proto = (struct ind_ap)gen_apl_proto();
    struct ind_tp trans_proto = (struct ind_tp)gen_trans_proto();
    apl_proto.mem_addr = mem_addr;
    apl_proto.option = 1;
    trans_proto.tpdu_type = 1;
    trans_proto.ap_proto = apl_proto;
    send(sock, &trans_proto, sizeof(trans_proto), 0);
    recv(sock, &trans_proto, sizeof(struct ind_tp),0);
    if (trans_proto.ID_proto==7 && strcmp(trans_proto.ID_source,"plc")==0 && strcmp(trans_proto.ID_dest,"gtw")==0 &&  
        trans_proto.tpdu_type==1 && trans_proto.credits==0 && trans_proto.ap_proto.type==1 && trans_proto.ap_proto.option==1 && 
        trans_proto.ap_proto.err==0){
            if (trans_proto.ap_proto.mem_addr==1000 && trans_proto.ap_proto.value==1)
                printf("on");
            else if (trans_proto.ap_proto.mem_addr==1000 && trans_proto.ap_proto.value==0)
                printf("off");
            else
            printf("%d",trans_proto.ap_proto.value);
        }
    else{
        printf("error");
    }
}

void mod_mem(int sock, unsigned int mem_addr, unsigned int value)
{
    notify(sock, 1);
    struct ind_ap apl_proto = (struct ind_ap)gen_apl_proto();
    struct ind_tp trans_proto = (struct ind_tp)gen_trans_proto();
    apl_proto.mem_addr = mem_addr;
    apl_proto.option = 2;
    apl_proto.value = value;
    trans_proto.tpdu_type = 1;
    trans_proto.ap_proto = apl_proto;
    send(sock, &trans_proto, sizeof(trans_proto), 0);
    recv(sock, &trans_proto, sizeof(struct ind_tp),0);
    if (trans_proto.ID_proto==7 && strcmp(trans_proto.ID_source,"plc")==0 && strcmp(trans_proto.ID_dest,"gtw")==0 &&  
        trans_proto.tpdu_type==1 && trans_proto.credits==0 && trans_proto.ap_proto.type==1 && trans_proto.ap_proto.option==2 && 
        trans_proto.ap_proto.err==0)
        printf("ok");
    else
        printf("error");
    
}

void erase_mem(int sock)
{
    notify(sock, 3);
    struct ind_ap apl_proto = (struct ind_ap)gen_apl_proto();
    struct ind_tp trans_proto = (struct ind_tp)gen_trans_proto();
    apl_proto.option = 3;
    trans_proto.tpdu_type = 1;
    
    apl_proto.mem_addr = 500;
    trans_proto.ap_proto = apl_proto;
    send(sock, &trans_proto, sizeof(trans_proto), 0);
    apl_proto.mem_addr = 1000;
    trans_proto.ap_proto = apl_proto;
    send(sock, &trans_proto, sizeof(trans_proto), 0);
    apl_proto.mem_addr = 1500;
    trans_proto.ap_proto = apl_proto;
    send(sock, &trans_proto, sizeof(trans_proto), 0);


    recv(sock, &trans_proto, sizeof(struct ind_tp),0);
    if (trans_proto.ID_proto==7 && strcmp(trans_proto.ID_source,"plc")==0 && strcmp(trans_proto.ID_dest,"gtw")==0 &&  
        trans_proto.tpdu_type==1 && trans_proto.credits==0 && trans_proto.ap_proto.type==1 && trans_proto.ap_proto.option==3 && 
        trans_proto.ap_proto.err==0)
        printf("ok");
    else
        printf("error");
}

int main(int argc, char *argv[])
{

    if (argc < 2 || argc > 3)
    {
        menu();
        exit(0);
    }

    struct sockaddr_in local_addr, server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_aton("10.0.0.20", &server_addr.sin_addr);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (!strcmp(argv[1], "on") || !strcmp(argv[1], "off"))
    {

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
        connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

        if (!strcmp(argv[1], "on"))
        {
            mod_mem(sock, 1000, 1);
        }
        else
        {
            mod_mem(sock, 1000, 0);
        }

        close(sock);
    }
    else if (!strcmp(argv[1], "move") && argc == 3)
    {

        int angle = atoi(argv[2]);
        if (angle > 90 || angle < 0)
        {
            printf("Angle must be between 0 and 90");
            exit(0);
        }

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
        connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
        
        mod_mem(sock, 1500, angle);

        close(sock);
    }
    else if (!strcmp(argv[1], "energy"))
    {

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
        connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
        
        read_mem(sock, 500);

        close(sock);
    }
    else if (!strcmp(argv[1], "status"))
    {

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
        connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

        read_mem(sock, 1000);

        close(sock);
    }
    else if (!strcmp(argv[1], "angle"))
    {

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
        connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

        read_mem(sock, 1500);

        close(sock);
    }
    else if (!strcmp(argv[1], "reset"))
    {

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
        connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

        erase_mem(sock);
       
        close(sock);
    }
    else
    {
        menu();
        exit(0);
    }

    return 0;
}