/* vim: set et : */
/************************************************************************
 * Id: cwmp.c                                                           *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014 netcwmp group                         *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/

#include "cwmp/cwmp.h"
#include "cwmp/buffer.h"
#include "cwmp/log.h"
#include "cwmp/event.h"
#include "cwmp_private.h"
#include "cwmp/model.h"

#define CWMP_BUF_SIZE 128
#define CWMP_RENEW_SIZE CWMP_NAME_MAX

#ifdef WIN32

//#define ESA(x, y) do { (x) = (y); if( (x) == NULL) { return NULL; } } while (0)
#define ESA(x, y)   (x) = (y); if( (x) == NULL)  return NULL
#define ESN(x, y)   if((x) != (y))  return NULL
#define ESQ(x, y)   if((x) == (y))  return NULL

//not eq
#define ESNE(x,y,z)  if((y) != (z)) return (x)
//eq
#define ESE(x,y,z)  if((y) == (z)) return (x)


#else


//#define ESA(x, y) do { (x) = (y); if( (x) == NULL) { return NULL; } } while (0)
#define ESA(x, y)  do { (x) = (y); if( (x) == NULL) { printf("ASSERT: \"%s\" is NULL. file: %s , func: %s , line: %d\n", #x, __FILE__, __func__, __LINE__); return NULL; } } while(0)
#define ESN(x, y)  do { if((x) != (y))  { printf("ASSERT: \"%s\" != \"%s\"  file: %s , func: %s , line: %d\n", #x, #y, __FILE__, __func__, __LINE__); return NULL; } } while (0)
#define ESQ(x, y)    do { if((x) != (y)) return NULL ; } while(0)
//true
#define ESNE(x,y,z) do { if((y) != (z))  { printf("ASSERT: \"%s\" != \"%s\"  file: %s , func: %s , line: %d\n", #y, #z, __FILE__, __func__, __LINE__); return (x); } } while (0)
//eq
#define ESE(x,y,z)  do { if((y) == (z))  { printf("ASSERT: \"%s\" == \"%s\"  file: %s , func: %s , line: %d\n", #y, #z, __FILE__, __func__, __LINE__); return (x); } } while (0)


#endif

static char SOAP_ENV[CWMP_NAME_MAX] = {0};
static char SOAP_ENC[CWMP_NAME_MAX] = {0};

static char SOAP_ENV_HEADER[CWMP_NAME_MAX] = {0};
static char SOAP_ENV_BODY[CWMP_NAME_MAX] = {0};
static char SOAP_ENV_ENVELOPE[CWMP_NAME_MAX] = {0};
static char SOAP_ENV_FAULT[CWMP_NAME_MAX]={0};
static char SOAP_ENC_ARRAYTYPE[CWMP_NAME_MAX] = {0};

//static parameter_node_t* gs_root_parameter_node = NULL;

int time_to_datatime(datatime_t *nowtime, time_t tn)
{
    struct tm t;

#ifdef WIN32
    memset(&t, 0, sizeof(struct tm));
#else
    t = *localtime(&tn);
#endif

    nowtime->year = t.tm_year + 1900;
    nowtime->month = t.tm_mon + 1;
    nowtime->day = t.tm_mday;
    nowtime->hour = t.tm_hour;
    nowtime->min = t.tm_min;
    nowtime->sec = t.tm_sec;

    return CWMP_OK;
}


#define CWMP_TYPE(x) cwmp_get_type_string(x)

static char * cwmp_get_format_string(const char * fmt, ...)
{
    va_list ap;

    static char g_cwmp_format_string[1024] = {0};
    va_start(ap, fmt);
#ifdef WIN32
    _vsnprintf(g_cwmp_format_string, 1023, fmt, ap);
#else
    vsnprintf(g_cwmp_format_string, 1023, fmt, ap);
#endif

    va_end(ap);
    return g_cwmp_format_string;
}


// type => string conversion
char * cwmp_get_type_string(int type)
{
    switch (type)
    {
    case TYPE_OBJECT:
        return "object";
    case TYPE_STRING:
        return "xsd:string";
    case TYPE_INT:
        return "xsd:int";
    case TYPE_UNSIGNEDINT:
        return "xsd:unsignedInt";
    case TYPE_STRING32:
        return "xsd:string[32]";
    case TYPE_STRING64:
        return "xsd:string[64]";
    case TYPE_STRING128:
        return "xsd:string[128]";
    case TYPE_STRING256:
        return "xsd:string[256]";
    case TYPE_STRING1024:
        return "xsd:string[1024]";
    case TYPE_STRING32768:
        return "xsd:string[32768]";
    case TYPE_BOOLEAN:
        return "xsd:bool";
    case TYPE_DATETIME:
        return "xsd:DateTime";
    default:
        return "";
    }

}

// string => type conversion
int cwmp_get_type_value(char * type)
{
    if(type == NULL)
    {
        return TYPE_UNKNOWN;
    }

    if(! TRstrcasecmp(type, "int"))
    {
        return TYPE_INT;
    }
    else if(! TRstrcasecmp(type, "uint"))
    {
        return TYPE_UNSIGNEDINT;
    }
    else if(! TRstrcasecmp(type, "s"))
    {
        return TYPE_STRING;
    }
    else if(! TRstrcasecmp(type, "s16"))
    {
        return TYPE_STRING16;
    }
    else if(! TRstrcasecmp(type, "s32"))
    {
        return TYPE_STRING32;
    }
    else if(! TRstrcasecmp(type, "s64"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "s128"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "s256"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "s1024"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "s32768"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "dt"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "bool"))
    {
        return TYPE_STRING64;
    }
     else if(! TRstrcasecmp(type, "base"))
    {
        return TYPE_BASE64;
    }
    else if(! TRstrcasecmp(type, "obj"))
    {
        return TYPE_OBJECT;
    }

    return TYPE_STRING;

}

// fault code => string
char * cwmp_get_fault_string(int code)
{
    switch(code)
    {
        case 9000:
            return  FAULT_STR_9000;

        case 9001:
            return  FAULT_STR_9001;

        case 9002:
            return  FAULT_STR_9002;

        case 9003:
            return  FAULT_STR_9003;

        case 9004:
            return  FAULT_STR_9004;

        case 9005:
            return  FAULT_STR_9005;

        case 9006:
            return  FAULT_STR_9006;

        case 9007:
            return  FAULT_STR_9007;

        case 9008:
            return  FAULT_STR_9008;

        case 9009:
            return  FAULT_STR_9009;

        case 9010:
            return  FAULT_STR_9010;

        case 9011:
            return  FAULT_STR_9011;

        case 9012:
            return  FAULT_STR_9012;

        case 9013:
            return  FAULT_STR_9013;

        case 9014:
            return  FAULT_STR_9014;

        case 9015:
            return  FAULT_STR_9015;

        case 9016:
            return  FAULT_STR_9016;

        case 9017:
            return  FAULT_STR_9017;

        case 9018:
            return  FAULT_STR_9018;

        case 9019:
            return  FAULT_STR_9019;
        default:
            return "";
    }
}


xmldoc_t * cwmp_xml_parse_buffer(pool_t * pool, char * buffer)
{
    return XmlParseBuffer(pool, buffer);
}

const char * cwmp_xml_get_node_name(xmlnode_t * node)
{
    xmlnode_t * child;
    child = XmlNodeGetFirstChild(node);
    return XmlNodeGetNodeName(child);
}

char * cwmp_xml_get_node_value(xmlnode_t * node)
{
    return XmlNodeGetNodeValue(XmlNodeGetFirstChild(node));
}

char * cwmp_xml_get_node_attribute(xmlnode_t * node, const char * name)
{
    return XmlElementGetAttribute((XmlElement*)node, name);
}

xmlnode_t * cwmp_xml_get_child_with_name(void * nodeptr, const char * nodeName)
{
    xmlnode_t * children;
    xmlnode_t * node = (xmlnode_t *)nodeptr;
    if (node == NULL)
    {
        cwmp_log_error("Invalid parameter 'param' (null)");
        return NULL;
    }

    children = XmlNodeGetFirstChild(node);
    while (children != NULL)
    {
        //cwmp_log_debug("Node Name %s, (%s) %d", XmlNodeGetNodeName(children), nodeName, children->nodeType );
        if (children->nodeType != XML_ELEMENT_NODE)
        {
            children = XmlNodeGetNextSibling(children);
        }
        else
        {
            if (! TRstrcasecmp(children->nodeName, nodeName))
            {
                //cwmp_log_debug("cwmp_xml_get_child_with_name found node(%p)", children);
                break;
            }
            else
                children = XmlNodeGetNextSibling(children);
        }
    }

    return children;
}

xmlnode_t * cwmp_xml_create_child_node(env_t * env ,  xmlnode_t * parentNode, const char * ns, const char * nodeName, const char * nodeValue)
{
    XmlElement * newNode;
    pool_t * pool = env->pool;

    assert(parentNode != NULL);
    newNode = ( XmlElement *  ) PMALLOC( sizeof( XmlElement ) );
    if ( newNode == NULL )
    {
        cwmp_log_error("cwmp_xml_create_child_node XMALLOC is error: newNode" );
        return NULL;
    }
    else
    {
        XmlElementInit( newNode );
        XmlElementSetTagName(pool , newNode, nodeName);
        XmlNodeSetNodeName(pool ,  (xmlnode_t *)newNode, nodeName );
        XmlNodeSetNodeValue(pool , (xmlnode_t *)newNode, nodeValue );


        XmlNodeAppendChild(parentNode, (xmlnode_t *)newNode);

    }

    return (xmlnode_t *)newNode;
}

int cwmp_xml_set_node_attribute(env_t * env,  xmlnode_t * node, const char * name, const char * value)
{
    int rv = XmlElementSetAttribute(env->pool, (XmlElement *)node, name, value);
    if (rv == XML_OK)
        return CWMP_OK;
    else
        return CWMP_ERROR;
}

void cwmp_xml_copy_to_chunk_with_escape(cwmp_chunk_t * cb,   IN char *p , pool_t * pool)
{
    int i;
    size_t plen;

    if ( p == NULL )
        return;

    plen = TRstrlen( p );

    for ( i = 0; i < plen; i++ )
    {
        switch ( p[i] )
        {
        case '<':
            cwmp_chunk_write_string(cb, "&lt;", 4, pool);
            break;

        case '>':
            cwmp_chunk_write_string(cb, "&gt;", 4, pool);
            break;

        case '&':
            cwmp_chunk_write_string(cb, "&amp;", 5, pool);
            break;

        case '\'':
            cwmp_chunk_write_string(cb, "&apos;", 6, pool);
            break;

        case '\"':
            cwmp_chunk_write_string(cb, "&quot;", 6, pool);
            break;

        default:
            cwmp_chunk_write_string(cb, &p[i], 1, pool);

            break;
        }
    }
}


int cwmp_xml_dom_tree_print_to_chunk(xmlnode_t * node, cwmp_chunk_t * cb, pool_t * pool )
{
    char *nodeName = NULL;
    char *nodeValue = NULL;
    xmlnode_t *  child = NULL;
    xmlnode_t *  sibling = NULL;

    if ( node != NULL )
    {
        nodeName = ( char * )XmlNodeGetNodeName( node )? ( char * )XmlNodeGetNodeName( node ):"";
        nodeValue = XmlNodeGetNodeValue( node );
        cwmp_log_debug("dom tree xmlnode type is %d", XmlNodeGetNodeType( node ));
        switch ( XmlNodeGetNodeType( node ) )
        {

        case XML_TEXT_NODE:
            cwmp_xml_copy_to_chunk_with_escape(cb, nodeValue, pool);

            break;

        case XML_CDATA_SECTION_NODE:
            cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);
            break;


        case XML_PROCESSING_INSTRUCTION_NODE:
            cwmp_chunk_write_string(cb, "<?", 2, pool);
            cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
            cwmp_chunk_write_string(cb, " ", 1, pool);
            cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);
            cwmp_chunk_write_string(cb, "?>\n", 3, pool);

            break;

        case XML_DOCUMENT_NODE:
            cwmp_xml_dom_tree_print_to_chunk(XmlNodeGetFirstChild( node ), cb, pool);

            break;

        case XML_ATTRIBUTE_NODE:
            cwmp_log_debug ("dom tree attribute: %s,%s", nodeName, nodeValue);
            cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
            cwmp_chunk_write_string(cb, "=\"", 2, pool);
            cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);
            cwmp_chunk_write_string(cb, "\"", 1, pool);

            if ( node->nextSibling != NULL )
            {
                cwmp_chunk_write_string(cb, " ", 1, pool);
                cwmp_xml_dom_tree_print_to_chunk(node->nextSibling, cb, pool);
            }
            break;

        case XML_ELEMENT_NODE:
            cwmp_chunk_write_string(cb, "<", 1, pool);
            cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);

            if ( node->firstAttr != NULL )
            {
                cwmp_chunk_write_string(cb, " ", 1, pool);
                cwmp_xml_dom_tree_print_to_chunk(node->firstAttr, cb, pool);

            }

            child = XmlNodeGetFirstChild( node );
            if ( ( child != NULL )
                    && ( XmlNodeGetNodeType( child ) == XML_ELEMENT_NODE ) )
            {
                cwmp_chunk_write_string(cb, ">\n", 2, pool);

                cwmp_xml_dom_tree_print_to_chunk(node, cb, pool);
                //  output the children

            }
            else
            {
                cwmp_chunk_write_string(cb, ">", 1, pool);

                if (nodeValue)
                {
                    cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);

                }
            }

            cwmp_chunk_write_string(cb, "</", 2, pool);
            // Done with children.  Output the end tag.
            cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);

            sibling = XmlNodeGetNextSibling( node );
            if ( sibling != NULL
                    && XmlNodeGetNodeType( sibling ) == XML_TEXT_NODE )
            {
                cwmp_chunk_write_string(cb, ">", 1, pool);

            }
            else
            {
                cwmp_chunk_write_string(cb, ">\n", 2, pool);
            }
            cwmp_xml_dom_tree_print_to_chunk(sibling, cb, pool);

            break;

        default:
            break;
        }
    }
    return XML_OK;
}


int cwmp_xml_print_doc_to_chunk(xmldoc_t *   doc, cwmp_chunk_t * cb, pool_t * pool )
{
    xmlnode_t * nodeptr = &doc->node;

    char *nodeName = NULL;
    char *nodeValue = NULL;
    xmlnode_t *  child = NULL;

    if ( nodeptr == NULL)
    {
        return CWMP_OK;
    }

    nodeName = ( char * )XmlNodeGetNodeName( nodeptr )? ( char * )XmlNodeGetNodeName( nodeptr ) :"";
    nodeValue = XmlNodeGetNodeValue( nodeptr );

    switch ( XmlNodeGetNodeType( nodeptr ) )
    {

    case XML_TEXT_NODE:
    case XML_CDATA_SECTION_NODE:
    case XML_PROCESSING_INSTRUCTION_NODE:
    case XML_DOCUMENT_NODE:
        cwmp_xml_dom_tree_print_to_chunk(nodeptr, cb, pool);

        break;

    case XML_ATTRIBUTE_NODE:
        cwmp_log_debug ("attribute: %s,%s", nodeName, nodeValue);
        cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
        cwmp_chunk_write_string(cb, "=\"", 2, pool);
        cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);
        cwmp_chunk_write_string(cb, "\"", 1, pool);
        break;

    case XML_ELEMENT_NODE:
        cwmp_chunk_write_string(cb, "<", 1, pool);

        cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
        if ( nodeptr->firstAttr != NULL )
        {
            cwmp_chunk_write_string(cb, " ", 1, pool);
            cwmp_xml_dom_tree_print_to_chunk(nodeptr->firstAttr, cb, pool);
        }

        child = XmlNodeGetFirstChild( nodeptr );
        if ( ( child != NULL )
                && ( XmlNodeGetNodeType( child ) == XML_ELEMENT_NODE ) )
        {
            cwmp_chunk_write_string(cb, ">\n", 2, pool);

            cwmp_xml_dom_tree_print_to_chunk(XmlNodeGetFirstChild( nodeptr ), cb, pool);
            //  output the children

        }
        else
        {
            cwmp_chunk_write_string(cb, ">", 1, pool);

            if (nodeValue)
            {
                cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);

            }
        }

        // Done with children.  Output the end tag.
        cwmp_chunk_write_string(cb, "</", 2, pool);
        cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
        cwmp_chunk_write_string(cb, ">\n", 2, pool);
        break;

    default:
        break;
    }
    return CWMP_OK;
}


void cwmp_set_envelope_ns(const char * envstr, const char * encstr)
{
    const char * envs;
    const char * encs;
    char buffer[CWMP_NAME_MAX] = {0};
    if (envstr == NULL)
    {
        envs = SOAP_ENV_DEFAULT;
    }
    else
    {
        envs = envstr;
    }

    if (encstr == NULL)
    {
        encs = SOAP_ENC_DEFAULT;
    }
    else
    {
        encs = encstr;
    }

    if(TRstrcasecmp(SOAP_ENV, envs) == 0)
    {
        return;
    }

    cwmp_log_debug("ENV: %s, ENC: %s", envs, encs);
    TRstrncpy(SOAP_ENV,  envs, CWMP_NAME_MAX);
    TRstrncpy(SOAP_ENC,  encs, CWMP_NAME_MAX);


    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", envs, SOAP_XML_HEADER);
    TRstrncpy(SOAP_ENV_HEADER, buffer, CWMP_NAME_MAX);

    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", envs, SOAP_XML_BODY);
    TRstrncpy(SOAP_ENV_BODY, buffer, CWMP_NAME_MAX);

    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", envs, SOAP_XML_FAULT);
    TRstrncpy(SOAP_ENV_FAULT, buffer, CWMP_NAME_MAX);

    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", envs, SOAP_XML_ENVELOPE);
    TRstrncpy(SOAP_ENV_ENVELOPE, buffer, CWMP_NAME_MAX);

    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", encs, SOAP_TYPE_ARRAYTYPE);
    TRstrncpy(SOAP_ENC_ARRAYTYPE, buffer, CWMP_NAME_MAX);

    cwmp_log_debug("%s\n%s\n%s\n%s", SOAP_ENV_HEADER, SOAP_ENV_BODY, SOAP_ENV_ENVELOPE, SOAP_ENC_ARRAYTYPE);
}

void cwmp_set_faultcode(fault_code_t * fault, int code)
{
    fault->fault_code = code;
//    fault->fault_string = FAULT_STRING(code);
}

void cwmp_initialize_header(header_t * header)
{
    TRBZERO(header->id, CWMP_NAME_MAX);
    header->hold_requests = -1;
    header->no_more_requests = -1;
}

parameter_node_t * cwmp_initialize_parameter_node(env_t * env ,
        parameter_node_t * root, const char * name,
        int rw,
        int type,
        //const char * type,
        const char * value,
        parameter_get_handler_pt get,
        parameter_set_handler_pt set,
        parameter_notify_handler_pt notify)
{
    parameter_node_t * node;
    char tmp[42];
    char * nodename;
    cwmp_log_debug("cwmp_initialize_parameter_node ...");
    if (cwmp_create_parameter_node(env ,  &node, name) != 0)
    {
        return NULL;
    }

    nodename = strrchr(name, '.');
    if (nodename)
    {
        node->name = XSTRDUP(nodename + 1);
    }
    else
    {
        node->name = XSTRDUP(name);
    }

    snprintf(tmp, sizeof(tmp), "cpe-%s", node->name);
    node->alias = XSTRDUP(tmp);

    node->rw = rw;
    node->type = type;
    //node->type = type;
    node->value = XSTRDUP(value);
    if (value)
    {
        node->value_length = TRstrlen(value);
    }
    node->get = get;
    node->set = set;
    node->notify = notify;

    return node;
}

int cwmp_add_child_parameter_node(parameter_node_t * parent, parameter_node_t * child)
{
    parameter_node_t * node;
    cwmp_log_debug("cwmp_add_child_parameter_node ...");

    for (node = parent->child; node && node->next_sibling; node = node->next_sibling);

    if (node)
    {
        node->next_sibling = child;
    }
    else
    {
        parent->child = child;
    }

    child->prev_sibling = node;
    child->parent = parent;
    return 0;
}

int  cwmp_add_parameter_to_list(env_t * env ,  parameter_list_t * pl, parameter_t * parameter)
{
    if (pl->count >= pl->size-1)
    {
        parameter_t ** pp = XREALLOC(pl->parameters, pl->size * sizeof(parameter_t*), sizeof(parameter_t*) * (pl->size+CWMP_RENEW_SIZE));
        if (pp == NULL)
        {
            return CWMP_ERROR;
        }

        pl->parameters = pp;
        pl->size += CWMP_RENEW_SIZE;
    }
    pl->parameters[pl->count++] = parameter;
    return CWMP_OK;
}

void  cwmp_add_event_to_list(env_t * env ,  event_list_t * eventList, event_code_t * event)
{
    eventList->events[eventList->count++] = event;
}

int cwmp_split_parameter_name_list(char * name, char * namelist[])
{
    int i = 0;
    char * p;
    char * s = name;
    namelist[i++] = name;

    while ((p = strstr(s, ".")))
    {
        (*p) = 0;
        p ++;
        namelist[i++] = p;
    }
    namelist[i] = 0;

    return 0;
}

xmlnode_t * cwmp_get_header_node(xmldoc_t *  doc)
{
    xmlnode_t *  node;
    xmlnode_t *  root;

    if (doc == NULL) {
        cwmp_log_error("xml is null");
        return NULL;
    }

    if (! (root = XmlNodeGetDocRoot(doc)))
    {
        cwmp_log_error("xml document root is null!");
        return NULL;
    }

    node = cwmp_xml_get_child_with_name(root, SOAP_ENV_HEADER);
    if (node == NULL)
    {
        cwmp_log_debug("xml soap header not found!");
    }
    return node;
}

xmlnode_t * cwmp_get_body_node(xmldoc_t *  doc)
{
    xmlnode_t *  node;
    xmlnode_t *  root;

    if (doc == NULL) {
        cwmp_log_error("xml is null");
        return NULL;
    }

    if (! (root = XmlNodeGetDocRoot(doc)))
    {
        cwmp_log_error("xml document root is null!");
        return NULL;
    }

    node = cwmp_xml_get_child_with_name(root, SOAP_ENV_BODY);
    if (node == NULL)
    {
        cwmp_log_info("xml soap body not found!");
    }

    return node;
}

xmlnode_t *  cwmp_get_rpc_method_node(xmldoc_t *  doc)
{
    xmlnode_t * body;
    body = cwmp_get_body_node(doc);
    if (!body)
    {
        return NULL;
    }
    return XmlNodeGetFirstChild(body);
}

xmlnode_t * cwmp_get_rpc_node(xmldoc_t *   doc, const char * method)
{
    xmlnode_t * node;
    node = cwmp_get_rpc_method_node(doc);
    if (!node)
    {
        cwmp_log_error("doc get method is null!");
        return NULL;
    }
    if (TRstrcmp(node->nodeName, method))
    {
        cwmp_log_debug("doc get method(%s) is not %s!", node->nodeName, method);
        return NULL;
    }
    return node;
}

char * cwmp_get_rpc_method_name(xmldoc_t *  doc)
{
    xmlnode_t * node = cwmp_get_rpc_method_node(doc);
    if (!node)
    {
        return NULL;
    }
    return node->nodeName;
}

char * cwmp_get_parameter_nodename(const char * name, char * buffer)
{
    char *p = (char *)name;
    char *q = buffer;

    while (*p && *p != '.') {
        *q++ = *p++;
    }

    if (*p) p++;
    *q = '\0';

    return p;
}

int
cwmp_get_parameter_fullpath(parameter_node_t *param, char *out, size_t out_size)
{
    parameter_node_t *p = param;
    size_t len = 0u;
    char *tmp = calloc(1, out_size);

    if (!tmp) {
        cwmp_log_error("malloc() error: %s", strerror(errno));
        return CWMP_ERROR;
    }

    while (len < out_size && p && p->name) {
        if (!*out) {
            len = snprintf(out, out_size, "%s", p->name);
        } else {
            len = snprintf(tmp, out_size, "%s.%s", p->name, out);
            memcpy(out, tmp, len);
        }
        p = p->parent;
    }
    free(tmp);
    return CWMP_OK;
}


parameter_node_t * cwmp_get_parameter_node(parameter_node_t * root, const char * param_name)
{
    parameter_node_t * node = root;
    char * dot;
    char  name[256];

    if ((!node) || (!param_name))
        return NULL;

    dot = (char*)param_name;
    while (*dot) {

        dot = cwmp_get_parameter_nodename(dot, name);

        while (node && node->name) {
            if (!TRstrcmp(node->name, name) ||
                    !TRstrcmp(node->alias, name)) {
                break;
            }
            node = node->next_sibling;
        }

        if (!node) {
            break;
        }

        if ((dot) && (*dot == 0)) {
            break;
        }

        node = node->child;
    }
    if (!node) {
        cwmp_log_error("Not found param node: %s (segment: %s)",
                param_name, name);
    }
    return node;
}


parameter_node_t * cwmp_get_parameter_path_node(parameter_node_t * parent, const char * param_name)
{
    parameter_node_t * param_node = parent;
    const char * dot;
    char  name[256];
    cwmp_log_trace("%s(parent=%p, param_name=\"%s\")",
            __func__, (void*)parent, param_name);

    if ((!param_node) || (!param_name))
        return NULL;

    dot = (char *)param_name;
    while (*dot) {
        dot = cwmp_get_parameter_nodename(dot, name);
        while (param_node && param_node->name) {
            if(TRstrcmp(param_node->name, "{i}") == 0) {
                param_node = param_node->next_sibling;
                continue;
            }

            if (TRstrcmp(param_node->name, name) == 0 ||
                    TRstrcmp(param_node->alias, name) == 0) {
                //cwmp_log_debug("Found param node: %s", name);
                break;
            }
            if (param_node->next_sibling) {
                param_node = param_node->next_sibling;
            } else {
                if (*dot != 0) {
                    cwmp_log_error("Error param node path. %s", param_name);
                    return NULL;
                } else {
                    if (param_node->parent) {
                        //cwmp_log_debug("Found param node path: %s.", param_node->parent->name);
                    } else {
                        cwmp_log_info("Not found param node parent path: %s.", param_name);
                    }
                    return param_node->parent;
                }
            }
        }

        if (!param_node) {
            return NULL;
        }

        if ((dot) && (*dot == 0)) {
            break;
        }
        if (param_node->child) {
            param_node = param_node->child;
        } else {
            break;
        }

    }
    if (param_node) {
        cwmp_log_debug("Found param node path: %s.", param_node->name);
    } else {
        cwmp_log_error("Not found param node path: %s.", param_name);
    }
    return param_node;
}

int cwmp_get_parameter_node_value(cwmp_t * cwmp, parameter_node_t * node, const char * name, char ** value, pool_t * pool)
{
    if (!node)
    {
        return FAULT_CODE_9000;
    }
    if (node->get)
    {
        return (*node->get)(cwmp, name, value, node->args, pool);
    }
    else
    {
        return FAULT_CODE_9000;
    }
}

int cwmp_set_parameter_node_value(cwmp_t * cwmp, parameter_node_t * node, const char * name, const char * value, int value_length)
{
    if (!node)
    {
        return CWMP_ERROR;
    }
    if (node->set)
    {
        return (*node->set)(cwmp, name,  value, value_length, node->args, callback_register_task);
    }
    else
    {
        if (node->value)
        {
            free(node->value);
        }

        //it's ok, no memory loss
        node->value = TRstrdup(value);
        node->value_length = value_length;
        return CWMP_OK;
    }
}


int cwmp_parse_header_node(xmlnode_t * node, header_t ** header, pool_t * pool)
{
    xmlnode_t * cwmpIdNode;
    xmlnode_t * cwmpHoldRequestsNode;
    xmlnode_t * cwmpNoMoreRequestsNode;
    char * value;

    *header  = NULL;
    if (node)
    {
        (*header) = pool_pcalloc(pool, sizeof(header_t));
        cwmpIdNode = cwmp_xml_get_child_with_name(node, CWMP_XML_HEADER_ID);
        cwmpHoldRequestsNode = cwmp_xml_get_child_with_name(node, CWMP_XML_HEADER_HOLDREQUESTS);
        cwmpNoMoreRequestsNode = cwmp_xml_get_child_with_name(node, CWMP_XML_HEADER_NOMOREREQUESTS);
        if (cwmpIdNode == NULL || cwmpHoldRequestsNode == NULL || cwmpNoMoreRequestsNode == NULL)
        {
            cwmp_log_debug("TR069Header cwmp:ID=%s, cwmp:HoldRequests=%s, cwmp:NoMoreRequests=%s",
                           cwmpIdNode ? cwmp_xml_get_node_value(cwmpIdNode):"null",
                           cwmpHoldRequestsNode ? cwmp_xml_get_node_value(cwmpHoldRequestsNode):"null",
                           cwmpNoMoreRequestsNode ? cwmp_xml_get_node_value(cwmpNoMoreRequestsNode):"null");
        }
        value = cwmp_xml_get_node_value(cwmpIdNode);
        if ((cwmpIdNode != NULL) || (value != NULL))
        {
            (*header)->id = pool_pcalloc(pool, CWMP_HEAD_MAX+1);
            TRstrncpy((*header)->id, value, CWMP_HEAD_MAX);
        }

        if (cwmpHoldRequestsNode != NULL)
        {
            value = cwmp_xml_get_node_value(cwmpHoldRequestsNode);
            (*header)->hold_requests = TRatoi(value);
        }

        if (cwmpNoMoreRequestsNode != NULL)
        {
            value = cwmp_xml_get_node_value(cwmpNoMoreRequestsNode);
            (*header)->no_more_requests = TRatoi(value);
        }


        return CWMP_OK;
    }

    return CWMP_ERROR;
}

int cwmp_parse_inform_response_message(xmlnode_t * node, unsigned int *max_envelopes)
{
    xmlnode_t * cwmpMaxEnvelopes;
    const char * value;
    if (node)
    {
        cwmpMaxEnvelopes = cwmp_xml_get_child_with_name(node, CWMP_XML_INFORM_MAXENVELOPES);
        if (!cwmpMaxEnvelopes)
        {
            return CWMP_ERROR;
        }
        value = XmlNodeGetNodeValue(cwmpMaxEnvelopes);
        if (!value)
        {
            cwmp_log_error("Invalid InformResponse MaxEnvelopes is null");
            return CWMP_ERROR;
        }
        (*max_envelopes) = TRatoi(value);
        return CWMP_OK;
    }

    return CWMP_ERROR;
}

//cwmp_parse_getrpcmethods_message
xmldoc_t * cwmp_parse_getrpcmethods_message(xmldoc_t *doc)
{
    return NULL;
}

int cwmp_parse_getparameternames_message(env_t * env, xmldoc_t * doc, char ** path_name, unsigned int * next_level, fault_code_t *fault)
{
    xmlnode_t * cwmpParamPath;
    xmlnode_t * cwmpNextLevel;
    const char * nl;
    xmlnode_t * node = NULL;

    cwmp_log_trace("%s(env=%p, doc=%p, path_name=%p, next_level=%p, fault=%p)",
            __func__, (void*)env, (void*)doc, (void*)path_name, (void*)next_level, (void*)fault);
    node = cwmp_get_rpc_method_node(doc);

    cwmpParamPath = cwmp_xml_get_child_with_name(node, CWMP_XML_GETPARAMETERNAMES_PARAMETERPATH);
    cwmpNextLevel = cwmp_xml_get_child_with_name(node, CWMP_XML_GETPARAMETERNAMES_NEXTLEVEL);
    if (path_name)
    {
        *path_name = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmpParamPath));

    }
    nl = cwmp_xml_get_node_value(cwmpNextLevel);
    if (!nl)
    {
        cwmp_log_error("Invalid GetParameterNames NextLevel is null");
        nl = "0";
    }
    if (TRstrcmp(nl,"true") == 0 || TRstrcmp(nl,"1") == 0)
    {
        (*next_level) = 1;
    }
    else
    {
        (*next_level) = 0;
    }
    return CWMP_OK;
}

int cwmp_parse_getparametervalues_message_parameter_iterator(env_t * env, parameter_list_t ** ppl, parameter_node_t * root, parameter_node_t * upperLevelNode, char* nodePath, pool_t * pool)
{

    int rc = CWMP_OK;
    parameter_node_t * parameterNode = upperLevelNode->child;
    while (parameterNode)
    {
        parameter_node_t * pnode  = parameterNode;
        parameterNode = parameterNode->next_sibling;

        if (pnode->type == TYPE_OBJECT && pnode->refresh)
        {
            (*pnode->refresh)(env->cwmp, pnode, callback_register_task, pool);
        }

        char cname[1024];
        char * name = &cname[0];

        if (nodePath != NULL)
        {
            strncpy(name,nodePath,1024);
            strncat(name,".",1024);
            strncat(name,pnode->name,1024);
        }
        else
        {
            strncat(name,pnode->name,1024);
        }

        if (strcmp(pnode->name, "{i}") == 0)
        {
            continue;
        }

        cwmp_log_debug("cwmp_parse_getparametervalues_message_parameter_iterator: param name %s .", name);

        parameter_t * parameter = cwmp_parameter_list_add_parameter(env, pool, ppl, root, name, NULL, 1, 0);//Add with getter exec

        if(!parameter || parameter->fault_code != FAULT_CODE_OK)
        {
//          cwmp_set_faultcode(fault, FAULT_CODE_9003);
            cwmp_log_error("cwmp_parse_getparametervalues_message: parameter (%s) getter returned fault code %i", name, parameter->fault_code);
            rc = CWMP_ERROR;
            continue;
        }


        if (parameter->type == TYPE_OBJECT) {
//          cwmp_log_error("WARN cwmp_parameter_list_add_parameter: object sent instead of param %s.", name);
            cwmp_parse_getparametervalues_message_parameter_iterator(env, ppl, root, pnode, name, pool);
        }
        else
        {
            /* OK */
        }

    }

    return rc;

}

int cwmp_parse_getparameterattributes_message_parameter_iterator(env_t * env, parameter_list_t ** ppl, parameter_node_t * root, parameter_node_t * upperLevelNode, char* nodePath, pool_t * pool)
{

    int rc = CWMP_OK;
    parameter_node_t * parameterNode = upperLevelNode->child;
    while (parameterNode)
    {
        parameter_node_t * pnode  = parameterNode;
        parameterNode = parameterNode->next_sibling;

        if (pnode->type == TYPE_OBJECT && pnode->refresh)
        {
            (*pnode->refresh)(env->cwmp, pnode, callback_register_task, pool);
        }

        char cname[1024];
        char * name = &cname[0];

        if (nodePath != NULL)
        {
            strncpy(name,nodePath,1024);
            strncat(name,".",1024);
            strncat(name,pnode->name,1024);
        }
        else
        {
            strncat(name,pnode->name,1024);
        }

        if (strcmp(pnode->name, "{i}") == 0)
        {
            continue;
        }

        cwmp_log_debug("cwmp_parse_getparameterattributes_message_parameter_iterator: param name %s .", name);

        parameter_t * parameter = cwmp_parameter_list_add_parameter(env, pool, ppl, root, name, NULL, 1, 0);//Add with getter exec

        if(!parameter || parameter->fault_code != FAULT_CODE_OK)
        {
//          cwmp_set_faultcode(fault, FAULT_CODE_9003);
            cwmp_log_error("cwmp_parse_getparametervalues_message: parameter (%s) getter returned fault code %i", name, parameter->fault_code);
            rc = CWMP_ERROR;
            continue;
        }


        if (parameter->type == TYPE_OBJECT) {
//          cwmp_log_error("WARN cwmp_parameter_list_add_parameter: object sent instead of param %s.", name);
            cwmp_parse_getparameterattributes_message_parameter_iterator(env, ppl, root, pnode, name, pool);
        }
        else
        {
            /* OK */
        }

    }

    return rc;

}

//cwmp_parse_getparametervalues_message
int cwmp_parse_getparametervalues_message(env_t * env , xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault)
{
    xmlnode_t * parameterListNode;
    xmlnode_t * parameterNode;
    xmlnode_t * node = NULL;

    cwmp_log_trace("%s(env=%p, doc=%p, root=%p, ppl=%p, fault=%p)",
            __func__, (void*)env, (void*)doc, (void*)root, (void*)ppl, (void*)fault);
    nvram_init(RT2860_NVRAM);

    node = cwmp_get_rpc_method_node(doc);
    if (!(parameterListNode = cwmp_xml_get_child_with_name(node, CWMP_RPC_PARAMETERNAMES))) {
        parameterListNode = cwmp_xml_get_child_with_name(node, CWMP_RPC_PARAMETERNAMES_SHORT);
    }

    if (!parameterListNode || !ppl)
    {
        cwmp_log_error("ERROR cwmp_parse_getparametervalues_message : request contains no ParameterNames node.");
        return CWMP_ERROR;
    }

    *ppl = cwmp_create_parameter_list(env );
    ESE(CWMP_ERROR, NULL, *ppl);

    pool_t * pool = pool_create(__func__, POOL_DEFAULT_SIZE);

    const char * name;

    parameterNode = XmlNodeGetFirstChild(parameterListNode);
    while (parameterNode)
    {
        xmlnode_t * pnode  = parameterNode;
        parameterNode = XmlNodeGetNextSibling(parameterNode);
        name = cwmp_xml_get_node_value(pnode);

        parameter_node_t * pn = cwmp_get_parameter_node(root, name);

        if (pn)
        {

            int rc = CWMP_OK;

            if (pn->type == TYPE_OBJECT)
            {
                char* name2 = pool_pstrdup(pool,name);
                int len = strlen(name2);
                if (len>0 && name2[len-1] == '.')
                {
                    name2[len-1] = '\0';
                }

                rc = cwmp_parse_getparametervalues_message_parameter_iterator(env, ppl, root, pn, name2, pool);
            }
            else
            {
                /*parameter_t * parameter =*/ cwmp_parameter_list_add_parameter(env, pool, ppl, root, name, NULL, 1, 0); //Add with getter exec
            }

            if (rc != CWMP_OK)
            {
                cwmp_set_faultcode(fault, FAULT_CODE_9003);
            }
        }
    }

    cwmp_log_debug("DEBUG: cwmp_parse_getparametervalues_message OK");

    pool_destroy(pool);
    return CWMP_OK;
}

parameter_t*  cwmp_parameter_list_add_parameter(env_t * env, pool_t * pool , parameter_list_t ** ppl, parameter_node_t * root, const char * name, const char * value, BOOL exec_get, BOOL exec_set)
{
    parameter_t* parameter = NULL;

    cwmp_log_trace("%s(env=%p, pool=%p, ppl=%p, root=%p [name=%s], "
            "name=%s, value=%s, exec_get=%s, exec_set=%s)",
            __func__, (void*)env, (void*)pool, (void*)ppl,
            (void*)root, root ? root->name : "",
            name, value,
            exec_get ? "true" : "false",
            exec_set ? "true" : "false"
            );

    parameter = cwmp_create_parameter(env, name, value, TRstrlen(value), 0);
    if (!parameter) {
        cwmp_log_error("%s: unable to create parameter (%s=%s)", __func__, name, value);
        return NULL;
    }

    parameter_node_t * pn = cwmp_get_parameter_node(root, name);
    if (pn) {
        parameter->type = pn->type;

        if (pn->type == TYPE_OBJECT) {
            // cwmp_log_warn("%s: object parameter (%s)", name);
            return parameter;
        }
    } else {
        //Create Fault code;
        parameter->fault_code = FAULT_CODE_9003;
        cwmp_log_warn("%s: can not find parameter node %s.", __func__, name);
    }

    // Execute getter
    if (exec_get && parameter && parameter->fault_code == FAULT_CODE_OK) {
        if (pn->get) {
            //exec get
            char * val = NULL;
            parameter->fault_code = (*pn->get)(env->cwmp, name, &val, pn->args, pool);
            if(parameter->fault_code != FAULT_CODE_OK) {
                // fault->fault_code = rc;
                cwmp_log_error("%s: parameter %s got error [getter: %s]",
                        __func__, name, cwmp_model_ptr_to_func((void*)pn->get));
                // return CWMP_ERROR;
            } else {
                parameter = cwmp_create_parameter(env,  name, val, TRstrlen(value), pn->type);
            }

        } else {
            parameter = cwmp_create_parameter(env ,  name, pn->value, pn->value_length, pn->type);
            cwmp_log_warn("%s: parameter %s has no getter.", __func__, name);
        }

        if (!parameter) {
            cwmp_log_warn("%s: parameter %s not available.", __func__, name);
        }
    }

    // Execute setter
    if (exec_set && parameter && parameter->fault_code == FAULT_CODE_OK) {
        if(pn->set) {
            //exec set function
            parameter->fault_code =  (*pn->set)(env->cwmp, name,  value, TRstrlen(value), pn->args, callback_register_task);
            if (parameter->fault_code != FAULT_CODE_OK) {
                cwmp_log_error("%s: parameter %s got error (value: %s) [setter: %s]",
                        __func__, name, value, cwmp_model_ptr_to_func((void*)pn->set));
/*
                // disable reload task
                if (pn->reload) {
                    queue_uniq_mark_invalid(env->cwmp->queue, pn->reload, TASK_RELOAD_TAG);
                }
*/
            } else if (pn->reload) {
                // reload task
                queue_uniq_push(env->cwmp->queue, pn->reload, TASK_RELOAD_TAG);
            }
        } else {
            cwmp_log_warn("%s: parameter %s has no setter", __func__, name);
            parameter->fault_code = FAULT_CODE_9008;
        }
    }

    if (cwmp_add_parameter_to_list(env, *ppl, parameter) != CWMP_OK) {
        cwmp_log_error("%s: parameter %s add error.", __func__, name);
    }
    return parameter;
}

int  cwmp_parse_setparameterattributes_message(env_t * env , xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault)
{
    cwmp_log_info("cwmp_parse_setparameterattributes_message");

    xmlnode_t * parameterListNode;
    xmlnode_t * parameterNode;
    parameter_t ** nextpv;
    int rc = CWMP_OK;


    parameterListNode = cwmp_xml_get_child_with_name(cwmp_get_rpc_method_node(doc), "ParameterList");


    if (!parameterListNode || !ppl)
    {
        return CWMP_ERROR;
    }

    *ppl = cwmp_create_parameter_list(env);
    ESE(CWMP_ERROR, NULL, *ppl);

    nextpv = (*ppl)->parameters;

    parameterNode = XmlNodeGetFirstChild(parameterListNode);

///////////////////////////////////////////////////////
    pool_t * pool = pool_create(__func__, POOL_DEFAULT_SIZE);

    const char * name;
    const char * value;
    parameter_t * parameter;

    while (parameterNode)
    {
        xmlnode_t * pnode  = parameterNode;
        parameterNode = XmlNodeGetNextSibling(parameterNode);

        name = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "Name"));
        value = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "Notification"));

        parameter = cwmp_parameter_list_add_parameter(env, pool, ppl, root, name, value, 0, 0);//Add without execs

        if(!parameter || parameter->fault_code != FAULT_CODE_OK)
        {
            cwmp_set_faultcode(fault, FAULT_CODE_9003);
            cwmp_log_error("%s: parameter (%s) setter returned fault code %i", __func__, name, parameter->fault_code);
            rc = CWMP_ERROR;
        }
        else
        {
            parameter_node_t * pn = cwmp_get_parameter_node(root, name);
            if (pn && pn->inform == 0)
            {
                switch (value[0])
                {
                    case '0': pn->inform = 0; break;
                    case '1': pn->inform = 1; break;
                    case '2': pn->inform = 1; break;
                }
            }
        }

        if (parameter) {
            *nextpv = parameter;
            nextpv++;
        }
    }

    cwmp_log_info("cwmp_parse_setparameterattributes_message OK");
    pool_destroy(pool);
    return rc;
}

int  cwmp_parse_getparameterattributes_message(env_t * env , xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault)
{
    xmlnode_t * parameterListNode;
    xmlnode_t * parameterNode;
    xmlnode_t * node = NULL;

    cwmp_log_trace("%s(env=%p, doc=%p, root=%p, ppl=%p, fault=%p)",
            __func__, (void*)env, (void*)doc, (void*)root, (void*)ppl, (void*)fault);

    node = cwmp_get_rpc_method_node(doc);
    if (!(parameterListNode = cwmp_xml_get_child_with_name(node, CWMP_RPC_PARAMETERNAMES))) {
        parameterListNode = cwmp_xml_get_child_with_name(node, CWMP_RPC_PARAMETERNAMES_SHORT);
    }

    if (!parameterListNode || !ppl)
    {
        cwmp_log_error("ERROR cwmp_parse_getparameterattributes_message : request contains no ParameterNames node.");
        return CWMP_ERROR;
    }

    *ppl = cwmp_create_parameter_list(env );
    ESE(CWMP_ERROR, NULL, *ppl);

    pool_t * pool = pool_create(__func__, POOL_DEFAULT_SIZE);

    const char * name;

    parameterNode = XmlNodeGetFirstChild(parameterListNode);
    while (parameterNode)
    {
        xmlnode_t * pnode  = parameterNode;
        parameterNode = XmlNodeGetNextSibling(parameterNode);
        name = cwmp_xml_get_node_value(pnode);

        parameter_node_t * pn = cwmp_get_parameter_node(root, name);

        if (pn)
        {
//            int rc = CWMP_OK;
            cwmp_log_info("cwmp_parse_getparameterattributes_message: param: %s", name);

            if (pn->type == TYPE_OBJECT)
            {
                cwmp_log_info("cwmp_parse_getparameterattributes_message: object param: %s", name);
                char* name2 = pool_pstrdup(pool,name);
                int len = strlen(name2);
                if (len>0 && name2[len-1] == '.')
                {
                    name2[len-1] = '\0';
                }

//                rc = cwmp_parse_getparameterattributes_message_parameter_iterator(env, ppl, root, pn, name2, pool);
            }
            else
            {
                cwmp_parameter_list_add_parameter(env, pool, ppl, root, name, NULL, 1, 0);//Add with getter exec
            }

//            if (rc != CWMP_OK)
//            {
//                cwmp_set_faultcode(fault, FAULT_CODE_9003);
//            }
        }
    }

    cwmp_log_debug("DEBUG: cwmp_parse_getparameterattributes_message OK");

    pool_destroy(pool);
    return CWMP_OK;

}

//cwmp_parse_setparametervalues_message
int  cwmp_parse_setparametervalues_message(env_t * env , xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault)
{
    xmlnode_t * node = NULL;
    xmlnode_t * parameterListNode;
    xmlnode_t * parameterNode;
    parameter_t ** nextpv;
    int rc = CWMP_OK;

    char *parameter_key = NULL;
    xmlnode_t * parameter_key_node = NULL;

    pool_t * pool = NULL;

    cwmp_log_trace("%s(env=%p, doc=%p, root=%p [name=%s], ppl=%p, fault=%p)",
            __func__,
            (void*)env, (void*)doc,
            (void*)root, root ? root->name : "",
            (void*)ppl, (void*)fault);

    node = cwmp_get_rpc_method_node(doc);

    parameterListNode = cwmp_xml_get_child_with_name(node, "ParameterList");
    if (!parameterListNode || !ppl) {
        return CWMP_ERROR;
    }

    parameter_key_node = cwmp_xml_get_child_with_name(node, "ParameterKey");

    *ppl = cwmp_create_parameter_list(env);
    ESE(CWMP_ERROR, NULL, *ppl);

    nextpv = (*ppl)->parameters;

    parameterNode = XmlNodeGetFirstChild(parameterListNode);

///////////////////////////////////////////////////////
    pool = pool_create(__func__, POOL_DEFAULT_SIZE);

    const char * name;
    const char * value;
    parameter_t * parameter;

    while (parameterNode) {
        xmlnode_t * pnode  = parameterNode;
        parameterNode = XmlNodeGetNextSibling(parameterNode);

        name = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "Name"));
        value = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "Value"));

        parameter = cwmp_parameter_list_add_parameter(env, pool, ppl, root, name, value, 0, 1);//Add with setter exec

        if(!parameter || parameter->fault_code != FAULT_CODE_OK) {
            cwmp_set_faultcode(fault, FAULT_CODE_9003);
            cwmp_log_error("%s: parameter (%s) setter returned fault code %i", __func__, name, parameter->fault_code);
            rc = CWMP_ERROR;
        } else {
        }

        if (parameter) {
                *nextpv = parameter;
            nextpv++;
        }
    }

    if (rc != CWMP_ERROR && parameter_key_node != NULL) {
        parameter_key = cwmp_xml_get_node_value(parameter_key_node);
        cwmp_conf_set("cwmp:ParameterKey", parameter_key ? parameter_key : "");
    }

    pool_destroy(pool);
    return rc;
}

int cwmp_parse_download_message(env_t * env , xmldoc_t *doc, download_arg_t ** pdlarg, fault_code_t *fault)
{
//    xmlnode_t * commandKeyNode;
//    xmlnode_t * cwmpNextLevel;

    FUNCTION_TRACE();
    xmlnode_t * node = cwmp_get_rpc_method_node(doc);
    download_arg_t * dlarg = pool_pcalloc(env->pool, sizeof(download_arg_t));

   //FIXME
    dlarg->cmdkey = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "CommandKey")));
    dlarg->filetype= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "FileType")));
    dlarg->url = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "URL")));
    dlarg->username= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "Username")));
    dlarg->password = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "Password")));
    dlarg->filesize= TRatoi(cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "FileSize")));

    dlarg->targetname= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "TargetFileName")));
    dlarg->delaysec= TRatoi(cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "DelaySeconds")));
    dlarg->succurl= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "SuccessURL")));
    dlarg->failurl= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "FailureURL")));

    *pdlarg = dlarg;

    return CWMP_OK;
}


int cwmp_parse_upload_message(env_t * env , xmldoc_t *doc, upload_arg_t ** pularg, fault_code_t *fault)
{
    xmlnode_t * node = cwmp_get_rpc_method_node(doc);
    upload_arg_t * ularg = pool_pcalloc(env->pool, sizeof(download_arg_t));

    ularg->cmdkey = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "CommandKey")));
    ularg->filetype= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "FileType")));
    ularg->url = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "URL")));
    ularg->username= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "Username")));
    ularg->password = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "Password")));
    ularg->delaysec= TRatoi(cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "DelaySeconds")));

    *pularg = ularg;

    return CWMP_OK;
}


int cwmp_parse_addobject_message(env_t * env , xmldoc_t *doc, parameter_node_t * root,  int * instances, int* status, fault_code_t *fault)
{
    xmlnode_t * node = NULL;
    xmlnode_t * parameter_key_node = NULL;
    int fault_code;
    int instance_num;
    char *object_name = NULL;
    char *parameter_key = NULL;

    cwmp_log_trace("%s(env=%p, doc=%p, root=%p [name=%s], instaces=%p, status=%p, fault=%p)",
            __func__,
            (void*)env,
            (void*)doc,
            (void*)root, root ? root->name : "",
            (void*)instances, (void*)status, (void*)fault);
    //FIXME
    *status = 0;

    node = cwmp_get_rpc_method_node(doc);
    object_name = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "ObjectName"));
    parameter_key_node = cwmp_xml_get_child_with_name(node, "ParameterKey");

    parameter_node_t * param = cwmp_get_parameter_path_node(root, object_name);
    if(!param)
    {
        cwmp_log_error("can't find AddObject parameter ObjectName %s", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }
//    parameter_node_t * child_param = param->child;
    if( (param->type != TYPE_OBJECT) || (param->rw  != 1))// || (TRstrcmp(child_param->name, "{i}") != 0))
    {
        cwmp_log_error("AddObject parameter ObjectName %s is invalid or not writable", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }
    if(! param->add)
    {
        cwmp_log_error("could not find %s add object function", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    fault_code = param->add(env->cwmp, param, &instance_num, callback_register_task);

    if(fault_code != FAULT_CODE_OK)
    {
        cwmp_log_error("exec %s add object function failed", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    } else if (parameter_key_node != NULL) {
        parameter_key = cwmp_xml_get_node_value(parameter_key_node);
        cwmp_conf_set("cwmp:ParameterKey", parameter_key ? parameter_key : "");
    }

    *instances = instance_num;

    return CWMP_OK;
}


int cwmp_parse_deleteobject_message(env_t * env , xmldoc_t *doc, parameter_node_t * root, int* status, fault_code_t *fault)
{

    xmlnode_t * node = NULL;
    int fault_code;
    int instance_num;

    xmlnode_t * parameter_key_node = NULL;
    char *parameter_key = NULL;

    char * object_name = NULL;
    cwmp_log_trace("%s(env=%p, doc=%p, root=%p [name=%s], status=%p, fault=%p)",
            __func__,
            (void*)env, (void*)doc,
            (void*)root, root ? root->name : "",
            (void*)status, (void*)fault);

    node = cwmp_get_rpc_method_node(doc);
    object_name = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "ObjectName"));
    parameter_key_node = cwmp_xml_get_child_with_name(node, "ParameterKey");

    parameter_node_t * param = cwmp_get_parameter_path_node(root, object_name);
    if(!param)
    {
        cwmp_log_error("can't find DeleteObject parameter ObjectName %s", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    if((param->type == TYPE_OBJECT && object_name[strlen(object_name)-1] != '.') ||  param->type != TYPE_OBJECT)
    {
        cwmp_log_error("DeleteObject parameter ObjectName %s is invalid", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    if(is_digit(param->name) != 0)
    {
        cwmp_log_error("DeleteObject parameter ObjectName %s is not digit", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    if(! param->del)
    {
        cwmp_log_error("could not find %s delete object function", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    instance_num = TRatoi(param->name);

    fault_code = param->del(env->cwmp, param, instance_num, callback_register_task);

    if(fault_code != FAULT_CODE_OK)
    {
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    } else if( param->reload) {
        queue_push(env->cwmp->queue, param->reload, TASK_RELOAD_TAG);
    }

    if (parameter_key_node != NULL) {
        parameter_key = cwmp_xml_get_node_value(parameter_key_node);
        cwmp_conf_set("cwmp:ParameterKey", parameter_key ? parameter_key : "");
    }

    return CWMP_OK;
}


int cwmp_parse_reboot_message(env_t * env , xmldoc_t *doc, char ** key, fault_code_t *fault)
{
    cwmp_log_debug("DEBUG: cwmp_parse_reboot_message");
    xmlnode_t * commandKeyNode;

    commandKeyNode = cwmp_xml_get_child_with_name(cwmp_get_rpc_method_node(doc), "CommandKey");

    if (!commandKeyNode)
    {
        return CWMP_ERROR;
    }

    *key = cwmp_xml_get_node_value(commandKeyNode);

    return CWMP_OK;
}

download_arg_t * cwmp_clone_download_arg(download_arg_t * dlarg)
{
    if(!dlarg)
    {
        return NULL;
    }
    download_arg_t * newdl = MALLOC(sizeof(download_arg_t));
    if(!newdl)
    {
        return NULL;
    }
    newdl->cmdkey = TRstrdup(dlarg->cmdkey);
    newdl->filetype = TRstrdup(dlarg->filetype);
    newdl->url = TRstrdup(dlarg->url);
    newdl->username = TRstrdup(dlarg->username);
    newdl->password = TRstrdup(dlarg->password);
    newdl->targetname = TRstrdup(dlarg->targetname);
    newdl->succurl = TRstrdup(dlarg->succurl);
    newdl->failurl = TRstrdup(dlarg->failurl);
    newdl->delaysec = dlarg->delaysec;
    newdl->filesize = dlarg->filesize;

    cwmp_log_debug("download arg: %s, %s, %s, %s, %s, targetname:%s,%s,%s, delaysecond:%d,%d",
        newdl->cmdkey?newdl->cmdkey:"null",
        newdl->filetype?newdl->filetype:"null",
        newdl->url?newdl->url:"null",
        newdl->username?newdl->username:"null",
        newdl->password?newdl->password:"null",
        newdl->targetname?newdl->targetname:"null",
        newdl->succurl?newdl->succurl:"null",
        newdl->failurl?newdl->cmdkey:"null",
        newdl->delaysec, newdl->filesize);

    return newdl;
}

upload_arg_t * cwmp_clone_upload_arg(upload_arg_t * ularg)
{
    if(!ularg)
    {
        return NULL;
    }
    upload_arg_t * newul = MALLOC(sizeof(upload_arg_t));
    if(!newul)
    {
        return NULL;
    }
    newul->cmdkey = TRstrdup(ularg->cmdkey);
    newul->filetype = TRstrdup(ularg->filetype);
    newul->url = TRstrdup(ularg->url);
    newul->username = TRstrdup(ularg->username);
    newul->password = TRstrdup(ularg->password);
    newul->delaysec = ularg->delaysec;


    return newul;

}

parameter_t* cwmp_create_parameter(env_t * env ,  const char * name, const char * value, size_t value_length, int type)
{
    parameter_t * pv = XMALLOC(sizeof(parameter_t));

    cwmp_log_trace("%s(env=%p, name=\"%s\", value=\"%s\", value_length=%"PRIuPTR", type=%d",
            __func__, (void*)env, name, value, value_length, type);

    if (!pv) {
        return NULL;
    }

    pv->name = XSTRDUP(name);

    pv->value = XSTRDUP(value);
    pv->value_length = value_length;

    pv->type = type;
    pv->fault_code = 0;

    return pv;
}

event_list_t * cwmp_create_event_list(env_t * env, int size )
{
    event_list_t * el;
    el = cwmp_event_list_create(env->pool, size);

    return el;
}

event_code_t * cwmp_create_event_code(env_t * env )
{
    event_code_t * ev;
    ev = cwmp_event_code_create(env->pool);
    return ev;
}

parameter_list_t* cwmp_create_parameter_list(env_t * env )
{
    parameter_list_t * pl;
    pl = XMALLOC(sizeof(parameter_list_t));
    if (!pl)
    {
        return NULL;
    }
    TRBZERO(pl, sizeof(parameter_list_t));

    pl->parameters = XMALLOC(sizeof(parameter_t*) * CWMP_RENEW_SIZE);
    pl->count = 0;
    pl->size = CWMP_RENEW_SIZE;

    return pl;
}

xmlnode_t * cwmp_create_time_node(env_t * env ,   xmlnode_t * parent, char * nodeName, const datatime_t *currentt)
{
    char buffer[CWMP_BUF_SIZE];

    xmlnode_t * currTimeNode;
    FUNCTION_TRACE();
    TRsnprintf(buffer, CWMP_BUF_SIZE, "%4u-%02u-%02uT%02u:%02u:%02u",
               currentt->year,
               currentt->month,
               currentt->day,
               currentt->hour,
               currentt->min,
               currentt->sec);

    ESA(currTimeNode, cwmp_xml_create_child_node(env ,  parent, NULL, nodeName, buffer));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  currTimeNode, SOAP_XSI_TYPE, SOAP_XSD_DATETIME));

    return currTimeNode;
}

xmlnode_t * cwmp_create_current_time_node(env_t * env ,   xmlnode_t * parent, const datatime_t *currentt)
{
    return cwmp_create_time_node(env, parent, "CurrentTime", currentt);
}

xmlnode_t * cwmp_create_event_node(env_t * env ,  xmlnode_t * parent, const event_list_t * eventlist)
{
    xmlnode_t * eventNode, *eventStructNode,  * eventCodeNode, * eventCommandKeyNode;

    int count = 0;

    event_code_t ** pe = eventlist->events;

    FUNCTION_TRACE();
    ESA(eventNode, cwmp_xml_create_child_node(env ,  parent, NULL, "Event", NULL));

    while (count < eventlist->count)
    {
        ESA(eventStructNode, cwmp_xml_create_child_node(env ,  eventNode, NULL, "EventStruct", NULL));

        ESA(eventCodeNode, cwmp_xml_create_child_node(env ,  eventStructNode, NULL, "EventCode", pe[count]->code));
        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  eventCodeNode, SOAP_XSI_TYPE, SOAP_XSD_STRING));


        if (pe[count]->event == INFORM_MREBOOT ) //|| pe[count]->event == INFORM_BOOTSTRAP)
        {
            ESA(eventCommandKeyNode, cwmp_xml_create_child_node(env ,  eventStructNode, NULL, "CommandKey", pe[count]->command_key));
        }
        else
        {
            ESA(eventCommandKeyNode, cwmp_xml_create_child_node(env ,  eventStructNode, NULL, "CommandKey", NULL));
        }

        ++count ;
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  eventNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:EventStruct[%d]", count)));

    return eventNode;
}


xmlnode_t * cwmp_create_header_node(env_t * env ,   xmlnode_t * root, header_t * header)
{
    xmlnode_t * headerNode;
    xmlnode_t * idNode;
    //xmlnode_t * holdRequestsNode;
    FUNCTION_TRACE();
    ESA(headerNode, cwmp_xml_create_child_node(env ,  root, NULL, SOAP_ENV_HEADER, NULL));
    ESA(idNode, cwmp_xml_create_child_node(env ,  headerNode, NULL, CWMP_XML_HEADER_ID, header->id));
    //ESA(holdRequestsNode, cwmp_xml_create_child_node(env ,  headerNode, NULL, CWMP_XML_HEADER_HOLDREQUESTS, NULL));

    ESN(XML_OK, cwmp_xml_set_node_attribute(env , idNode, cwmp_get_format_string("%s:%s", SOAP_ENV, CWMP_XML_MUSTUNDERSTAND), "1"));
    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  holdRequestsNode, XMLNS_APPEND(SOAP_ENV, CWMP_XML_MUSTUNDERSTAND), "1"));

    return headerNode;
}

xmlnode_t * cwmp_create_body_node(env_t * env ,  xmlnode_t * root)
{
    xmlnode_t * bodyNode;
    cwmp_log_trace("%s(env=%p, root=%p)", __func__, (void*)env, (void*)root);
    ESA(bodyNode, cwmp_xml_create_child_node(env ,  root, NULL, SOAP_ENV_BODY, NULL));
    return bodyNode;
}

xmlnode_t * cwmp_create_envelope_node(env_t * env ,  xmlnode_t * parent)
{

    xmlnode_t * envelopeNode;
    FUNCTION_TRACE();
    cwmp_log_debug("ENV: %s, ENC: %s", SOAP_ENV, SOAP_ENC);
    ESA(envelopeNode, cwmp_xml_create_child_node(env ,  parent, NULL, SOAP_ENV_ENVELOPE, NULL));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", SOAP_ENV), SOAP_ENV_NS));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", SOAP_ENC), SOAP_ENC_NS));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", "xsi"), SOAP_XSI_NS));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", "xsd"), SOAP_XSD_NS));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", "cwmp"), SOAP_CWMP_NS));

    return envelopeNode;
}


xmlnode_t * cwmp_create_device_id_node(env_t * env ,  xmlnode_t * parent, const device_id_t * deviceid)
{
    xmlnode_t * deviceIdNode;
    xmlnode_t * mf;
    xmlnode_t * oui;
    xmlnode_t * pc;
    xmlnode_t * sn;

    FUNCTION_TRACE();

    ESA(deviceIdNode, cwmp_xml_create_child_node(env ,  parent, NULL, "DeviceId", NULL));
    ESA(mf, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "Manufacturer", deviceid->manufactorer));
    ESA(oui, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "OUI", deviceid->oui));
    ESA(pc, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "ProductClass", deviceid->product_class));
    ESA(sn, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "SerialNumber",  deviceid->serial_number));


    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  deviceIdNode, SOAP_XSI_TYPE, "cwmp:DeviceIdStruct"));
    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  mf, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    ////ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  mf, SOAP_XSI_NAME, deviceId->name));
    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  oui, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  pc, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  sn, SOAP_XSI_TYPE, SOAP_XSD_STRING));


    return deviceIdNode;
}

xmlnode_t * cwmp_create_max_envelope_node(env_t * env ,  xmlnode_t * parent, unsigned int max_envelope)
{
    xmlnode_t * maxEnvNode;
    FUNCTION_TRACE();

    ESA(maxEnvNode, cwmp_xml_create_child_node(env ,  parent, NULL, CWMP_XML_INFORM_MAXENVELOPES, cwmp_get_format_string("%d", max_envelope)));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  maxEnvNode, SOAP_XSI_TYPE, SOAP_XSD_UNSIGNEDINT));

    return maxEnvNode;
}

xmlnode_t * cwmp_create_retry_count_node(env_t * env ,  xmlnode_t * parent, unsigned int retry_count)
{
    xmlnode_t * retryCountNode;
    FUNCTION_TRACE();

    ESA(retryCountNode, cwmp_xml_create_child_node(env ,  parent, NULL, "RetryCount", cwmp_get_format_string("%d", retry_count)));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  retryCountNode, SOAP_XSI_TYPE, SOAP_XSD_UNSIGNEDINT));

    return retryCountNode;
}

int cwmp_create_parameter_node(env_t * env ,  parameter_node_t ** news, const char * name)
{
    parameter_node_t * param_node = NULL;

    cwmp_log_trace("%s(env=%p, news=%p, name=\"%s\")",
            __func__, (void*)env, (void*)news, name);
    param_node = XMALLOC(sizeof(parameter_node_t));
    if (!param_node)
    {
        return CWMP_ERROR;
    }
    //param_node->full_name = XSTRDUP(name);
    param_node->name = NULL;
    param_node->alias = NULL;
    param_node->rw = 0;
    param_node->type = 0;
    //param_node->type = NULL;
    param_node->value = NULL;
    param_node->child = param_node->next_sibling = param_node->prev_sibling = param_node->parent = NULL;

    (*news) = param_node;

    return CWMP_OK;
}


xmlnode_t * cwmp_create_parameter_list_node(env_t * env ,  xmlnode_t * parent, parameter_list_t * pl)
{
    xmlnode_t * parameterValueStructNode;
    parameter_t ** ps;
    parameter_t* pv;
    xmlnode_t * parameterListNode, *nameNode, *valueNode;
    int i = 0;

    FUNCTION_TRACE();

    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  parent, NULL, "ParameterList", NULL));
    ps = pl->parameters;

    while (i < pl->count)
    {
        pv = *ps;
        ESA(parameterValueStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterValueStruct", NULL));

        ESA(nameNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Name", pv->name));
        ESA(valueNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Value", pv->value));

        //D-Link doesn't support name types
        //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  nameNode, SOAP_XSI_TYPE, SOAP_XSD_STRING));
        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  valueNode, SOAP_XSI_TYPE, SOAP_XSD_STRING));

        parameterValueStructNode = NULL;
        ps ++;
        i++;
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  parameterListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:ParameterValueStruct[%d]", pl->count) ));
    cwmp_log_debug("created parameter list: [%d]", i);
    return parameterListNode;
}

int cwmp_create_inform_message_parameter_iterator(env_t * env, parameter_list_t ** ppl, parameter_node_t * rootNode, parameter_node_t * upperLevelNode, char* nodePath, int sortFilter, pool_t * pool)
{
    int rc = CWMP_OK;
    parameter_node_t * parameterNode = upperLevelNode->child;
    while (parameterNode)
    {
        parameter_node_t * pnode  = parameterNode;
        parameterNode = parameterNode->next_sibling;

        char cname[1024];
        char * name = &cname[0];

        if (nodePath != NULL) {
            strncpy(name,nodePath,1024);
            strncat(name,".",1024);
            strncat(name,pnode->name,1024);
        } else {
            strncat(name,pnode->name,1024);
        }

        // cwmp_log_debug("cwmp_parameter_list_add_parameter: param name %s .", name);
        if (pnode->inform > 0 && pnode->inform_sort == sortFilter) {
            if (pnode->inform == 2)
                pnode->inform = 0;

            cwmp_log_debug("INFORM PARAM: %s", name);
            parameter_t * parameter = cwmp_parameter_list_add_parameter(env, pool, ppl, rootNode, name, NULL, 1, 0); //Add with getter exec

            if(!parameter || parameter->fault_code != FAULT_CODE_OK) {
                cwmp_log_error("%s: parameter (%s) getter returned fault code %i",
                        __func__, name, parameter->fault_code);
                rc = CWMP_ERROR;
                continue;
            }
        }

        if (pnode->child != NULL) {
            cwmp_create_inform_message_parameter_iterator(env, ppl, rootNode, pnode, name, sortFilter, pool);
        }
    }

    return rc;
}

xmldoc_t* cwmp_create_inform_message(env_t * env ,  header_t * header,
                                     device_id_t * deviceid,
                                     event_list_t *events,
                                     datatime_t * currentt,
                                     unsigned int max_envelope,
                                     unsigned int retry_count,
                                     parameter_list_t * pl,
                                     parameter_node_t * root)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * informNode;
    xmlnode_t * headerNode;
    xmlnode_t * deviceIdNode, *eventsNode, *maxenvNode, *currtimeNode, *retryCountNode, *paramlistNode;
    xmldoc_t * doc = NULL;

    cwmp_log_trace(
            "%s(env=%p, deviceid=%p, events=%p, "
            "currentt=%p, max_envelope=%u, retry_count=%u,"
            "pl=%p, root=%p [name=%s])",
            __func__, (void*)header, (void*)deviceid, (void*)events,
            (void*)currentt, max_envelope, retry_count,
            (void*)pl, (void*)root, (root ? root->name : ""));

    doc = XmlDocCreateDocument(env->pool);

    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);

    int n = -2;
    int k = 0;
    while (pl->count > n)
    {
        n = pl->count;
        cwmp_create_inform_message_parameter_iterator(env, &pl, root, root, root->name, k, env->pool);
        k++;
    }

    ESA(informNode,     cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_INFORM, NULL));
    ESA(deviceIdNode    , cwmp_create_device_id_node(env ,  informNode, deviceid));
    ESA(eventsNode      , cwmp_create_event_node(env ,  informNode, events));
    ESA(maxenvNode      , cwmp_create_max_envelope_node(env ,  informNode, max_envelope));
    ESA(currtimeNode    , cwmp_create_current_time_node(env ,  informNode, currentt));
    ESA(retryCountNode  , cwmp_create_retry_count_node(env ,  informNode, retry_count));
    ESA(paramlistNode   , cwmp_create_parameter_list_node(env ,  informNode, pl));



    return doc;
}

//cwmp_create_getrpcmethods_response_message
xmldoc_t * cwmp_create_getrpcmethods_response_message(env_t * env ,  header_t * header, char ** methods, unsigned int count)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * methodListNode;
    xmlnode_t * methodNode;

    xmldoc_t * doc;
    char ** method;
    int num = 0;
    if (!methods)
    {
        return NULL;
    }

    doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_GETRPCMETHODSRESPONSE, NULL));
    ESA(methodListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "MethodList", NULL));

    method = (methods);
    num = 0;
    while ((num < count) && method && (*method))
    {
        ESA(methodNode, cwmp_xml_create_child_node(env ,  methodListNode, NULL, "string", *method));

        method ++;
        num ++;
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  methodListNode, cwmp_get_format_string("%s:%s", "xmlns", SOAP_ENV), SOAP_ENV_NS ));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  methodListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("xsd:string[%d]", count) ));

    return doc;
}

void * cwmp_create_getparameternames_response_all_parameter_names(env_t * env , xmlnode_t * parent_node, const char * path_name, parameter_node_t *  param_node, int * count)
{
    cwmp_buffer_t buffer;
    xmlnode_t * parameterInfoStructNode;
    xmlnode_t * parameterWritableNode;
    xmlnode_t * parameterNameNode;
    parameter_node_t * param_child;

    cwmp_log_trace("%s(env=%p, parent_node=%p, path_name=\"%s\", param_node=%p [name=%s], count=%p)",
            __func__, (void*)env, (void*)parent_node, path_name, (void*)param_node,
            param_node ? param_node->name : NULL, (void*)count);

    if (!param_node)
        return NULL;

    //if(param_node->type != TYPE_OBJECT)
    {
        ESA(parameterInfoStructNode, cwmp_xml_create_child_node(env ,  parent_node, NULL, "ParameterInfoStruct", NULL));
        ESA(parameterNameNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Name", path_name ));
        ESA(parameterWritableNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Writable", param_node->rw==0? "0" : "1"));
        (*count) ++;
    }

    if (param_node->type == TYPE_OBJECT && param_node->refresh)
    {
        (*param_node->refresh)(env->cwmp, param_node, callback_register_task, env->pool);
    }

    for (param_child = param_node->child; param_child; param_child = param_child->next_sibling)
    {
        if(TRstrcmp(param_child->name, "{i}") == 0)
            continue;
        cwmp_buffer_init(&buffer);
        if (param_child->type == TYPE_OBJECT)
        {
            cwmp_buffer_write_format_string(&buffer,"%s%s.", path_name, param_child->name);
        }
        else
        {
            cwmp_buffer_write_format_string(&buffer,"%s%s", path_name, param_child->name);

        }
        cwmp_create_getparameternames_response_all_parameter_names(env, parent_node, cwmp_buffer_string(&buffer), param_child, count);

    }

    return NULL;
}


//cwmp_create_getparameternames_response_message
xmldoc_t* cwmp_create_getparameternames_response_message(env_t * env ,
        header_t * header,
        const char * path_name,
        parameter_node_t * param_node,
        unsigned int next_subset,
        unsigned int next_level)
{
    cwmp_buffer_t buffer;
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * parameterListNode;
    xmlnode_t * parameterInfoStructNode;
    xmlnode_t * parameterWritableNode;
    xmlnode_t * parameterNameNode;

    int count;

    parameter_node_t * child;

    cwmp_log_trace("%s(env=%p, header=%p, path_name=\"%s\", param_node=%p [name=%s], next_subset=%u, next_level=%u)",
            __func__, (void*)env, (void*)header, path_name, (void*)param_node,
            (param_node ? param_node->name : NULL), next_subset, next_level);
    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (!param_node)
    {
        return NULL;
    }

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_GETPARAMETERNAMESRESPONSE, NULL));
    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "ParameterList", NULL));

    count = 0;
    if (next_subset == CWMP_NO)
    {
        ESA(parameterInfoStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterInfoStruct", NULL));
        ESA(parameterNameNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Name", path_name));
        ESA(parameterWritableNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Writable", param_node->rw==0? "0" : "1"));
        count++;
    }
    else
    {
        if (next_level == CWMP_YES)
        {
            if (param_node->type == TYPE_OBJECT && param_node->refresh)
            {
                (*param_node->refresh)(env->cwmp, param_node, callback_register_task, env->pool);
            }

            for (child = param_node->child; child; child = child->next_sibling)
            {
                if(TRstrcmp(child->name, "{i}") == 0)
                    continue;

                cwmp_buffer_init(&buffer);
                if (child->type == TYPE_OBJECT)
                {
                    cwmp_buffer_write_format_string(&buffer,"%s%s.", path_name, child->name);
                }
                else
                {
                    cwmp_buffer_write_format_string(&buffer,"%s%s", path_name, child->name);
                }
                ESA(parameterInfoStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterInfoStruct", NULL));
                ESA(parameterNameNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Name", cwmp_buffer_string(&buffer)));
                ESA(parameterWritableNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Writable", child->rw==0? "0" : "1"));
                count++;

            }
        }
        else
        {
            //all parameters
            cwmp_create_getparameternames_response_all_parameter_names(env, parameterListNode, path_name, param_node, &count);

        }
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  parameterListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:ParameterInfoStruct[%d]", count) ));

    return doc;
}

//cwmp_create_getparametervalues_response_message
xmldoc_t* cwmp_create_getparametervalues_response_message(env_t * env ,  header_t * header, parameter_list_t * pl)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * parameterListNode;

    xmlnode_t * parameterValueStructNode;
    xmlnode_t * nameStructNode;
    xmlnode_t * valueStructNode;
    int count;
    parameter_t ** pv;
    parameter_t * pvc;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_GETPARAMETERVALUESRESPONSE, NULL));
    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "ParameterList", NULL));
    pv = pl->parameters;
    count = 0;
    while (count < pl->count)
    {
        pvc = *(pv+count);
        ESA(parameterValueStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterValueStruct", NULL));
        ESA(nameStructNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Name", pvc->name));
        ESA(valueStructNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Value", pvc->value));
        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  valueStructNode, SOAP_XSI_TYPE, CWMP_TYPE( (*(pv+count))->type ) ));
        count++;
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  parameterListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:ParameterValueStruct[%d]", pl->count) ));

    return doc;
}

xmldoc_t * cwmp_create_setparametervalues_response_message(env_t * env ,  header_t * header, unsigned int status, parameter_list_t * pl)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  &doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);

    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_SETPARAMETERVALUESRESPONSE, NULL));
//    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "ParameterList", NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));

    return doc;
}

xmldoc_t * cwmp_create_setparameterattributes_response_message(env_t * env ,  header_t * header, unsigned int status, parameter_list_t * pl)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  &doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_SETPARAMETERATTRIBUTESRESPONSE, NULL));

    return doc;
}

xmldoc_t * cwmp_create_getparameterattributes_response_message(env_t * env , parameter_node_t * root,  header_t * header, unsigned int status, parameter_list_t * pl)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * parameterListNode;

    xmlnode_t * parameterValueStructNode;
    xmlnode_t * nameStructNode;
    xmlnode_t * notifyStructNode;
    xmlnode_t * aclStructNode;

    int count;
    parameter_t ** pv;
    char * parameterName  = NULL;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_GETPARAMETERATTRIBUTESRESPONSE, NULL));
    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "ParameterList", NULL));
    pv = pl->parameters;
    count = 0;
    while (count < pl->count)
    {
        parameterName = (*(pv+count))->name;
        parameter_node_t * pn = cwmp_get_parameter_node(root, parameterName);
        char* notificationStr = "0";
        if (pn != NULL && pn->notify != 0)
        {
            notificationStr = "1";
        }

        ESA(parameterValueStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterAttributeStruct", NULL));
        ESA(nameStructNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Name", parameterName));
        ESA(notifyStructNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Notification", notificationStr));
//        ESA(notifyStructNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Notification", "0"));
        ESA(aclStructNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "AccessList", "")); // FIXME

//        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  notifyStructNode, SOAP_XSI_TYPE, CWMP_TYPE("xsd:string") ));
        count++;
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  parameterListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:ParameterAttributeStruct[%d]", pl->count) ));

    return doc;
}


xmldoc_t * cwmp_create_download_response_message(env_t * env , header_t * header, int status)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;
    xmlnode_t * startTimeNode;
    xmlnode_t * completeTimeNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_DOWNLOADRESPONSE, NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));
    ESA(startTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "StartTime", NULL));
    ESA(completeTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "CompleteTime", NULL));

    return doc;
}

xmldoc_t * cwmp_create_upload_response_message(env_t * env , header_t * header, int status)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;
    xmlnode_t * startTimeNode;
    xmlnode_t * completeTimeNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_UPLOADRESPONSE, NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));
    ESA(startTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "StartTime", NULL));
    ESA(completeTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "CompleteTime", NULL));



    return doc;
}

xmldoc_t * cwmp_create_addobject_response_message(env_t * env , header_t * header, int instances, int status)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;
    xmlnode_t * startTimeNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_ADDOBJECTRESPONSE, NULL));

    ESA(startTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "InstanceNumber", TRitoa(instances)));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));

    return doc;
}

xmldoc_t * cwmp_create_deleteobject_response_message(env_t * env , header_t * header, int status)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_DELETEOBJECTRESPONSE, NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));

    return doc;
}

xmldoc_t * cwmp_create_reboot_response_message(env_t * env ,  header_t * header)
{
    cwmp_log_debug("DEBUG: cwmp_create_reboot_response_message");

    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_REBOOTRESPONSE, NULL));

    return doc;
}

xmldoc_t * cwmp_create_factoryreset_response_message(env_t * env ,  header_t * header)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_FACTORYRESETRESPONSE, NULL));

    return doc;
}


xmldoc_t* cwmp_create_faultcode_response_message(env_t * env , header_t * header, fault_code_t * fault)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
//    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * faultStructNode;
    xmlnode_t * newNode, *detailNode, *faultNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );

    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env , envelopeNode, header));
    }
    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);

    cwmp_log_debug("create fault response , code is %d", fault->fault_code);
    ESA(faultStructNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, SOAP_ENV_FAULT, NULL));

    switch(fault->fault_code)
    {
        case FAULT_CODE_9000:
        case FAULT_CODE_9001:
        case FAULT_CODE_9002:
        case FAULT_CODE_9004:
        case FAULT_CODE_9009:
        case FAULT_CODE_9010:
        case FAULT_CODE_9011:
        case FAULT_CODE_9012:
        case FAULT_CODE_9013:
        case FAULT_CODE_9014:
        case FAULT_CODE_9015:
        case FAULT_CODE_9016:
        case FAULT_CODE_9017:
        case FAULT_CODE_9018:
        case FAULT_CODE_9019:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Server"));
            break;
        }
        default:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Client"));
            break;
        }
    }

    ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultstring", "Client fault"));

    ESA(detailNode, cwmp_xml_create_child_node(env ,  newNode, NULL, "detail", NULL));
    ESA(faultNode, cwmp_xml_create_child_node(env ,  detailNode, NULL, "cwmp:Fault", NULL));


    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultCode", TRitoa(fault->fault_code)));
    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultString", FAULT_STRING(fault->fault_code)));

    return doc;
}

xmldoc_t* cwmp_create_faultcode_setparametervalues_response_message(env_t * env , header_t * header, parameter_list_t * param_list, fault_code_t * fault)
{
    int i, count;
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
//    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * faultStructNode;
    xmlnode_t * newNode, *detailNode, *faultNode;
    parameter_t ** param = param_list->parameters;

    FUNCTION_TRACE();

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );

    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env , envelopeNode, header));
    }
    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);

    cwmp_log_debug("create fault response , code is %d", fault->fault_code);
    ESA(faultStructNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, SOAP_ENV_FAULT, NULL));

    switch(fault->fault_code)
    {
        case FAULT_CODE_9000:
        case FAULT_CODE_9001:
        case FAULT_CODE_9002:
        case FAULT_CODE_9004:
        case FAULT_CODE_9009:
        case FAULT_CODE_9010:
        case FAULT_CODE_9011:
        case FAULT_CODE_9012:
        case FAULT_CODE_9013:
        case FAULT_CODE_9014:
        case FAULT_CODE_9015:
        case FAULT_CODE_9016:
        case FAULT_CODE_9017:
        case FAULT_CODE_9018:
        case FAULT_CODE_9019:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Server"));
            break;
        }
        default:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Client"));
            break;
        }
    }

    ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultstring", "Client fault"));

    ESA(detailNode, cwmp_xml_create_child_node(env ,  newNode, NULL, "detail", NULL));
    ESA(faultNode, cwmp_xml_create_child_node(env ,  detailNode, NULL, "cwmp:Fault", NULL));


    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultCode", TRitoa(fault->fault_code)));
    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultString", FAULT_STRING(fault->fault_code)));

    cwmp_log_debug("cwmp_create_faultcode_setparametervalues_response_message count %d, %p", param_list->count, *param);
    for(i=0, count = param_list->count; (*param != NULL) && (i<count); i++, param++)
    {
        ESA(faultStructNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "SetParameterValuesFault", NULL));
        ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "ParameterName", (*param)->name));
        ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultCode", TRitoa((*param)->fault_code)));
        ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultString", FAULT_STRING((*param)->fault_code)));
    }


    return doc;
}


xmldoc_t * cwmp_create_transfercomplete_message(env_t * env ,  header_t * header, event_code_t * evcode)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * rpcNode;
    xmlnode_t * headerNode;
    xmlnode_t * node;

        xmlnode_t * faultStructNode;
        xmlnode_t * faultCode;
        xmlnode_t * faultString;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        ESA(headerNode, cwmp_create_header_node(env ,  envelopeNode, header));
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(rpcNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_TRANSFERCOMPLETE, NULL));
    ESA(node, cwmp_xml_create_child_node(env ,  rpcNode, NULL, "CommandKey", evcode->command_key));
    ESA(faultStructNode, cwmp_xml_create_child_node(env ,  rpcNode, NULL, "FaultStruct", NULL));

    if(evcode->fault_code)
    {
        ESA(faultCode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultCode", TRitoa(evcode->fault_code)));
        ESA(faultString, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultString", FAULT_STRING(evcode->fault_code)));
    }
    else
    {
        ESA(faultCode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultCode", "0"));
        ESA(faultString, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultString", ""));
    }

    datatime_t startTime, endTime;
    time_to_datatime(&startTime, evcode->start);
    time_to_datatime(&endTime, evcode->end);

    ESA(node, cwmp_create_time_node(env, rpcNode, "StartTime", &startTime ));
    ESA(node, cwmp_create_time_node(env, rpcNode, "CompleteTime", &endTime ));

    return doc;
}

int cwmp_write_doc_to_chunk(xmldoc_t *  doc, cwmp_chunk_t * chunk, pool_t * pool)
{
    // return cwmp_xml_print_doc_to_chunk(doc, chunk, pool);
    char * xml;
    xml = XmlPrintDocument(pool, doc);
    cwmp_chunk_write_string(chunk, xml, TRstrlen(xml), pool);

    return CWMP_OK;
}
