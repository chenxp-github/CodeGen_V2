#ifndef __C_LINKED_LIST_H
#define __C_LINKED_LIST_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "test_node.h"

struct linkedlist_entry{
    struct test_node *raw_ptr;
    struct linkedlist_entry *next,*prev;
};

status_t linkedlist_entry_init(struct linkedlist_entry *self);
status_t linkedlist_entry_destroy(struct linkedlist_entry *self);
status_t linkedlist_entry_set(struct linkedlist_entry *self, struct test_node *node);
struct test_node* linkedlist_entry_get(struct linkedlist_entry *self);

/******************************************************/
struct linked_list{
	struct linkedlist_entry *head,*tail;
	int len;
};

status_t linked_list_init(struct linked_list *self);
status_t linked_list_destroy(struct linked_list *self);
status_t linked_list_add_from_head(struct linked_list *self,struct linkedlist_entry *node);
status_t linked_list_add_from_tail(struct linked_list *self,struct linkedlist_entry *node);
bool_t linked_list_is_empty(struct linked_list *self);
struct linkedlist_entry* linked_list_search_from_head(struct linked_list *self,struct test_node *node);
struct linkedlist_entry* linked_list_search_from_tail(struct linked_list *self,struct test_node *node);
struct linkedlist_entry* linked_list_remove(struct linked_list *self,struct linkedlist_entry *node);
status_t linked_list_delete(struct linked_list *self,struct linkedlist_entry *node);
status_t linked_list_insert_after(struct linked_list *self,struct linkedlist_entry *after, struct linkedlist_entry *node);
status_t linked_list_insert_before(struct linked_list *self,struct linkedlist_entry *before, struct linkedlist_entry *node);
struct linkedlist_entry* linked_list_get_head(struct linked_list *self);
struct linkedlist_entry* linked_list_get_tail(struct linked_list *self);
int linked_list_get_len(struct linked_list *self);
status_t linked_list_del_entry(struct linked_list *self,struct linkedlist_entry *entry);
status_t linked_list_copy(struct linked_list *self,struct linked_list *_list);
status_t linked_list_print(struct linked_list *self,struct log_buffer *buf);

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
    {"struct linked_list","%s","names.c_class_name"},
    {"struct linkedlist_entry","%s","names.c_entry_class_name"},
    {"struct test_node","%s","names.c_node_class_name"},
    {"LINKED_LIST","%s","names.class_name"},
    {"linked_list","%s","names.class_name_lower"},
    {"linkedlist_entry","%s","names.entry_class_name"},
    {"test_node","%s","names.node_class_name_lower"},
};

#endif

