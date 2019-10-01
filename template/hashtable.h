#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include "filebase.h"
#include "closure.h"
#include "testnode.h"

namespace test_name_space{

class TestNode_HashEntry{
public:
    CTestNode *m_RawPtr;
    TestNode_HashEntry *next;
public:
    TestNode_HashEntry();
    ~TestNode_HashEntry();
    CTestNode *get();
    status_t set(CTestNode *node);
};

class CHashTable{
private:
	TestNode_HashEntry **m_Data;
	int m_Capacity;
	int m_Size;
public:	
	status_t Clear();
	status_t DiscardAll();
	bool IsEmpty();
	int GetSize();
	int GetCapacity();
	status_t Del(CTestNode *key);
	TestNode_HashEntry* Remove(CTestNode *key);
	status_t EnumAll(CClosure *closure);		
	CTestNode* Get(CTestNode *key);	
	status_t Put(CTestNode *hashentry);
	status_t PutPtr(CTestNode *ptr);
	int HashCode(CTestNode *hashentry);
    status_t ToArray(CTestNode *arr[], int *len);

	static int HashCode(CTestNode *hashentry,int capacity);
	static bool Equals(CTestNode *hashentry1, CTestNode *hashentry2);	
	static CTestNode * CloneNode(CTestNode *hashentry);

	CHashTable();
	virtual ~CHashTable();
	status_t InitBasic();
	status_t Init(int capacity=1024);
	status_t Destroy();
	status_t Copy(CHashTable *p);
	status_t Comp(CHashTable *p);
	status_t Print(CFileBase *_buf);	
};

} //namespace test_name_space

#endif

//////////////////////////////////////////////////////////////////
#if 0  //lua工具生成printf的方法
//////////////////////////////////////////////////////////////////

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
    {"HASHTABLE","%s","names.class_name_upper"},
    {"hashtable","%s","names.class_name_lower"},
    {"CHashTable","%s","names.c_class_name"},
    {"TestNode_HashEntry","%s","names.c_entry_class_name"},
    {"CTestNode","%s","names.c_node_class_name"},
    {"testnode","%s","names.node_class_name_lower"},
    {"test_name_space","%s","names.name_space"},
};

//////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////