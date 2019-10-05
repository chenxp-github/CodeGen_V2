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
}

int main(int argc, char **argv)
{
    Mem_Tool_Init("z:\\leak.txt");
	
	struct mem_stk stk;

	memstk_init(&stk,1024);


	memstk_push_str(&stk,"Hello 1");
	memstk_push_str(&stk,"Hello 2");
	memstk_push_str(&stk,"Hello 3");

	C_LOCAL_MEM(mem);
	memstk_save_path(&stk,mem_file);

	PS(mem_cstr(&mem));


	memstk_destroy(&stk);

    return 0;
}

