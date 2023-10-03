obj-m += netlink_family.o
PWD = /home/raiii/Network/networking/Code/B5


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o   netlink_soc netlink_soc.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
