#define MAX_LOG_SIZE 32768
//#define MAX_LOG_SIZE 1024
///////////////////// HELPERS /////////////////////

// escape all XML chars and copy into new text buffer
char* pool_xml_escape_text(char* buffer, size_t text_length, size_t buffer_size, pool_t * pool) 
{
    const int realloc_size = 1024; // block size for reallocation

    int i;
    char* resbuffer;
    char* ptr;
    int written;
    int resbuffer_size;

    resbuffer_size = buffer_size;
    resbuffer = pool_palloc(pool,resbuffer_size); 
    
    if (!resbuffer) return NULL;
    ptr = resbuffer;

    for (i=0;i<text_length;i++)
    {
	written = ptr - resbuffer;

	if ( written >= (resbuffer_size - 6) ) // resbuffer overloaded
	{
	    char* tmp_ptr = pool_prealloc(pool,resbuffer,resbuffer_size,resbuffer_size+realloc_size);
	    if ( !tmp_ptr )
	    {
		// unable to allocate additional block, let's skip some characters
		break; 
	    }

	    resbuffer = tmp_ptr;
	    resbuffer_size += realloc_size;
	    ptr = resbuffer + written;
	}

	switch (buffer[i])
	{
	    case '\0':	ptr[0] = ' ';ptr++;		break;
	    case '<':	memcpy(ptr,"&lt;",4);ptr+=4;	break;
	    case '>':	memcpy(ptr,"&gt;",4);ptr+=4;	break;
	    case '&':	memcpy(ptr,"&amp;",5);ptr+=5;	break;
	    case '\"':	memcpy(ptr,"&quot;",6);ptr+=6;	break;
	    case '\'':	memcpy(ptr,"&apos;",6);ptr+=6;	break;

	    default: ptr[0] = buffer[i];ptr++;
	}
    }

    ptr[0] = 0;

    return resbuffer;
}

///////////////////////////////////////////////////

//InternetGatewayDevice.DeviceInfo.Manufacturer
int cpe_get_igd_di_manufacturer(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_manufacture");
    cwmp_log_debug("cpe_get_igd_di_manufacturer: value is %s", *value);
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ManufacturerOUI
int cpe_get_igd_di_manufactureroui(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_oui");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ProductClass
int cpe_get_igd_di_productclass(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_pc");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SerialNumber
/*
int cpe_get_igd_di_serialnumber(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_sn");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SpecVersion
int cpe_get_igd_di_specversion(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_specver");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.HardwareVersion
int cpe_get_igd_di_hardwareversion(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_hwver");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SoftwareVersion
int cpe_get_igd_di_softwareversion(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_version");
    return	FAULT_CODE_OK;
}
*/
//InternetGatewayDevice.DeviceInfo.ProvisioningCode
/*
int cpe_get_igd_di_provisioningcode(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_prov");
    return	FAULT_CODE_OK;
}
*/
//InternetGatewayDevice.DeviceInfo.DeviceLog
int cpe_get_igd_di_devicelog(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
//    cwmp_log_debug("DEBUG: cpe_get_igd_di_devicelog");
    FUNCTION_TRACE();

    long length, length2;
    char* buffer = pool_palloc(pool,MAX_LOG_SIZE);
    char* resbuffer;

    if (!buffer)
    {
	cwmp_log_error("cpe_get_igd_di_devicelog: unable to allocate devicelog buffer of size %u",MAX_LOG_SIZE);
	return FAULT_CODE_9002; // 9002 Internal error
    }

    char* filename = cwmp_conf_pool_get(pool, "cwmp:devicelog_filename");
    FILE * f = fopen(filename, "rt");

    if (!f)
    {
	cwmp_log_error("cpe_get_igd_di_devicelog: unable to read device log from file (%s)",filename);
	return FAULT_CODE_9002; // 9002 Internal error
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (length > MAX_LOG_SIZE) {
        fseek(f, length - MAX_LOG_SIZE-1, SEEK_SET);
	length = MAX_LOG_SIZE;
    }

    length2 = fread(buffer, 1, length, f);
    cwmp_log_debug("cpe_get_igd_di_devicelog: devicelog file (%s) length is %lu, write length is %lu", filename, length, length2);
    if (ferror(f)) 
    {
        cwmp_log_error("cpe_get_igd_di_devicelog: devicelog file (%s) read error %i", ferror(f));
    }

    fclose(f);

    resbuffer = pool_xml_escape_text(buffer, length2, MAX_LOG_SIZE, pool);
    if (!resbuffer) {
        cwmp_log_error("cpe_get_igd_di_devicelog: unable to escape buffer in pool");
	*value = NULL;
	return FAULT_CODE_9002; // 9002 Internal error
    }

    length2 = strlen(resbuffer);
    if (length2 > MAX_LOG_SIZE) 
    {
	// skip a couple of first characters to fit value buffer
	resbuffer += length2-MAX_LOG_SIZE;
    }

    *value = resbuffer;

//    cwmp_log_debug("DEBUG: cpe_get_igd_di_devicelog OK");
    return FAULT_CODE_OK;
}

