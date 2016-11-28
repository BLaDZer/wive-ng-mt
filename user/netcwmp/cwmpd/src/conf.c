/* vim: set et: */
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

#include "cwmp_module.h"
#include <cwmp/cfg.h>

void cwmp_conf_init(cwmp_t * cwmp)
{
    pool_t * pool;
    cwmp_log_trace("%s(cwmp=%p)", __func__, (void*)cwmp);

    pool = cwmp->pool;

    cwmp->conf.periodic_interval = (unsigned long)cwmp_conf_get_int_def("cwmpd:inform_periodic_interval", 3600);
    cwmp->conf.periodic_enable = (bool)cwmp_conf_get_int_def("cwmpd:inform_periodic_enable", 0);
    if (!cwmp->conf.periodic_interval) {
        cwmp->conf.periodic_interval = 1lu;
    }

    cwmp->httpd_port =  cwmp_conf_get_int_def("cwmpd:httpd_port", 1008); //cwmp_nvram_get_int("cwmp:httpd_port",8080);

    cwmp->cpe_auth  =   cwmp_conf_get_int_def("cwmp:cpe_auth", 0);

    cwmp->acs_user = cwmp_conf_pool_get(pool, "cwmp:acs_username");
    cwmp->acs_pwd = cwmp_conf_pool_get(pool, "cwmp:acs_password");

    if(cwmp->cpe_auth)
    {
        cwmp->cpe_user = cwmp_conf_pool_get(pool, "cwmp:cpe_username");
        cwmp->cpe_pwd = cwmp_conf_pool_get(pool, "cwmp:cpe_password");
    }

    cwmp->acs_url   =   cwmp_conf_pool_get(pool, "cwmp:acs_url"); //  "http://192.168.0.69:8000/otnms/acs/webservice.action";

    cwmp->cpe_mf    =   cwmp_conf_pool_get(pool, "cwmp:cpe_manufacture"); //     "ZTE"; //cwmp_nvram_getdup(pool, "cwmp:cpe_manufacture");

    cwmp->cpe_oui   =   cwmp_conf_pool_get(pool, "cwmp:cpe_oui"); //   "00D0D0";cwmp_nvram_getdup(pool, "cwmp:cpe_oui");

    cwmp->cpe_sn    =   cwmp_nvram_pool_get(pool, "WAN_MAC_ADDR");//cwmp_conf_pool_get(pool, "cwmp:cpe_sn"); //    "0410400AA11AA2255"; //cwmp_nvram_getdup(pool, "cwmp:cpe_sn");

    cwmp->cpe_name  =   cwmp_conf_pool_get(pool, "cwmp:cpe_name"); //  "00D0D0";

    cwmp->cpe_pc    =   cwmp_conf_pool_get(pool, "cwmp:cpe_pc"); //      "ZXECS EBG2100";

    cwmp_log_debug("url:%s\nmf:%s\noui:%s\nsn:%s\nname:%s\npc:%s\nhttpd port:%d",    cwmp->acs_url, cwmp->cpe_mf, cwmp->cpe_oui, cwmp->cpe_sn, cwmp->cpe_name, cwmp->cpe_pc,
                   cwmp->httpd_port);
    cwmp->event_filename = cwmp_conf_pool_get(pool, "cwmp:event_filename");
}
