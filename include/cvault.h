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


int _ok();

t_file_rights cv_getrights(const char *filepath);


#endif