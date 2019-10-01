#ifndef __C_BASIC_QUEUE_H
#define __C_BASIC_QUEUE_H

#include "cruntime.h"
#include "c_log_buffer.h"

struct basic_queue{
	double *data;
	int head, tail;
	int size;
	bool_t is_full;
};

status_t basic_queue_init_basic(struct basic_queue *self);
status_t basic_queue_init(struct basic_queue *self,int size);
status_t basic_queue_destroy(struct basic_queue *self);
bool_t basic_queue_is_empty(struct basic_queue *self);
bool_t basic_queue_is_full(struct basic_queue *self);
status_t basic_queue_clear(struct basic_queue *self);
status_t basic_queue_enqueue(struct basic_queue *self,double node);
status_t basic_queue_dequeue(struct basic_queue *self,double *node);
int basic_queue_get_len(struct basic_queue *self);
status_t basic_queue_print(struct basic_queue *self,struct log_buffer *buf);

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
    {"struct basic_queue","%s","names.c_class_name"},
    {"BASIC_QUEUE","%s","names.class_name"},
    {"basic_queue","%s","names.class_name_lower"},
};

#endif

