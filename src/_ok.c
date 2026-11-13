#include "../include/cvault.h"

int cv_ok(){
    const char *buf = "ok.";
    ssize_t bytes = write(STDOUT_FILENO, buf, 3);
    if (bytes <= 0)
        return 0;
    (void)bytes;
    return 1;   
}
