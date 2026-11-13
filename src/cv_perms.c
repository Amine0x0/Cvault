#include "../include/cvault.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pwd.h>

t_file_rights cv_getrights(const char *filepath){
    t_file_rights rights;
    explicit_bzero(&rights, sizeof(t_file_rights));

    if (!filepath || filepath[0] == '\0')
        return rights;

    char *expanded_path = cv_expand_home_path(filepath);
    if (!expanded_path) {
        return rights;
    }

    {
        int status = access(expanded_path, F_OK);
        if (status != -1)
            rights.is_exist = 1;
    }

    {
        int status = access(expanded_path, X_OK);
        if (status != -1)
            rights.is_executable = 1;
    }

    {
        int status = access(expanded_path, R_OK);
        if (status != -1)
            rights.is_readable = 1;
    }

    {
        int status = access(expanded_path, W_OK);
        if (status != -1)
            rights.is_writeable = 1;
    }
    free(expanded_path);
    return rights; 
}

int cv_isroot(){
    if (!geteuid())
        return (cv_log("user is root.\n", LOG_VERBOSE, 0), 1);
    else
        return (cv_log("user_is no root.\n", LOG_VERBOSE, 0), 0);
}
