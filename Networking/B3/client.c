#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/aes.h>

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

void encodeData(const unsigned char *in, unsigned char *out, const unsigned char *key)
{
    AES_KEY aesKey;
    if (AES_set_encrypt_key(key, 128, &aesKey) != 0)
    {
        handle_error("Failed to set encryption key");
    }

    AES_encrypt(in, out, &aesKey);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <server_ip> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    unsigned char key[] = "my_very_secure_key"; // Khóa AES cho việc mã hóa

    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    inet_pton(AF_INET, argv[1], &server_address.sin_addr);

    connect(client_sock, (struct sockaddr *)&server_address, sizeof(server_address));

    char session_token[] = "valid_token";
    uint32_t token_length = htonl(strlen(session_token));
    send(client_sock, &token_length, sizeof(uint32_t), 0);
    send(client_sock, session_token, strlen(session_token), 0);

    char request[16] = {0};        // Initialize to zero
    strncpy(request, argv[2], 15); // Copy up to 15 characters from argv[2]
    unsigned char encodedRequest[16];
    encodeData((unsigned char *)request, encodedRequest, key);
    send(client_sock, encodedRequest, 16, 0); // Gửi dữ liệu đã mã hóa
    printf("%s\n", encodedRequest);

    char server_response[1024];
    memset(&server_response, 0, sizeof(server_response));
    recv(client_sock, server_response, 1024, 0);
    printf("Received from server: %s\n", server_response);

    close(client_sock);
    return 0;
}
