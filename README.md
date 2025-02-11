# WSLでのデバイスドライバ環境のインストール手順・実行手順
* 参考サイト
  https://zenn.dev/melos/articles/f9afeaa953c279
* WSLにはデフォルトでカーネルのヘッダーがインストールされているわけでないため、クローンする必要がある

# 開発環境の準備
```shell
sudo apt update
```
## カーネルのバージョン
```shell
uname -r
# 5.15.167.4-microsoft-standard-WSL2
```

## カーネルのダウンロード
```shell
wget https://github.com/microsoft/WSL2-Linux-Kernel/archive/refs/tags/linux-msft-wsl-5.15.167.4.zip
unzip linux-msft-wsl-5.15.167.4.zip
```
* 下記サイトからダウンロード  
    https://github.com/microsoft/WSL2-Linux-Kernel/tags

## ビルドパッケージ
```shell
sudo apt install build-essential flex bison dwarves libssl-dev libelf-dev
cd WSL2-Linux-Kernel-linux-msft-wsl-5.15.167.4
cp arch/x86/configs/config-wsl .config
```

## .configの編集
```diff
-CONFIG_LOCALVERSION="-microsoft-standard-WSL2"
+CONFIG_LOCALVERSION="-microsoft-standard-WSL2-meloq"
```

## 一部のスクリプトを編集
* cloneしたヘッダースクリプトには、誤字脱字などが多くビルドしてもエラーとなる
* 下記フォルダのREADME.mdに記載の通りに編集
```shell
buildable__kernel_header/README.md
```


## カーネルのビルド
```shell
make
```
* wslのhomeディレクトリの方が良いらしい？  
  https://github.com/microsoft/WSL/issues/8045

## ビルドしたカーネルを好きなディレクトリにコピー
```shell
target_dir=/mnt/c/Users/{ユーザ名}/SW/dev_drv
cp vmlinux $target_dir
touch /mnt/c/Users/.wslconfig
```
## /.wslconfigの編集
```wsll2
kernel=C:\\Users\\{ユーザ名}\\SW\\dev_drv\\vmlinux
```
* `C:\Users\{ユーザ名}`,`C:\Users`のどちらかには`.wslconfig`を置く必要がある


# wslの再起動
```shell
wsl --shutdown
```

その後、カーネルヘッダーを確認
```shell
wsl
uname -r
# 5.15.167.4-microsoft-standard-WSL2-meloq
```

# テスト用のデバイスドライのビルド
```shell
cd dev/ori_phy_mem_dev
```


# テストコードのビルド
```shell
cd test/dev/ori_phy_mem ; make
cd ../..
gcc -o test_mmap test_mmap.c
./test_mmap
```





