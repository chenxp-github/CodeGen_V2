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
    
    C_LOCAL_MEM(mem);
    
    filebase_write(mem_file,"Hello",5);
    
    C_PRINT_OBJ(mem,mem_print);

    PS(mem_cstr(&mem));
    
    
    mem_realloc(&mem,8000);
    PS(mem_cstr(&mem));
    C_PRINT_OBJ(mem,mem_print);

    mem_destroy(&mem);
    return 0;
}

