#include "basicqueue.h"
#include "syslog.h"
#include "mem_tool.h"

CBasicQueue::CBasicQueue()
{
	this->InitBasic();
}
CBasicQueue::~CBasicQueue()
{
	this->Destroy();
}
status_t CBasicQueue::InitBasic()
{
	this->m_Head = 0;
	this->m_Tail = 0;
	this->m_Data = NULL;
	this->m_Size = 0;
	this->m_IsFull = 0;
	return OK;
}
status_t CBasicQueue::Init(int size)
{
	this->InitBasic();
	MALLOC(this->m_Data,double,size);
	this->m_Size = size;
	return OK;
}
status_t CBasicQueue::Destroy()
{
	FREE(this->m_Data);
	this->InitBasic();
	return OK;
}

bool CBasicQueue::IsEmpty()
{
	return this->GetLen() <= 0;
}

bool CBasicQueue::IsFull()
{
	return this->m_IsFull;
}
status_t CBasicQueue::Clear()
{
	this->m_Head = 0;
	this->m_Tail = 0;
	this->m_IsFull = 0;
	return OK;
}
status_t CBasicQueue::EnQueue(double node)
{
	ASSERT(!this->IsFull());
	this->m_Data[this->m_Tail] = node;
	this->m_Tail=(this->m_Tail+1) % this->m_Size;	
	if(this->m_Tail == this->m_Head)
		this->m_IsFull = TRUE;
	return OK;
}
status_t CBasicQueue::DeQueue(double *node)
{
	ASSERT(node && (!this->IsEmpty()));
	*node = this->m_Data[this->m_Head];
	this->m_Head = (this->m_Head+1) % this->m_Size;
	this->m_IsFull = FALSE;
	return OK;
}

int CBasicQueue::GetLen()
{
	if(this->IsFull())
		return this->m_Size;
	return (this->m_Tail - this->m_Head + this->m_Size) % this->m_Size;
}

status_t CBasicQueue::GetHead(double *node)
{
    ASSERT(node);
    if(this->IsEmpty())return NULL;   
    *node = this->m_Data[this->m_Head];
    return OK;
}

status_t CBasicQueue::GetElem(int index,double *node)
{
    ASSERT(node);
    ASSERT(index >= 0 && index < GetLen());   
    int i =	(this->m_Head+index) % m_Size;   
    *node = m_Data[i];
    return OK;
}

status_t CBasicQueue::GetTail(double *node)
{
    ASSERT(node);
    if(this->IsEmpty())return NULL;   
    return this->GetElem(GetLen()-1,node);
}
/////////////////////////////////////////////////////
status_t CBasicQueue::Copy(CBasicQueue *queue)
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
        double v;
        queue->GetElem(i,&v);
        this->EnQueue(v);
    }
    
    return OK;
}

status_t CBasicQueue::Comp(CBasicQueue *queue)
{
    ASSERT(0);
    ASSERT(queue);
    if(this == queue)
        return 0;
    return 0;
}

status_t CBasicQueue::Print(CFileBase *_buf)
{
	int i;
	_buf->Log("head=%d",this->m_Head);
	_buf->Log("tail=%d",this->m_Tail);
	_buf->Log("size=%d",this->m_Size);
	_buf->Log("length=%d",this->GetLen());
	_buf->Log("is_full=%d",this->m_IsFull);
	for(i=0;i<this->GetLen();i++)
	{
		//add your code
	}

	return TRUE;
}
