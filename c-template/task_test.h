#ifndef __TASK_TEST_H
#define __TASK_TEST_H

#include "c_taskmgr.h"

#define TASK_TEST_ERROR_NONE   0

struct task_test{
	struct task base_task;
	int step;
};

void* tasktest_get_this_pointer(struct task_test *self);
status_t tasktest_init_basic(struct task_test *self);
status_t tasktest_init(struct task_test *self,struct taskmgr *mgr);
status_t tasktest_destroy(struct task_test *self);
status_t tasktest_report_error(struct task_test *self,int err);
status_t tasktest_start(struct task_test *self);
status_t tasktest_stop(struct task_test *self,int err);
status_t tasktest_run(struct task_test *self, uint32_t interval);

#endif

/////////////////////////////////////////////
#if 0
/////////////////////////////////////////////
--分隔符--
split_chars=" \t\r\n!@#$%^=&*()|+-\\*/{}[];.\":\',?/<>~";

--是否单字搜索--
word_only = false;

--是否大小写敏感--
case_sensive = true;

--输出结果的方式--
function output(format_str, param_str)
    local comma = (param_str ~= "") and "," or "";
    printfnl(
         [[printfnl("%s"%s%s);]],
         format_str,comma,param_str
    );
end

--替换表--
replace_table={
    {"struct task_test","%s","names.c_class_name"},
    {"TASK_TEST","%s","names.file_name_upper"},
    {"tasktest","%s","names.class_name_lwr"},
};

////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////

