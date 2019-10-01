#ifndef __C_C_CLOSURE_LIST_H
#define __C_C_CLOSURE_LIST_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_closure.h"

struct closure_list{
    int top,size;
    struct closure **data;
};

status_t closurelist_init_basic(struct closure_list *self);
status_t closurelist_init(struct closure_list *self, int init_size);
status_t closurelist_destroy(struct closure_list *self);
bool_t closurelist_is_empty(struct closure_list *self);
bool_t closurelist_is_full(struct closure_list *self);
status_t closurelist_auto_resize(struct closure_list *self);
status_t closurelist_push(struct closure_list *self, struct closure *node);
status_t closurelist_push_ptr(struct closure_list *self,struct closure *node);
struct closure * closurelist_pop_ptr(struct closure_list *self);
status_t closurelist_pop(struct closure_list *self,struct closure *node);
int closurelist_get_len(struct closure_list *self);
int closurelist_search_pos(struct closure_list *self,struct closure *node);
struct closure* closurelist_search(struct closure_list *self,struct closure *node);
struct closure* closurelist_get_top_ptr(struct closure_list *self);
status_t closurelist_del_top(struct closure_list *self);
status_t closurelist_clear(struct closure_list *self);
struct closure * closurelist_get_elem(struct closure_list *self,int index);
int closurelist_bsearch_pos(struct closure_list *self,struct closure *node, int order, int *find_flag);
struct closure * closurelist_bsearch_node(struct closure_list *self,struct closure *node,int order);
int closurelist_bsearch(struct closure_list *self,struct closure *node,int order);
status_t closurelist_insert_elem_ptr(struct closure_list *self,int i, struct closure *node);
status_t closurelist_insert_elem(struct closure_list *self,int i, struct closure *node);
struct closure * closurelist_remove_elem(struct closure_list *self,int index);
status_t closurelist_del_elem(struct closure_list *self,int i);
status_t closurelist_insert_ordered_ptr(struct closure_list *self,struct closure *node, int order,int unique);
status_t closurelist_insert_ordered(struct closure_list *self,struct closure *node, int order,int unique);
status_t closurelist_sort(struct closure_list *self,int order);
status_t closurelist_copy(struct closure_list *self,struct closure_list *stk);
status_t closurelist_print(struct closure_list *self,struct log_buffer *buf);

#endif
