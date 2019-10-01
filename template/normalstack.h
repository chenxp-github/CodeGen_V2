#ifndef __T_NORMALSTACK_H
#define __T_NORMALSTACK_H

#include "mem.h"
#include "testnode.h"
#include "minibson.h"

class CNormalStack{
private:
	CTestNode **m_Index;
	int m_Top;
	int m_Size;
public:
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
	CTestNode* RemoveElem(int index);
	status_t InsElemPtr(int i, CTestNode *node);
	CTestNode* PopPtr();
	status_t AutoResize();
	status_t PushPtr(CTestNode *node);
	status_t InitBasic();  
	int BSearchPos(CTestNode *node,int order,int *find_flag);
	status_t InsOrderedPtr(CTestNode *node,int order,int unique);
	status_t InsOrdered(CTestNode *node,int order,int unique);
	status_t DelElem(int i);
	status_t InsElem(int i,CTestNode *node);
	int BSearch(CTestNode *node,int order);
	CTestNode* BSearchNode(CTestNode *node,int order);
	status_t Sort(int order);
	CTestNode* GetElem(int index);
	CTestNode* GetTopPtr();
	CTestNode* Search(CTestNode *node);
	int SearchPos(CTestNode *node);
    status_t Clear();
    status_t DelTop();
    status_t Pop(CTestNode *node);
    status_t Push(CTestNode *node);
    int GetLen();
    bool IsEmpty();
    bool IsFull();

    int CompNode(CTestNode *node1, CTestNode *node2);
    status_t CopyNode(CTestNode *dst, CTestNode *src);
    status_t DelNode(CTestNode *node);
	CTestNode* CloneNode(CTestNode *node);

	CNormalStack();
    ~CNormalStack();
	status_t Destroy();
	status_t Init(int init_size=256);
	status_t Print(CFileBase *_buf);
  	status_t Copy(CNormalStack *stk);
    int Comp(CNormalStack *stk);
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
    {"CNormalStack","%s","names.c_class_name"},
    {"NORMALSTACK","%s","names.file_name_upper"},
    {"normalstack","%s","names.file_name"},
    {"CTestNode","%s","names.c_node_class_name"},
    {"testnode","%s","names.node_file_name"},
};

////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////