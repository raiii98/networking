# Định nghĩa Netlink Socket

Netlink socket là một cơ chế IPC (Inter-Process Communication) giữa kernel và các tiến trình ở user space trong hệ điều hành Linux. Netlink được thiết kế để làm cho việc truyền thông tin, cấu hình và thậm chí là điều khiển các đối tượng như routing tables, network interfaces, firewall rules, và nhiều hơn nữa trở nên dễ dàng. Netlink thực hiện việc này thông qua cấu trúc tin nhắn đặc biệt, có thể chứa dữ liệu phức tạp.

Các loại Netlink sockets phổ biến:

NETLINK_ROUTE: Dùng để truyền thông tin về định tuyến và quản lý interface mạng.
NETLINK_GENERIC: Dùng để giao tiếp tổng quát, không phải dành riêng cho một dịch vụ cụ thể.
NETLINK_NETFILTER: Dùng cho việc tương tác với bộ lọc gói tin Netfilter.

Các Thư Viện Hỗ Trợ

libnl: Đây là một thư viện không gian người dùng được thiết kế để giảm thiểu sự phức tạp khi làm việc với Netlink. Thư viện này cung cấp các API cao cấp để tạo và phân tích các tin nhắn Netlink.

iproute2: Đây không phải là một thư viện nhưng là một bộ công cụ mạng Linux, và nó sử dụng Netlink để tương tác với nhân hệ điều hành. Các lệnh như ip, tc, ss đều là phần của iproute2.

Cách sử dụng Netlink Socket trong C

```
#include <linux/netlink.h>
#include <sys/socket.h>

int sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
```

So sánh với các phương pháp giao tiếp khác

Netlink Socket:

Ưu điểm
- Linh động và mạnh mẽ.
- Có khả năng truyền các tin nhắn có cấu trúc phức tạp.

Nhược điểm
- Có độ phức tạp cao khi cài đặt.

IOCTL (Input/Output Control):

Đặc điểm
- Được sử dụng để thực hiện các thao tác I/O control trên các file descriptors.
- Thông thường được sử dụng cho việc cấu hình các thiết bị.
- API đơn giản.

```
#include <sys/ioctl.h>
// ...
int fd = open("/dev/some_device", O_RDWR);
ioctl(fd, SOME_IOCTL_CMD, &some_data);

```

Ưu điểm
- API đơn giản và dễ sử dụng.
- Tiết kiệm bộ nhớ và CPU hơn so với Netlink.

Nhược điểm
- Không có khả năng truyền tin nhắn có cấu trúc.
- Khó mở rộng và không linh động như Netlink.

Sysfs:

Đặc điểm

Là một file system ảo, cung cấp interface giữa kernel space và user space thông qua file system.
Thông thường được sử dụng để đọc thông tin từ các thiết bị.

Ví dụ

Đọc thông tin của CPU:

```
FILE *fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
// ...

```

Ưu điểm
- Rất dễ sử dụng.
- Có thể truy cập thông qua các lệnh shell đơn giản như cat, echo.


Nhược điểm
- Chỉ có khả năng truyền các thông tin đơn giản.
- Không hỗ trợ giao tiếp hai chiều phức tạp.

Tóm tắt:

Netlink: Phức tạp nhưng linh động, có thể truyền dữ liệu phức tạp và thực hiện các thao tác real-time.

ioctl: Đơn giản và nhanh chóng nhưng khó mở rộng, thích hợp cho các thao tác đơn giản.

sysfs: Dễ sử dụng và tổ chức tốt nhưng có chi phí IO, không phải lúc nào cũng đáp ứng được yêu cầu real-time.

# API cơ bản trong sử dụng Netlink

1. socket(): Tạo Socket
Để tạo một socket Netlink, bạn sẽ sử dụng hàm socket() với các tham số sau:

- PF_NETLINK hoặc AF_NETLINK: Để chỉ định rằng đây là một socket Netlink.
- SOCK_RAW: Để chỉ định rằng socket sẽ sử dụng giao thức raw.
- NETLINK_ROUTE, NETLINK_XFRM, NETLINK_AUDIT,...: Để chỉ định loại Netlink socket.

```
int sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
```

2. bind(): Bind Socket vào Địa chỉ
Sau khi tạo socket, bạn cần bind nó vào một địa chỉ sử dụng cấu trúc sockaddr_nl.

```
struct sockaddr_nl src_addr;
memset(&src_addr, 0, sizeof(src_addr));
src_addr.nl_family = AF_NETLINK;
src_addr.nl_pid = getpid();  // Process ID

bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
```

3. sendmsg(): Gửi Message
Để gửi một message, bạn sẽ sử dụng hàm sendmsg(). Bạn cần tạo một cấu trúc msghdr để chứa thông tin về message.

```
struct msghdr msg;
memset(&msg, 0, sizeof(msg));
msg.msg_name = (void *)&dest_addr;
msg.msg_namelen = sizeof(dest_addr);
msg.msg_iov = &iov;
msg.msg_iovlen = 1;

sendmsg(sock_fd, &msg, 0);
```

4. recvmsg(): Nhận Message
Để nhận message, bạn sẽ sử dụng hàm recvmsg().

```
struct msghdr msg;
memset(&msg, 0, sizeof(msg));
msg.msg_name = (void *)&src_addr;
msg.msg_namelen = sizeof(src_addr);
msg.msg_iov = &iov;
msg.msg_iovlen = 1;

recvmsg(sock_fd, &msg, 0);
```

# Netlink trong Kernel Space


Trong nhân Linux (kernel space), Netlink được sử dụng như một cơ chế giao tiếp giữa các thành phần khác nhau của nhân và cũng như với không gian người dùng (user space). Cấu trúc cơ bản và các API của Netlink trong kernel được thiết kế để đảm bảo tính linh hoạt và mở rộng.

Các thành phần chính:

- Netlink Socket: Đây là socket ảo trong nhân, và nó không liên quan đến phần cứng mạng thực sự.
- SKB (Socket Buffer): Đây là cấu trúc dữ liệu được sử dụng để lưu trữ tin nhắn Netlink. SKB cho phép dễ dàng thêm hoặc xóa các trường dữ liệu.
- Netlink Family: Mỗi loại Netlink (như NETLINK_ROUTE, NETLINK_GENERIC, v.v.) được gọi là một "family". Mỗi family có thể có nhiều operations và các hàm callback để xử lý tin nhắn.

Ứng dụng trong Kernel:

Netlink được sử dụng rộng rãi trong nhân Linux cho nhiều mục đích, bao gồm quản lý định tuyến, cấu hình interface mạng, quản lý cgroup, và nhiều hơn nữa. Nó cũng là cơ sở cho nhiều tiện ích không gian người dùng như ip, ss, tc, và thư viện như libnl.

## 1. Cách định nghĩa và đăng ký một Netlink Family

Để tạo một Netlink family trong kernel, bạn cần sử dụng cấu trúc netlink_kernel_cfg để cấu hình và đăng ký family.

```
#include <net/netlink.h>
#include <net/net_namespace.h>

static struct netlink_kernel_cfg cfg = {
    .input = my_input_function,  // Hàm callback để xử lý message từ user-space
};

struct sock *nl_sk = NULL;

void my_module_init(void) {
    nl_sk = netlink_kernel_create(&init_net, MY_NETLINK_PROTOCOL, &cfg);
}
```

## 2. Trong kernel Linux, có nhiều Netlink families được định nghĩa sẵn để phục vụ các mục đích quản lý và cấu hình hệ thống. Một số Netlink families phổ biến bao gồm:

- NETLINK_ROUTE: Dùng cho việc quản lý định tuyến và các thiết bị mạng. Đây có lẽ là family phổ biến nhất, được sử dụng bởi các công cụ như ip và route.
- NETLINK_USERSOCK: Dành cho các ứng dụng user-space muốn sử dụng Netlink như là một cơ chế IPC.
- NETLINK_FIREWALL: Dùng cho việc cấu hình và quản lý tường lửa (IPv4).
- NETLINK_INET_DIAG: Được sử dụng để lấy thông tin về các socket TCP/IP.
- NETLINK_NFLOG: Dùng để giao tiếp với netfilter để lấy các log về gói tin.
- NETLINK_XFRM: Dùng trong quản lý và cấu hình bảo mật IPsec.
- NETLINK_SELINUX: Dùng để giao tiếp với chính sách bảo mật SELinux.
- NETLINK_ISCSI: Dùng cho các tác vụ quản lý iSCSI.
- NETLINK_AUDIT: Dùng để giao tiếp với hệ thống audit.
- NETLINK_FIB_LOOKUP: Dùng trong việc tra cứu định tuyến FIB.
- NETLINK_CONNECTOR: Dùng để gửi các sự kiện từ kernel đến user space.
- NETLINK_NETFILTER: Dùng cho việc cấu hình và quản lý netfilter.
- NETLINK_IP6_FW: Tương tự như NETLINK_FIREWALL nhưng dành cho IPv6.
- NETLINK_DNRTMSG: Dùng cho việc giao tiếp với DECnet.
- NETLINK_KOBJECT_UEVENT: Dùng để thông báo các sự kiện liên quan đến kobject đến user space.
- NETLINK_GENERIC: Một family đa dụng, thường được sử dụng để triển khai các tính năng mới mà không cần thêm một family riêng biệt.



# Netlink trong Kernel Space và User Space

Khi nói về Netlink Socket, có thể đề cập đến cả không gian người dùng (User Space) và không gian hệ thống (Kernel Space), vì Netlink Socket là cầu nối giữa hai không gian này.

Netlink trong kernel space và user space tương tác với nhau thông qua một giao diện IPC (Inter-Process Communication) đặc biệt, giúp cho việc trao đổi thông tin và các yêu cầu điều khiển giữa kernel và các ứng dụng hoặc tiến trình trong user space. Dưới đây là cách mà chúng tương tác với nhau:

## Khởi tạo và Đăng ký Socket

User Space: Ứng dụng mở một netlink socket sử dụng các hàm API như socket().
Kernel Space: Tương ứng với socket mở từ user space, kernel cũng sẽ có một netlink socket được đăng ký sẵn hoặc được tạo khi có yêu cầu.

User space:

```
int sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
```


## Gửi và Nhận Thông điệp

User Space → Kernel Space: Khi ứng dụng muốn gửi thông tin đến kernel, nó sẽ sử dụng hàm như sendmsg() để gửi một thông điệp netlink đến kernel.
Kernel Space → User Space: Khi kernel cần thông báo hoặc gửi dữ liệu đến user space, nó sẽ sử dụng các hàm API của netlink trong kernel để gửi thông điệp đến socket tương ứng trong user space.

## Xử lý Thông điệp và Callback

Kernel Space: Kernel sẽ xử lý thông điệp nhận được từ user space, có thể thông qua các hàm callback được định nghĩa trước.
User Space: Tương tự, khi nhận được thông điệp từ kernel, ứng dụng trong user space cũng sẽ xử lý thông điệp đó, thường thông qua các hàm callback hoặc event loop.

## Đồng bộ và Đa bộ xử lý

Netlink hỗ trợ cả giao tiếp đồng bộ và đa bộ xử lý. Một thông điệp có thể được gửi từ một nguồn đến nhiều đích (multicast).

Đồng Bộ (Synchronous):

User Space → Kernel Space: Khi một ứng dụng trong user space gửi một thông điệp đến kernel và chờ đợi một phản hồi, đây là một ví dụ về giao tiếp đồng bộ.

Ứng dụng sử dụng sendmsg() để gửi thông điệp và recvmsg() để đợi phản hồi.
Kernel Space → User Space: Kernel cũng có thể gửi thông điệp đến user space và đợi một phản hồi. Tuy nhiên, điều này ít phổ biến hơn.

Đa Bộ Xử Lý (Asynchronous):

- Multicast: Netlink cho phép gửi các thông điệp từ một nguồn đến nhiều đích. Điều này hữu ích trong các tình huống như thông báo các sự kiện hệ thống tới nhiều ứng dụng đang lắng nghe.

  - Ví dụ: Khi một thiết bị mạng mới được thêm vào, kernel có thể gửi một thông điệp multicast tới tất cả các ứng dụng quan tâm.

- Non-blocking Mode: Cả trong user space và kernel space, sockets có thể được cấu hình để hoạt động trong chế độ non-blocking, cho phép thực hiện các tác vụ khác mà không cần đợi phản hồi.

- Event Loop: Trong user space, có thể sử dụng các thư viện hoặc event loop (như select() hoặc epoll()) để xử lý thông điệp một cách không đồng bộ.

## Đóng Socket

Khi việc giao tiếp giữa user space và kernel space kết thúc, cả hai đối tác này đều có thể đóng netlink socket của mình.

Ví dụ, một ứng dụng trong user space có thể sử dụng netlink để yêu cầu thông tin về các thiết bị mạng. Kernel sẽ nhận yêu cầu này, xử lý, và gửi thông tin trả lại cho ứng dụng thông qua cùng một netlink socket.

Tóm lại, netlink trong kernel space và user space tương tác chặt chẽ với nhau, tạo thành một cầu nối mạnh mẽ cho việc giao tiếp và điều khiển giữa kernel và các ứng dụng trong user space.



# Code VD

Kernel module:

```
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#define DRIVER_AUTHOR "Raiii quoctoan24898@gmail.com"
#define DRIVER_DESC "Hello world kernel module"
#define DRIVER_VERS "1.0"
#define NETLINK_USER 31

struct sock *nl_sk = NULL;

static void my_receive_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg = "Hello from kernel";
    int res;

    msg_size = strlen(msg);

    nlh = (struct nlmsghdr *)skb->data;
    printk(KERN_INFO "Received message payload: %s\n", (char *)nlmsg_data(nlh));
    pid = nlh->nlmsg_pid; /* pid of sending process */

    skb_out = nlmsg_new(msg_size, 0);
    if (!skb_out)
    {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0)
    {
        printk(KERN_INFO "Error while sending back to user\n");
    }
    else
    {
    }
}

static int __init my_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = my_receive_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk)
    {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }
    else
    {
        printk("Creating socket %p\n", nl_sk);
    }

    return 0;
}

static void __exit my_exit(void)
{
    netlink_kernel_release(nl_sk);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);
```


User program:

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

#define NETLINK_USER 31

int main()
{
    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    int sock_fd;
    struct msghdr msg;

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0)
    {
        perror("socket");
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    if (bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)))
    {
        perror("bind");
        return -1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(1024));
    memset(nlh, 0, NLMSG_SPACE(1024));
    nlh->nlmsg_len = NLMSG_SPACE(1024);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    strcpy(NLMSG_DATA(nlh), "Hello from user-space");

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    int send_fd = sendmsg(sock_fd, &msg, 0);
    if (send_fd < 0)
    {
        perror("sendmsg");

        return -1;
    }

    printf("Listening for events...\n");
    while (1)
    {

        ssize_t len = recvmsg(sock_fd, &msg, 0);
        if (len > 0)
        {
            printf("Received message: %s\n", (char *)NLMSG_DATA(nlh));
        }
    }

    close(sock_fd);
    return 0;
}
```


Make file:

```
obj-m += netlink_family.o
PWD = /home/raiii/Network/networking/Code/B5


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o   netlink_soc netlink_soc.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```