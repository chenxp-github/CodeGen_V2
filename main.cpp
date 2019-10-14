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
#include "test_thread.h"
}

int main(int argc, char **argv)
{
    Mem_Tool_Init("z:\\tmp\\leak.txt");

    struct mem *pmem;

    X_MALLOC(pmem,struct mem, 1);
    PP(pmem);
    mem_init(pmem);
    mem_malloc(pmem,1024);

    struct file_base *pf = &pmem->base_file_base;

    filebase_printf(pf,"Hello world");
    
    
    PP(pf->get_this_pointer(pf));

    filebase_destroy(pf);
    X_VIRTUAL_FREE(pf);


    return 0;
}

