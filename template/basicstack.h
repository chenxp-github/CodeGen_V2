#ifndef __BASICSTACK_H
#define __BASICSTACK_H

#include "mem.h"
#include "memfile.h"
#include "minibson.h"

class CBasicStack{
private:
	double *m_Data;
	int m_Top,m_Size;
public:
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
	int CompNode(double node1, double node2);	
	status_t AutoResize();
	status_t Sort(int order);
	status_t Push(double node);
	double Pop();
	double GetElem(int index);
	int BSearchPos(double node, int order, int *find_flag);
	status_t InsElem(int index, double node);
	status_t DelElem(int index);
	status_t InsOrdered(double node, int order, int unique);
	int SearchPos(double node);
	double GetTop();
	status_t SetElem(int index, double node);
	status_t Clear();
	int GetLen();
	bool IsEmpty();
	bool IsFull();

	CBasicStack();
	~CBasicStack();
    status_t InitBasic();
	status_t Init(int init_size=256);
	status_t Destroy();
	status_t Print(CFileBase *_buf);
    status_t Copy(CBasicStack *stk);	
    int Comp(CBasicStack *stk);
};

#endif

/////////////////////////////////////////////
#if 0
/////////////////////////////////////////////
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
    {"CBasicStack","%s","names.c_class_name"},
    {"BASICSTACK","%s","names.file_name_upper"},
    {"basicstack","%s","names.file_name"},
    {"double","%s","names.node_class_name"},
};

////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////