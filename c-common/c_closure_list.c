#include "c_closure_list.h"
#include "syslog.h"
#include "mem_tool.h"

status_t closurelist_copy_node(struct closure_list *self,struct closure *dst, struct closure *src);
struct closure * closurelist_clone_node(struct closure_list *self,struct closure *node);
status_t closurelist_del_node(struct closure_list *self,struct closure *node);
int closurelist_comp_node(struct closure_list *self,struct closure *node1, struct closure *node2);

status_t closurelist_init_basic(struct closure_list *self)
{
    self->data = NULL;
    self->top = 0;;
    self->size = 0;    
    return OK;
}
status_t closurelist_init(struct closure_list *self,int init_size)
{
    int i;
    closurelist_init_basic(self);  
    self->size = init_size;
    X_MALLOC(self->data,struct closure * ,self->size);
    for(i = 0; i < self->size; i++)
        self->data[i] = NULL;
    return OK;
}
status_t closurelist_destroy(struct closure_list *self)
{
    int i;
    if(self == NULL)return ERROR;
    if(self->data == NULL)
        return ERROR;
    for(i = 0; i < self->top; i++)
    {
        closurelist_del_node(self,self->data[i]);
    }
    X_FREE(self->data);
    closurelist_init_basic(self);    
    return OK;
}
bool_t closurelist_is_empty(struct closure_list *self)
{
    return self->top <= 0;
}
bool_t closurelist_is_full(struct closure_list *self)
{
    return self->top >= self->size;
}

status_t closurelist_auto_resize(struct closure_list *self)
{
    int i;

    if(closurelist_is_full(self))
    {
        X_REALLOC(self->data,struct closure*,self->size,self->size*2);
        self->size *= 2;
        for(i = self->top; i < self->size; i++)
        {
            self->data[i] = NULL;
        }
    }
    return OK;
}

status_t closurelist_push(struct closure_list *self,struct closure *node)
{
    struct closure *tmp;
    ASSERT(node);
    tmp = closurelist_clone_node(self,node);
    if(!closurelist_push_ptr(self,tmp))
    {
        closurelist_del_node(self,tmp);
        return ERROR;
    }
    
    return OK;
}

status_t closurelist_push_ptr(struct closure_list *self,struct closure *node)
{
    ASSERT(node);    
    closurelist_auto_resize(self);
    ASSERT(!closurelist_is_full(self));
    self->data[self->top] = node;
    self->top++;
    return OK;
}

struct closure * closurelist_pop_ptr(struct closure_list *self)
{
    if(closurelist_is_empty(self))
        return NULL;
    self->top--;
    return self->data[self->top];
}

status_t closurelist_pop(struct closure_list *self,struct closure *node)
{
    ASSERT(!closurelist_is_empty(self));    
    self->top--;
    closurelist_copy_node(self,node,self->data[self->top]);
    closurelist_del_node(self,self->data[self->top]);
    self->data[self->top] = NULL;
    return OK;
}
int closurelist_get_len(struct closure_list *self)
{
    return self->top;
}

int closurelist_search_pos(struct closure_list *self,struct closure *node)
{
    int i;
    for(i=0;i<self->top;i++)
    {
        if(closurelist_comp_node(self,self->data[i],node) == 0)
            return i;
    }
    return -1;
}

struct closure * closurelist_search(struct closure_list *self,struct closure *node)
{
    int pos = closurelist_search_pos(self,node);
    if(pos >= 0 && pos < self->top)
        return self->data[pos];;
    return NULL;
}
struct closure * closurelist_get_top_ptr(struct closure_list *self)
{
    if(closurelist_is_empty(self))
        return NULL;
    return self->data[self->top - 1];
}
status_t closurelist_del_top(struct closure_list *self)
{
    if(closurelist_is_empty(self))
        return ERROR;
    self->top--;
    closurelist_del_node(self,self->data[self->top]);
    return OK;
}
status_t closurelist_clear(struct closure_list *self)
{
    while(closurelist_del_top(self));
    return OK;
}

struct closure * closurelist_get_elem(struct closure_list *self,int index)
{
    if(index < 0 || index >= self->top)
        return NULL;
    return self->data[index];
}

struct closure * closurelist_bsearch_node(struct closure_list *self,struct closure *node,int order)
{
    return closurelist_get_elem(self,closurelist_bsearch(self,node,order));
}
int closurelist_bsearch(struct closure_list *self,struct closure *node,int order)
{
    int find,pos;
    pos = closurelist_bsearch_pos(self,node,order,&find);
    if(find) return pos;
    return -1;
}

status_t closurelist_insert_elem_ptr(struct closure_list *self,int i, struct closure *node)
{
    int k;

    ASSERT(node);
    ASSERT(i >= 0 && i <= self->top);

    closurelist_auto_resize(self);
    ASSERT(!closurelist_is_full(self));
    for(k = self->top; k > i; k--)
    {
        self->data[k] = self->data[k - 1];
    }
    self->data[i] = node;
    self->top++;
    return OK;
}

status_t closurelist_insert_elem(struct closure_list *self,int i, struct closure *node)
{
    struct closure *tmp;
    ASSERT(node);
    tmp = closurelist_clone_node(self,node);
    if(!closurelist_insert_elem_ptr(self,i,tmp))
    {
        closurelist_del_node(self,tmp);
        return ERROR;
    }
    return OK;
}

struct closure * closurelist_remove_elem(struct closure_list *self,int index)
{
    int k;
    struct closure *p;
    
    ASSERT(index >= 0 && index < self->top);
    p = closurelist_get_elem(self,index);

    for(k = index; k < self->top-1; k++)
    {
        self->data[k] = self->data[k + 1];
    }
    self->top --;
    self->data[self->top] = NULL;
    return p;
}

status_t closurelist_del_elem(struct closure_list *self,int i)
{
    struct closure *p = closurelist_remove_elem(self,i);
    if(p != NULL)
    {
        closurelist_del_node(self,p);
        return OK;
    }
    return ERROR;
}

status_t closurelist_insert_ordered_ptr(struct closure_list *self,struct closure *node, int order,int unique)
{
    int pos,find;
    pos = closurelist_bsearch_pos(self,node,order,&find);
    if(find && unique)
        return ERROR;
    return closurelist_insert_elem_ptr(self,pos,node);
}

status_t closurelist_insert_ordered(struct closure_list *self,struct closure *node, int order,int unique)
{
    int pos,find;
    ASSERT(node);
    pos = closurelist_bsearch_pos(self,node,order,&find);
    if(find && unique)
        return ERROR;
    return closurelist_insert_elem(self,pos,node);
}

int closurelist_bsearch_pos(struct closure_list *self,struct closure *node, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = closurelist_get_len(self) - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = closurelist_comp_node(self,self->data[mid],node);
        if(comp == 0)
        {
            *find_flag = TRUE;
            return mid;
        }
    
        if(order != 0) comp = -comp;
        if(comp>0)high=mid-1;else low=mid+1;
    }
    *find_flag = FALSE;
    return low;
}

status_t closurelist_sort(struct closure_list *self,int order)
{
    int i,len;
    struct closure_list tmp;

    len = closurelist_get_len(self);
    closurelist_init(&tmp,len);

    for(i = 0; i < len; i++)
    {
        closurelist_push_ptr(&tmp,closurelist_get_elem(self,i));
    }

    self->top = 0;
    for(i = 0; i < len; i++)
    {
        closurelist_insert_ordered_ptr(self,closurelist_pop_ptr(&tmp),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.data[i] = NULL;
    }
    
    closurelist_destroy(&tmp);
    return OK;
}
status_t closurelist_copy(struct closure_list *self,struct closure_list *stk)
{
    int i;
    ASSERT(stk);
    closurelist_destroy(self);
    closurelist_init(self,closurelist_get_len(stk));
    for(i = 0; i < closurelist_get_len(stk); i++)
    {
        struct closure *p = closurelist_get_elem(stk,i);
        ASSERT(p);
        closurelist_push(self,p);
    }
    return OK;
}
status_t closurelist_print(struct closure_list *self,struct log_buffer *buf)
{
    int i;

    for(i = 0; i <self->top; i++)
    {
        if(self->data[i])
        {
            log_buffer_log(buf,"[%d]={\r\n",i);
            log_buffer_inc_level(buf,1);
            closure_print(self->data[i],buf);
            log_buffer_inc_level(buf,-1);
            log_buffer_log(buf,"}\r\n");
        }
    }
    log_buffer_log(buf,"size=%d\r\n",self->size);
    log_buffer_log(buf,"top=%d\r\n",self->top);
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t closurelist_copy_node(struct closure_list *self,struct closure *dst, struct closure *src)
{
    return closure_copy(dst,src);
}

struct closure * closurelist_clone_node(struct closure_list *self,struct closure *node)
{
    struct closure *tmp;
    X_MALLOC(tmp,struct closure,1);
    closure_init(tmp);
    closure_copy(tmp,node);
    return tmp;
}

status_t closurelist_del_node(struct closure_list *self,struct closure *node)
{
    closure_destroy(node);
    X_FREE(node);
    return OK;
}

int closurelist_comp_node(struct closure_list *self,struct closure *node1, struct closure *node2)
{
    ASSERT(node1 && node2);
    return closure_comp(node1,node2);
}
