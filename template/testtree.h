#ifndef __TESTTREE_H
#define __TESTTREE_H

#include "filebase.h"
#include "closure.h"
#include "testnode.h"

class TestTree_Entry{
public:
    CTestNode *m_RawPtr;
public:
    TestTree_Entry *parent,*child,*next,*prev;
public:
    TestTree_Entry();
    ~TestTree_Entry();
    CTestNode *get();
    status_t set(CTestNode *node);  
    status_t Traverse(CClosure *closure);
    
    static status_t DelNode_Recursive(TestTree_Entry *node);
    static status_t Print_Recursive(TestTree_Entry *node, CFileBase *buf);    
    static TestTree_Entry* NewFromPointer(CTestNode *node);
    static TestTree_Entry* New(CTestNode *node);

    status_t InsertBefore(TestTree_Entry *node);
    status_t InsertAfter(TestTree_Entry *node);
    status_t AddChild(TestTree_Entry *node);
    status_t RemoveFromTheTree();

    status_t SaveBson(CMiniBson *_bson);
    status_t LoadBson(CMiniBson *_bson);
};

class CTestTree{
private:
    TestTree_Entry *m_Root;
public:
	CTestTree();
	virtual ~CTestTree();
	status_t InitBasic();
	status_t Init();
	status_t Destroy();
	status_t Copy(CTestTree *p);
	status_t Comp(CTestTree *p);
	status_t Print(CFileBase *_buf);
    
    TestTree_Entry* GetRoot();
    status_t SetRoot(TestTree_Entry *root);    

    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
};

#endif



//////////////////////////////////////////////////////////////////
#if 0  //lua��������printf�ķ���
//////////////////////////////////////////////////////////////////

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
    {"TESTTREE","%s","names.class_name_upper"},
    {"testtree","%s","names.class_name_lower"},
    {"CTestTree","%s","names.c_class_name"},
    {"TestTree_Entry","%s","names.c_entry_class_name"},
    {"CTestNode","%s","names.c_node_class_name"},
    {"testnode","%s","names.node_class_name_lower"},
};

//////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////
