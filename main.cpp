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
}

static status_t test(struct closure *closure)
{
    C_CLOSURE_PARAM_INT(i,0);
    C_CLOSURE_PARAM_STRING(str,1);
    PD(i);
    PS(str);
    return OK;
}

int main(int argc, char **argv)
{
    Mem_Tool_Init("/tmp/leak.txt");
    
    struct closure c;
    closure_init(&c);
    closure_set_func(&c,test);
    closure_set_param_int(&c,0,123);
    closure_set_param_string(&c,1,"Hello",-1);
    closure_run(&c);

    closure_destroy(&c);

    return 0;
}

