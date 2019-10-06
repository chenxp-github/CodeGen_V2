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
    Mem_Tool_Init("z:\\tmp\\leak.txt");

	C_LOCAL_MEM(mem);

	mem_strcpy(&mem,"/home/chenxp/temp/../../123.txt");	
	
	filemanager_to_abs_path(&mem);





	PS(mem_cstr(&mem));
	


    return 0;
}

