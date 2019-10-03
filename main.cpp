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
}

int main(int argc, char **argv)
{
    Mem_Tool_Init("/tmp/leak.txt");
    
    C_MEM_FILE(mem);

    filebase_load_file(mem_file,"z:\\tmp\\1.cpp");

    filebase_dump(mem_file);

    memfile_destroy(&mem);

    return 0;
}

