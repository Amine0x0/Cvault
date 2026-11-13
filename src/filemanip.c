#include "../include/cvault.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>

char *cv_expand_home_path(const char *path) {
    if (!path || path[0] == '\0') {
        return NULL;
    }

    if (path[0] != '~' || (path[1] != '/' && path[1] != '\0')) {
        return strdup(path);
    }

    const char *home = getenv("HOME");
    if (!home) {
        struct passwd *pw = getpwuid(getuid());
        if (pw != NULL) {
            home = pw->pw_dir;
        }
    }
    if (!home) {
        return NULL;
    }

    size_t len = strlen(home) + strlen(path) + 1;
    char *expanded = malloc(len);
    if (!expanded) {
        return NULL;
    }

    if (path[1] == '\0') {
        snprintf(expanded, len, "%s", home);
    } else {
        snprintf(expanded, len, "%s%s", home, path + 1);
    }
    return expanded;
}

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

void cv_dump_memory(const unsigned char *memory, size_t length, int fd) {
    if (!memory || fd < 0) {
        return;
    }

    for (size_t i = 0; i < length; i += 8) {
        char line[256];
        size_t pos = 0;
        pos += snprintf(line + pos, sizeof(line) - pos, "");

        for (int j = 0; j < 8 && i + (size_t)j < length; j++) {
            int n = snprintf(line + pos, sizeof(line) - pos, "%s[0x%02x]", (j == 0 ? "" : " "), memory[i + (size_t)j]);
            if (n < 0) {
                return;
            }
            pos += (size_t)n;
            if (pos >= sizeof(line)) {
                break;
            }
        }

        if (pos < sizeof(line)) {
            snprintf(line + pos, sizeof(line) - pos, "\n");
        }
        ssize_t written_bytes = write(fd, line, strlen(line));
        (void)written_bytes;
    }
}

char* cv_getfilecontent(const char* filepath){
    if (!filepath || filepath[0] == '\0') {
        return NULL;
    }

    char *expanded_path = cv_expand_home_path(filepath);
    if (!expanded_path) {
        return NULL;
    }

    t_file_rights rights;
    struct stat st;
    if (stat(expanded_path, &st) != 0 || !S_ISREG(st.st_mode)) {
        free(expanded_path);
        return NULL;
    }

    rights = cv_getrights(expanded_path);
    if (rights.is_exist && rights.is_readable){
        FILE *file = fopen(expanded_path, "rb");
        if (!file) {
            free(expanded_path);
            return NULL;
        }

        if (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            free(expanded_path);
            return NULL;
        }
        long size = ftell(file);
        if (size < 0) {
            fclose(file);
            free(expanded_path);
            return NULL;
        }
        rewind(file);
        char *buffer = malloc(size + 1);
        if (!buffer) {
            fclose(file);
            free(expanded_path);
            return NULL;
        }
        size_t bytes_read = fread(buffer, 1, size, file);
        if (bytes_read != (size_t)size) {
            free(buffer);
            fclose(file);
            free(expanded_path);
            return NULL;
        }
        buffer[size] = '\0';
        fclose(file);
        free(expanded_path);
        return buffer;
    }
    free(expanded_path);
    return NULL;
}

char** cv_getdirfiles(const char *dirpath) {
    if (!dirpath) {
        return NULL;
    }

    DIR *dir = opendir(dirpath);
    if (!dir) {
        return NULL;
    }

    struct dirent *entry;
    size_t count = 0;
    size_t capacity = 10;
    char **files = malloc(capacity * sizeof(char*));
    if (!files) {
        closedir(dir);
        return NULL;
    }

    size_t dir_len = strlen(dirpath);
    int needs_slash = (dir_len > 0 && dirpath[dir_len - 1] != '/');

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (count >= capacity - 1) {
            capacity *= 2;
            char **temp = realloc(files, capacity * sizeof(char*));
            if (!temp) {
                for (size_t i = 0; i < count; i++) {
                    free(files[i]);
                }
                free(files);
                closedir(dir);
                return NULL;
            }
            files = temp;
        }

        size_t full_len = dir_len + strlen(entry->d_name) + (needs_slash ? 2 : 1);
        files[count] = malloc(full_len);
        if (!files[count]) {
            for (size_t i = 0; i < count; i++) {
                free(files[i]);
            }
            free(files);
            closedir(dir);
            return NULL;
        }

        if (needs_slash) {
            snprintf(files[count], full_len, "%s/%s", dirpath, entry->d_name);
        } else {
            snprintf(files[count], full_len, "%s%s", dirpath, entry->d_name);
        }
        count++;
    }

    files[count] = NULL;
    closedir(dir);
    return files;
}
