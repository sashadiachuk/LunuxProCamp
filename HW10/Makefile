ifneq ($(KERNELRELEASE),)
obj-m := threads.o
else

KDIR ?= /home/odiachuk/Desktop/BBB/linux-stable
#/lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M="$$PWD"

clean:
	$(MAKE) -C $(KDIR) M=$$PWD $@

endif
