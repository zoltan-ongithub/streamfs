#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/**
 *
 *
 *
 */
#define NUM_FSELECT_FILES	1

int main(int argc, char *argv[]) {
    char buf[8];
    int f;

    if (argc != 2) {
        fprintf(stderr, "Usage %s <file_name> \n", argv[0]);
        return -1;
    }

    char* fbPath = argv[1];

	int fds[NUM_FSELECT_FILES];

    fds[0] = open(fbPath, O_RDONLY);

    int  nfds = fds[NUM_FSELECT_FILES - 1] + 1;

    if (fds < 0)  {
        perror("failed to open file");
        return 1;
    }

	for(int tries=0; tries < 16; tries++) {
	    static char buf[256];
		fd_set rfds;
		int res;

		FD_ZERO(&rfds);
		for (int i = 0; i < NUM_FSELECT_FILES; i++) {
			FD_SET(fds[i], &rfds);
        }

		res = select(nfds, &rfds, NULL, NULL, NULL);

		if (res < 0) {
			perror("Calling select failed");
			return 1;
		}

		for (int i = 0; i < NUM_FSELECT_FILES; i++) {
			if (!FD_ISSET(fds[i], &rfds)) {
				continue;
			}

            lseek(fds[i], 0, SEEK_SET);

			res = read(fds[i], buf, sizeof(buf));
			if (res < 0) {
				perror("Failed to read data");
				return 1;
			}
			printf("Got value: %s\n", buf);
		}
	}
	return 0;
}
