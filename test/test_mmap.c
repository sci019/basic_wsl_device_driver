#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/ori_dev"
#define DEVICE_SIZE 4096

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    char *buf = mmap(NULL, DEVICE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return 1;
    }

    // デバイスに書き込み
    sprintf(buf, "Hello from user space!\n");

    // デバイスから読み込み
    printf("Device says: %s", buf);

    munmap(buf, DEVICE_SIZE);
    close(fd);
    return 0;
}
