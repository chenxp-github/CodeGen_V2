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
    
    C_MEM(mem);
    C_LOCAL_MEM(buf);
      
    filebase_load_file(mem_file,"z:\\tmp\\1.cpp");

    while(filebase_read_line(mem_file,buf_file))
    {
        filebase_trim(buf_file);
        PS(mem_cstr(&buf));
    }

    mem_destroy(&mem);

    return 0;
}

