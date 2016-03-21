/************************************************************************
 *                                                                      *
 * Netcwmp/Opencwmp Project                                             *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU General Public License          *
 * as published by the Free Software Foundation; either version 2       *
 * of the License, or (at your option) any later version.               *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the                *
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,          *
 * Boston, MA  02111-1307 USA                                           *
 *                                                                      *
 * Copyright 2013-2014  Mr.x(Mr.x) <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#include <cwmp/upload.h>

#include "cwmp_module.h"
#include "cwmp_agent.h"
#include <cwmp/session.h>
#include <cwmp/http.h>
#include "modules/data_model.h"

#define CWMP_TRUE   1

#define MAX_SESSION_RETRY 3

enum
{
    CWMP_ST_START = 0, CWMP_ST_INFORM, CWMP_ST_SEND, CWMP_ST_RESEMD, CWMP_ST_RECV, CWMP_ST_ANSLYSE, CWMP_ST_RETRY, CWMP_ST_END, CWMP_ST_EXIT
};

int cwmp_agent_retry_session(cwmp_session_t * session)
{

    int sec = 0;

    srand(time(NULL));
    switch (session->retry_count)
    {
    case 0:
    {
        sec = 5 + rand()%5; //5~10
        break;
    }
    case 1:
    {
        sec = 5 + rand()%10; //5~15
        break;
    }
    case 2:
    {
        sec = 5 + rand()%20; //5~25
        break;
    }
    default:
    {
        sec = 5 + rand()%30; //5~35
        break;
    }
    }

    while (sec>0)
    {
        sleep(1);
        sec--;
    }

    if (session->retry_count > MAX_SESSION_RETRY)
    {
        session->retry_count = 0;
        return CWMP_TIMEOUT;
    }
    else
    {
        session->retry_count ++;
        return CWMP_OK;
    }

}

int cwmp_agent_create_datetimes(datatime_t *nowtime)
{
    struct tm t;
    time_t tn;

    //FUNCTION_TRACE();
    tn = time(NULL);
#ifdef WIN32
    cwmp_log_debug("inform datatime");
    //localtime_s(&t, &tn);
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

//取得active event以及count
int cwmp_agent_get_active_event(cwmp_t *cwmp, cwmp_session_t * session,  event_list_t **pevent_list)
{
    event_list_t * el;
    event_code_t * ev;
    int i=0;
    FUNCTION_TRACE();

    el = cwmp_create_event_list(session->env, INFORM_MAX);

    event_code_t ** pec = cwmp->el->events;

    int elsize = cwmp->el->count;
    for(i=0; i<elsize; i++)
    {
	if(pec[i]  && pec[i]->ref > 0)
        {
            event_code_t * ec = pec[i];
            ev = cwmp_create_event_code(session->env);
            ev->event = ec->event;
            ev->code = ec->code;
            if (pec[i]->event == INFORM_MREBOOT || pec[i]->event == INFORM_BOOTSTRAP)
            {
                strcpy(ev->command_key , ec->command_key);
            }
            el->events[el->count++] = ev;
            ev = NULL;
        }
    }
    if (el->count == 0)
    {
        ev = cwmp_create_event_code(session->env);
        ev->event = INFORM_BOOT;
        ev->code = CWMP_INFORM_EVENT_CODE_1;
        el->events[el->count++] = ev;
    }


    *pevent_list = el;

    return CWMP_OK;
}

int cwmp_agent_send_request(cwmp_session_t * session)
{
    FUNCTION_TRACE();
    return cwmp_session_send_request(session);
}

int cwmp_agent_recv_response(cwmp_session_t * session)
{
    return cwmp_session_recv_response(session);
}

void cwmp_agent_start_session(cwmp_t * cwmp)
{
    int rv;
    unsigned int periodic = 0;
    cwmp_session_t * session;
    int session_close = CWMP_NO;
    xmldoc_t * newdoc;
    FUNCTION_TRACE();
    event_list_t  *evtlist;

    while (TRUE)
    {
        if (cwmp->new_request == CWMP_NO)
        {
            cwmp_log_debug("No new request from ACS\n");
            sleep(2);
	    periodic++;
            //cwmp->new_request = CWMP_YES;

	    if (periodic<2000) 
	    {
        	continue;
	    }
	    else
	    {
	        cwmp_log_error("Periodic response\n");
		periodic = 0;
	    }
        }
	else
	{
            cwmp_log_error("### ### ### New request from ACS ### ### ###\n");
	}


        cwmp->new_request = CWMP_NO;
        session = cwmp_session_create(cwmp);
        session_close  = CWMP_NO;
        session->timeout = cwmp_conf_get_int("cwmpd:http_timeout");
        //cwmp_session_set_timeout(cwmp_conf_get_int("cwmpd:http_timeout"));
        cwmp_log_debug("session timeout is %d", session->timeout);
        cwmp_session_open(session);

        while (!session_close)
        {
            cwmp_log_info("session status: %d", session->status);

            switch (session->status)
            {
            case CWMP_ST_START:
                //create a new connection to acs
                cwmp_log_debug("session status: New START\n");

                if (cwmp_session_connect(session, cwmp->acs_url) != CWMP_OK)
                {
                    cwmp_log_error("connect to acs: %s failed.\n", cwmp->acs_url);
                    session->status = CWMP_ST_RETRY;
                }
                else
                {
                    session->status = CWMP_ST_INFORM;
                }
                break;
            case CWMP_ST_INFORM:
				evtlist = NULL;
				cwmp_log_debug("session stutus: INFORM\n");
				cwmp_agent_get_active_event(cwmp, session,  & evtlist);
				if(evtlist != NULL)
				{
					cwmp_event_clear_active(cwmp);
				}
				cwmp_log_debug("session stutus: INFORM2\n");
                if (cwmp->acs_auth)
                {
                    cwmp_session_set_auth(session,   cwmp->acs_user  , cwmp->acs_pwd );
                }

		cwmp_log_debug("session stutus: INFORM3\n");
                newdoc = cwmp_session_create_inform_message(session, evtlist, session->envpool);

                cwmp_write_doc_to_chunk(newdoc, session->writers,  session->envpool);
                session->last_method = CWMP_INFORM_METHOD;
                session->status = CWMP_ST_SEND;


                break;

            case CWMP_ST_SEND:

                cwmp_log_debug("session stutus: SEND");
                cwmp_log_debug("session data request length: %d", cwmp_chunk_length(session->writers));
                session->newdata = CWMP_NO;

                rv = cwmp_agent_send_request(session);

                if (rv == CWMP_OK)
                {
					cwmp_log_debug("session data sended OK, rv=%d", rv);
					session->status = CWMP_ST_RECV;
                }
				else
				{
					cwmp_log_debug("session data sended faild! rv=%d", rv);
					session->status = CWMP_ST_EXIT;
                }

                break;
            case CWMP_ST_RECV:
                cwmp_log_debug("session stutus: RECV");
                cwmp_chunk_clear(session->readers);

                rv = cwmp_agent_recv_response(session);

                if (rv == HTTP_200 || rv == CWMP_OK)
                {
                    session->status = CWMP_ST_ANSLYSE;
                }
                else
                {
                    session->status = CWMP_ST_END;
                }
                break;

            case CWMP_ST_ANSLYSE:
                cwmp_log_debug("session stutus: ANSLYSE");
                rv = cwmp_agent_analyse_session(session);
                if (rv == CWMP_OK)
                {
                    session->status = CWMP_ST_SEND;
                }
                else
                {
                    session->status = CWMP_ST_END;
                }
                break;
            case CWMP_ST_RETRY:
				cwmp_log_debug("session stutus: RETRY");
                if (cwmp_agent_retry_session(session) == CWMP_TIMEOUT)
                {
					cwmp_log_debug("session retry timeover, go out");
                    session->status = CWMP_ST_EXIT;
                }
                else
                {
                    session->status = CWMP_ST_START;
                }
                break;
            case CWMP_ST_END:
                //close connection of ACS
                cwmp_log_debug("session stutus: END");
				//run task from queue

                if (session->newdata == CWMP_YES)
                {
                    session->status = CWMP_ST_SEND;
                }
                else
                {
                    session->status = CWMP_ST_EXIT;
                }
                break;

            case CWMP_ST_EXIT:
                cwmp_log_debug("session stutus: EXIT");
                cwmp_session_close(session);
                if (session->reconnect == CWMP_YES)
                {
                    session->reconnect = CWMP_NO;
                    session->status = CWMP_ST_START;
                    break;
                }
                session_close = CWMP_YES;
                break;


            default:
				cwmp_log_debug("Unknown session stutus");
                break;
            }//end switch
        }//end while(!session_close)

        cwmp_log_error("session stutus: EXIT");
        cwmp_session_free(session);
        session = NULL;

		int newtaskres = cwmp_agent_run_tasks(cwmp);
		if(newtaskres == CWMP_YES)
		{
			cwmp->new_request = CWMP_YES;
		}
    }//end while(TRUE)
}

int cwmp_agent_analyse_session(cwmp_session_t * session)
{
    cwmp_log_error("DEBUG: cwmp_agent_analyse_session");
    pool_t * doctmppool  = NULL;
    char * xmlbuf;
    cwmp_uint32_t len;
    xmldoc_t *  doc;
    char * method;
    xmldoc_t *   newdoc = NULL;
    int rc;

    static char * xml_fault = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns=\"urn:dslforum-org:cwmp-1-0\"><SOAP-ENV:Body SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"  id=\"_0\"><SOAP-ENV:Fault>Error Message</SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>";

    cwmp_uint32_t msglength = cwmp_chunk_length(session->readers);

    if (msglength<= 0)
    {
        session->newdata = CWMP_NO;
        cwmp_log_debug("analyse receive length is 0");
	cwmp_log_error("DEBUG: cwmp_agent_analyse_session ERROR 1");
	goto eventcheck;
    }

    doctmppool = pool_create(POOL_DEFAULT_SIZE);

    xmlbuf = pool_palloc(doctmppool, msglength+32);

    len = sprintf(xmlbuf,"<cwmp>");
    cwmp_chunk_copy(xmlbuf + len, session->readers, msglength);
    strcpy(xmlbuf+len+msglength, "</cwmp>");

    cwmp_log_debug("agent analyse xml: \n%s", xmlbuf);

    doc = XmlParseBuffer(doctmppool, xmlbuf);
    if (!doc)
    {
        cwmp_log_debug("analyse create doc null\n");
        cwmp_chunk_write_string(session->writers, xml_fault, TRstrlen(xml_fault), session->envpool);
	cwmp_log_error("DEBUG: cwmp_agent_analyse_session ERROR 2");
        goto finished;

    }

    method = cwmp_get_rpc_method_name(doc);
//    cwmp_log_debug("analyse method is: %s\n", method);
    cwmp_log_error("analyse method is: %s\n", method);

    cwmp_chunk_clear(session->writers);
    pool_clear(session->envpool);

    if (TRstrcmp(method, CWMP_RPC_GETRPCMETHODS) == 0)
    {
        newdoc = cwmp_session_create_getrpcmethods_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_INFORMRESPONSE) == 0)
    {
        newdoc = NULL;
    }
    else if (TRstrcmp(method, CWMP_RPC_GETPARAMETERNAMES) == 0)
    {
        newdoc = cwmp_session_create_getparameternames_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_GETPARAMETERVALUES) == 0)
    {
        newdoc = cwmp_session_create_getparametervalues_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_SETPARAMETERVALUES) == 0)
    {
        newdoc = cwmp_session_create_setparametervalues_response_message(session, doc, doctmppool);
    }

    else if (TRstrcmp(method, CWMP_RPC_SETPARAMETERATTRIBUTES) == 0)
    {
        newdoc = cwmp_session_create_setparameterattributes_response_message(session, doc, doctmppool);
    }


    else if (TRstrcmp(method, CWMP_RPC_DOWNLOAD) == 0)
    {
        newdoc = cwmp_session_create_download_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_UPLOAD) == 0)
    {
        newdoc = cwmp_session_create_upload_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_TRANSFERCOMPLETERESPONSE) == 0)
    {
        newdoc = NULL;
    }
    else if (TRstrcmp(method, CWMP_RPC_REBOOT) == 0)
    {
	cwmp_log_error("DEBUG: cwmp_agent_analyse_session REBOOT");
        newdoc = cwmp_session_create_reboot_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_ADDOBJECT) == 0)
    {
        newdoc = cwmp_session_create_addobject_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_DELETEOBJECT) == 0)
    {
        newdoc = cwmp_session_create_deleteobject_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_FACTORYRESET) == 0)
    {
        newdoc = cwmp_session_create_factoryreset_response_message(session, doc, doctmppool);
    }
    else
    {
    	//check event queue
    	//newdoc = cwmp_session_create_event_response_message(session, doc, doctmppool);
	cwmp_log_error("DEBUG: cwmp_agent_analyse_session UNKNOWN METHOD: %s", method);
    }

    cwmp_t * cwmp = session->cwmp;
    if(newdoc == NULL)
    {
        cwmp_log_error("agent analyse newdoc is null. ");
eventcheck:
	{
		cwmp_log_debug("agent analyse begin check global event, %d", cwmp->event_global.event_flag);
		//check global event for transfercomplete
		if(cwmp->event_global.event_flag & EVENT_REBOOT_TRANSFERCOMPLETE_FLAG)
		{
			cwmp->event_global.event_flag &=  ~EVENT_REBOOT_TRANSFERCOMPLETE_FLAG;
			if(!doctmppool)
			{
				doctmppool = pool_create(POOL_DEFAULT_SIZE);
			}
			event_code_t ec;
			ec.event = INFORM_TRANSFERCOMPLETE;
			TRstrncpy(ec.command_key, cwmp->event_global.event_key, COMMAND_KEY_LEN);
			ec.fault_code = cwmp->event_global.fault_code;
			ec.start = cwmp->event_global.start;
			ec.end = cwmp->event_global.end;
			newdoc = cwmp_session_create_transfercomplete_message(session, &ec, doctmppool);
		}
	}

    }


    cwmp_log_error("newdoc %p, msglength: %d", newdoc, msglength );
    if((newdoc != NULL) || (newdoc == NULL && msglength != 0)) // || (newdoc == NULL && msglength == 0 && session->retry_count < 2))
    {
        session->newdata = CWMP_YES;
        cwmp_write_doc_to_chunk(newdoc, session->writers,  session->envpool);
	rc = CWMP_OK;
        cwmp_log_error("DEBUG: cwmp_agent_analyse_session OK");
    }
    else
    {
	rc = CWMP_ERROR;
        cwmp_log_error("DEBUG: cwmp_agent_analyse_session ERR");
    }

finished:
    if(doctmppool  != NULL)
    {
	pool_destroy(doctmppool);
    }

    return rc;
}

static void print_param(parameter_node_t * param, int level)
{
  if(!param) return;

  parameter_node_t * child;
  char fmt[64];
  //cwmp_log_debug("name: %s, type: %s, level: %d\n", param->name, cwmp_get_type_string(param->type), level);
//  int i=0;

  sprintf(fmt, "|%%-%ds%%s,  get:%%p set:%%p refresh:%%p", level*4);

  cwmp_log_debug(fmt, "----", param->name, param->get, param->set, param->refresh);


  child = param->child;

  if(!child)
	return;
  print_param(child, level+1);

  parameter_node_t * next = child->next_sibling;

  while(next) {
    print_param(next, level+1);
    next = next->next_sibling;
  }
}

void cwmp_agent_session(cwmp_t * cwmp)
{
    char name[1024] = {0};
    char value[1024]= {0};
    char local_ip[32];

    char * envstr;
    char * encstr;

    envstr = cwmp_conf_pool_get(cwmp->pool,"cwmp:soap_env");
    encstr = cwmp_conf_pool_get(cwmp->pool,"cwmp:soap_enc");

    cwmp_set_envelope_ns(envstr, encstr);

    if (cwmp_session_get_localip(local_ip) == -1)
    {
        cwmp_log_error("get local ip error. exited.\n");
        exit(-1);
    }

    print_param(cwmp->root, 0);

    CWMP_SPRINTF_PARAMETER_NAME(name, 3, InternetGatewayDeviceModule, ManagementServerModule, URLModule);
    cwmp_data_set_parameter_value(cwmp, cwmp->root, name, cwmp->acs_url, TRstrlen(cwmp->acs_url), cwmp->pool);

    CWMP_SPRINTF_PARAMETER_NAME(name, 3, InternetGatewayDeviceModule, ManagementServerModule, ConnectionRequestURLModule);
    TRsnprintf(value, 1024, "http://%s:%d", local_ip, cwmp->httpd_port);
    cwmp_data_set_parameter_value(cwmp, cwmp->root, name, value, TRstrlen(value), cwmp->pool);

    CWMP_SPRINTF_PARAMETER_NAME(name, 3, InternetGatewayDeviceModule, DeviceInfoModule, ManufacturerModule);
    cwmp_data_set_parameter_value(cwmp, cwmp->root, name, cwmp->cpe_mf, TRstrlen(cwmp->cpe_mf), cwmp->pool);


    CWMP_SPRINTF_PARAMETER_NAME(name, 3, InternetGatewayDeviceModule, DeviceInfoModule, ManufacturerOUIModule);
    cwmp_data_set_parameter_value(cwmp, cwmp->root, name, cwmp->cpe_oui, TRstrlen(cwmp->cpe_oui), cwmp->pool);

    CWMP_SPRINTF_PARAMETER_NAME(name, 3, InternetGatewayDeviceModule, DeviceInfoModule, ProductClassModule);
    cwmp_data_set_parameter_value(cwmp, cwmp->root, name, cwmp->cpe_pc, TRstrlen(cwmp->cpe_pc), cwmp->pool);

    CWMP_SPRINTF_PARAMETER_NAME(name, 3, InternetGatewayDeviceModule, DeviceInfoModule, SerialNumberModule);
    cwmp_data_set_parameter_value(cwmp, cwmp->root, name, cwmp->cpe_sn, TRstrlen(cwmp->cpe_sn), cwmp->pool);


    cwmp_agent_start_session(cwmp);
}

int cwmp_agent_download_file(download_arg_t * dlarg)
{
    int faultcode = 0;
    char * fromurl = dlarg->url;
    char * tofile = "/tmp/download.img";

    cwmp_log_error("DEBUG: cwmp_agent_download_file");
    FUNCTION_TRACE();

    if(dlarg->url && TRstrncasecmp("ftp://", dlarg->url, 6) == 0)
    {
	    return 9001;
    }

    faultcode = (http_receive_file(fromurl, tofile) == HTTP_200) ? CWMP_OK : 9001 ;

    if(faultcode != CWMP_OK)
    {
        cwmp_log_error("ERROR: cwmp_agent_download_file FAULT %i", faultcode);
	return 9001;
	    
    }

    cwmp_log_error("FILETYPE: %s \n", dlarg->filetype);

    if(strcmp(dlarg->filetype, "1 Firmware Upgrade Image") == 0)
    {
        cwmp_log_error("INFO: ### FIRMWARE UPGRADE ### \n");

//	setenv("UPLOAD_FILENAME", "/tmp/download.img", 1);
//	system("cat /tmp/download.img | /web/cgi-bin/upload.cgi");
	if (access("/tmp/download.img", F_OK) != -1)
	{
        	firmware_upgrade("/tmp/download.img");
		system("rm /tmp/download.img");
		system("( sleep 10 && reboot ) &");
	}
	else
	{
            cwmp_log_error("ERROR: downloaded firmware does not exist! \n");
	}

    }
    else
    if(strcmp(dlarg->filetype, "3 Vendor Configuration File") == 0)
    {
        cwmp_log_error("INFO: ### CONFIG UPGRADE ### \n");

	system("rm /tmp/mysystem.cfg");
	system("tar -C / -xf /tmp/download.img");

	if (access("/tmp/mysystem.cfg", F_OK) != -1)
	{
	    system("nvram_clear 2860");
    	    system("nvram_renew 2860 /tmp/mysystem.cfg");
	}
	else
	{
            cwmp_log_error("ERROR: downloaded config does not exist! \n");
	}
	
    }
    else
    {
        cwmp_log_error("ERROR! Unknown DOWNLOAD FILETYPE: %s.\n",dlarg->filetype);	
    }


    return faultcode;
}

int cwmp_agent_upload_file(cwmp_t * cwmp, upload_arg_t * ularg)
{
    int faultcode = 0;
    FUNCTION_TRACE();
    char * fromfile;

    char * tourl = ularg->url;

    if(strcmp(ularg->filetype, "1 Vendor Configuration File") == 0)
    {
	// send currect config to acs
    	system("echo \"#The following line must not be removed.\" > /tmp/mysystem.cfg");
	system("echo \"Default\" >> /tmp/mysystem.cfg");

	system("nvram_show 2860 >> /tmp/mysystem.cfg");
	system("tar -zcvf /tmp/mysystem.tar.gz /tmp/mysystem.cfg");
	fromfile = "/tmp/mysystem.tar.gz";
	//fromfile = cwmp_conf_pool_get(cwmp->pool,"cwmp:vconf_filename");

	cwmp_log_error("DEBUG: cwmp_agent_upload_file: try 1 %s",tourl);

	//Send 1
	if (http_send_file(fromfile, tourl) == CWMP_OK) return CWMP_OK;

	http_dest_t* tourl_dest;
	http_dest_create(&tourl_dest, tourl, cwmp->pool);

	char* acs_url = cwmp_nvram_pool_get(cwmp->pool, "cwmp_acs_url");
	http_dest_t* acsurl_dest;
	http_dest_create(&acsurl_dest, acs_url, cwmp->pool);

	char tourl2[1024];
	strncpy(tourl_dest->host,acsurl_dest->host,MAX_HOST_NAME_LEN);
	snprintf(&tourl2[0],1024,"%s://%s:%i/%s",tourl_dest->scheme,tourl_dest->host,tourl_dest->port,tourl_dest->uri);

        tourl = pool_pstrdup(cwmp->pool, tourl2);
	cwmp_log_error("DEBUG: cwmp_agent_upload_file: try 2 %s",tourl);

	//Send 2
	if (http_send_file(fromfile, tourl) == CWMP_OK) return CWMP_OK;

	tourl_dest->port = acsurl_dest->port;
	snprintf(&tourl2[0],1024,"%s://%s:%i/%s",tourl_dest->scheme,tourl_dest->host,tourl_dest->port,tourl_dest->uri);

        tourl = pool_pstrdup(cwmp->pool, tourl2);
	cwmp_log_error("DEBUG: cwmp_agent_upload_file: try 3 %s",tourl);

	//Send 3
	if (http_send_file(fromfile, tourl) == CWMP_OK) return CWMP_OK;



/*

	if (strcmp(tourl_host,"0.0.0.0") == 0) 
	{
	    tourl_dest->host = acsurl_dest->host;
	}
	tourl_dest->port = acsurl_dest->port;

	//FIXME Correct url parsing!
	char tourl_part2[1024];
	cwmp_log_error("DEBUG: cwmp_agent_upload_file: before check %s",tourl);
	if (sscanf(tourl, "http://0.0.0.0/%s", tourl_part2) > 0) 
	{
    	    cwmp_log_error("DEBUG: cwmp_agent_upload_file: part 2 %s",tourl_part2);

	    char* acs_url = cwmp_nvram_pool_get(cwmp->pool, "cwmp_acs_url");
    	    cwmp_log_error("DEBUG: cwmp_agent_upload_file: acs_url %s",acs_url);
	    
	    
	    char* acs_url_part2 = strtok(NULL, "/");

	    if (acs_url_part2 != NULL)
	    {
		    char tourl_over[1024];
		    char* acs_url_part2_host = strtok(acs_url_part2, ":");
		    snprintf(tourl_over, 1024, "http://%s/%s", acs_url_part2, tourl_part2);
	    	    cwmp_log_error("DEBUG: cwmp_agent_upload_file: tourl_over %s",tourl_over);
		    tourl = pool_pstrdup(cwmp->pool, tourl_over);
		    if (http_send_file(fromfile, tourl) == CWMP_OK) return CWMP_OK;

		    if (acs_url_part2_host != NULL)
		    {

    			    char tourl_over[1024];
			    snprintf(tourl_over, 1024, "http://%s/%s", acs_url_part2_host, tourl_part2);
		    	    cwmp_log_error("DEBUG: cwmp_agent_upload_file: tourl_over %s",tourl_over);
			    tourl = pool_pstrdup(cwmp->pool, tourl_over);
		    }

	    }
	}
*/

    	cwmp_log_error("DEBUG: cwmp_agent_upload_file: tourl_result %s",tourl);
    }
    else if(strcmp(ularg->filetype, "2 Vendor Log File") == 0)
    {
	// send currect log to acs
	fromfile = cwmp_conf_pool_get(cwmp->pool,"cwmp:devicelog_filename");

    }
/*    else
    {
	fromfile = cwmp_conf_pool_get(cwmp->pool,"cwmp:vconf_filename");
    }
*/
    faultcode = http_send_file(fromfile, tourl);

    if(faultcode != CWMP_OK)
    {
	faultcode = 9001;
    }

    return faultcode;
}

int cwmp_agent_run_tasks(cwmp_t * cwmp)
{
	void * data;
	int tasktype = 0;;
	int ok = CWMP_NO;

	FUNCTION_TRACE();

	while(1)
	{
		tasktype = queue_pop(cwmp->queue, &data);
		if(tasktype == -1)
		{
			cwmp_log_debug("no more task to run");
			break;
		}
		ok = CWMP_YES;
		switch(tasktype)
		{
			case TASK_DOWNLOAD_TAG:
				{
					download_arg_t * dlarg = (download_arg_t*)data;
					//begin download file
					time_t starttime = time(NULL);
					int faultcode = 0;

					faultcode = cwmp_agent_download_file(dlarg);

					time_t endtime = time(NULL);
					cwmp_event_set_value(cwmp, INFORM_TRANSFERCOMPLETE, 1,dlarg->cmdkey, faultcode, starttime, endtime);

					free(dlarg);
				}
				break;

			case TASK_UPLOAD_TAG:
				{
					upload_arg_t * ularg = (upload_arg_t*)data;
					//begin download file
					time_t starttime = time(NULL);
					int faultcode = 0;

					faultcode = cwmp_agent_upload_file(cwmp, ularg);

					time_t endtime = time(NULL);
					cwmp_event_set_value(cwmp, INFORM_TRANSFERCOMPLETE, 1,ularg->cmdkey, faultcode, starttime, endtime);

					free(ularg);
				}
				break;

			case TASK_REBOOT_TAG:
				{
					//begin reboot system
					cwmp_log_debug("reboot ...");
					cwmp_event_set_value(cwmp, INFORM_MREBOOT, 1, NULL, 0, 0, 0);
					cwmp_event_clear_active(cwmp);
					system("reboot");
				}
				break;

			case TASK_FACTORYRESET_TAG:
				{
					//begin factory reset system
					cwmp_log_debug("factory reset ...");
					cwmp_event_clear_active(cwmp);
					system("fs nvramreset");
				}
				break;

			default:
					cwmp_log_error("!!! ERROR !!! Unknown task tag: %s \n", tasktype);
				break;

		}
	}

	return ok;
}
