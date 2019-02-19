obj-m:=lradc.o
PWD:=$(shell pwd)
KERNEL_PATH="/home/michael/EasyARM-File/linux-2.6.35.3"
KDIR:=$(KERNEL_PATH)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) 
clean:
	rm -rf *.ko *.order *.symvers *.cmd *.o *.mod.c *.tmp_versions .*.cmd .tmp_versions
