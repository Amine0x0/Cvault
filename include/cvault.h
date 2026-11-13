#ifndef CVAULT_H
#define CVAULT_H

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct s_netcontext {
    char    private_ip[INET_ADDRSTRLEN];
    char    public_ip[INET_ADDRSTRLEN];
    int     open_ports[64];
    int     port_count;
}               t_netcontext;

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
int cv_ok();

//rights
t_file_rights cv_getrights(const char *filepath);

//logs
void cv_log(const char *msg, e_log_mode mode, int fd);
int cv_isroot();
int cv_createfile(const char* filepath, int isStartHome);
char *cv_expand_home_path(const char *path);
char* cv_getfilecontent(const char* filepath);
void cv_dump_memory(const unsigned char *memory, size_t length, int fd);
t_netcontext *cv_netrecon(void);
char** cv_getdirfiles(const char *dirpath);

#endif
