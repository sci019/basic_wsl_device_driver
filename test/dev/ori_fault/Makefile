obj-m := ori_fault.o

all:
	make -C ../../../WSL2-Linux-Kernel-linux-msft-wsl-5.15.167.4 M=$(shell pwd) modules
clean:
	make -C ../../../WSL2-Linux-Kernel-linux-msft-wsl-5.15.167.4 M=$(shell pwd) clean