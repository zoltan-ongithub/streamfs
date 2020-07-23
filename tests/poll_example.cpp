#include <poll.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FILE_BUFFER_SIZE 256
int main(int argc, char *argv[]) {
    char buf[8];
    struct pollfd pfd;
    int f;


    if (argc != 2) {
        fprintf(stderr, "Usage %s <file_name> \n", argv[0]);
        return -1;
    }

    char* fbPath = argv[1];

    for(;;) {
        memset(buf, 0, sizeof(buf));

        if ((f = open(fbPath, O_RDONLY)) < 0) {
            fprintf(stderr, "Failed to open file%s\n", fbPath);
            goto quit;
        }

        if ((lseek(f, 0L, SEEK_SET)) < 0) {
            fprintf(stderr, "Failed to seek %s\n", fbPath);
            goto fail;
        }

        if ((read(f, buf, 1)) < 0) {
            fprintf(stdout, "Failed to read value %s\n", fbPath);
            goto fail;
        }


        fprintf(stdout, "Got buffer %s\n",  buf);

        pfd.fd = f;
        pfd.events = POLLPRI;
        fprintf(stdout, "Calling poll\n");
        poll(&pfd, 1, -1);
        fprintf(stdout, "Poll completed\n");
        close(f);
    }
    return 0;

fail:
    close(f);
quit:
    return -1;
}
