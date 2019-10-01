#ifndef __T_LINKEDLIST_H
#define __T_LINKEDLIST_H

#include "cruntime.h"
#include "testnode.h"

class LinkedList_Element{
public:
    CTestNode *m_RawPtr;
    LinkedList_Element *next;
    LinkedList_Element *prev;
public:
    LinkedList_Element();
    ~LinkedList_Element();
    CTestNode *get();
    status_t set(CTestNode *node);
};

class CLinkedList{
private:
	LinkedList_Element *m_Head;
	LinkedList_Element *m_Tail;
	int m_Len;
private:
	int CompNode(CTestNode *node1, CTestNode *node2);
	CTestNode * CloneNode(CTestNode *node);
    status_t DelEntry(LinkedList_Element *entry);
public:
	int GetLen();
	LinkedList_Element* GetTail();
	LinkedList_Element* GetHead();
	status_t InsertBefore(LinkedList_Element *before, LinkedList_Element *node);
	status_t InsertAfter(LinkedList_Element *after,LinkedList_Element *node);
	status_t Delete(LinkedList_Element *node);
	LinkedList_Element* Remove(LinkedList_Element *node);
	LinkedList_Element* SearchFromTail(CTestNode *node);
	LinkedList_Element* SearchFromHead(CTestNode *node);
	bool IsEmpty();
	status_t AddFromHead(LinkedList_Element *node);
	status_t AddFromTail(LinkedList_Element *node);
	CLinkedList();
	virtual ~CLinkedList();
	status_t InitBasic();
	status_t Init();
	status_t Destroy();
	status_t Print(CFileBase *_buf);
	status_t Copy(CLinkedList *_list);
};

#endif

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
    {"LINKEDLIST","%s","names.class_name_upper"},
    {"linkedlist","%s","names.class_name_lower"},
    {"CLinkedList","%s","names.c_class_name"},
    {"LinkedList_Element","%s","names.c_entry_class_name"},
    {"CTestNode","%s","names.c_node_class_name"},
    {"testnode","%s","names.node_class_name_lower"},
};
#endif
