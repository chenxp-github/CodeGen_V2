#include "hashtable.h"
#include "syslog.h"
#include "mem_tool.h"
#include "misc.h"

namespace test_name_space{

TestNode_HashEntry::TestNode_HashEntry()
{
    m_RawPtr = NULL;
    next = NULL;
}

TestNode_HashEntry::~TestNode_HashEntry()
{
    DEL(m_RawPtr);
    next = NULL;
}
    
CTestNode* TestNode_HashEntry::get()
{
    return m_RawPtr;
}

status_t TestNode_HashEntry::set(CTestNode *node)
{
    ASSERT(m_RawPtr == NULL);
    m_RawPtr = node;
    return OK;
}

/*********************************************/

CHashTable::CHashTable()
{
	this->InitBasic();
}
CHashTable::~CHashTable()
{
	this->Destroy();
}
status_t CHashTable::InitBasic()
{
/*#Code_InitBasic#*/
	this->m_Capacity = 0;
	this->m_Data = 0;
	this->m_Size = 0;
	return OK;
}
status_t CHashTable::Init(int capacity)
{
	this->InitBasic();
	this->m_Capacity = capacity;
	MALLOC(this->m_Data,TestNode_HashEntry*,capacity);
	memset(this->m_Data,0,capacity*sizeof(TestNode_HashEntry*));
	return OK;
}
status_t CHashTable::Destroy()
{
	int i;
	TestNode_HashEntry *q,*p;

	if(this->m_Data == NULL)
		return OK;

	for(i = 0; i < this->m_Capacity; i++)
	{
		p  = this->m_Data[i];
		while(p)
		{
			q = p->next;
			DEL(p);
			p = q;			
		}			
	}
	FREE(this->m_Data);
	this->InitBasic();
	return OK;
}

status_t CHashTable::PutPtr(CTestNode *ptr)
{
	int code;
	TestNode_HashEntry *p;

	ASSERT(ptr);

	if(this->Get(ptr) != NULL)
		return ERROR;

	code = this->HashCode(ptr);
	ASSERT(code >= 0 && code < this->m_Capacity);
	p = this->m_Data[code];

    TestNode_HashEntry *ptr_entry;
    NEW(ptr_entry,TestNode_HashEntry);
    ptr_entry->set(ptr);

	if (p == NULL)
	{
		this->m_Data[code] = ptr_entry;
		ptr_entry->next = NULL;
	}
	else
	{
		while(p->next)
		{
			p = p->next;
		}
		p->next = ptr_entry;
		ptr_entry->next = NULL;
	}
	this->m_Size++;
	return OK;
}

status_t CHashTable::Put(CTestNode *hashentry)
{
	CTestNode *tmp = this->CloneNode(hashentry);
	if(!this->PutPtr(tmp))
	{
		DEL(tmp);
		return ERROR;
	}
	return OK;
}

CTestNode* CHashTable::Get(CTestNode *key)
{
	int code;
	TestNode_HashEntry *p;

	ASSERT(key);

	code = this->HashCode(key);
	ASSERT(code >= 0 && code < this->m_Capacity);
	p = this->m_Data[code];
	while(p)
	{
		if(this->Equals(p->get(),key))
			return p->get();
		p = p->next;
	}
	return NULL;
}

status_t CHashTable::EnumAll(CClosure *closure)
{
	int i,_contine;
	TestNode_HashEntry *pre,*p,*next;
	
	ASSERT(closure);
	
	for(i = 0; i < this->m_Capacity; i++)
	{
		p = this->m_Data[i];
		if(p == NULL)continue;
		
		pre = p;
		p = p->next;
		while(p)
		{			
			next = p->next;
			closure->SetParamPointer(0,p->get());
			closure->SetParamInt(1,p==m_Data[i]); //is first
			_contine = closure->Run();
            //mark param 0 to NULL mean to delete the entry
			if(closure->GetParamPointer(0) == NULL) 
			{
                DEL(p);
				pre->next = next;
				p = next;
				this->m_Size --;
			}
			else
			{
				pre = p;
				p = next;
			}
			
			if(!_contine)
				goto end;
		}
		
		p = this->m_Data[i];
		next = p->next;
		closure->SetParamPointer(0,p->get());
		closure->SetParamInt(1,p==m_Data[i]);
		_contine = closure->Run();
		if(closure->GetParamPointer(0) == NULL)
		{
			this->m_Data[i] = next;
			this->m_Size --;
		}
		if(!_contine)
			goto end;
	}
	
end:
	return OK;
}

TestNode_HashEntry* CHashTable::Remove(CTestNode *key)
{
	int code;
	TestNode_HashEntry *pre,*p;

	ASSERT(key);
	code = this->HashCode(key);
	if(code < 0 || code >= this->m_Capacity)
		return NULL;
	p = this->m_Data[code];
	if(p == NULL) return NULL;

	if(this->Equals(p->get(),key))
	{
		this->m_Data[code] = p->next;
		this->m_Size --;
		return p;
	}
	else
	{
		while(p)
		{
			pre = p;
			p = p->next;
			if(p && this->Equals(p->get(),key))
			{
				pre->next = p->next;
				this->m_Size --;
				return p;
			}
		}
	}

	return NULL;
}
status_t CHashTable::Del(CTestNode *key)
{
	TestNode_HashEntry *p = this->Remove(key);
	if(p != NULL)
	{
		DEL(p);
        return OK;
	}
	return ERROR;
}
int CHashTable::GetSize()
{
	return this->m_Size;
}
int CHashTable::GetCapacity()
{
	return this->m_Capacity;
}
bool CHashTable::IsEmpty()
{
	return this->GetSize() <= 0;
}
status_t CHashTable::DiscardAll()
{
	m_Data = NULL;
	return OK;
}
int CHashTable::HashCode(CTestNode *hashentry)
{
	return HashCode(hashentry,m_Capacity);
}

status_t CHashTable::Clear()
{
	int capacity = m_Capacity;
	this->Destroy();
	this->Init(capacity);
	return OK;
}

status_t CHashTable::Copy(CHashTable *p)
{
	ASSERT(p);
	this->Destroy();
	this->Init(p->GetCapacity());

	BEGIN_CLOSURE(on_copy)
	{
		CLOSURE_PARAM_PTR(CHashTable*,self,10);
		CLOSURE_PARAM_PTR(CTestNode*,node,0);
		self->Put(node);
		return OK;
	}
	END_CLOSURE(on_copy);
	
	on_copy.SetParamPointer(10,this);
	p->EnumAll(&on_copy);
	return OK;
}

status_t CHashTable::Comp(CHashTable *p)
{
    ASSERT(p);
    if(p == this)
        return 0;
	ASSERT(0);
	return 0;
}

status_t CHashTable::Print(CFileBase *_buf)
{
	int i;
	int collision = 0;
	int maxLength = 0;

	for(i = 0; i < this->m_Capacity; i++)
	{
		TestNode_HashEntry *p = this->m_Data[i];
		if(p != NULL)
		{
			_buf->Log("[%d]->",i);
			int len = 0;
			collision--;
			while(p)
			{
				if(p->get())
                {
                    p->get()->Print(_buf);
                }
				_buf->Log("->");
				p = p->next;
				len ++;
				collision++;
			}
			if(len > maxLength)
				maxLength = len;
			_buf->Log("");
		}
	}

	_buf->Log("capacity is %d", m_Capacity);
	_buf->Log("total size is %d",m_Size);
	_buf->Log("maximum linked list length is %d",maxLength);
	_buf->Log("total collison is %d",collision);

	return OK;
}

status_t CHashTable::ToArray(CTestNode *arr[], int *len)
{    
    ASSERT(arr && len);   
    int max_len = *len;    
    *len = 0;    
       
    BEGIN_CLOSURE(on_enum)        
    {        
        CLOSURE_PARAM_PTR(CTestNode*,node,0);        
        CLOSURE_PARAM_PTR(CHashTable*,self,10);        
        CLOSURE_PARAM_PTR(CTestNode**,arr,11);        
        CLOSURE_PARAM_PTR(int*,len,12);        
        CLOSURE_PARAM_INT(max_len,13);        
        ASSERT(*len < max_len);        
        arr[(*len)++] = node;           
        return OK;        
    }    
    END_CLOSURE(on_enum);
            
    on_enum.SetParamPointer(10,this);    
    on_enum.SetParamPointer(11,arr);    
    on_enum.SetParamPointer(12,len);    
    on_enum.SetParamInt(13,max_len);        
    this->EnumAll(&on_enum);    
    return OK;
}
/*********************************************/
/*********************************************/
int CHashTable::HashCode(CTestNode *hashentry,int capacity)
{
	ASSERT(hashentry);
    ASSERT(0);
	return 0;
}

bool CHashTable::Equals(CTestNode *hashentry1, CTestNode *hashentry2)
{
	ASSERT(hashentry1 && hashentry2);
	return hashentry1->Comp(hashentry2) == 0;
}

CTestNode* CHashTable::CloneNode(CTestNode *hashentry)
{
	CTestNode *ptr;
	NEW(ptr,CTestNode);
	ptr->Init();
	ptr->Copy(hashentry);
	return ptr;
}

} //namespace test_name_space