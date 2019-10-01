#ifndef __C_BASIC_STACK_H
#define __C_BASIC_STACK_H

#include "cruntime.h"
#include "c_log_buffer.h"

struct basic_stack{
	double *data;
	int top,size;
};

status_t basic_stack_init_basic(struct basic_stack *self);
status_t basic_stack_init(struct basic_stack *self,int init_size);
status_t basic_stack_destroy(struct basic_stack *self);
bool_t basic_stack_is_empty(struct basic_stack *self);
bool_t basic_stack_is_full(struct basic_stack *self);
int basic_stack_get_len(struct basic_stack *self);
status_t basic_stack_clear(struct basic_stack *self);
status_t basic_stack_auto_resize(struct basic_stack *self);
status_t basic_stack_push(struct basic_stack *self,double node);
double basic_stack_pop(struct basic_stack *self);
double basic_stack_get_elem(struct basic_stack *self,int index);
int basic_stack_bsearch_pos(struct basic_stack *self,double node, int order, int *find_flag);
status_t basic_stack_insert_elem(struct basic_stack *self,int index, double node);
status_t basic_stack_del_elem(struct basic_stack *self,int index);
status_t basic_stack_insert_ordered(struct basic_stack *self,double node, int order, int unique);
int basic_stack_search_pos(struct basic_stack *self,double node);
double basic_stack_get_top(struct basic_stack *self);
status_t basic_stack_set_elem(struct basic_stack *self,int index, double node);
status_t basic_stack_sort(struct basic_stack *self,int order);
status_t basic_stack_copy(struct basic_stack *self,struct basic_stack *stk);
status_t basic_stack_print(struct basic_stack *self,struct log_buffer *buf);

#endif

/*******************************************/
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
    {"struct basic_stack","%s","names.c_class_name"},
    {"BASIC_STACK","%s","names.class_name"},
    {"basic_stack","%s","names.class_name_lower"},
};

#endif