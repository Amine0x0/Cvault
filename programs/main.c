#include "../include/cvault.h"

#include <stdio.h>

int main(){
    if (_ok()){
        t_file_rights r;

        r = cv_getrights("./main.c");
        printf("current rights: %d %d %d %d\n", r.is_executable, r.is_exist, r.is_readable, r.is_writeable);
    }
}