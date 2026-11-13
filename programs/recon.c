#include "../include/cvault.h"

#define SLNT_EXT 0

int wm = LOG_SILENT;

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
                int ignored = cv_createfile("/dev/shm/.done", 0);
            }
        }
        return SLNT_EXT;
}