#ifndef __T_NORMALQUEUE_H
#define __T_NORMALQUEUE_H

#include "testnode.h"
#include "filebase.h"
#include "minibson.h"

class CNormalQueue{
private:
	CTestNode *m_Data;
	int m_Head, m_Tail;
	int m_Size;
	bool m_IsFull;
public:
	status_t Clear();
	int GetLen();
	CTestNode* EnQueue();
	CTestNode* DeQueue();
    CTestNode* GetHead();    
    CTestNode* GetTail();
    CTestNode* GetElem(int index);
    status_t EnQueue(CTestNode *pos);
	bool IsFull();
	bool IsEmpty();

	CNormalQueue();
	virtual ~CNormalQueue();
	status_t Init(int size=256);
	status_t Destroy();
    status_t Copy(CNormalQueue *queue);
    status_t Comp(CNormalQueue *queue);
	status_t Print(CFileBase *_buf);
	status_t InitBasic();
};

#endif

/////////////////////////////////////////////
#if 0
/////////////////////////////////////////////
--�ָ���--
split_chars=" \t\r\n!@#$%^=&*()|+-\\*/{}[];.\":\',?/<>~";

--�Ƿ�������--
word_only = false;

--�Ƿ��Сд����--
case_sensive = true;

--�������ķ�ʽ--
function output(format_str, param_str)
    local comma = (param_str ~= "") and "," or "";

    printfnl(
         [[printfnl("%s"%s%s);]],
         format_str,comma,param_str
    );
end

--�滻��--
replace_table={
    {"CNormalQueue","%s","names.c_class_name"},
    {"NORMALQUEUE","%s","names.file_name_upper"},
    {"normalqueue","%s","names.file_name"},
    {"CTestNode","%s","names.c_node_class_name"},
    {"testnode","%s","names.node_file_name"},
};

////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////

