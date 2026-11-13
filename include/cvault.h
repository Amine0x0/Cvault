#ifndef CVAULT_H
#define CVAULT_H

#include <unistd.h>

typedef enum{
    READACCESS,
    WRITEACCESS,
    EXECUTEACCESS,
    FILEXISTS,
}   e_accessWhat;

typedef struct{
    unsigned int is_exist : 1;
    unsigned int is_readable: 1;
    unsigned int is_writeable: 1;
    unsigned int is_executable: 1;
}   t_file_rights;

typedef enum {
    LOG_SILENT,
    LOG_VERBOSE,
}   e_log_mode;

//health
int _ok();

//rights
t_file_rights cv_getrights(const char *filepath);

//logs
void cv_log(const char *msg, e_log_mode mode, int fd);
int cv_isroot();

#endif