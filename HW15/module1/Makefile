

ifneq ($(KERNELRELEASE),)

# kbuild part of makefile

CFLAGS_hw1.o := -DDEBUG
obj-m := hw1.o

else

# normal makefile


KDIR ?= /home/odiachuk/Desktop/BBB/linux-stable
module:
	$(MAKE) -C $(KDIR) M=$(PWD) C=1 modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) C=1 clean

.PHONY: module clean

endif
