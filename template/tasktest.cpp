#include "tasktest.h"
#include "mem_tool.h"
#include "syslog.h"

enum{
    STEP_NONE = 0,
};

#define FIRST_STEP STEP_NONE

CTaskTest::CTaskTest()
{
	this->InitBasic();
}
CTaskTest::~CTaskTest()
{
	this->Destroy();
}
status_t CTaskTest::InitBasic()
{
	CTask::InitBasic();
	this->m_Step = 0;
	this->m_Callback.InitBasic();
	return OK;
}
status_t CTaskTest::Init(CTaskMgr *mgr)
{
	this->Destroy();
	CTask::Init(mgr);
	this->m_Callback.Init();
	return OK;
}
status_t CTaskTest::Destroy()
{
	this->m_Callback.Destroy();
	CTask::Destroy();
	this->InitBasic();
	return OK;
}
const char * CTaskTest::ErrStr(int err)
{
	ERR_STR(ERROR_NONE,"no error");
	return "unknown error";
}
status_t CTaskTest::ReportError(int err)
{
	XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
		"CTaskTest(%d): exit with error:%s",
		this->GetId(),this->ErrStr(err)
	);
	return OK;
}
status_t CTaskTest::Start()
{
	this->Resume();
	GOTO_STATE(0,FIRST_STEP);
	return OK;
}
status_t CTaskTest::Stop(int err)
{
	if(this->IsDead())return OK;
	this->Quit();
	this->ReportError(err);
    
    m_Callback.SetParamInt(1,err);
    m_Callback.SetParamPointer(2,(void*)ErrStr(err));
    m_Callback.Run(EVENT_STOPPED);
	return OK;
}

CClosure* CTaskTest::Callback()
{
	return &this->m_Callback;
}

status_t CTaskTest::Run(uint32_t interval)
{
	BEGIN_FSM();
///////////////
///////////////
	END_FSM();
	return OK;
}
