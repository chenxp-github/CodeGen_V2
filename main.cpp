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
#include "c_simple_link_service.h"
}

int main(int argc, char **argv)
{
    crt_socket_start();
    Mem_Tool_Init("z:\\tmp\\leak.txt");
    
    struct taskmgr mgr;
    taskmgr_init(&mgr,1024);

    struct simple_link_service ls;

    simplelinkservice_init(&ls,&mgr,0);
    simplelinkservice_set_server_name(&ls,"127.0.0.1");
    simplelinkservice_set_port(&ls,8001);
    simplelinkservice_start(&ls);

    while(!kbhit())
    {
        taskmgr_schedule(&mgr);
        crt_msleep(10);
    }

    taskmgr_destroy(&mgr);
    crt_socket_end();
    return 0;
}

