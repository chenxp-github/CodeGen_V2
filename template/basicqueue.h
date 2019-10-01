#ifndef __T_BASICQUEUE_H
#define __T_BASICQUEUE_H

#include "mem.h"
#include "memfile.h"
#include "minibson.h"

class CBasicQueue{
private:
	double *m_Data;
	int m_Head, m_Tail;
	int m_Size;
	bool m_IsFull;
public:
	int GetLen();
	status_t DeQueue(double *node);
	status_t EnQueue(double node);
    status_t GetElem(int index,double *node);
    status_t GetHead(double *node);    
    status_t GetTail(double *node);
	status_t Clear();
	bool IsFull();
	bool IsEmpty();

	CBasicQueue();
	virtual ~CBasicQueue();
	status_t Init(int size=256);
	status_t Destroy();
    status_t Copy(CBasicQueue *queue);
    int Comp(CBasicQueue *queue);
	status_t Print(CFileBase *_buf);
	status_t InitBasic();
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
    {"CBasicQueue","%s","names.c_class_name"},
    {"BASICQUEUE","%s","names.file_name_upper"},
    {"basicqueue","%s","names.file_name"},
    {"double","%s","names.node_class_name"},
};

////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////


