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
}

int main(int argc, char **argv)
{
    Mem_Tool_Init("/tmp/leak.txt");
    
    C_MEM_FILE(mf);
    filebase_printf(mf_file,"Hello");

    C_LOCAL_MEM(mem);
    filebase_printf(mem_file,"World");

    C_COMB_FILE(comb,10);

    combfile_add_file(&comb,mf_file);
    combfile_add_file(&comb,mem_file);

    C_FILE(f);
    file_open_file(&f,"z:\\tmp\\123.txt","wb+");

    filebase_write_file(f_file,comb_file);

    file_destroy(&f);

    combfile_destroy(&comb);

    return 0;
}

