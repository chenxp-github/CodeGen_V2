#include "basic_stack.h"
#include "mem_tool.h"
#include "syslog.h"

int basic_stack_comp_node(struct basic_stack *self,double node1, double node2);

status_t basic_stack_init_basic(struct basic_stack *self)
{
	self->data = NULL;
	self->top = 0;
	self->size = 0;		
	return OK;
}
status_t basic_stack_init(struct basic_stack *self,int init_size)
{
	basic_stack_init_basic(self);
	X_MALLOC(self->data,double,init_size);
	self->size = init_size;
	return OK;
}
status_t basic_stack_destroy(struct basic_stack *self)
{
	if(self == NULL)return ERROR;
	X_FREE(self->data);
	basic_stack_init_basic(self);
	return OK;
}
bool_t basic_stack_is_empty(struct basic_stack *self)
{
	return self->top <= 0;
}
bool_t basic_stack_is_full(struct basic_stack *self)
{
	return self->top >= self->size;
}
int basic_stack_get_len(struct basic_stack *self)
{
	return self->top;
}
status_t basic_stack_clear(struct basic_stack *self)
{	
	self->top = 0;
	return OK;
}
status_t basic_stack_auto_resize(struct basic_stack *self)
{
	if(basic_stack_is_full(self))
	{
		REALLOC(self->data,double,self->size,self->size * 2);
		self->size *= 2;
	}
	return OK;
}

status_t basic_stack_push(struct basic_stack *self,double node)
{	
	basic_stack_auto_resize(self);
	ASSERT(!basic_stack_is_full(self));
	self->data[self->top]= node;
	self->top ++;	
	return OK;
}

double basic_stack_pop(struct basic_stack *self)
{
	ASSERT(!basic_stack_is_empty(self));
	self->top --;
	return self->data[self->top];
}

double basic_stack_get_elem(struct basic_stack *self,int index)
{
	ASSERT(index >= 0 && index < self->top);
	return self->data[index];
}

int basic_stack_bsearch_pos(struct basic_stack *self,double node, int order, int *find_flag)
{
	int low,high,mid;
	int comp;

	low = 0; high=basic_stack_get_len(self) - 1;
	while(low<=high)
	{
		mid = (low+high) >> 1;
		comp = basic_stack_comp_node(self,self->data[mid],node);
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
status_t basic_stack_insert_elem(struct basic_stack *self,int index, double node)
{
	int k;
	ASSERT(index >= 0 && index <= self->top);	
	basic_stack_auto_resize(self);
	ASSERT(!basic_stack_is_full(self));
	for(k = self->top; k > index; k--)
	{
		self->data[k] = self->data[k - 1];
	}
	self->data[index] = node;
	self->top++;
	return OK;
}
status_t basic_stack_del_elem(struct basic_stack *self,int index)
{
	int k;
	ASSERT(index >= 0 && index < self->top);
	for(k = index; k < self->top-1; k++)
	{
		self->data[k] = self->data[k + 1];
	}
	self->top --;
	return OK;
}
status_t basic_stack_insert_ordered(struct basic_stack *self,double node, int order, int unique)
{
	int pos,find;
	
	pos = basic_stack_bsearch_pos(self,node,order,&find);
	if(find && unique)
		return ERROR;
	
	return basic_stack_insert_elem(self,pos,node);
}
int basic_stack_search_pos(struct basic_stack *self,double node)
{
	int i;
	for(i=0;i<self->top;i++)
	{
		if(basic_stack_comp_node(self,self->data[i],node) == 0)
			return i;
	}
	return -1;
}
double basic_stack_get_top(struct basic_stack *self)
{
	ASSERT(!basic_stack_is_empty(self));
	return self->data[self->top - 1];
}
status_t basic_stack_set_elem(struct basic_stack *self,int index, double node)
{
	ASSERT(index >= 0 && index < self->top);
	self->data[index] = node;
	return OK;
}
status_t basic_stack_sort(struct basic_stack *self,int order)
{
	int i;
	struct basic_stack tmp;

	basic_stack_init(&tmp,basic_stack_get_len(self));
	for(i = 0; i < basic_stack_get_len(self); i++)
	{
		basic_stack_push(&tmp,basic_stack_get_elem(self,i));
	}
	
	self->top = 0;
	for(i = 0; i < basic_stack_get_len(&tmp); i++)
	{
		basic_stack_insert_ordered(self,basic_stack_get_elem(&tmp,i),order,0);
	}
	basic_stack_destroy(&tmp);

	return OK;
}
status_t basic_stack_copy(struct basic_stack *self,struct basic_stack *stk)
{
	int i;
	ASSERT(stk);
	basic_stack_destroy(self);
	basic_stack_init(self,basic_stack_get_len(stk));
	for(i = 0; i < basic_stack_get_len(stk); i++)
	{
		basic_stack_push(self,basic_stack_get_elem(stk,i));
	}
	return OK;
}
/***************************************************/
status_t basic_stack_print(struct basic_stack *self,struct log_buffer *buf)
{
	int i;
	for(i = 0; i< self->top; i++)
	{
		//print your element
	}
	return TRUE;
}

int basic_stack_comp_node(struct basic_stack *self,double node1, double node2)
{
	if(node1 > node2) return 1;
	if(node1 < node2) return -1;
	return 0;
}
