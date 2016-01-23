/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/* vi: set sw=4 ts=4 sts=4: */
/*
 *	wireless.c -- Wireless Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: storage.c,v 1.10 2008-03-19 11:11:05 chhung Exp $
 */

#include	<stdlib.h>
#include	<dirent.h>
#include	<arpa/inet.h>

#include 	"utils.h"
#include	"usb.h"
#include 	"internet.h"
#include        "firewall.h"
#include	"helpers.h"

static void storageDiskAdm(webs_t wp, char_t *path, char_t *query);
static void storageDiskPart(webs_t wp, char_t *path, char_t *query);
static int ShowPartition(int eid, webs_t wp, int argc, char_t **argv);
static int ShowAllDir(int eid, webs_t wp, int argc, char_t **argv);
static int getCount(int eid, webs_t wp, int argc, char_t **argv);
static int getMaxVol(int eid, webs_t wp, int argc, char_t **argv);
#ifdef CONFIG_FTPD
static void storageFtpSrv(webs_t wp, char_t *path, char_t *query);
#endif
#ifdef CONFIG_USER_TRANSMISSION
static void transmission(webs_t wp, char_t *path, char_t *query);
#endif

#define	LSDIR_INFO	"/tmp/lsdir"
#define	MOUNT_INFO	"/proc/mounts"

#define STORAGE_PATH	"/media"
#define STORAGE_SIGN	"/media/sd"

#define DEBUG(x) do{fprintf(stderr, #x); fprintf(stderr, ": %s\n", x); }while(0)

void formDefineSTORAGE(void) {
	websAspDefine(T("ShowPartition"), ShowPartition);
	websAspDefine(T("ShowAllDir"), ShowAllDir);
	websAspDefine(T("getCount"), getCount);
	websAspDefine(T("getMaxVol"), getMaxVol);
	websFormDefine(T("storageDiskAdm"), storageDiskAdm);
	websFormDefine(T("storageDiskPart"), storageDiskPart);
#ifdef CONFIG_FTPD
	websFormDefine(T("storageFtpSrv"), storageFtpSrv);
#endif
#ifdef CONFIG_USER_TRANSMISSION
	websFormDefine(T("formTrans"), transmission);
#endif
}

static int dir_count;
static int part_count;
static int media_dir_count;

static void storageDiskAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		char_t *dir_path = websGetVar(wp, T("dir_path"), T(""));
		doSystem("storage.sh deldir \"%s\"", dir_path);
		websRedirect(wp, "storage/disk_admin.asp");
	}
	else if (0 == strcmp(submit, "add"))
	{
		char_t *dir_name, *disk_part;

		dir_name = websGetVar(wp, T("adddir_name"), T(""));
		disk_part = websGetVar(wp, T("disk_part"), T(""));
		doSystem("storage.sh adddir \"%s/%s\"", disk_part, dir_name);
		websRedirect(wp, "form_close.asp");
	}
	else if (0 == strcmp(submit, "format"))
	{
		char_t *disk_part = websGetVar(wp, T("disk_part"), T(""));
		FILE *fp_mount = NULL;
		char part[30], path[30];

		if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
		{
        		websRedirect(wp, "storage/disk_admin.asp");
			return;
		}
		while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, path))
		{
			if (0 == strcmp(path, disk_part))
				break;
		}
		fclose(fp_mount);
		doSystem("storage.sh %s %s %s",submit, part, path);
		websRedirect(wp, "storage/disk_admin.asp");
	}
	else if (0 == strcmp(submit, "remove"))
	{
		doSystem("storage.sh remove");
		websRedirect(wp, "storage/disk_admin.asp");
	}
}

static void storageDiskPart(webs_t wp, char_t *path, char_t *query)
{
	char_t *part1_vol, *part2_vol, *part3_vol, *part4_vol;
	FILE *fp_mount = NULL;
	char part[30];

	if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
		return;

	while(EOF != fscanf(fp_mount, "%s %*s %*s %*s %*s %*s\n", part))
	{
		fprintf(stderr, "chhung part: %s\n", part);
		if (NULL != strstr(part, "/dev/sd"))
			doSystem("umount -fl %s", part);
	}
	part1_vol = websGetVar(wp, T("part1_vol"), T(""));
	part2_vol = websGetVar(wp, T("part2_vol"), T(""));
	part3_vol = websGetVar(wp, T("part3_vol"), T(""));
	part4_vol = websGetVar(wp, T("part4_vol"), T(""));

	doSystem("storage.sh reparted %s %s %s %s", part1_vol, part2_vol, part3_vol, part4_vol);
	fclose(fp_mount);
}

#ifdef CONFIG_FTPD
const parameter_fetch_t ftp_server_args[] =
{
	{ T("ftp_port"), "FtpPort", 0, T("") },
	{ T("ftp_rootdir"), "FtpRootDir", 0, T("") },
	{ T("ftp_idle_timeout"), "FtpIdleTime", 0, T("") },
	{ NULL, NULL, 0, NULL }
};

static void storageFtpSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t *ftp_enable = websGetVar(wp, T("ftp_enabled"), T("0"));
	char_t *submitUrl;
	if (ftp_enable == NULL)
		ftp_enable = "0";

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "RemoteFTP", ftp_enable);

	if (CHK_IF_DIGIT(ftp_enable, 1) || CHK_IF_DIGIT(ftp_enable, 2))
		setupParameters(wp, ftp_server_args, 0);

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	firewall_rebuild();
	doSystem("service inetd restart");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));
	websRedirect(wp, submitUrl);
}
#endif

#ifdef CONFIG_USER_TRANSMISSION
const parameter_fetch_t transmission_args[] =
{
	{ T("transRPCPort"), "TransRPCPort", 0, T("") },
	{ T("transAccess"), "TransAccess", 0, T("") },
	{ T("transAuthor"), "TransAuthor", 0, T("") },
	{ T("transLog"), "TransLogin", 0, T("") },
	{ T("transPass"), "TransPass", 0, T("") },
	{ T("transInPort"), "TransInPort", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void transmission(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;
	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "apply")) {
		char_t *trans_enabled = websGetVar(wp, T("TransEnabled"), T("0"));
		if (trans_enabled == NULL)
		    trans_enabled = "0";

		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "TransmissionEnabled", trans_enabled);

		if (CHK_IF_DIGIT(trans_enabled, 1))
			setupParameters(wp, transmission_args, 0);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		firewall_rebuild();
		doSystem("service transmission restart");
	} else if (0 == strcmp(submit, "start")) {
		doSystem("service transmission start");
	} else if (0 == strcmp(submit, "stop")) {
		doSystem("service transmission stop");
	} else if (0 == strcmp(submit, "reload")) {
		doSystem("service transmission reload");
	}

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}
#endif

static int getCount(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char count[3];

	if (2 > ejArgs(argc, argv, T("%d %s"), &type, &field))
		return websWrite(wp, T("Insufficient args\n"));

	if (0 == strcmp(field, "AllDir"))
		sprintf(count, "%d", dir_count);
	else if (0 == strcmp(field, "AllPart"))
		sprintf(count, "%d", part_count);
	else if (0 == strcmp(field, "AllMediaDir"))
		sprintf(count, "%d", media_dir_count);

	if (1 == type) {
		if (!strcmp(count, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), count);
	}

	if (!strcmp(count, ""))
		ejSetResult(eid, "0");

	ejSetResult(eid, count);
	return 0;
}

static int ShowAllDir(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp_mount = fopen(MOUNT_INFO, "r");
	char part[50], path[30];
	char dir_name[30];
	int dir_len = 0;

	if (NULL == fp_mount)
		return -1;

	dir_count = 0;

	while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		DIR *dp;
		struct dirent *dirp;
		struct stat statbuf;

		if (0 != strncmp(path, "/media/sd", 9))
		{
			continue;
		}
		if (NULL == (dp = opendir(path)))
		{
			fprintf(stderr, "open %s error\n", path);
			return -1;
		}
		chdir(path);
		while(NULL != (dirp = readdir(dp)))
		{
			lstat(dirp->d_name, &statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
				if (0 == strncmp(dirp->d_name, ".", 1) ||
					0 == strcmp(dirp->d_name, "home"))
					continue;
				strcpy(dir_name, dirp->d_name);
				dir_len = strlen(dir_name);
				if (dir_len < 30 && dir_len > 0)
				{
					websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), 
							  path, dir_name);
					websWrite(wp, T("<td>%s/%s</td>"), path, dir_name);
					websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), 
							  part);
					websWrite(wp, T("<td>%s</td>"), part);
					websWrite(wp, T("</tr>"));
					dir_count++;
				}
			}
		}
		chdir("/");
		closedir(dp);
	}
	fclose(fp_mount);
	return 0;
}

static int ShowPartition(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp = fopen(MOUNT_INFO, "r");
	char part[50], path[30];

	if (NULL == fp)
	    return -1;

	part_count = 0;

	while(EOF != fscanf(fp, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		if (0 != strncmp(path, "/media/sd", 9))
			continue;
		websWrite(wp, T("<tr align=center>"));
		websWrite(wp, T("<td><input type=\"radio\" name=\"disk_part\" value=\"%s\"></td>"), 
				  path);
		websWrite(wp, T("<td>%s</td>"), part);
		websWrite(wp, T("<td>%s</td>"), path);
		websWrite(wp, T("</tr>"));
		part_count++;
	}
	fclose(fp);
	return 0;
}

static int getMaxVol(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *pp = popen("fdisk -l /dev/sda", "r");
	char maxvol[5], unit[5];
	double transfer, result=0;

	fscanf(pp, "%*s %*s %s %s %*s %*s\n", maxvol, unit);
	pclose(pp);

	transfer = atof(maxvol);
	if (0 == strcmp(unit, "GB,"))
		result = transfer*1000;
	else if (0 == strcmp(unit, "MB,"))
		result = transfer;

	return websWrite(wp, T("%d"), result);
}
