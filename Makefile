obj-m += kthread_test.o
obj-m += hrtimer_test.o
obj-m += Lab1_Part2_ISR.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
