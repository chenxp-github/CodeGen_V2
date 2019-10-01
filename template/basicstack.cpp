#include "basicstack.h"
#include "mem_tool.h"
#include "syslog.h"

CBasicStack::CBasicStack()
{
	this->InitBasic();
}
CBasicStack::~CBasicStack()
{
	this->Destroy();
}
status_t CBasicStack::InitBasic()
{
	this->m_Data = NULL;
	this->m_Top = 0;
	this->m_Size = 0;		
	return OK;
}
status_t CBasicStack::Init(int init_size)
{
	this->InitBasic();
	MALLOC(this->m_Data,double,init_size);
	this->m_Size = init_size;
	return OK;
}
status_t CBasicStack::Destroy()
{
	FREE(this->m_Data);
	this->InitBasic();
	return OK;
}
bool CBasicStack::IsEmpty()
{
	return this->m_Top <= 0;
}
bool CBasicStack::IsFull()
{
	return this->m_Top >= this->m_Size;
}
int CBasicStack::GetLen()
{
	return this->m_Top;
}
status_t CBasicStack::Clear()
{	
	this->m_Top = 0;
	return OK;
}
status_t CBasicStack::AutoResize()
{
	if(this->IsFull())
	{
		REALLOC(this->m_Data,double,this->m_Size,this->m_Size * 2);
		this->m_Size *= 2;
	}
	return OK;
}

status_t CBasicStack::Push(double node)
{	
	this->AutoResize();
	ASSERT(!this->IsFull());
	this->m_Data[this->m_Top]= node;
	this->m_Top ++;	
	return OK;
}

double CBasicStack::Pop()
{
	ASSERT(!this->IsEmpty());
	this->m_Top --;
	return this->m_Data[this->m_Top];
}

double CBasicStack::GetElem(int index)
{
	ASSERT(index >= 0 && index < this->m_Top);
	return this->m_Data[index];
}

int CBasicStack::BSearchPos(double node, int order, int *find_flag)
{
	int low,high,mid;
	int comp;

	low = 0; high=this->GetLen() - 1;
	while(low<=high)
	{
		mid = (low+high) >> 1;
		comp = this->CompNode(this->m_Data[mid],node);
		if(comp == 0)
		{
			*find_flag = TRUE;
			return mid;
		}

		if(order != 0) comp = -comp;
		if(comp>0)high=mid-1;else low=mid+1;
	}
	*find_flag = FALSE;
	return low;
}
status_t CBasicStack::InsElem(int index, double node)
{
	int k;
	ASSERT(index >= 0 && index <= this->m_Top);	
	this->AutoResize();
	ASSERT(!this->IsFull());
	for(k = this->m_Top; k > index; k--)
	{
		this->m_Data[k] = this->m_Data[k - 1];
	}
	this->m_Data[index] = node;
	this->m_Top++;
	return OK;
}
status_t CBasicStack::DelElem(int index)
{
	int k;
	ASSERT(index >= 0 && index < this->m_Top);
	for(k = index; k < this->m_Top-1; k++)
	{
		this->m_Data[k] = this->m_Data[k + 1];
	}
	this->m_Top --;
	return OK;
}
status_t CBasicStack::InsOrdered(double node, int order, int unique)
{
	int pos,find;
	
	pos = this->BSearchPos(node,order,&find);
	if(find && unique)
		return ERROR;
	
	return this->InsElem(pos,node);
}
int CBasicStack::SearchPos(double node)
{
	int i;
	for(i=0;i<this->m_Top;i++)
	{
		if(this->CompNode(this->m_Data[i],node) == 0)
			return i;
	}
	return -1;
}
double CBasicStack::GetTop()
{
	ASSERT(!this->IsEmpty());
	return this->m_Data[this->m_Top - 1];
}
status_t CBasicStack::SetElem(int index, double node)
{
	ASSERT(index >= 0 && index < this->m_Top);
	this->m_Data[index] = node;
	return OK;
}
status_t CBasicStack::Sort(int order)
{
	int i;
	CBasicStack tmp;

	tmp.Init(this->GetLen());
	for(i = 0; i < this->GetLen(); i++)
	{
		tmp.Push(this->GetElem(i));
	}
	
	this->m_Top = 0;
	for(i = 0; i < tmp.GetLen(); i++)
	{
		this->InsOrdered(tmp.GetElem(i),order,0);
	}
	return OK;
}

int CBasicStack::Comp(CBasicStack *stk)
{
    ASSERT(stk);
    if(this == stk)
        return 0;
    ASSERT(0);
    return 0;
}

status_t CBasicStack::Copy(CBasicStack *_p)
{
	int i;
	ASSERT(_p);
	this->Destroy();
	this->Init(_p->GetLen());
	for(i = 0; i < _p->GetLen(); i++)
	{
		this->Push(_p->GetElem(i));
	}
	return OK;
}

status_t CBasicStack::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    
    fsize_t off;
    _bson->StartArray("_array_",&off);
    char name[256];
    for(int i = 0; i < this->GetLen(); i++)
    {           
        sprintf(name,"%d",i);
        _bson->PutDouble(name,this->GetElem(i));     
    }
    _bson->EndArray(off,this->GetLen());    
    return OK;
}

status_t CBasicStack::SaveBson(CMem *_mem)
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

status_t CBasicStack::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    
    CMiniBson doc;
    doc.Init();
    
    int len = 0;
    BSON_CHECK(_bson->GetArray("_array_",&doc,&len));
    this->Clear();
    
    doc.ResetPointer();
    for(int i = 0; i < len; i++)
    {
        double t = 0;
        _bson->GetDouble(NULL,&t);
        this->Push(t);
    }
    
    return OK;
}
status_t CBasicStack::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}
/*#End Code_Bson_Cpp#*/

/***************************************************/
status_t CBasicStack::Print(CFileBase *_buf)
{
	return OK;
}

int CBasicStack::CompNode(double node1, double node2)
{
	if(node1 > node2) return 1;
	if(node1 < node2) return -1;
	return 0;
}
/***************************************************/
