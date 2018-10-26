#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc, char const *argv[])
{
    /* code */

    int fd = 0; // stdin = 0;
    char buf[1025];
    char buffer[1025];
    char *message = "OMGGGGG";
    int ret, sret;
    fd_set readFds;
    int type = 0;
    struct timeval timeout;
    int filedescriptor, filedescriptor2;
    filedescriptor = open("fifo-1-0", O_RDONLY | O_NONBLOCK);

    if (filedescriptor == -1)
    {
        perror("error oepning");
    }
    filedescriptor2 = open("fifo-0-1", O_RDWR | O_NONBLOCK);
    if (filedescriptor2 == -1)
    {
        perror("error opening");
    }

    for (;;)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 10;
        FD_ZERO(&readFds);
        FD_SET(fd, &readFds);
        FD_SET(filedescriptor, &readFds);
        FD_SET(filedescriptor2, &readFds);
        ret = select(4, &readFds, NULL, NULL, &timeout);
        if (ret == -1)
        {
            perror("select");
        }
        else if (ret)
        {
            if (FD_ISSET(fd, &readFds))
            {

                memset(buf, 0, sizeof(buf));
                size_t length = read(fd, (void *)buf, sizeof(buf));
                if (length != -1)
                {
                    // printf("%s", buf);
                    // if (strcmp(bu))
                    if (strcmp(buf, "list\n") == 0)
                    {
                        printf("damn");
                    }
                    else if (strcmp(buf, "exit\n") == 0)
                    {
                        printf("daniel");
                    }
                }

                // }
            }
            else if (FD_ISSET(filedescriptor, &readFds))
            {
                memset(buffer, 0, sizeof(buffer));
                size_t len = read(filedescriptor, &buffer, sizeof(buffer));
                if (len != -1)
                {
                    printf("%s\n", buf);
                }
                if (FD_ISSET(filedescriptor2, &readFds))
                {
                    write(filedescriptor2, &message, sizeof(message));
                }
            }
        }
    }

    return 0;
}
