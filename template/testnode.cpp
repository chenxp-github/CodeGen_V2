#include "testnode.h"
#include "syslog.h"
#include "mem_tool.h"

CTestNode::CTestNode()
{
	this->InitBasic();
}
CTestNode::~CTestNode()
{
	this->Destroy();
}
status_t CTestNode::InitBasic()
{
    this->m_m = 0;
	return OK;
}
status_t CTestNode::Init()
{
	this->InitBasic();
	//add your code
	return OK;
}
status_t CTestNode::Destroy()
{
	//add your code
	this->InitBasic();
	return OK;
}

status_t CTestNode::Copy(CTestNode *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

    this->m_m = _p->m_m;
	return OK;
}

status_t CTestNode::Comp(CTestNode *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}
status_t CTestNode::Print(CFileBase *_buf)
{
    _buf->Log("m = %d",m_m);
	return OK;
}
int CTestNode::GetM()
{
    return m_m;
}
status_t CTestNode::SetM(int _m)
{
    this->m_m = _m;
    return OK;
}
status_t CTestNode::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    _bson->PutInt32("m",m_m);
    return OK;
}
status_t CTestNode::SaveBson(CMem *_mem)
{
    ASSERT(_mem);
    CMiniBson _bson;
    _bson.Init();
    _bson.SetRawBuf(_mem);
    _bson.StartDocument();
    this->SaveBson(&_bson);
    _bson.EndDocument();
    _mem->SetSize(_bson.GetDocumentSize());
    return OK;
}
status_t CTestNode::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    BSON_CHECK(_bson->GetInt32("m",&m_m));
    return OK;
}
status_t CTestNode::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}
