#ifndef __C_NORMAL_STACK_H
#define __C_NORMAL_STACK_H

#include "test_node.h"
#include "c_log_buffer.h"

struct normal_stack{
	int top,size;
	struct test_node **data;
};

status_t normal_stack_init_basic(struct normal_stack *self);
status_t normal_stack_init(struct normal_stack *self, int init_size);
status_t normal_stack_destroy(struct normal_stack *self);
bool_t normal_stack_is_empty(struct normal_stack *self);
bool_t normal_stack_is_full(struct normal_stack *self);
status_t normal_stack_auto_resize(struct normal_stack *self);
status_t normal_stack_push(struct normal_stack *self, struct test_node *node);
status_t normal_stack_push_ptr(struct normal_stack *self,struct test_node *node);
struct test_node * normal_stack_pop_ptr(struct normal_stack *self);
status_t normal_stack_pop(struct normal_stack *self,struct test_node *node);
int normal_stack_get_len(struct normal_stack *self);
int normal_stack_search_pos(struct normal_stack *self,struct test_node *node);
struct test_node* normal_stack_search(struct normal_stack *self,struct test_node *node);
struct test_node* normal_stack_get_top_ptr(struct normal_stack *self);
status_t normal_stack_del_top(struct normal_stack *self);
status_t normal_stack_clear(struct normal_stack *self);
struct test_node * normal_stack_get_elem(struct normal_stack *self,int index);
int normal_stack_bsearch_pos(struct normal_stack *self,struct test_node *node, int order, int *find_flag);
struct test_node * normal_stack_bsearch_node(struct normal_stack *self,struct test_node *node,int order);
int normal_stack_bsearch(struct normal_stack *self,struct test_node *node,int order);
status_t normal_stack_insert_elem_ptr(struct normal_stack *self,int i, struct test_node *node);
status_t normal_stack_insert_elem(struct normal_stack *self,int i, struct test_node *node);
struct test_node * normal_stack_remove_elem(struct normal_stack *self,int index);
status_t normal_stack_del_elem(struct normal_stack *self,int i);
status_t normal_stack_insert_ordered_ptr(struct normal_stack *self,struct test_node *node, int order,int unique);
status_t normal_stack_insert_ordered(struct normal_stack *self,struct test_node *node, int order,int unique);
status_t normal_stack_sort(struct normal_stack *self,int order);
status_t normal_stack_copy(struct normal_stack *self,struct normal_stack *stk);
status_t normal_stack_print(struct normal_stack *self,struct log_buffer *buf);

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
    {"struct normal_stack","%s","names.c_class_name"},
    {"struct test_node","%s","names.c_node_class_name"},
    {"NORMAL_STACK","%s","names.class_name"},
    {"normal_stack","%s","names.class_name_lower"},
    {"test_node","%s","names.node_class_name_lower"},
};

#endif




