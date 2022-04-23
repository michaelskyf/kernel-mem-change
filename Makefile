obj-m := mem-change.o

all:
	gcc loop.c -o loop
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm loop
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

.PHONY: setup
setup:
	apt-get install build-essential linux-headers-`uname -r`
