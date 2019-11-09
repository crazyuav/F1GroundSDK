#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SEND_UDP        0
#define SEND_TCP        1
#define SEND_TYPE       SEND_UDP
#define BOARD_PORT      7

#define SIZE            99
#define LOOP_NUM        1024 * 1024 * 1024

int SendUdp()
{
    int                     client1;
    struct sockaddr_in      tar_addr;

    int                     i = 0, ret = 0;
    unsigned char           sendbufudp[SIZE] = { 0 };
    unsigned int *          pintudp = (unsigned int *)sendbufudp;

    memset(&tar_addr, 0, sizeof(tar_addr));
    tar_addr.sin_family = AF_INET;
    tar_addr.sin_addr.s_addr = inet_addr("192.168.1.110");
    tar_addr.sin_port = htons(BOARD_PORT);

    for (i = 4; i < SIZE; i++)
    {
        sendbufudp[i] = i % 256;
    }

    client1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client1 == -1)
    {
        printf("Socket1 create failed!\n");
    }

    for (i = 0; i < LOOP_NUM; i++)
    {
        printf("UDP index : %d\n", i);
        *pintudp = htonl(i);
        ret = sendto(client1, (char *)sendbufudp, SIZE, 0, (struct sockaddr *)&tar_addr, sizeof(tar_addr));
        sleep(1);
    }

    close(client1);

    return 0;
}

int main()
{
    SendUdp();

    return 0;
}