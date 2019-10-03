#include "syslog.h"
#include "taskmgr.h"
#include "syslog.h"
#include "mem.h"
#include "mem_tool.h"
#include "minibson.h"
#include "memfile.h"
#include "testtree.h"

extern "C"{
#include "task_test.h"
#include "c_closure.h"
#include "c_task_runner.h"
#include "c_mem.h"
#include "c_file_base.h"
#include "c_mem_file.h"
#include "c_comb_file.h"
#include "c_file.h"
#include "c_file_manager.h"
}


C_BEGIN_CLOSURE_FUNC(on_search_dir)
{
    C_CLOSURE_PARAM_INT(e,0);
    if(e == C_EVENT_BEGIN_DIR || e == C_EVENT_SINGLE_FILE)
    {
        C_CLOSURE_PARAM_PTR(char*,file_name,1);
        C_CLOSURE_PARAM_PTR(char*,full_name,2);
        C_CLOSURE_PARAM_INT(is_dir,3);
        C_CLOSURE_PARAM_INT64(size,4);
        C_CLOSURE_PARAM_INT64(last_write_time,5);

        PS(file_name);
        PS(full_name);
        PD(is_dir);
    }
    return OK;
}
C_END_CLOSURE_FUNC(on_search_dir)

int main(int argc, char **argv)
{
    Mem_Tool_Init("/tmp/leak.txt");
    
    struct closure c;
    closure_init(&c);
    closure_set_func(&c,on_search_dir);

    filemanager_search_dir("Z:\\temp\\ftp-cicv",1,&c,NULL);

    return 0;
}

