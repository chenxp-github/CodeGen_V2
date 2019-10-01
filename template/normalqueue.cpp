#include "normalqueue.h"
#include "syslog.h"
#include "mem_tool.h"

CNormalQueue::CNormalQueue()
{
	this->InitBasic();
}
CNormalQueue::~CNormalQueue()
{
	this->Destroy();
}
status_t CNormalQueue::InitBasic()
{
	this->m_Head = 0;
	this->m_Tail = 0;
	this->m_Data = NULL;
	this->m_Size = 0;
	this->m_IsFull = 0;
	return OK;
}
status_t CNormalQueue::Init(int size)
{
	int i;
	this->InitBasic();

	NEW_ARRAY(this->m_Data,CTestNode,size);
	for(i = 0; i < size; i++)
	{
		this->m_Data[i].Init();
	}
	this->m_Size = size;
	return OK;
}
status_t CNormalQueue::Destroy()
{
	DEL_ARRAY(this->m_Data);
	this->InitBasic();
	return OK;
}

status_t CNormalQueue::Clear()
{
	this->m_Head = 0;
	this->m_Tail = 0;
	this->m_IsFull = false;
	return OK;
}

bool CNormalQueue::IsEmpty()
{
	return this->GetLen() <= 0;
}
bool CNormalQueue::IsFull()
{
	return this->m_IsFull;
}

CTestNode* CNormalQueue::EnQueue()
{
	CTestNode *ret;
	if(this->IsFull())return NULL;
	ret = &this->m_Data[this->m_Tail];
	this->m_Tail=(this->m_Tail+1) % this->m_Size;	
	if(this->m_Tail == this->m_Head)
		this->m_IsFull = TRUE;
	return ret;
}
CTestNode* CNormalQueue::DeQueue()
{
	CTestNode *ret;
	if(this->IsEmpty())return NULL;
	ret = &this->m_Data[this->m_Head];
	this->m_Head = (this->m_Head+1) % this->m_Size;
	this->m_IsFull = FALSE;
	return ret;
}

int CNormalQueue::GetLen()
{
	if(this->IsFull())return this->m_Size;
	return (this->m_Tail - this->m_Head + this->m_Size) % this->m_Size;
}

status_t CNormalQueue::Copy(CNormalQueue *queue)
{
    ASSERT(queue);

    if(m_Size != queue->m_Size)
    {
        this->Destroy();
        this->Init(queue->m_Size);
    }
    
    this->Clear();

    for(int i = 0; i < queue->GetLen(); i++)
    {
        this->EnQueue(queue->GetElem(i));
    }

    return OK;
}

status_t CNormalQueue::Comp(CNormalQueue *queue)
{
    ASSERT(0);
    ASSERT(queue);
    if(this == queue)
        return 0;
    return 0;
}

status_t CNormalQueue::Print(CFileBase *_buf)
{
	int i;

	_buf->Log("head=%d",this->m_Head);
	_buf->Log("tail=%d",this->m_Tail);
	_buf->Log("size=%d",this->m_Size);
	_buf->Log("length=%d",this->GetLen());
	_buf->Log("is_full=%d",this->m_IsFull);

	for(i=0;i<this->GetLen();i++)
	{
        CTestNode *p = GetElem(i);
        ASSERT(p);
		_buf->Log("[%d]={",i);
		_buf->IncLogLevel(1);
		p->Print(_buf);
		_buf->IncLogLevel(-1);
		_buf->Log("}");
	}

	return TRUE;
}

status_t CNormalQueue::EnQueue(CTestNode *pos)
{
    CTestNode *n = this->EnQueue();    
    ASSERT(n);   
    n->Copy(pos);
    return OK;    
}

CTestNode* CNormalQueue::GetHead()
{
    if(this->IsEmpty())return NULL;   
    return &this->m_Data[this->m_Head];
}

CTestNode* CNormalQueue::GetElem(int index)
{
    ASSERT(index >= 0 && index < GetLen());   
    int i =	(this->m_Head+index) % m_Size;   
    return &m_Data[i];
}

CTestNode* CNormalQueue::GetTail()
{
    if(this->IsEmpty())return NULL;   
    return this->GetElem(GetLen()-1);
}





