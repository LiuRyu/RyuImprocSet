#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "windows.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "general_utils.h"

xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		printf("No result\n");
		return NULL;
	}
	return result;
}

/*********************************************************************
* ��������:int parseXmlNodeValue(char * docname, char * elename, char * content)
* ˵��:��ȡxml�ļ���ĳ���ڵ�(Ԫ��)��ֵ(����)
* �����ߣ�
* �������:
* const char * docname --�û�ָ����xml�ļ�·��
* const char * elename --�û�ָ����xmlԪ��
* ���������
* char * content --���xmlԪ������
* ����ֵ:
* int  -- XML_PARSE_FAILED:  ʧ��
*      -- XML_PARSE_SUCCESS: �ɹ�
* ����: liuryu
* ʱ��: 2017-3-31
*********************************************************************/ 
int parseXmlNodeValue(const char * docname, const char * elename, char * content)
{
	char cpath[MAX_PATH] = "//";

	xmlDocPtr doc;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlChar * keyword;

	content[0] = 0;
	strcat_s(cpath, elename); 

	doc = xmlParseFile(docname);
	if (doc == NULL ) {
		printf("Document not parsed successfully.[parseXmlNodeValue]\n");
		return XML_PARSE_FAILED;
	}
	result = getnodeset (doc, (xmlChar *)cpath);
	if (result) {
		nodeset = result->nodesetval;
		keyword = xmlNodeListGetString(doc,
			nodeset->nodeTab[0]->xmlChildrenNode, 1);
		memcpy(content, keyword, strlen((char *)keyword));
		xmlXPathFreeObject (result);
		xmlFree(keyword);
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return XML_PARSE_SUCCESS;
}

/*********************************************************************
* ��������:int reviseXmlNodeValue(char * docname, char * elename, char * content)
* ˵��:�޸�xml�ļ���ĳ���ڵ�(Ԫ��)��ֵ(����)
* �����ߣ�
* �������:
* const char * docname --�û�ָ����xml�ļ�·��
* const char * elename --�û�ָ����xmlԪ��
* const char * content --���xmlԪ������
* ���������
* ��
* ����ֵ:
* int  -- XML_PARSE_FAILED:  ʧ��
*      -- XML_PARSE_SUCCESS: �ɹ�
* ����: liuryu
* ʱ��: 2017-3-31
*********************************************************************/ 
int reviseXmlNodeValue(const char * docname, const char * elename, const char * content)
{
	char cpath[MAX_PATH] = "//";

	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;

	strcat_s(cpath, elename); 

	doc = xmlParseFile(docname);
	if (doc == NULL ) {
		printf("Document not parsed successfully.[reviseXmlNodeValue]\n");
		return XML_PARSE_FAILED;
	}
	result = getnodeset (doc, (xmlChar *)cpath);
	if (result) {
		nodeset = result->nodesetval;
		cur = nodeset->nodeTab[0];
		xmlNodeSetContent(cur, (xmlChar *)content);
		xmlXPathFreeObject (result);
		xmlSaveFormatFile (docname, doc, 0);
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return XML_PARSE_SUCCESS;
}

