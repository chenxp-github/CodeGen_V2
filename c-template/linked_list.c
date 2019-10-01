#include "linked_list.h"
#include "syslog.h"
#include "mem_tool.h"

status_t linkedlist_entry_init(struct linkedlist_entry *self)
{
    self->next = NULL;
    self->raw_ptr = NULL;
    return OK;
}

status_t linkedlist_entry_destroy(struct linkedlist_entry *self)
{
    test_node_destroy(self->raw_ptr);
    X_FREE(self->raw_ptr);
    self->next = NULL;
    return OK;
}

status_t linkedlist_entry_set(struct linkedlist_entry *self, struct test_node *node)
{
    ASSERT(self->raw_ptr == NULL);
    self->raw_ptr = node;
    return OK;
}

struct test_node* linkedlist_entry_get(struct linkedlist_entry *self)
{
    return self->raw_ptr;
}
/********************************************************************/
struct test_node* linked_list_clone_node(struct linked_list *self,struct test_node *node);
int linked_list_comp_node(struct linked_list *self,struct test_node *node1, struct test_node *node2);

status_t linked_list_init_basic(struct linked_list *self)
{
	self->head = NULL;
	self->tail = NULL;
	self->len = 0;
	return OK;
}
status_t linked_list_init(struct linked_list *self)
{
	linked_list_init_basic(self);
	//add your code
	return OK;
}
status_t linked_list_destroy(struct linked_list *self)
{
	struct linkedlist_entry *q,*p = self->head;
	while(p)
	{
		q = p;
		p = p->next;
		linked_list_del_entry(self,q);
	}
	linked_list_init_basic(self);
	return OK;
}

status_t linked_list_add_from_head(struct linked_list *self,struct linkedlist_entry *node)
{
	ASSERT(node);
	if(self->head)
		self->head->prev = node;
	node->next = self->head;
	self->head = node;
	if(self->tail == NULL)
		self->tail = node;
	self->len++;
	return OK;
}
status_t linked_list_add_from_tail(struct linked_list *self,struct linkedlist_entry *node)
{
	ASSERT(node);
	if(self->tail)
		self->tail->next = node;
	node->prev = self->tail;
	self->tail = node;
	if(self->head == NULL)
		self->head = node;
	self->len++;
	return OK;
}
bool_t linked_list_is_empty(struct linked_list *self)
{
	return self->head == NULL && self->tail == NULL;
}

struct linkedlist_entry* linked_list_search_from_head(struct linked_list *self,struct test_node *node)
{
	struct linkedlist_entry *p = self->head;
	while(p)
	{
		if(linked_list_comp_node(self,linkedlist_entry_get(p),node) == 0)
        {
			return p;
        }
		p = p->next;
	}
	return NULL;
}

struct linkedlist_entry* linked_list_search_from_tail(struct linked_list *self,struct test_node *node)
{
	struct linkedlist_entry *p = self->tail;
	while(p)
	{
		if(linked_list_comp_node(self,linkedlist_entry_get(p),node) == 0)
			return p;
		p = p->prev;
	}
	return NULL;	
}

struct linkedlist_entry* linked_list_remove(struct linked_list *self,struct linkedlist_entry *node)
{
	ASSERT(node);

	if(node->prev)
		node->prev->next = node->next;
	else
		self->head = node->next;

	if(node->next)
		node->next->prev = node->prev;
	else
		self->tail = node->prev;

	self->len--;
	node->prev = NULL;
	node->next = NULL;
	return node;
}

status_t linked_list_delete(struct linked_list *self,struct linkedlist_entry *node)
{
	return linked_list_del_entry(self,linked_list_remove(self,node));
}

status_t linked_list_insert_after(struct linked_list *self,struct linkedlist_entry *after, struct linkedlist_entry *node)
{
	ASSERT(node && after);
	node->next = after->next;
	if(after->next)
		after->next->prev = node;
	node->prev = after;
	after->next = node;
	if(after == self->tail)
		self->tail = node;
	self->len++;
	return OK;
}

status_t linked_list_insert_before(struct linked_list *self,struct linkedlist_entry *before, struct linkedlist_entry *node)
{
	ASSERT(node && before);
	node->prev = before->prev;
	if(before->prev)
		before->prev->next = node;
	node->next = before;
	before->prev = node;
	if(before == self->head)
		self->head = node;
	self->len++;
	return OK;
}

struct linkedlist_entry* linked_list_get_head(struct linked_list *self)
{
	return self->head;
}

struct linkedlist_entry* linked_list_get_tail(struct linked_list *self)
{
	return self->tail;
}

int linked_list_get_len(struct linked_list *self)
{
	return self->len;
}
status_t linked_list_copy(struct linked_list *self,struct linked_list *_list)
{
	struct linkedlist_entry *p;
	
	ASSERT(_list);
	
	linked_list_destroy(self);
	linked_list_init(self);
	
	p = _list->head;
	while(p)
	{
        struct test_node *node = linked_list_clone_node(self,
            linkedlist_entry_get(p));
        struct linkedlist_entry *entry;
        X_MALLOC(entry,struct linkedlist_entry,1);
        linkedlist_entry_set(entry,node);

		if(!linked_list_add_from_tail(self,entry))
        {
            linked_list_del_entry(self,entry);
            return ERROR;
        }
		p = p->next;
	}
	
	return OK;
}

status_t linked_list_print(struct linked_list *self,struct log_buffer *buf)
{
	struct linkedlist_entry *p;
	p = self->head;
	while(p)
	{
		log_buffer_log(buf,"{");
        log_buffer_inc_level(buf,1);
		test_node_print(linkedlist_entry_get(p),buf);
        log_buffer_inc_level(buf,-1);
		log_buffer_log(buf,"}->");
		p = p->next;
	}
	log_buffer_log(buf,"len = %d",self->len);
	return OK;
}

status_t linked_list_del_entry(struct linked_list *self,struct linkedlist_entry *entry)
{
    if(entry == NULL)return ERROR;
    linkedlist_entry_destroy(entry);
    X_FREE(entry);
    return OK;
}
/*************************************************/
int linked_list_comp_node(struct linked_list *self,struct test_node *node1, struct test_node *node2)
{
	ASSERT(node1 && node2);
	return test_node_comp(node1,node2);
}

struct test_node* linked_list_clone_node(struct linked_list *self,struct test_node *node)
{
	struct test_node *tmp;
	ASSERT(node);

	X_MALLOC(tmp,struct test_node,1);
	test_node_init(tmp);
	test_node_copy(tmp,node);

	return tmp;
}

