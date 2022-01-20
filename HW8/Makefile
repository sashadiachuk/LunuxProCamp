ifneq ($(KERNELRELEASE),)
# Kbuild part of makefile
obj-m := mmap.o 
else

# kernel sources

KDIR ?= /home/odiachuk/Desktop/BBB/linux-stable 
#/lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M="$$PWD"

clean:
	$(MAKE) -C $(KDIR) M="$$PWD" $@

%.i %.s : %.c
	$(ENV_CROSS) \
	$(MAKE) -C $(KDIR) M=$$PWD $@
endif
