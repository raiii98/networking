#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int client_sock;
    struct sockaddr_in serverAddr;
    int number = 4660;
    int received_number;

    client_sock = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);

    connect(client_sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Chuyển sang network byte order và gửi
    number = htonl(number);
    send(client_sock, &number, sizeof(int), 0);

    // Nhận dữ liệu, chuyển về host byte order và in ra
    recv(client_sock, &received_number, sizeof(int), 0);
    // received_number = ntohl(received_number);
    printf("Received from server: %d\n", received_number);

    close(client_sock);
    return 0;
}