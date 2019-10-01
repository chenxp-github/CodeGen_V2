#ifndef __C_HASH_TABLE_H
#define __C_HASH_TABLE_H

#include "cruntime.h"
#include "c_closure.h"
#include "test_node.h"
#include "c_log_buffer.h"

struct hashtable_entry{
    struct test_node *raw_ptr;
    struct hashtable_entry *next;
};

status_t hashtable_entry_init(struct hashtable_entry *self);
status_t hashtable_entry_destroy(struct hashtable_entry *self);
status_t hashtable_entry_set(struct hashtable_entry *self, struct test_node *node);
struct test_node* hashtable_entry_get(struct hashtable_entry *self);
/*****************************************************/
struct hash_table{
	struct hashtable_entry **data;
	int capacity;
	int size;
};

status_t hash_table_init_basic(struct hash_table *self);
status_t hash_table_init(struct hash_table *self,int capacity);
status_t hash_table_destroy(struct hash_table *self);
status_t hash_table_put_ptr(struct hash_table *self,struct test_node *ptr);
status_t hash_table_put(struct hash_table *self,struct test_node *hashentry);
struct test_node* hash_table_get(struct hash_table *self,struct test_node *key);
status_t hash_table_enum_all(struct hash_table *self,struct closure *callback);
struct hashtable_entry* hash_table_remove(struct hash_table *self,struct test_node *key);
status_t hash_table_del(struct hash_table *self,struct test_node *key);
int hash_table_get_size(struct hash_table *self);
bool_t hash_table_is_empty(struct hash_table *self);
status_t hash_table_print(struct hash_table *self,struct log_buffer *buf);
int hash_table_hash_code(struct hash_table *self,struct test_node *node);

#endif

/***************************************************************/

#if 0

--分隔符--
split_chars=" \t\r\n!@#$%^=&*()|+-\\*/{}[];.\":\',?/<>~";

--是否单字搜索--
word_only = false;

--是否大小写敏感--
case_sensive = true;

--输出结果的方式--
function output(format_str, param_str)
    local comma = (param_str ~= "") and "," or "";
    printfnl(
         [[printfnl("%s"%s%s);]],
         format_str,comma,param_str
    );
end

--替换表--
replace_table={
    {"struct hash_table","%s","names.c_class_name"},
    {"struct hashtable_entry","%s","names.c_entry_class_name"},
    {"struct test_node","%s","names.c_node_class_name"},
    {"HASH_TABLE","%s","names.class_name_upper"},
    {"hash_table","%s","names.class_name_lower"},
    {"hashtable_entry","%s","names.entry_class_name"},
    {"test_node","%s","names.node_class_name_lower"},
};

#endif


