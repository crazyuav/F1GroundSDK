#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SEND_UDP        0
#define SEND_TCP        1
#define SEND_TYPE       SEND_TCP
#define BOARD_PORT      7

#define SIZE            1460
#define LOOP_NUM        1024 * 1024 * 1024

int SendTcp()
{
    int                     client2;
    struct sockaddr_in      tar_addr;

    unsigned int            i = 0;
    int                     ret = 0;
    unsigned char           sendbuftcp[SIZE] = { 0 };
    unsigned int *          pinttcp = (unsigned int *)sendbuftcp;
    char                    recvbuf[SIZE * 2];

    memset(&tar_addr, 0, sizeof(tar_addr));
    tar_addr.sin_family = AF_INET;
    tar_addr.sin_addr.s_addr = inet_addr("192.168.1.110");
    tar_addr.sin_port = htons(BOARD_PORT);

    for (i = 4; i < SIZE; i++)
    {
        sendbuftcp[i] = i % 256;
    }

    while (1)
    {
        client2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (client2 == -1)
        {
            printf("Socket2 create failed!\n");
            return -1;
        }
        printf("Socket2 create success!\n");

        while (1)
        {
            if (connect(client2, (struct sockaddr *)&tar_addr, sizeof(tar_addr)) == -1)
            {
                printf("TCP Socket error!\n");
                break;
                //return -1;
            }
            else
            {
                printf("TCP connect success!\n");
            }

            for (i = 0; i < LOOP_NUM; i++)
            {
                printf("TCP index : %u\n", i);
                *pinttcp = htonl(i);
                ret = send(client2, (char *)sendbuftcp, SIZE, 0);
                ret = recv(client2, recvbuf, SIZE, 0);
                if (ret > 0)
                {
                    unsigned int * ptmp = (unsigned int *)recvbuf;
                    printf("TCP %u receive success!\n", ntohl(*ptmp));
                }
                else
                {
                    printf("TCP receive error!\n");
                    sleep(5);
                    break;
                }
                sleep(1);
            }

            if (i == LOOP_NUM)
            {
                break;
            }
        }

        close(client2);
    }

    return 0;
}

int main()
{
    SendTcp();

    return 0;
}
