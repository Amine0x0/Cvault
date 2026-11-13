#include "../include/cvault.h"
#include <string.h>
#include <unistd.h>

void cv_log(const char *msg, e_log_mode mode, int fd){
    ssize_t bytes = 0;
    switch (mode)
    {
        case LOG_SILENT:
            if (fd == -1)
                return ;
            bytes = write(fd, "[CVault]: ", 10);
            bytes = write(fd, msg, strlen(msg));
            bytes = write(fd, "\n", 1);
            break;
        case LOG_VERBOSE:
            bytes = write(1, "[CVault]: ", 10);
            bytes = write(1, msg, strlen(msg));
            bytes = write(1, "\n", 1);
            break;
        default:
            break;
    }
    (void)bytes;
}
