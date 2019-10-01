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
}

int main(int argc, char **argv)
{
    Mem_Tool_Init("/tmp/leak.txt");
        
    struct taskmgr mgr;    
    taskmgr_init(&mgr,1024);
    
    struct task_test *pt;

    MALLOC(pt,struct task_test,1);
    task_test_init(pt,&mgr);
    task_test_start(pt);

    while(!kbhit())
    {
        taskmgr_schedule(&mgr);
        crt_msleep(1);
    }

    taskmgr_destroy(&mgr);
    return 0;
}

