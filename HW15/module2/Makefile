ifneq ($(KERNELRELEASE),)

# kbuild part of makefile

CFLAGS_hw2.o := -DDEBUG
obj-m := hw2.o

else

# normal makefile

KDIR ?= /home/odiachuk/Desktop/BBB/linux-stable

module:
	$(MAKE) -C $(KDIR) M=$(PWD) C=1 modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) C=1 clean

.PHONY: module clean

endif
