#include "../include/cvault.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

int cv_createfile(const char* filepath, int isStartHome) {
    if (!filepath) {
        return -1;
    }
    char *fullpath = NULL;
    const char *target_path = filepath;
    
    if (isStartHome) {
        const char* homedir = getenv("HOME");
        if (!homedir) {
            struct passwd *pw = getpwuid(getuid());
            if (pw != NULL) {
                homedir = pw->pw_dir;
            }
        }
        if (!homedir) {
            return -1;
        }
        int needs_slash = (filepath[0] != '/');
        size_t len = strlen(homedir) + strlen(filepath) + (needs_slash ? 2 : 1);

        fullpath = malloc(len);
        if (!fullpath) {
            return -1;
        }
        if (needs_slash) {
            snprintf(fullpath, len, "%s/%s", homedir, filepath);
        } else {
            snprintf(fullpath, len, "%s%s", homedir, filepath);
        }
        target_path = fullpath;
    }

    int fd = open(target_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    free(fullpath);
    
    return fd;
}

char* cv_getfilecontent(const char* filepath){
    t_file_rights rights;
    rights = cv_getrights(filepath);
    if (rights.is_exist && rights.is_readable){
        FILE *file = fopen(filepath, "rb");
        if (!file)
            return NULL;

        if (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            return NULL;
        }
        long size = ftell(file);
        if (size < 0) {
            fclose(file);
            return NULL;
        }
        rewind(file);
        char *buffer = malloc(size + 1);
        if (!buffer) {
            fclose(file);
            return NULL;
        }
        size_t bytes_read = fread(buffer, 1, size, file);
        if (bytes_read != (size_t)size) {
            free(buffer);
            fclose(file);
            return NULL;
        }
        buffer[size] = '\0';
        fclose(file);
        return buffer;
    }
    return NULL;
}
