#include "linkedlist.h"
#include "syslog.h"
#include "mem_tool.h"

LinkedList_Element::LinkedList_Element()
{
    m_RawPtr = NULL;
    next = NULL;
    prev = NULL;
}

LinkedList_Element::~LinkedList_Element()
{
    DEL(m_RawPtr);
    next = NULL;
    prev = NULL;
}

CTestNode* LinkedList_Element::get()
{
    return m_RawPtr;
}

status_t LinkedList_Element::set(CTestNode *node)
{
    ASSERT(m_RawPtr == NULL);
    m_RawPtr = node;
    return OK;
}

/*********************************************/

CLinkedList::CLinkedList()
{
	this->InitBasic();
}
CLinkedList::~CLinkedList()
{
	this->Destroy();
}
status_t CLinkedList::InitBasic()
{
	this->m_Head = NULL;
	this->m_Tail = NULL;
	this->m_Len = 0;
	return OK;
}
status_t CLinkedList::Init()
{
	this->InitBasic();
	return OK;
}
status_t CLinkedList::Destroy()
{
	LinkedList_Element *q,*p = this->m_Head;
	while(p)
	{
		q = p;
		p = p->next;
		this->DelEntry(q);
	}
	this->InitBasic();
	return OK;
}

status_t CLinkedList::AddFromHead(LinkedList_Element *node)
{
	ASSERT(node);
	if(this->m_Head)
		this->m_Head->prev = node;
	node->next = this->m_Head;
	this->m_Head = node;
	if(this->m_Tail == NULL)
		this->m_Tail = node;
	this->m_Len++;
	return OK;
}
status_t CLinkedList::AddFromTail(LinkedList_Element *node)
{
	ASSERT(node);
	if(this->m_Tail)
		this->m_Tail->next = node;
	node->prev = this->m_Tail;
	this->m_Tail = node;
	if(this->m_Head == NULL)
		this->m_Head = node;
	this->m_Len++;
	return OK;
}
bool CLinkedList::IsEmpty()
{
	return this->m_Head == NULL && this->m_Tail == NULL;
}

LinkedList_Element* CLinkedList::SearchFromHead(CTestNode *node)
{
	LinkedList_Element *p = this->m_Head;
	while(p)
	{
		if(this->CompNode(p->get(),node) == 0)
			return p;
		p = p->next;
	}
	return NULL;
}

LinkedList_Element* CLinkedList::SearchFromTail(CTestNode *node)
{
	LinkedList_Element *p = this->m_Tail;
	while(p)
	{
		if(this->CompNode(p->get(),node) == 0)
			return p;
		p = p->prev;
	}
	return NULL;	
}

LinkedList_Element* CLinkedList::Remove(LinkedList_Element *node)
{
	ASSERT(node);

	if(node->prev)
		node->prev->next = node->next;
	else
		this->m_Head = node->next;

	if(node->next)
		node->next->prev = node->prev;
	else
		this->m_Tail = node->prev;

	this->m_Len--;
	node->prev = NULL;
	node->next = NULL;
	return node;
}
status_t CLinkedList::Delete(LinkedList_Element *node)
{
    LinkedList_Element *elem = this->Remove(node);
    if(elem)
    {
	    return this->DelEntry(elem);
    }
    return ERROR;
}
status_t CLinkedList::InsertAfter(LinkedList_Element *after, LinkedList_Element *node)
{
	ASSERT(node && after);
	node->next = after->next;
	if(after->next)
		after->next->prev = node;
	node->prev = after;
	after->next = node;
	if(after == this->m_Tail)
		this->m_Tail = node;
	this->m_Len++;
	return OK;
}
status_t CLinkedList::InsertBefore(LinkedList_Element *before, LinkedList_Element *node)
{
	ASSERT(node && before);
	node->prev = before->prev;
	if(before->prev)
		before->prev->next = node;
	node->next = before;
	before->prev = node;
	if(before == this->m_Head)
		this->m_Head = node;
	this->m_Len++;
	return OK;
}

LinkedList_Element* CLinkedList::GetHead()
{
	return this->m_Head;
}

LinkedList_Element* CLinkedList::GetTail()
{
	return this->m_Tail;
}

int CLinkedList::GetLen()
{
	return this->m_Len;
}

status_t CLinkedList::Copy(CLinkedList *_list)
{
	LinkedList_Element *p;

	ASSERT(_list);

	this->Destroy();
	this->Init();

	p = _list->m_Head;
	while(p)
	{
        LinkedList_Element *entry;
        NEW(entry,LinkedList_Element);
        entry->set(this->CloneNode(p->get()));

		if(!this->AddFromTail(entry))
        {
            return ERROR;
        }

		p = p->next;
	}

	return OK;
}

status_t CLinkedList::Print(CFileBase *_buf)
{
/*#Code_Print#*/
	LinkedList_Element *p;
	p = this->m_Head;
	while(p)
	{
		_buf->Log("{");
		_buf->IncLogLevel(1);
        if(p->get())
		    p->get()->Print(_buf);
		_buf->IncLogLevel(-1);
		_buf->Log("} ->");
		p = p->next;
	}
	_buf->Log("len = %d",this->m_Len);
	return OK;
}

///////////////////////////////////////////
status_t CLinkedList::DelEntry(LinkedList_Element *entry)
{
	if(entry == NULL)
        return ERROR;
	DEL(entry);
	return OK;
}
int CLinkedList::CompNode(CTestNode *node1, CTestNode *node2)
{
	ASSERT(node1 && node2);
	return node1->Comp(node2);
}

CTestNode* CLinkedList::CloneNode(CTestNode *node)
{
	CTestNode *tmp;

	ASSERT(node);

	NEW(tmp,CTestNode);
	tmp->Init();
	tmp->Copy(node);

	return tmp;
}