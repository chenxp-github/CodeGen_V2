#include "testtree.h"
#include "syslog.h"
#include "mem_tool.h"

TestTree_Entry::TestTree_Entry()
{
    m_RawPtr = NULL;
    parent = NULL;
    next = NULL;
    prev = NULL;
    child = NULL;
}

TestTree_Entry::~TestTree_Entry()
{
    parent = NULL;
    next = NULL;
    child = NULL;
    prev = NULL;
    DEL(m_RawPtr);
}
    
CTestNode* TestTree_Entry::get()
{
    return m_RawPtr;
}

status_t TestTree_Entry::set(CTestNode *node)
{
    ASSERT(m_RawPtr == NULL);
    m_RawPtr = node;
    return OK;
}

status_t TestTree_Entry::DetachFromTheTree()
{
    TestTree_Entry *node = this;

    if(node->parent)
    {
        if(node->parent->child == node)
        {            
            node->parent->child = node->next;
        }
    }
    
    if(node->prev)
    {
        node->prev->next = node->next;
    }
    
    if(node->next)
    {
        node->next->prev = node->prev;
    }
    
    return OK;
}
status_t TestTree_Entry::DelNode_Recursive(TestTree_Entry *node)
{
    ASSERT(node);
    
    TestTree_Entry *child = node->child;
    while(child)
    {
        TestTree_Entry *p = child;
        child = child->next;
        DelNode_Recursive(p);        
    }

    node->DetachFromTheTree();    
    DEL(node);    
    return OK;
}

status_t TestTree_Entry::Traverse(CClosure *closure)
{
    ASSERT(closure);
    
    if(closure->GetParamType(1) != PARAM_TYPE_INT)
    {
        closure->SetParamInt(1,0); //level
    }

    closure->SetParamPointer(0,this);
    closure->Run();
	if(closure->GetParamPointer(0) == NULL)
	{
		return OK;
	}

    TestTree_Entry *next, *child = this->child;
    while(child)
    {
        int level = closure->GetParamInt(1);
        closure->SetParamInt(1,level+1);		
		next = child->next;
        child->Traverse(closure); //child maybe deleted
        closure->SetParamInt(1,level);
        child = next;
    }    
    return OK;
}

status_t TestTree_Entry::Print_Recursive(TestTree_Entry *node, CFileBase *buf)
{
    ASSERT(node && buf);
    
    BEGIN_CLOSURE(on_traverse)
    {
        CLOSURE_PARAM_PTR(CFileBase*,buf,10);
        CLOSURE_PARAM_PTR(TestTree_Entry*,pn,0);
        CLOSURE_PARAM_INT(level,1);        
        buf->SetLogLevel(level);        
        ASSERT(pn->get());
        buf->Log("{");
        buf->IncLogLevel(1);
        pn->get()->Print(buf);       
        buf->IncLogLevel(-1);
        buf->Log("},");
        return OK;
    }
    END_CLOSURE(on_traverse);
    
    on_traverse.SetParamPointer(10,buf);
    return node->Traverse(&on_traverse);
}

TestTree_Entry* TestTree_Entry::NewFromPointer(CTestNode *node)
{
    TestTree_Entry *tmp;
    NEW(tmp,TestTree_Entry);
    tmp->set(node);
    return tmp;
}

TestTree_Entry* TestTree_Entry::New(CTestNode *node)
{
    CTestNode *p;
    NEW(p,CTestNode);
    p->Init();
    p->Copy(node);
    return NewFromPointer(p);
}

status_t TestTree_Entry::InsertBefore(TestTree_Entry *node)
{   
    ASSERT(node);

    if(this->prev)
    {
        this->prev->next = node;
        node->prev = this->prev;
    }
    
    node->next = this;
    this->prev = node;
    node->parent = this->parent;

    return OK;
}

status_t TestTree_Entry::InsertAfter(TestTree_Entry *node)
{
    ASSERT(node);

    if(this->next)
    {
        node->next = this->next;
        this->next->prev = node;
    }
    
    this->next = node;
    node->prev = this;
    node->parent = this->parent;
    return OK;
}

status_t TestTree_Entry::AddChild(TestTree_Entry *node)
{
    ASSERT(node);
    if(this->child == NULL)
    {
        node->parent = this;
        this->child = node;
        return OK;
    }

    this->child->InsertBefore(node);
    this->child = node;
    return OK;
}

status_t TestTree_Entry::AddChildToTail(TestTree_Entry *node)
{
    ASSERT(node);

    TestTree_Entry *tail = this->GetLastChild();    
    if(!tail)
    {
        this->AddChild(node);
    }
    else
    {
        tail->InsertAfter(node);
    }
    return OK;
}

TestTree_Entry* TestTree_Entry::GetLastChild()
{
    TestTree_Entry *last = NULL;
    TestTree_Entry *child = this->child;
    while(child)
    {
        last = child;
        child = child->next;
    }
    return last;
}

status_t TestTree_Entry::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    ASSERT(m_RawPtr);
    
    m_RawPtr->SaveBson(_bson);
    
    if(this->child)
    {
        fsize_t off;
        _bson->StartArray("children",&off);
        int array_len = 0;
        TestTree_Entry *child = this->child;
        while(child)
        {    
            char name[100];
            sprintf(name,"%d",array_len);
            fsize_t _off;
            
            _bson->StartDocument(name,&_off);
            child->SaveBson(_bson);
            _bson->EndDocument(_off);
            
            array_len++;
            child = child->next;
        }
        _bson->EndArray(off,array_len);
    }
    return OK;
}

status_t TestTree_Entry::LoadBson(CMiniBson *_bson)
{
    if(!m_RawPtr)
    {
        CTestNode *tmp;
        NEW(tmp,CTestNode);
        tmp->Init();
        this->set(tmp);
    }
    
    ASSERT(m_RawPtr);
    BSON_CHECK(m_RawPtr->LoadBson(_bson));
    
    CMiniBson children;
    int array_len = 0;
    BSON_CHECK(_bson->GetArray("children",&children,&array_len));
    
    for(int i = 0; i < array_len; i++)
    {
        char name[100];
        sprintf(name,"%d",i);
        
        CMiniBson elem;
        BSON_CHECK(children.GetDocument(name,&elem));
        
        TestTree_Entry *entry;
        NEW(entry,TestTree_Entry);
        
        BSON_CHECK(entry->LoadBson(_bson));
        
        ASSERT(this->InsertBefore(entry));
    }
    
    return OK;
}

/***********************************************************/
/***********************************************************/
CTestTree::CTestTree()
{
	this->InitBasic();
}
CTestTree::~CTestTree()
{
	this->Destroy();
}
status_t CTestTree::InitBasic()
{
    m_Root = NULL;
	return OK;
}
status_t CTestTree::Init()
{
	this->InitBasic();
	return OK;
}

status_t CTestTree::Destroy()
{
    if(m_Root)
    {
        TestTree_Entry::DelNode_Recursive(m_Root);
        m_Root = NULL;
    }
	return OK;
}

status_t CTestTree::Copy(CTestTree *p)
{
	ASSERT(p);
    ASSERT(0);
	return OK;
}

status_t CTestTree::Comp(CTestTree *p)
{
    ASSERT(p);
    if(p == this)
        return 0;
	ASSERT(0);
	return 0;
}

status_t CTestTree::Print(CFileBase *_buf)
{
    if(m_Root)
    {
        TestTree_Entry::Print_Recursive(m_Root,_buf);
    }
    else
    {
        _buf->Log("<NULL>");
    }
	return OK;
}


TestTree_Entry* CTestTree::GetRoot()
{
    return m_Root;
}

status_t CTestTree::SetRoot(TestTree_Entry *root)
{
    ASSERT(root);
    ASSERT(m_Root == NULL);
    m_Root = root;
    return OK;
}
status_t CTestTree::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    ASSERT(m_Root);
    fsize_t off;
    _bson->StartDocument("root",&off);
    m_Root->SaveBson(_bson);
    _bson->EndDocument(off);
    return OK;
}

status_t CTestTree::SaveBson(CMem *_mem)
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

status_t CTestTree::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    
    CMiniBson doc;
    BSON_CHECK(_bson->GetDocument("root",&doc));
    
    if(!m_Root)
    {
        NEW(m_Root,TestTree_Entry);
    }
    
    m_Root->LoadBson(_bson);
    return OK;
}

status_t CTestTree::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

