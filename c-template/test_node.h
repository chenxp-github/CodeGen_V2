#ifndef __S2_TEST_NODE_H
#define __S2_TEST_NODE_H

#include "cruntime.h"
#include "c_log_buffer.h"

struct test_node{
	int m_a;    
};

status_t test_node_init_basic(struct test_node *self);
status_t test_node_init(struct test_node *self);
status_t test_node_destroy(struct test_node *self);
status_t test_node_copy(struct test_node *self,struct test_node *_p);
int test_node_comp(struct test_node *self,struct test_node *_p);
status_t test_node_print(struct test_node *self,struct log_buffer *buf);
int test_node_get_a(struct test_node *self);
status_t test_node_set_a(struct test_node *self,int _a);

#endif
