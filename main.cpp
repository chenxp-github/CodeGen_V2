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
}

int main(int argc, char **argv)
{
    Mem_Tool_Init("/tmp/leak.txt");
    
    C_LOCAL_MEM(mem);

    filebase_printf(mem_file,"Hello %d",123);


    filebase_save_file(mem_file,"z:\\tmp\\123.txt");

    mem_destroy(&mem);

    return 0;
}

