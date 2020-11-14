#ifndef __TEST_NODE_H
#define __TEST_NODE_H

#include "mem.h"
#include "memfile.h"
#include "minibson.h"
#include "xml.h"

class CTestNode{
public:
    int m_m;
public:
	CTestNode();
    virtual ~CTestNode();
    status_t InitBasic();
	status_t Init();
	status_t Destroy();
    status_t Copy(CTestNode *_p);
    int Comp(CTestNode *_p);
	status_t Print(CFileBase *_buf);
    int GetM();
    status_t SetM(int _m);
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
	
    status_t LoadXml(CXmlNode *_root);
    status_t SaveXml(CFileBase *_xml, const char *node_name);
    status_t LoadXml(const char *fn, const char *path);
    status_t SaveXml(const char *fn, const char *node_name);
};

#endif
