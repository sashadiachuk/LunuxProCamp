ifneq ($(KERNELRELEASE),)
obj-m := hrt.o  timer.o simple_wq.o
else

KDIR ?= /home/odiachuk/Desktop/BBB/linux-stable
#/lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M="$$PWD"

clean:
	$(MAKE) -C $(KDIR) M=$$PWD $@

endif
