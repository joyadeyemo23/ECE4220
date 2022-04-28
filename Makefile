#obj-m+=Lab1_Week1_Old.o
obj-m+=Lab1_Week1_New.o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

all:
	make -C  /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C  /lib/modules/$(shell uname -r)/build M=$(PWD) clean
