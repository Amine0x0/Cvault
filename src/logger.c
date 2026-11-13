#include "../include/cvault.h"
#include <string.h>

void cv_log(const char *msg, e_log_mode mode, int fd){
    ssize_t bytes = 0;
    switch (mode)
    {
        case LOG_SILENT:
            if (fd == -1)
                return ;
            bytes = write(fd, "[CVault]: ", strlen("[CVault]: "));
            bytes = write(fd, msg, strlen(msg));
            break;
        case LOG_VERBOSE:
            bytes = write(1, "[CVault]: ", strlen("[CVault]: "));
            bytes = write(1, msg, strlen(msg));
            break;
        default:
            break;
    }
    (void)bytes;
    return ;
}