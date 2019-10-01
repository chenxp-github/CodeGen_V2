#include "basic_queue.h"
#include "syslog.h"
#include "mem_tool.h"

status_t basic_queue_init_basic(struct basic_queue *self)
{
	self->head = 0;
	self->tail = 0;
	self->data = NULL;
	self->size = 0;
	self->is_full = 0;
	return OK;
}
status_t basic_queue_init(struct basic_queue *self,int size)
{
	basic_queue_init_basic(self);
	X_MALLOC(self->data,double,size);
	self->size = size;
	return OK;
}
status_t basic_queue_destroy(struct basic_queue *self)
{
	if(self == NULL)return ERROR;
	X_FREE(self->data);
	basic_queue_init_basic(self);
	return OK;
}

bool_t basic_queue_is_empty(struct basic_queue *self)
{
	return basic_queue_get_len(self) <= 0;
}

bool_t basic_queue_is_full(struct basic_queue *self)
{
	return self->is_full;
}
status_t basic_queue_clear(struct basic_queue *self)
{
	self->head = 0;
	self->tail = 0;
	self->is_full = 0;
	return OK;
}
status_t basic_queue_enqueue(struct basic_queue *self,double node)
{
	ASSERT(!basic_queue_is_full(self));
	self->data[self->tail] = node;
	self->tail=(self->tail+1) % self->size;	
	if(self->tail == self->head)
		self->is_full = TRUE;

	return OK;
}
status_t basic_queue_dequeue(struct basic_queue *self,double *node)
{
	ASSERT(node && (!basic_queue_is_empty(self)));
	*node = self->data[self->head];
	self->head = (self->head+1) % self->size;
	self->is_full = FALSE;
	return OK;
}

int basic_queue_get_len(struct basic_queue *self)
{
	if(basic_queue_is_full(self))
		return self->size;
	return (self->tail - self->head + self->size) % self->size;
}

status_t basic_queue_print(struct basic_queue *self,struct log_buffer *buf)
{
	int i;
	
	for(i=0;i<self->size;i++)
	{
		//add your code
	}
	
	log_buffer_log(buf,"head=%d",self->head);
	log_buffer_log(buf,"tail=%d",self->tail);
	log_buffer_log(buf,"size=%d",self->size);
	log_buffer_log(buf,"length=%d",basic_queue_get_len(self));
	log_buffer_log(buf,"is_full=%d",self->is_full);
	return TRUE;
}
