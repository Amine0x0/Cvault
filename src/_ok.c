#include "../include/cvault.h"

void _ok(){
    const char *buf = "ok.";
    ssize_t bytes = write(STDOUT_FILENO, buf, 3);
    (void)bytes;
    return;   
}