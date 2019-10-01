#ifndef __C_TASKMGR_H
#define __C_TASKMGR_H

#include "c_task.h"

struct taskmgr{
    int top,size;
    struct task **index;
    int unique_id;
};

status_t taskmgr_init_basic(struct taskmgr *self);
status_t taskmgr_init(struct taskmgr *self,int init_size);
status_t taskmgr_destroy(struct taskmgr *self);
bool_t taskmgr_is_empty(struct taskmgr *self);
bool_t taskmgr_is_full(struct taskmgr *self);
status_t taskmgr_auto_resize(struct taskmgr *self);
status_t taskmgr_push_ptr(struct taskmgr *self,struct task *node);
struct task * taskmgr_pop_ptr(struct taskmgr *self);
int taskmgr_get_len(struct taskmgr *self);
struct task * taskmgr_get_top_ptr(struct taskmgr *self);
status_t taskmgr_del_top(struct taskmgr *self);
status_t taskmgr_clear(struct taskmgr *self);
struct task * taskmgr_get_elem(struct taskmgr *self,int index);
struct task * taskmgr_bsearch_node(struct taskmgr *self,struct task *node,int order);
int taskmgr_bsearch(struct taskmgr *self,struct task *node,int order);
status_t taskmgr_ins_elem_ptr(struct taskmgr *self,int i, struct task *node);
struct task * taskmgr_remove_elem(struct taskmgr *self,int index);
status_t taskmgr_del_elem(struct taskmgr *self,int i);
status_t taskmgr_ins_ordered_ptr(struct taskmgr *self,struct task *node, int order,int unique);
int taskmgr_bsearch_pos(struct taskmgr *self,struct task *node, int order, int *find_flag);
status_t taskmgr_del_node(struct taskmgr *self,struct task *node);
int taskmgr_id_to_index(struct taskmgr *self, int id);
struct task *taskmgr_get_task(struct taskmgr *self,int id);
status_t taskmgr_add_task(struct taskmgr *self,struct task *task);
int taskmgr_alloc_id(struct taskmgr *self);
status_t taskmgr_schedule_once(struct taskmgr *self);
status_t taskmgr_check_delete(struct taskmgr *self);
status_t taskmgr_schedule(struct taskmgr *self);
status_t taskmgr_suspend_task(struct taskmgr *self,int task_id);
status_t taskmgr_resume_task(struct taskmgr *self,int task_id);
bool_t taskmgr_is_task_running(struct taskmgr *self,int task_id);
status_t taskmgr_del_task(struct taskmgr *self,int task_id);
bool_t taskmgr_is_task(struct taskmgr *self,int tid);
status_t taskmgr_quit_task(struct taskmgr *self,int *task_id);


#endif
