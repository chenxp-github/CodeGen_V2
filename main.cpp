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
#include "c_weak_ptr.h"
#include "c_mem_stk.h"
#include "c_taskmgr.h"
#include "c_task_timer.h"
}

C_BEGIN_CLOSURE_FUNC(on_timer)
{
    LOG("here");
    return OK;
}
C_END_CLOSURE_FUNC(on_timer);

int main(int argc, char **argv)
{
    Mem_Tool_Init("z:\\tmp\\leak.txt");
    
    struct taskmgr mgr;
    taskmgr_init(&mgr,1024);

    struct task_timer *pt = tasktimer_new(&mgr,1000,0);
    closure_set_func(&pt->callback,on_timer);

    while(!kbhit())
    {
        taskmgr_schedule(&mgr);
        crt_msleep(1);
    }

    taskmgr_destroy(&mgr);

    return 0;
}

