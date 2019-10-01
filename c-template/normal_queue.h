#ifndef __C_NORMAL_QUEUUE_H
#define __C_NORMAL_QUEUUE_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "test_node.h"

struct normal_queue{
	struct test_node *data;
	int head, tail;
	int size;
	bool_t is_full;
};

status_t normal_queue_init_basic(struct normal_queue *self);
status_t normal_queue_init(struct normal_queue *self,int size);
status_t normal_queue_destroy(struct normal_queue *self);
bool_t normal_queue_is_empty(struct normal_queue *self);
bool_t normal_queue_is_full(struct normal_queue *self);
status_t normal_queue_clear(struct normal_queue *self);
struct test_node* normal_queue_enqueue(struct normal_queue *self);
struct test_node* normal_queue_dequeue(struct normal_queue *self);
int normal_queue_get_len(struct normal_queue *self);
status_t normal_queue_print(struct normal_queue *self,struct log_buffer *buf);

#endif

/***************************************************************/
#if 0

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
    {"struct normal_queue","%s","names.c_class_name"},
    {"struct test_node","%s","names.c_node_class_name"},
    {"NORMAL_QUEUUE","%s","names.class_name"},
    {"normal_queue","%s","names.class_name_lower"},
    {"test_node","%s","names.node_class_name_lower"},
};

#endif


