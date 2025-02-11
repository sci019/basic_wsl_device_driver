# kernelヘッダーのビルドでエラーを起こさない
`dev_drv/buildable__kernel_header`以下のファイルで下記パスのファイルを置き換える


ipt_ECN.c
```
WSL2-Linux-Kernel-linux-msft-wsl-5.15.167.4/net/ipv4/netfilter/ipt_ECN.c
```

ipt_ecn.h
```
WSL2-Linux-Kernel-linux-msft-wsl-5.15.167.4/include/uapi/linux/netfilter_ipv4/ipt_ecn.h
```


Makefile
```
WSL2-Linux-Kernel-linux-msft-wsl-5.15.167.4/net/netfilter/Makefile
```

