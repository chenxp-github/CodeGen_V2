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
}

#define C_WEAK_REF_ID_DEFINE() int __weak_ref_id
#define C_WEAK_REF_ID_INIT(self)  self->__weak_ref_id = crt_get_unique_id()
#define C_WEAK_REF_ID_CLEAR(self) self->__weak_ref_id = 0

struct test{
    C_WEAK_REF_ID_DEFINE();
    int a;
};

status_t test_init_basic(struct test *self)
{
    C_WEAK_REF_ID_CLEAR(self);
    self->a = 0;
    return OK;
}

status_t test_init(struct test *self)
{
    test_init_basic(self);
    C_WEAK_REF_ID_INIT(self);
    return OK;
}

status_t test_destroy(struct test *self)
{
    test_init_basic(self);
    return OK;
}

status_t test_copy(struct test *self,struct test *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;
    
    return OK;
}

status_t test_comp(struct test *self,struct test *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t test_print(struct test *self)
{
    PD(self->a);
    return OK;
}

int main(int argc, char **argv)
{
    Mem_Tool_Init("/tmp/leak.txt");

    struct test t;
    test_init(&t);
    t.a = 123;
    test_print(&t);

    struct weak_ptr wptr;
    weakptr_init(&wptr);
    WEAK_PTR_REF(wptr,&t);

    test_destroy(&t);
    WEAK_PTR_GET(wptr,struct test,p);

    PP(p);

    
    return 0;
}

