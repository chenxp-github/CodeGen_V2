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
status_t CTestNode::LoadXml(const char *fn, const char *path)
{
    ASSERT(fn && path);
	
    CXml xml;
    xml.Init();
    ASSERT(xml.LoadXml(fn));
	
    CXmlNode *px = xml.GetNodeByPath(path);
    ASSERT(px);
	
    return this->LoadXml(px);
}

status_t CTestNode::SaveXml(const char *fn, const char *node_name)
{
    ASSERT(fn && node_name);
	
    CMemFile mf;
    mf.Init();
    mf.Log("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    this->SaveXml(&mf,node_name);
	
    return mf.WriteToFile(fn) > 0;
}

status_t CTestNode::LoadXml(CXmlNode *_root)
{
    ASSERT(_root);
    CXmlNode *px = _root;
	
    LOCAL_MEM(name);
    LOCAL_MEM(val);
	
    px->RestartAttrib();
    while(px->GetNextAttrib(&name,&val))
    {
    }
	
    px = px->child;
    while(px)
    {
        px = px->next;
    }
    return OK;
}

status_t CTestNode::SaveXml(CFileBase *_xml,const char *node_name)
{
    ASSERT(_xml && node_name);
    _xml->Tab();
    _xml->Printf("<%s",node_name);
    _xml->Printf(">");
    _xml->Eol();
    _xml->IncLogLevel(1);
	
	
    _xml->IncLogLevel(-1);
    _xml->Log("</%s>",node_name);
    return OK;
}
