#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
    int sockfd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    int received_number;
    // Tạo socket
    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    // Định nghĩa địa chỉ và port cho server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind và listen
    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(sockfd, 5);

    addr_size = sizeof(client_addr);
    new_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

    // Nhận dữ liệu và chuyển về host byte order
    recv(new_sock, &received_number, sizeof(int), 0);
    received_number = ntohl(received_number);

    printf("Received from client: %d\n", received_number);

    received_number = htonl(received_number);
    send(new_sock, &received_number, sizeof(int), 0);

    close(new_sock);
    close(sockfd);

    return 0;
}