#ifndef __C_C_MEM_STK_H
#define __C_C_MEM_STK_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_mem.h"

struct mem_stk{
    int top,size;
    struct mem **data;
};

status_t memstk_init_basic(struct mem_stk *self);
status_t memstk_init(struct mem_stk *self, int init_size);
status_t memstk_destroy(struct mem_stk *self);
bool_t memstk_is_empty(struct mem_stk *self);
bool_t memstk_is_full(struct mem_stk *self);
status_t memstk_auto_resize(struct mem_stk *self);
status_t memstk_push(struct mem_stk *self, struct mem *node);
status_t memstk_push_ptr(struct mem_stk *self,struct mem *node);
struct mem * memstk_pop_ptr(struct mem_stk *self);
status_t memstk_pop(struct mem_stk *self,struct mem *node);
int memstk_get_len(struct mem_stk *self);
int memstk_search_pos(struct mem_stk *self,struct mem *node);
struct mem* memstk_search(struct mem_stk *self,struct mem *node);
struct mem* memstk_get_top_ptr(struct mem_stk *self);
status_t memstk_del_top(struct mem_stk *self);
status_t memstk_clear(struct mem_stk *self);
struct mem * memstk_get_elem(struct mem_stk *self,int index);
int memstk_bsearch_pos(struct mem_stk *self,struct mem *node, int order, int *find_flag);
struct mem * memstk_bsearch_node(struct mem_stk *self,struct mem *node,int order);
int memstk_bsearch(struct mem_stk *self,struct mem *node,int order);
status_t memstk_insert_elem_ptr(struct mem_stk *self,int i, struct mem *node);
status_t memstk_insert_elem(struct mem_stk *self,int i, struct mem *node);
struct mem * memstk_remove_elem(struct mem_stk *self,int index);
status_t memstk_del_elem(struct mem_stk *self,int i);
status_t memstk_insert_ordered_ptr(struct mem_stk *self,struct mem *node, int order,int unique);
status_t memstk_insert_ordered(struct mem_stk *self,struct mem *node, int order,int unique);
status_t memstk_sort(struct mem_stk *self,int order);
status_t memstk_copy(struct mem_stk *self,struct mem_stk *stk);
status_t memstk_print(struct mem_stk *self,struct log_buffer *buf);

status_t memstk_push_str(struct mem_stk *self,const char *str);
status_t memstk_push_file(struct mem_stk *self,struct file_base *file);
status_t memstk_load_path(struct mem_stk *self,struct file_base* path);
status_t memstk_save_path(struct mem_stk *self,struct file_base *file);
status_t memstk_load_lines(struct mem_stk *self,struct file_base *file);
status_t memstk_save_lines(struct mem_stk *self,struct file_base *file);

#endif
