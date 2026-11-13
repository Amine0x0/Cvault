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
    FILE *file = fopen(target_path, "w");
    free(fullpath);
    if (!file) {
        return -1;
    }
    fclose(file);
    return 0;
}