#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define SIZE            1500
#define BOARD_PORT      8888

int main()
{
    struct sockaddr_in  recv_addr;//处理网络通信的地址 
    struct sockaddr_in  remote_addr;//处理网络通信的地址 
    unsigned char       recvbufudp[SIZE] = { 0 };
    unsigned int *      target_ip;
    int                 client0;
    int                 addrlen;

    client0 = socket(AF_INET, SOCK_DGRAM, 0);
    if (client0 == -1)
    {
        printf("open socket failed!\n");
        return -1;
    }

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = INADDR_ANY;
    recv_addr.sin_port = htons(BOARD_PORT);

    if (bind(client0, (struct sockaddr *)&recv_addr, sizeof(recv_addr)))
    {
        printf("connecting to broad failed\n");
        close(client0);
        return -1;
    }
    addrlen = sizeof(remote_addr);
    recvfrom(client0, (char *)recvbufudp, SIZE, 0, (struct sockaddr *)&remote_addr, &addrlen);
    target_ip = (unsigned int *)recvbufudp;
    printf("target ip: %d.%d.%d.%d\n", \
        recvbufudp[0], recvbufudp[1], recvbufudp[2], recvbufudp[3]);

    close(client0);

    return 0;
}

