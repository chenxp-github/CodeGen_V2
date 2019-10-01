#include "normal_queue.h"
#include "syslog.h"
#include "mem_tool.h"

status_t normal_queue_init_basic(struct normal_queue *self)
{
	self->head = 0;
	self->tail = 0;
	self->data = NULL;
	self->size = 0;
	self->is_full = 0;
	return OK;
}
status_t normal_queue_init(struct normal_queue *self,int size)
{
	int i;
	normal_queue_init_basic(self);
	X_MALLOC(self->data,struct test_node,size);
	for(i = 0; i < size; i++)
	{
		test_node_init(&self->data[i]);
	}
	self->size = size;
	return OK;
}
status_t normal_queue_destroy(struct normal_queue *self)
{
	int i;

	if(self == NULL)
        return ERROR;

	if(self->data)
	{
		for(i = 0; i < self->size; i++)
		{
			test_node_destroy(&self->data[i]);
		}
		X_FREE(self->data);
	}
	normal_queue_init_basic(self);
	return OK;
}

bool_t normal_queue_is_empty(struct normal_queue *self)
{
	return normal_queue_get_len(self) <= 0;
}
bool_t normal_queue_is_full(struct normal_queue *self)
{
	return self->is_full;
}
status_t normal_queue_clear(struct normal_queue *self)
{
	self->head = 0;
	self->tail = 0;
	self->is_full = 0;
	return OK;
}
struct test_node* normal_queue_enqueue(struct normal_queue *self)
{
	struct test_node *ret;
	if(normal_queue_is_full(self))return NULL;
	ret = &self->data[self->tail];
	self->tail=(self->tail+1) % self->size;	
	if(self->tail == self->head)
		self->is_full = TRUE;
	return ret;
}
struct test_node* normal_queue_dequeue(struct normal_queue *self)
{
	struct test_node *ret;
	if(normal_queue_is_empty(self))return NULL;
	ret = &self->data[self->head];
	self->head = (self->head+1) % self->size;
	self->is_full = FALSE;
	return ret;
}
int normal_queue_get_len(struct normal_queue *self)
{
	if(normal_queue_is_full(self))
		return self->size;
	return (self->tail - self->head + self->size) % self->size;
}

status_t normal_queue_print(struct normal_queue *self,struct log_buffer *buf)
{
	int i;
	
	for(i=0;i<self->size;i++)
	{
		log_buffer_log(buf,"{");
        log_buffer_inc_level(buf,1);
		test_node_print(&self->data[i],buf);
        log_buffer_inc_level(buf,-1);
		log_buffer_log(buf,"}<-->");
	}
	
	log_buffer_log(buf,"head=%d",self->head);
	log_buffer_log(buf,"tail=%d",self->tail);
	log_buffer_log(buf,"size=%d",self->size);
	log_buffer_log(buf,"length=%d",normal_queue_get_len(self));
	log_buffer_log(buf,"is_full=%d",self->is_full);
	return OK;
}
