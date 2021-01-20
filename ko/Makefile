obj-m	:= enable_arm_pmu.o
KDIR	:= /lib/modules/$(shell uname -r)/build
#KDIR	:= /lib/modules/5.4.0-26-generic/build
PWD	:= $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
