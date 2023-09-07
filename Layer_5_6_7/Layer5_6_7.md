# Layer 7 (Application Layer)

Tầng ứng dụng, hay tầng thứ bảy trong mô hình OSI, là tầng cao nhất, trực tiếp giao tiếp với người dùng cuối. Tầng này cung cấp giao diện để người dùng tương tác với ứng dụng thông qua các giao thức mạng.

## 1 Mô tả chung
Tầng ứng dụng không thực sự phải là một ứng dụng mà người dùng tương tác trực tiếp. Thay vào đó, nó cung cấp các dịch vụ mà các ứng dụng cần để thực hiện các hoạt động mạng, như gửi email, truy cập trang web, hay chia sẻ file.

Các giao thức thông dụng ở tầng này bao gồm HTTP (Hypertext Transfer Protocol) cho web browsing, SMTP (Simple Mail Transfer Protocol) cho email, FTP (File Transfer Protocol) cho file transfer, và DNS (Domain Name System) cho dịch vụ tên miền.

## 2 Nhiệm vụ chính

Lập trình viên khi làm việc với tầng ứng dụng trong mô hình OSI cần nắm vững những giao thức được sử dụng ở tầng này. Họ cần hiểu rõ cách mà các giao thức này hoạt động và cách ứng dụng của họ tương tác với chúng.

Ví dụ, nếu một lập trình viên đang phát triển một ứng dụng web, họ cần biết cách sử dụng HTTP để gửi và nhận thông tin qua mạng. Nếu họ đang viết một ứng dụng email, họ cần hiểu cách SMTP hoạt động.

Lập trình viên cũng cần hiểu rõ cách các ứng dụng của họ tương tác với tầng dưới, tầng vận chuyển (Transport Layer). Điều này đòi hỏi hiểu biết về các giao thức như TCP và UDP, cũng như cách họ được sử dụng để chuyển giao dữ liệu giữa máy tính.

# Layer 6 (Session Layer)
## 1 Mô tả chung
Tầng Phiên trong mô hình OSI đóng vai trò như một "phiên dịch viên" giữa tầng ứng dụng (Layer 7) và tầng vận chuyển (Layer 5). Nó đảm bảo rằng dữ liệu được truyền qua mạng có thể được hiểu và xử lý bởi các hệ thống nhận.

## 2 Các Chức năng Chính
### 2.1 Dịch
Tầng này chuyển đổi dữ liệu giữa các định dạng mà tầng ứng dụng và tầng vận chuyển có thể hiểu được. Điều này bao gồm việc chuyển đổi giữa các định dạng dữ liệu chuẩn, như ASCII, EBCDIC, hoặc biểu diễn số.

### 2.2 Mã hóa
Tầng Phiên có thể mã hóa dữ liệu để bảo mật thông tin. Các thuật toán mã hóa, như SSL/TLS, có thể được áp dụng tại tầng này để đảm bảo rằng dữ liệu được truyền một cách an toàn trên mạng.

### 2.3 Nén
Tầng này cũng cung cấp khả năng nén dữ liệu, giúp tối ưu hóa băng thông và thời gian truyền.

# Layer 5 (Session Layer)
## 1. Mô tả chung
Tầng phiên (Session Layer) quản lý việc thiết lập, duy trì và đóng các phiên làm việc giữa các ứng dụng. Các giao thức thông dụng ở tầng này có thể bao gồm NetBIOS, RPC, và PPTP.

## 2. Nhiệm vụ chính
Lập trình viên cần phải quản lý việc thiết lập và đóng các phiên làm việc trong các ứng dụng của họ. Điều này đặc biệt quan trọng trong các ứng dụng mạng có nhiều người dùng hoặc các ứng dụng yêu cầu duy trì trạng thái.

Ví dụ, trong việc phát triển một ứng dụng chat, việc quản lý các phiên làm việc giữa người gửi và người nhận là cần thiết để đảm bảo rằng mỗi người chỉ có thể truy cập vào các thông điệp được gửi đến họ.

Trong các ứng dụng yêu cầu đăng nhập, việc duy trì và quản lý phiên làm việc giữa ứng dụng và người dùng là cần thiết để cung cấp trải nghiệm người dùng liền mạch và an toàn.

Với những thông tin này, lập trình viên có thể hiểu rõ hơn về các tầng 5, 6, và 7 trong mô hình OSI, cũng như biết cách áp dụng chúng trong các ứng dụng thực tế.

# Ví dụ gửi và nhận dữ liệu.

Server:

```
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
```

Client:

```
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
```