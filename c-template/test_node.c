#include "test_node.h"
#include "syslog.h"
#include "mem_tool.h"

status_t test_node_init_basic(struct test_node *self)
{
	self->m_a = 0;
	return OK;
}
status_t test_node_init(struct test_node *self)
{
	test_node_init_basic(self);
	return OK;
}
status_t test_node_destroy(struct test_node *self)
{
	test_node_init_basic(self);
	return OK;
}
int test_node_comp(struct test_node *self,struct test_node *_p)
{
	ASSERT(_p);
	return 0;
}
status_t test_node_copy(struct test_node *self,struct test_node *_p)
{
	self->m_a = _p->m_a;
	return OK;
}
status_t test_node_print(struct test_node *self,struct log_buffer *buf)
{
	log_buffer_log(buf,"a = %d",self->m_a);
	return OK;
}
int test_node_get_a(struct test_node *self)
{
	return self->m_a;
}
status_t test_node_set_a(struct test_node *self,int _a)
{
	self->m_a = _a;
	return OK;
}
