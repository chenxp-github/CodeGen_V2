#include "c_mem_stk.h"
#include "syslog.h"
#include "mem_tool.h"

status_t memstk_copy_node(struct mem_stk *self,struct mem *dst, struct mem *src);
struct mem * memstk_clone_node(struct mem_stk *self,struct mem *node);
status_t memstk_del_node(struct mem_stk *self,struct mem *node);
int memstk_comp_node(struct mem_stk *self,struct mem *node1, struct mem *node2);

status_t memstk_init_basic(struct mem_stk *self)
{
    self->data = NULL;
    self->top = 0;;
    self->size = 0;    
    return OK;
}
status_t memstk_init(struct mem_stk *self,int init_size)
{
    int i;
    memstk_init_basic(self);  
    self->size = init_size;
    X_MALLOC(self->data,struct mem * ,self->size);
    for(i = 0; i < self->size; i++)
        self->data[i] = NULL;
    return OK;
}
status_t memstk_destroy(struct mem_stk *self)
{
    int i;
    if(self == NULL)return ERROR;
    if(self->data == NULL)
        return ERROR;
    for(i = 0; i < self->top; i++)
    {
        memstk_del_node(self,self->data[i]);
    }
    X_FREE(self->data);
    memstk_init_basic(self);    
    return OK;
}
bool_t memstk_is_empty(struct mem_stk *self)
{
    return self->top <= 0;
}
bool_t memstk_is_full(struct mem_stk *self)
{
    return self->top >= self->size;
}

status_t memstk_auto_resize(struct mem_stk *self)
{
    int i;

    if(memstk_is_full(self))
    {
        X_REALLOC(self->data,struct mem*,self->size,self->size*2);
        self->size *= 2;
        for(i = self->top; i < self->size; i++)
        {
            self->data[i] = NULL;
        }
    }
    return OK;
}

status_t memstk_push(struct mem_stk *self,struct mem *node)
{
    struct mem *tmp;
    ASSERT(node);
    tmp = memstk_clone_node(self,node);
    if(!memstk_push_ptr(self,tmp))
    {
        memstk_del_node(self,tmp);
        return ERROR;
    }
    
    return OK;
}

status_t memstk_push_ptr(struct mem_stk *self,struct mem *node)
{
    ASSERT(node);    
    memstk_auto_resize(self);
    ASSERT(!memstk_is_full(self));
    self->data[self->top] = node;
    self->top++;
    return OK;
}

struct mem * memstk_pop_ptr(struct mem_stk *self)
{
    if(memstk_is_empty(self))
        return NULL;
    self->top--;
    return self->data[self->top];
}

status_t memstk_pop(struct mem_stk *self,struct mem *node)
{
    ASSERT(!memstk_is_empty(self));    
    self->top--;
    memstk_copy_node(self,node,self->data[self->top]);
    memstk_del_node(self,self->data[self->top]);
    self->data[self->top] = NULL;
    return OK;
}
int memstk_get_len(struct mem_stk *self)
{
    return self->top;
}

int memstk_search_pos(struct mem_stk *self,struct mem *node)
{
    int i;
    for(i=0;i<self->top;i++)
    {
        if(memstk_comp_node(self,self->data[i],node) == 0)
            return i;
    }
    return -1;
}

struct mem * memstk_search(struct mem_stk *self,struct mem *node)
{
    int pos = memstk_search_pos(self,node);
    if(pos >= 0 && pos < self->top)
        return self->data[pos];;
    return NULL;
}
struct mem * memstk_get_top_ptr(struct mem_stk *self)
{
    if(memstk_is_empty(self))
        return NULL;
    return self->data[self->top - 1];
}
status_t memstk_del_top(struct mem_stk *self)
{
    if(memstk_is_empty(self))
        return ERROR;
    self->top--;
    memstk_del_node(self,self->data[self->top]);
    return OK;
}
status_t memstk_clear(struct mem_stk *self)
{
    while(memstk_del_top(self));
    return OK;
}

struct mem * memstk_get_elem(struct mem_stk *self,int index)
{
    if(index < 0 || index >= self->top)
        return NULL;
    return self->data[index];
}

struct mem * memstk_bsearch_node(struct mem_stk *self,struct mem *node,int order)
{
    return memstk_get_elem(self,memstk_bsearch(self,node,order));
}
int memstk_bsearch(struct mem_stk *self,struct mem *node,int order)
{
    int find,pos;
    pos = memstk_bsearch_pos(self,node,order,&find);
    if(find) return pos;
    return -1;
}

status_t memstk_insert_elem_ptr(struct mem_stk *self,int i, struct mem *node)
{
    int k;

    ASSERT(node);
    ASSERT(i >= 0 && i <= self->top);

    memstk_auto_resize(self);
    ASSERT(!memstk_is_full(self));
    for(k = self->top; k > i; k--)
    {
        self->data[k] = self->data[k - 1];
    }
    self->data[i] = node;
    self->top++;
    return OK;
}

status_t memstk_insert_elem(struct mem_stk *self,int i, struct mem *node)
{
    struct mem *tmp;
    ASSERT(node);
    tmp = memstk_clone_node(self,node);
    if(!memstk_insert_elem_ptr(self,i,tmp))
    {
        memstk_del_node(self,tmp);
        return ERROR;
    }
    return OK;
}

struct mem * memstk_remove_elem(struct mem_stk *self,int index)
{
    int k;
    struct mem *p;
    
    ASSERT(index >= 0 && index < self->top);
    p = memstk_get_elem(self,index);

    for(k = index; k < self->top-1; k++)
    {
        self->data[k] = self->data[k + 1];
    }
    self->top --;
    self->data[self->top] = NULL;
    return p;
}

status_t memstk_del_elem(struct mem_stk *self,int i)
{
    struct mem *p = memstk_remove_elem(self,i);
    if(p != NULL)
    {
        memstk_del_node(self,p);
        return OK;
    }
    return ERROR;
}

status_t memstk_insert_ordered_ptr(struct mem_stk *self,struct mem *node, int order,int unique)
{
    int pos,find;
    pos = memstk_bsearch_pos(self,node,order,&find);
    if(find && unique)
        return ERROR;
    return memstk_insert_elem_ptr(self,pos,node);
}

status_t memstk_insert_ordered(struct mem_stk *self,struct mem *node, int order,int unique)
{
    int pos,find;
    ASSERT(node);
    pos = memstk_bsearch_pos(self,node,order,&find);
    if(find && unique)
        return ERROR;
    return memstk_insert_elem(self,pos,node);
}

int memstk_bsearch_pos(struct mem_stk *self,struct mem *node, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = memstk_get_len(self) - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = memstk_comp_node(self,self->data[mid],node);
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

status_t memstk_sort(struct mem_stk *self,int order)
{
    int i,len;
    struct mem_stk tmp;

    len = memstk_get_len(self);
    memstk_init(&tmp,len);

    for(i = 0; i < len; i++)
    {
        memstk_push_ptr(&tmp,memstk_get_elem(self,i));
    }

    self->top = 0;
    for(i = 0; i < len; i++)
    {
        memstk_insert_ordered_ptr(self,memstk_pop_ptr(&tmp),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.data[i] = NULL;
    }
    
    memstk_destroy(&tmp);
    return OK;
}
status_t memstk_copy(struct mem_stk *self,struct mem_stk *stk)
{
    int i;
    ASSERT(stk);
    memstk_destroy(self);
    memstk_init(self,memstk_get_len(stk));
    for(i = 0; i < memstk_get_len(stk); i++)
    {
        struct mem *p = memstk_get_elem(stk,i);
        ASSERT(p);
        memstk_push(self,p);
    }
    return OK;
}
status_t memstk_print(struct mem_stk *self,struct log_buffer *buf)
{
    int i;

    for(i = 0; i <self->top; i++)
    {
        if(self->data[i])
        {
            log_buffer_log(buf,"[%d]={\r\n",i);
            log_buffer_inc_level(buf,1);
            log_buffer_puts(buf,mem_cstr(self->data[i]));
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
status_t memstk_copy_node(struct mem_stk *self,struct mem *dst, struct mem *src)
{
    return mem_copy(dst,src);
}

struct mem * memstk_clone_node(struct mem_stk *self,struct mem *node)
{
    struct mem *tmp;
    X_MALLOC(tmp,struct mem,1);
    mem_init(tmp);
    mem_copy(tmp,node);
    return tmp;
}

status_t memstk_del_node(struct mem_stk *self,struct mem *node)
{
    mem_destroy(node);
    X_FREE(node);
    return OK;
}

int memstk_comp_node(struct mem_stk *self,struct mem *node1, struct mem *node2)
{
    ASSERT(node1 && node2);
    return mem_comp(node1,node2);
}

status_t memstk_push_str(struct mem_stk *self,const char *str)
{
	struct mem *tmp;
	ASSERT(str);
	X_MALLOC(tmp,struct mem,1);
	mem_init(tmp);
	mem_set_str(tmp,str);

	if(!memstk_push_ptr(self,tmp))
	{
		memstk_del_node(self,tmp);
		return ERROR;
	}
	return OK;
}

status_t memstk_push_file(struct mem_stk *self,struct file_base *file)
{
	struct mem *new_node;
    ASSERT(file);    	
    X_MALLOC(new_node,struct mem,1);
    mem_init(new_node);
    mem_copy_file(new_node,file);
    if(!memstk_push_ptr(self,new_node))
	{
		memstk_del_node(self,new_node);
		return ERROR;
	}
	return OK;
}


status_t memstk_load_path(struct mem_stk *self,struct file_base* path)
{
    C_LOCAL_MEM(mem);

    filebase_set_split_chars(path,"/\\");
    filebase_seek(path,0);
    
	while(filebase_read_string(path,mem_file))
    {
        if(crt_stricmp(mem_cstr(&mem),"..") == 0)
        {
            memstk_del_top(self);
        }
        else if(crt_stricmp(mem_cstr(&mem),".") != 0)
        {
            memstk_push(self,&mem);
        }   
    }
    return OK;
}

status_t memstk_save_path(struct mem_stk *self,struct file_base *file)
{
	int i;
    struct mem *pstr;

    ASSERT(file);
    
    filebase_set_size(file,0);
    if(memstk_is_empty(self))
    {
        filebase_putc(file,'/');
        return OK;
    }

    for(i = 0 ; i < memstk_get_len(self) ;i++)
    {
        pstr = memstk_get_elem(self,i);
        ASSERT(pstr);
        if(mem_c(pstr,0) != 0)
        {
            filebase_putc(file,'/');
            filebase_write_file(file,&pstr->base_file_base);
        }
    }
    return OK;
}

status_t memstk_load_lines(struct mem_stk *self,struct file_base *file)
{
	C_LOCAL_MEM(mem);
    ASSERT(file);
	
    filebase_seek(file,0);
    while(filebase_read_line(file,mem_file))
    {
        if(mem_c(&mem,0) == 0)continue;
        memstk_push(self,&mem);
    }
    
    return OK;
}

status_t memstk_save_lines(struct mem_stk *self,struct file_base *file)
{
	int i;
    ASSERT(file);
    
    filebase_set_size(file,0);
    for(i = 0; i < memstk_get_len(self); i++)
    {
        struct mem *p = memstk_get_elem(self,i);
        ASSERT(p);
        filebase_write_file(file,&p->base_file_base);
        filebase_puts(file,"\r\n");
    }    
    return OK;
}