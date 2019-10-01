#ifndef __TASKTEST_H
#define __TASKTEST_H

#include "taskmgr.h"
#include "closure.h"

class CTaskTest:public CTask{
public:
	enum{
		ERROR_NONE = 0,
	};
    enum{
        EVENT_NONE = 0,
        EVENT_STOPPED,
    };
	int m_Step;
	CClosure m_Callback;
public:
	CTaskTest();
	virtual ~CTaskTest();
	status_t Init(CTaskMgr *mgr);
	status_t Destroy();
	status_t InitBasic();
	status_t Run(uint32_t interval);
	const char* ErrStr(int err);
	status_t ReportError(int err);	
	status_t Start();
	status_t Stop(int err);
	CClosure* Callback();
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
    {"CTaskTest","%s","names.c_class_name"},
    {"TASKTEST","%s","names.file_name_upper"},
    {"tasktest","%s","names.file_name"},
};

////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////
