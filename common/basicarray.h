#ifndef __BASICARRAY_H
#define __BASICARRAY_H

#include "cruntime.h"
#include "filebase.h"
#include "syslog.h"
#include "mem_tool.h"

template <class T>
class CBasicArray{
    typedef status_t (*HOW_TO_PRINT)(CFileBase *_buf,T t);
    typedef status_t (*HOW_TO_COMP)(T t1,T t2);
public:
    WEAK_REF_DEFINE();
private:
    T *m_Data;
    int m_Top,m_Size;
    HOW_TO_PRINT how_to_print;
    HOW_TO_COMP how_to_comp;
public:

CBasicArray()
{
    this->InitBasic();
}
~CBasicArray()
{
    this->Destroy();
}

status_t InitBasic()
{
    WEAK_REF_CLEAR();
    this->m_Data = NULL;
    this->m_Top = 0;
    this->m_Size = 0;
    this->how_to_print = NULL;
    this->how_to_comp = NULL;
    return OK;
}
status_t Init(int init_size=256)
{
    this->InitBasic();
    
    MALLOC(this->m_Data,T,init_size);
    this->m_Size = init_size;
    return OK;
}
status_t Destroy()
{
    WEAK_REF_DESTROY();
    FREE(this->m_Data);
    this->InitBasic();
    return OK;
}
bool IsEmpty()
{
    return this->m_Top <= 0;
}
bool IsFull()
{
    return this->m_Top >= this->m_Size;
}
int GetLen()
{
    return this->m_Top;
}
status_t Clear()
{   
    this->m_Top = 0;
    return OK;
}
status_t AutoResize()
{
    if(this->IsFull())
    {
        REALLOC(this->m_Data,T,this->m_Size,this->m_Size * 2);
        this->m_Size *= 2;
    }
    return OK;
}

status_t Push(T node)
{   
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Data[this->m_Top]= node;
    this->m_Top ++; 
    return OK;
}

T Pop()
{
    ASSERT(!this->IsEmpty());
    this->m_Top --;
    return this->m_Data[this->m_Top];
}

T GetElem(int index)
{
    ASSERT(index >= 0 && index < this->m_Top);
    return this->m_Data[index];
}

int BSearchPos(T node, int order, int *find_flag)
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
status_t InsElem(int index, T node)
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
status_t DelElem(int index)
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
status_t InsOrdered(T node, int order, int unique)
{
    int pos,find;
    
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    
    return this->InsElem(pos,node);
}
int SearchPos(T node)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Data[i],node) == 0)
            return i;
    }
    return -1;
}
T GetTop()
{
    ASSERT(!this->IsEmpty());
    return this->m_Data[this->m_Top - 1];
}
status_t SetElem(int index, T node)
{
    ASSERT(index >= 0 && index < this->m_Top);
    this->m_Data[index] = node;
    return OK;
}
status_t Sort(int order)
{
    int i;
    CBasicArray tmp;

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
status_t Copy(CBasicArray *stk)
{
    int i;
    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(i = 0; i < stk->GetLen(); i++)
    {
        this->Push(stk->GetElem(i));
    }
    return OK;
}

status_t Print(CFileBase *_buf)
{   
    if(how_to_print)
    {
        for(int i = 0; i < m_Top; i++)
        {
            how_to_print(_buf,m_Data[i]);
        }
    }

    return OK;
}

int CompNode(T node1, T node2)
{
    if(how_to_comp)
    {
        return how_to_comp(node1,node2);
    }
    if(node1 > node2) return 1;
    if(node1 < node2) return -1;
    return 0;
}

void SetPrintFunc(HOW_TO_PRINT func)
{
    how_to_print = func;
}

void SetCompFunc(HOW_TO_COMP func)
{
    how_to_comp = func;
}

};

#endif
