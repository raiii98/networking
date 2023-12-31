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