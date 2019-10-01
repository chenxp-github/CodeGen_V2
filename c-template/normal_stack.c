#include "normal_stack.h"
#include "mem_tool.h"
#include "syslog.h"

status_t normal_stack_copy_node(struct normal_stack *self,struct test_node *dst, struct test_node *src);
struct test_node * normal_stack_clone_node(struct normal_stack *self,struct test_node *node);
status_t normal_stack_del_node(struct normal_stack *self,struct test_node *node);
int normal_stack_comp_node(struct normal_stack *self,struct test_node *node1, struct test_node *node2);

status_t normal_stack_init_basic(struct normal_stack *self)
{
	self->data = NULL;
	self->top = 0;;
	self->size = 0;	
	return OK;
}
status_t normal_stack_init(struct normal_stack *self,int init_size)
{
	int i;
	normal_stack_init_basic(self);  
	self->size = init_size;
	X_MALLOC(self->data,struct test_node * ,self->size);
	for(i = 0; i < self->size; i++)
		self->data[i] = NULL;
	return OK;
}
status_t normal_stack_destroy(struct normal_stack *self)
{
	int i;
	if(self == NULL)return ERROR;
	if(self->data == NULL)
		return ERROR;
	for(i = 0; i < self->top; i++)
	{
		normal_stack_del_node(self,self->data[i]);
	}
	X_FREE(self->data);
	normal_stack_init_basic(self);	
	return OK;
}
bool_t normal_stack_is_empty(struct normal_stack *self)
{
	return self->top <= 0;
}
bool_t normal_stack_is_full(struct normal_stack *self)
{
	return self->top >= self->size;
}

status_t normal_stack_auto_resize(struct normal_stack *self)
{
	int i;

	if(normal_stack_is_full(self))
	{
		X_REALLOC(self->data,struct test_node*,self->size,self->size*2);
		self->size *= 2;
		for(i = self->top; i < self->size; i++)
		{
			self->data[i] = NULL;
		}
	}
	return OK;
}

status_t normal_stack_push(struct normal_stack *self,struct test_node *node)
{
	struct test_node *tmp;
	ASSERT(node);
	tmp = normal_stack_clone_node(self,node);
	if(!normal_stack_push_ptr(self,tmp))
	{
		normal_stack_del_node(self,tmp);
		return ERROR;
	}
	
	return OK;
}

status_t normal_stack_push_ptr(struct normal_stack *self,struct test_node *node)
{
	ASSERT(node);	
	normal_stack_auto_resize(self);
	ASSERT(!normal_stack_is_full(self));
	self->data[self->top] = node;
	self->top++;
	return OK;
}

struct test_node * normal_stack_pop_ptr(struct normal_stack *self)
{
	if(normal_stack_is_empty(self))
		return NULL;
	self->top--;
	return self->data[self->top];
}

status_t normal_stack_pop(struct normal_stack *self,struct test_node *node)
{
	ASSERT(!normal_stack_is_empty(self));	
	self->top--;
	normal_stack_copy_node(self,node,self->data[self->top]);
	normal_stack_del_node(self,self->data[self->top]);
	self->data[self->top] = NULL;
	return OK;
}
int normal_stack_get_len(struct normal_stack *self)
{
	return self->top;
}

int normal_stack_search_pos(struct normal_stack *self,struct test_node *node)
{
	int i;
	for(i=0;i<self->top;i++)
	{
		if(normal_stack_comp_node(self,self->data[i],node) == 0)
			return i;
	}
	return -1;
}

struct test_node * normal_stack_search(struct normal_stack *self,struct test_node *node)
{
	int pos = normal_stack_search_pos(self,node);
	if(pos >= 0 && pos < self->top)
		return self->data[pos];;
	return NULL;
}
struct test_node * normal_stack_get_top_ptr(struct normal_stack *self)
{
	if(normal_stack_is_empty(self))
		return NULL;
	return self->data[self->top - 1];
}
status_t normal_stack_del_top(struct normal_stack *self)
{
	if(normal_stack_is_empty(self))
		return ERROR;
	self->top--;
	normal_stack_del_node(self,self->data[self->top]);
	return OK;
}
status_t normal_stack_clear(struct normal_stack *self)
{
	while(normal_stack_del_top(self));
	return OK;
}

struct test_node * normal_stack_get_elem(struct normal_stack *self,int index)
{
	if(index < 0 || index >= self->top)
		return NULL;
	return self->data[index];
}

struct test_node * normal_stack_bsearch_node(struct normal_stack *self,struct test_node *node,int order)
{
	return normal_stack_get_elem(self,normal_stack_bsearch(self,node,order));
}
int normal_stack_bsearch(struct normal_stack *self,struct test_node *node,int order)
{
	int find,pos;
	pos = normal_stack_bsearch_pos(self,node,order,&find);
	if(find) return pos;
	return -1;
}

status_t normal_stack_insert_elem_ptr(struct normal_stack *self,int i, struct test_node *node)
{
	int k;

	ASSERT(node);
	ASSERT(i >= 0 && i <= self->top);

	normal_stack_auto_resize(self);
	ASSERT(!normal_stack_is_full(self));
	for(k = self->top; k > i; k--)
	{
		self->data[k] = self->data[k - 1];
	}
	self->data[i] = node;
	self->top++;
	return OK;
}

status_t normal_stack_insert_elem(struct normal_stack *self,int i, struct test_node *node)
{
	struct test_node *tmp;
	ASSERT(node);
	tmp = normal_stack_clone_node(self,node);
	if(!normal_stack_insert_elem_ptr(self,i,tmp))
	{
		normal_stack_del_node(self,tmp);
		return ERROR;
	}
	return OK;
}

struct test_node * normal_stack_remove_elem(struct normal_stack *self,int index)
{
	int k;
	struct test_node *p;
	
	ASSERT(index >= 0 && index < self->top);
	p = normal_stack_get_elem(self,index);

	for(k = index; k < self->top-1; k++)
	{
		self->data[k] = self->data[k + 1];
	}
	self->top --;
	self->data[self->top] = NULL;
	return p;
}

status_t normal_stack_del_elem(struct normal_stack *self,int i)
{
	struct test_node *p = normal_stack_remove_elem(self,i);
	if(p != NULL)
	{
		normal_stack_del_node(self,p);
		return OK;
	}
	return ERROR;
}

status_t normal_stack_insert_ordered_ptr(struct normal_stack *self,struct test_node *node, int order,int unique)
{
	int pos,find;
	pos = normal_stack_bsearch_pos(self,node,order,&find);
	if(find && unique)
		return ERROR;
	return normal_stack_insert_elem_ptr(self,pos,node);
}

status_t normal_stack_insert_ordered(struct normal_stack *self,struct test_node *node, int order,int unique)
{
	int pos,find;
	ASSERT(node);
	pos = normal_stack_bsearch_pos(self,node,order,&find);
	if(find && unique)
		return ERROR;
	return normal_stack_insert_elem(self,pos,node);
}

int normal_stack_bsearch_pos(struct normal_stack *self,struct test_node *node, int order, int *find_flag)
{
	int low,high,mid,comp;
	
	low = 0; 
	high = normal_stack_get_len(self) - 1;

	while(low<=high)
	{
		mid = (low+high) >> 1;

		comp = normal_stack_comp_node(self,self->data[mid],node);
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

status_t normal_stack_sort(struct normal_stack *self,int order)
{
	int i,len;
	struct normal_stack tmp;

	len = normal_stack_get_len(self);
	normal_stack_init(&tmp,len);

	for(i = 0; i < len; i++)
	{
		normal_stack_push_ptr(&tmp,normal_stack_get_elem(self,i));
	}

	self->top = 0;
	for(i = 0; i < len; i++)
	{
		normal_stack_insert_ordered_ptr(self,normal_stack_pop_ptr(&tmp),order,0);
	}
	
	for(i = 0; i < len; i++)
	{
		tmp.data[i] = NULL;
	}
	
	normal_stack_destroy(&tmp);
	return OK;
}
status_t normal_stack_copy(struct normal_stack *self,struct normal_stack *stk)
{
	int i;
	ASSERT(stk);
	normal_stack_destroy(self);
	normal_stack_init(self,normal_stack_get_len(stk));
	for(i = 0; i < normal_stack_get_len(stk); i++)
	{
		struct test_node *p = normal_stack_get_elem(stk,i);
		ASSERT(p);
		normal_stack_push(self,p);
	}
	return OK;
}
status_t normal_stack_print(struct normal_stack *self,struct log_buffer *buf)
{
	int i;

	for(i = 0; i <self->top; i++)
	{
		if(self->data[i])
		{
			log_buffer_log(buf,"[%d]={\r\n",i);
            log_buffer_inc_level(buf,1);
			test_node_print(self->data[i],buf);
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
status_t normal_stack_copy_node(struct normal_stack *self,struct test_node *dst, struct test_node *src)
{
	return test_node_copy(dst,src);
}

struct test_node * normal_stack_clone_node(struct normal_stack *self,struct test_node *node)
{
	struct test_node *tmp;
	X_MALLOC(tmp,struct test_node,1);
	test_node_init(tmp);
	test_node_copy(tmp,node);
	return tmp;
}

status_t normal_stack_del_node(struct normal_stack *self,struct test_node *node)
{
	test_node_destroy(node);
	X_FREE(node);
	return OK;
}

int normal_stack_comp_node(struct normal_stack *self,struct test_node *node1, struct test_node *node2)
{
	ASSERT(node1 && node2);
	return test_node_comp(node1,node2);
}
