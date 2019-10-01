#include "hash_table.h"
#include "syslog.h"
#include "mem_tool.h"

status_t hashtable_entry_init(struct hashtable_entry *self)
{
    self->next = NULL;
    self->raw_ptr = NULL;
    return OK;
}

status_t hashtable_entry_destroy(struct hashtable_entry *self)
{
    test_node_destroy(self->raw_ptr);
    X_FREE(self->raw_ptr);
    self->next = NULL;
    return OK;
}

status_t hashtable_entry_set(struct hashtable_entry *self, struct test_node *node)
{
    ASSERT(self->raw_ptr == NULL);
    self->raw_ptr = node;
    return OK;
}

struct test_node* hashtable_entry_get(struct hashtable_entry *self)
{
    return self->raw_ptr;
}

/********************************************************************/
bool_t hash_table_equals(struct hash_table *self,struct test_node *node1, struct test_node *node2);
struct test_node* hash_table_clone_node(struct hash_table *self,struct test_node *node);
status_t hash_table_del_entry(struct hash_table *self,struct hashtable_entry *entry);

status_t hash_table_init_basic(struct hash_table *self)
{
	self->capacity = 0;
	self->data = 0;
	self->size = 0;
	return OK;
}
status_t hash_table_init(struct hash_table *self,int capacity)
{
	hash_table_init_basic(self);
	self->capacity = capacity;
	X_MALLOC(self->data,struct hashtable_entry*,capacity);
	memset(self->data,0,capacity*sizeof(struct hashtable_entry*));
	return OK;
}
status_t hash_table_destroy(struct hash_table *self)
{
	int i;
	struct hashtable_entry *q,*p;
	
	if(self == NULL)return ERROR;

	if(self->data == NULL)
		return OK;

	for(i = 0; i < self->capacity; i++)
	{
		p  = self->data[i];
		while(p)
		{
			q = p->next;
			hash_table_del_entry(self,p);
			p = q;			
		}			
	}
	X_FREE(self->data);
	hash_table_init_basic(self);
	return OK;
}

status_t hash_table_put_ptr(struct hash_table *self,struct test_node *ptr)
{
	int code;
	struct hashtable_entry *p,*ptr_entry;

	ASSERT(ptr);

	if(hash_table_get(self,ptr) != NULL)
		return ERROR;

	code = hash_table_hash_code(self,ptr);
	ASSERT(code >= 0 && code < self->capacity);
	p = self->data[code];

    X_MALLOC(ptr_entry,struct hashtable_entry,1);
    hashtable_entry_init(ptr_entry);
    hashtable_entry_set(ptr_entry,ptr);

	if (p == NULL)
	{
		self->data[code] = ptr_entry;
		ptr_entry->next = NULL;
	}
	else
	{
		while(p->next)
		{
			p = p->next;
		}
		p->next = ptr_entry;
		ptr_entry->next = NULL;
	}
	self->size++;
	return OK;
}

status_t hash_table_put(struct hash_table *self,struct test_node *hashentry)
{
	struct test_node *tmp = hash_table_clone_node(self,hashentry);
	if(!hash_table_put_ptr(self,tmp))
	{
		test_node_destroy(tmp);
		return ERROR;
	}
	return OK;
}

struct test_node* hash_table_get(struct hash_table *self,struct test_node *key)
{
	int code;
	struct hashtable_entry *p;

	ASSERT(key);

	code = hash_table_hash_code(self,key);
	ASSERT(code >= 0 && code < self->capacity);
	p = self->data[code];
	while(p)
	{
        struct test_node *node = hashtable_entry_get(p);
		if(hash_table_equals(self,node,key))
        {			
            return node;
        }
		p = p->next;
	}
	return NULL;
}

status_t hash_table_enum_all(struct hash_table *self,struct closure *callback)
{
	int i,_contine;
	struct hashtable_entry *pre,*p,*next;

	ASSERT(callback);
	
	for(i = 0; i < self->capacity; i++)
	{
		p = self->data[i];
		if(p == NULL)continue;

		pre = p;
		p = p->next;
		while(p)
		{			
			next = p->next;
			closure_set_param_pointer(callback,0,p);
			_contine = closure_run(callback);
			if(closure_get_param_pointer(callback,0) == NULL)
			{
				pre->next = next;
				p = next;
				self->size--;
			}
			else
			{
				pre = p;
				p = next;
			}

			if(!_contine)
				goto end;
		}

		p = self->data[i];
		next = p->next;
		closure_set_param_pointer(callback,0,p);
		_contine = closure_run(callback);
		if(closure_get_param_pointer(callback,0) == NULL)
		{
			self->data[i] = next;
			self->size--;
		}
		if(!_contine)
			goto end;
	}

end:
	return OK;
}

struct hashtable_entry* hash_table_remove(struct hash_table *self,struct test_node *key)
{
	int code;
	struct hashtable_entry *pre,*p;

	ASSERT(key);
	code = hash_table_hash_code(self,key);
	if(code < 0 || code >= self->capacity)
		return NULL;
	p = self->data[code];
	if(p == NULL) return NULL;

	if(hash_table_equals(self,hashtable_entry_get(p),key))
	{
		self->data[code] = p->next;
		self->size --;
		return p;
	}
	else
	{
		while(p)
		{
			pre = p;
			p = p->next;
			if(p && hash_table_equals(self,hashtable_entry_get(p),key))
			{
				pre->next = p->next;
				self->size --;
				return p;
			}
		}
	}

	return NULL;
}

status_t hash_table_del(struct hash_table *self,struct test_node *key)
{
	struct hashtable_entry *p = hash_table_remove(self,key);
	if(p != NULL)
	{
		return hash_table_del_entry(self,p);
	}
	return ERROR;
}

int hash_table_get_size(struct hash_table *self)
{
	return self->size;
}

bool_t hash_table_is_empty(struct hash_table *self)
{
	return hash_table_get_size(self) <= 0;
}

status_t hash_table_print(struct hash_table *self,struct log_buffer *buf)
{
	int i;
	int collision = 0;
	int maxLength = 0;

	for(i = 0; i < self->capacity; i++)
	{
		struct hashtable_entry *p = self->data[i];
		if(p != NULL)
		{
			int len = 0;
			collision--;
			while(p)
			{				
				p = p->next;
				len ++;
				collision++;
			}
			if(len > maxLength)
				maxLength = len;
		}
	}

	log_buffer_log(buf,"capacity is %d", self->capacity);
	log_buffer_log(buf,"total size is %d",self->size);
	log_buffer_log(buf,"maximum linked list length is %d",maxLength);
	log_buffer_log(buf,"total collison is %d",collision);
	return OK;
}

status_t hash_table_del_entry(struct hash_table *self,struct hashtable_entry *entry)
{
    hashtable_entry_destroy(entry);
    X_FREE(entry);
    return OK;
}
/*************************************************************************/
/*************************************************************************/
int hash_table_hash_code(struct hash_table *self,struct test_node *node)
{
	ASSERT(node);
	ASSERT(0);
	return 0;
}

bool_t hash_table_equals(struct hash_table *self,struct test_node *node1, struct test_node *node2)
{
	ASSERT(node1 && node2);
	return test_node_comp(node1,node2) == 0;
}

struct test_node* hash_table_clone_node(struct hash_table *self,struct test_node *node)
{
	struct test_node *ptr;
	X_MALLOC(ptr,struct test_node,1);
	test_node_init(ptr);
	test_node_copy(ptr,node);
	return ptr;
}



