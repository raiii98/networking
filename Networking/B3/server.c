#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <openssl/aes.h>

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

void decodeData(const unsigned char *in, unsigned char *out, const unsigned char *key)
{
    AES_KEY aesKey;
    if (AES_set_decrypt_key(key, 128, &aesKey) != 0)
    {
        handle_error("Failed to set decryption key");
    }

    AES_decrypt(in, out, &aesKey);
}

int main()
{
    unsigned char key[] = "my_very_secure_key1234"; // Khóa AES cho việc giải mã

    int opt = 1;
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        handle_error("setsockotp()");
    }
    bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_sock, 5);

    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_len);

    uint32_t token_length;
    recv(client_sock, &token_length, sizeof(uint32_t), 0);
    token_length = ntohl(token_length);

    char session_token[token_length + 1];
    recv(client_sock, session_token, token_length, 0);
    session_token[token_length] = '\0';

    if (strcmp(session_token, "valid_token") != 0)
    {
        char error_message[] = "Invalid session token";
        uint32_t error_length = htonl(strlen(error_message));
        send(client_sock, &error_length, sizeof(uint32_t), 0);
        send(client_sock, error_message, strlen(error_message), 0);
        close(client_sock);
        return 0;
    }

    unsigned char buffer[1024];
    unsigned char decryptedMessage[1024];
    memset(&buffer, 0, sizeof(buffer));
    memset(&decryptedMessage, 0, sizeof(decryptedMessage));

    recv(client_sock, buffer, 16, 0); // Giả sử đã biết trước kích thước là 16 byte

    // ... (Phần cuối mã nguồn giữ nguyên)
    printf("%s\n", buffer);
    decodeData(buffer, decryptedMessage, key); // Giải mã tin nhắn
    decryptedMessage[15] = '\0';               // Null-terminate the string
    printf("%s\n", (char *)decryptedMessage);

    if (strcmp((char *)decryptedMessage, "GETdata") == 0)
    {
        char data[] = "Here is the data you requested.";
        send(client_sock, data, strlen(data), 0);
    }
    else if (strcmp((char *)decryptedMessage, "GETstatus") == 0)
    {
        char status[] = "Server is running.";
        send(client_sock, status, strlen(status), 0);
    }
    else
    {
        char msg[] = "Invalid request";
        send(client_sock, msg, strlen(msg), 0);
    }

    close(client_sock);
    close(server_sock);
    return 0;
}
