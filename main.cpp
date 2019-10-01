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
}

C_BEGIN_CLOSURE_FUNC(test_func)
{
    C_CLOSURE_PARAM_INT(e,0);
    PD(e);
    return OK;
}
C_END_CLOSURE_FUNC(test_func)


int main(int argc, char **argv)
{
    Mem_Tool_Init("/tmp/leak.txt");

    struct task_runner runner;
    taskrunner_init(&runner);
        
    C_NEW_CLOSURE(pc,test_func);
    closure_set_param_int(pc,0,123);
    
    taskrunner_add_closure(&runner,pc,0);

    while(!kbhit())
    {
        taskrunner_schedule(&runner);
        crt_msleep(1);
    }

    taskrunner_destroy(&runner);
    return 0;
}

